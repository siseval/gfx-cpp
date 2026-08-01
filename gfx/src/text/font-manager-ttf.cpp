#include "gfx/text/font-manager-ttf.h"

#include "gfx/text/font-ttf.h"

#include <fstream>
#include <iostream>
#include <map>

namespace gfx
{
    static int16_t read_s16(const uint8_t* data)
    {
        return static_cast<int16_t>(data[0]) << 8 | static_cast<int16_t>(data[1]);
    }

    [[maybe_unused]] static int32_t read_s32(const uint8_t* data)
    {
        return static_cast<int32_t>(data[0]) << 24 | static_cast<int32_t>(data[1]) << 16 | static_cast<int32_t>(data[2])
            << 8 | static_cast<int32_t>(data[3]);
    }

    static uint16_t read_u16(const uint8_t* data)
    {
        return static_cast<uint16_t>(data[0]) << 8 | static_cast<uint16_t>(data[1]);
    }

    static uint32_t read_u32(const uint8_t* data)
    {
        return static_cast<uint32_t>(data[0]) << 24 | static_cast<uint32_t>(data[1]) << 16 | static_cast<uint32_t>(data[
            2]) << 8 | static_cast<uint32_t>(data[3]);
    }

    std::shared_ptr<FontTTF> FontManagerTTF::load_from_file(const std::string& path, const std::string& name)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " + path;
            return nullptr;
        }

        const std::streamsize size { file.tellg() };
        if (size <= 0)
        {
            std::cerr << "File is empty: " + path;
            return nullptr;
        }
        file.seekg(0, std::ios::beg);
        const auto buffer { new uint8_t[size] };
        if (!file.read(reinterpret_cast<char*>(buffer), size))
        {
            std::cerr << "Failed to read file: " + path;
            delete[] buffer;
            return nullptr;
        }

        auto font { load_from_memory(buffer, size, name.empty() ? path : name) };
        delete[] buffer;
        return font;
    }

    std::shared_ptr<FontTTF> FontManagerTTF::load_from_memory(
        const uint8_t* data,
        const std::size_t size,
        const std::string& name
    )
    {
        if (size < 12)
        {
            std::cerr << "Data size is too small to be a valid TTF font.";
            return nullptr;
        }

        const uint8_t* ptr { data };

        uint32_t scaler_type { read_u32(ptr) };
        ptr += 4;
        uint16_t num_tables { read_u16(ptr) };
        ptr += 8;

        if (scaler_type != 0x00010000 && scaler_type != 0x74727565)
        {
            std::cerr << "Unsupported TTF scaler type.";
            return nullptr;
        }

        struct TableEntry
        {
            uint32_t offset;
            uint32_t length;
        };

        std::map<std::string, TableEntry> tables;

        for (uint16_t i = 0; i < num_tables; ++i)
        {
            if (ptr + 16 > data + size)
            {
                std::cerr << "Unexpected end of data while reading table entries.";
                return nullptr;
            }
            std::string tag { reinterpret_cast<const char*>(ptr), 4 };
            ptr += 8;
            uint32_t offset { read_u32(ptr) };
            ptr += 4;
            uint32_t length { read_u32(ptr) };
            ptr += 4;

            tables[tag] = { offset, length };
        }

        std::string required_tables[5] { "head", "maxp", "loca", "glyf", "cmap" };
        for (const auto& tag : required_tables)
        {
            if (!tables.contains(tag))
            {
                std::cerr << "Missing required table: " + tag;
                return nullptr;
            }
        }

        auto it_head { tables.find("head") };
        if (it_head == tables.end())
        {
            std::cerr << "Missing 'head' table.";
            return nullptr;
        }
        const uint8_t* head_table { data + it_head->second.offset };
        uint16_t units_per_em { read_u16(head_table + 18) };
        int16_t ascender { (read_s16(head_table + 40)) };
        int16_t descender { (read_s16(head_table + 42)) };
        int16_t line_gap { (read_s16(head_table + 44)) };
        uint16_t index_to_loc_format { read_u16(head_table + 50) };

        auto it_maxp { tables.find("maxp") };
        if (it_maxp == tables.end())
        {
            std::cerr << "Missing 'maxp' table.";
            return nullptr;
        }
        const uint8_t* maxp_table { data + it_maxp->second.offset };
        uint16_t num_glyphs { read_u16(maxp_table + 4) };

        auto it_loca { tables.find("loca") };
        const uint8_t* loca_table { data + it_loca->second.offset };

        auto it_cmap { tables.find("cmap") };
        if (it_cmap == tables.end())
        {
            std::cerr << "Missing 'cmap' table.";
            return nullptr;
        }

        const uint8_t* cmap_table { data + it_cmap->second.offset };
        read_u16(cmap_table);
        uint16_t num_subtables { read_u16(cmap_table + 2) };

        const uint8_t* cmap_format_4 { nullptr };
        const uint8_t* cmap_format_12 { nullptr };

        for (uint16_t i = 0; i < num_subtables; ++i)
        {
            const uint8_t* subtable_ptr { cmap_table + 4 + i * 8 };
            uint16_t platform_id { read_u16(subtable_ptr) };
            uint16_t encoding_id { read_u16(subtable_ptr + 2) };
            uint32_t offset { read_u32(subtable_ptr + 4) };

            const uint8_t* subtable { cmap_table + offset };
            uint16_t format { read_u16(subtable) };

            if (format == 4 && !cmap_format_4 && platform_id == 3 && encoding_id == 1)
            {
                cmap_format_4 = subtable;
            }
            else if (format == 12 && !cmap_format_12 && platform_id == 3 && encoding_id == 10)
            {
                cmap_format_12 = subtable;
            }
        }

        if (!cmap_format_4)
        {
            std::cerr << "No supported cmap subtable found (format 4).";
            return nullptr;
        }

        std::unordered_map codepoint_to_index { parse_cmap_format_4(cmap_format_4, it_cmap->second.length) };

        auto it_glyf { tables.find("glyf") };
        if (it_glyf == tables.end())
        {
            std::cerr << "Missing 'glyf' table.";
            return nullptr;
        }
        const uint8_t* glyf_table { data + it_glyf->second.offset };
        std::size_t glyf_table_length { it_glyf->second.length };

        auto font { std::make_shared<FontTTF>(units_per_em, ascender, descender, line_gap, num_glyphs) };

        std::vector glyph_offsets { parse_loca_table(loca_table, num_glyphs, index_to_loc_format) };

        if (glyph_offsets.size() != num_glyphs + 1)
        {
            std::cerr << "Invalid number of glyph offsets.";
            return nullptr;
        }

        std::vector<std::shared_ptr<FontTTF::GlyphTTF>> glyphs;
        for (int i = 0; i < num_glyphs; ++i)
        {
            uint32_t offset_start { glyph_offsets[i] };
            uint32_t offset_end { glyph_offsets[i + 1] };
            uint32_t glyph_length { offset_end - offset_start };

            if (glyph_length == 0)
            {
                glyphs.push_back(std::make_shared<FontTTF::GlyphTTF>());
                continue;
            }

            if (offset_start + glyph_length > glyf_table_length)
            {
                std::cerr << "Glyph offset out of bounds.";
                return nullptr;
            }

            glyphs.push_back(parse_glyph(glyf_table, glyph_offsets, i));
        }

        std::unordered_map<uint32_t, std::shared_ptr<FontTTF::GlyphTTF>> codepoint_to_glyph;
        for (const auto& [codepoint, glyph_index] : codepoint_to_index)
        {
            if (glyph_index < glyphs.size())
            {
                codepoint_to_glyph[codepoint] = glyphs[glyph_index];
            }
        }
        font->set_glyphs(codepoint_to_glyph);

        auto it_kern { tables.find("kern") };
        if (it_kern != tables.end())
        {
            const uint8_t* kern_table { data + it_kern->second.offset };
            std::size_t kern_length { it_kern->second.length };

            if (kern_length >= 4)
            {
                uint16_t subtables { read_u16(kern_table + 2) };
                const uint8_t* subtable_ptr { kern_table + 4 };

                for (uint16_t i = 0; i < subtables; ++i)
                {
                    if (subtable_ptr + 6 > kern_table + kern_length)
                    {
                        break;
                    }

                    uint16_t st_length { read_u16(subtable_ptr + 2) };
                    uint16_t st_format { read_u16(subtable_ptr + 4) };

                    if (st_format == 0)
                    {
                        const uint8_t* data_ptr { subtable_ptr + 6 };
                        std::size_t num_pairs { read_u16(data_ptr) };
                        data_ptr += 8;

                        for (std::size_t j = 0; j < num_pairs; ++j)
                        {
                            uint16_t left { read_u16(data_ptr + j * 6 + 0) };
                            uint16_t right { read_u16(data_ptr + j * 6 + 2) };
                            int16_t value { read_s16(data_ptr + j * 6 + 4) };
                            font->set_kerning(left, right, value);
                        }
                    }
                    subtable_ptr += st_length;
                }
            }
        }

        auto it_hmtx { tables.find("hmtx") };
        if (it_hmtx == tables.end())
        {
            std::cerr << "Missing 'hmtx' table.";
            return nullptr;
        }

        auto it_hhea { tables.find("hhea") };
        if (it_hhea == tables.end())
        {
            std::cerr << "Missing 'hhea' table.";
            return nullptr;
        }

        const uint8_t* hhea_table { data + it_hhea->second.offset };
        uint16_t number_of_h_metrics { read_u16(hhea_table + 34) };

        const uint8_t* hmtx_table { data + it_hmtx->second.offset };
        std::size_t hmtx_length { it_hmtx->second.length };

        if (hmtx_length < number_of_h_metrics * 4)
        {
            std::cerr << "Invalid 'hmtx' table length.";
            return nullptr;
        }

        std::vector<FontTTF::GlyphMetrics> glyph_metrics(num_glyphs);

        for (uint16_t i = 0; i < number_of_h_metrics; ++i)
        {
            glyph_metrics[i].advance_width = read_u16(hmtx_table + i * 4);
            glyph_metrics[i].left_side_bearing = read_s16(hmtx_table + i * 4 + 2);
        }

        if (number_of_h_metrics < num_glyphs)
        {
            int last_advance { glyph_metrics[number_of_h_metrics - 1].advance_width };
            const uint8_t* lsb_ptr { hmtx_table + number_of_h_metrics * 4 };
            for (uint16_t i = number_of_h_metrics; i < num_glyphs; ++i)
            {
                glyph_metrics[i].advance_width = last_advance;
                glyph_metrics[i].left_side_bearing = read_s16(lsb_ptr + (i - number_of_h_metrics) * 2);
            }
        }

        for (auto& [codepoint, glyph_index] : codepoint_to_index)
        {
            if (glyph_index < glyph_metrics.size())
            {
                font->set_metrics(codepoint, glyph_metrics[glyph_index]);
            }
        }

        font->set_name(name);
        _loaded_fonts[name] = font;

        return font;
    }

    void FontManagerTTF::load_font_directory(const std::filesystem::path& path)
    {
        std::filesystem::path dir_path = path;

        if (dir_path.empty())
        {
            dir_path = _font_directory_path;
        }

        if (!exists(dir_path) || !is_directory(dir_path))
        {
            std::cerr << "Font directory does not exist: " + dir_path.string();
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(dir_path))
        {
            if (entry.is_regular_file())
            {
                std::string extension { entry.path().extension().string() };
                if (extension == ".ttf" || extension == ".TTF")
                {
                    load_from_file(entry.path().string(), entry.path().stem().string());
                }
            }
        }
    }

    std::unordered_map<std::string, std::shared_ptr<FontTTF>> FontManagerTTF::get_loaded_fonts() const
    {
        return _loaded_fonts;
    }

    std::shared_ptr<FontTTF> FontManagerTTF::get_font(const std::string& name)
    {
        const auto it = _loaded_fonts.find(name);
        return it != _loaded_fonts.end() ? it->second : nullptr;
    }

    bool FontManagerTTF::is_font_loaded(const std::string& name) const
    {
        return _loaded_fonts.contains(name);
    }

    void FontManagerTTF::add_font(const std::string& name, const std::shared_ptr<FontTTF> font)
    {
        _loaded_fonts[name] = font;
    }

    void FontManagerTTF::unload_font(const std::string& name)
    {
        _loaded_fonts.erase(name);
    }

    void FontManagerTTF::unload_all_fonts()
    {
        _loaded_fonts.clear();
    }

    void FontManagerTTF::set_font_directory_path(const std::filesystem::path& path)
    {
        _font_directory_path = path;
    }

    std::filesystem::path FontManagerTTF::get_font_directory_path() const
    {
        return _font_directory_path;
    }

    std::unordered_map<uint32_t, uint16_t> FontManagerTTF::parse_cmap_format_4(
        const uint8_t* cmap_table,
        const uint32_t length
    )
    {
        std::unordered_map<uint32_t, uint16_t> char_to_glyph;

        const uint16_t seg_count_x2 { read_u16(cmap_table + 6) };
        const uint16_t seg_count { static_cast<uint16_t>(seg_count_x2 / 2) };

        const uint8_t* end_code_ptr { cmap_table + 14 };
        const uint8_t* start_code_ptr { end_code_ptr + 2 + seg_count * 2 };
        const uint8_t* id_delta_ptr { start_code_ptr + seg_count * 2 };
        const uint8_t* id_range_offset_ptr { id_delta_ptr + seg_count * 2 };

        for (uint16_t i = 0; i < seg_count; ++i)
        {
            const uint16_t end_code { read_u16(end_code_ptr + i * 2) };
            const uint16_t start_code { read_u16(start_code_ptr + i * 2) };
            const int16_t id_delta { read_s16(id_delta_ptr + i * 2) };
            const uint16_t id_range_offset { read_u16(id_range_offset_ptr + i * 2) };

            for (uint32_t c = start_code; c <= end_code; ++c)
            {
                uint16_t glyph_id { 0 };
                if (id_range_offset == 0)
                {
                    glyph_id = static_cast<uint16_t>((c + id_delta) % 0xFFFF);
                }
                else
                {
                    const uint32_t offset { id_range_offset / 2 + (c - start_code) - (seg_count - i) };
                    const uint8_t* glyph_id_ptr { id_range_offset_ptr + i * 2 + offset * 2 };

                    if (glyph_id_ptr + 1 < cmap_table + length)
                    {
                        glyph_id = read_u16(glyph_id_ptr);
                        if (glyph_id != 0)
                        {
                            glyph_id = static_cast<uint16_t>((glyph_id + id_delta) % 0xFFFF);
                        }
                        else
                        {
                            glyph_id = 0;
                        }
                    }
                }
                char_to_glyph[c] = glyph_id;
            }
        }
        return char_to_glyph;
    }

    std::vector<uint32_t> FontManagerTTF::parse_loca_table(
        const uint8_t* loca_table,
        const uint16_t num_glyphs,
        const uint16_t index_to_loc_format
    )
    {
        std::vector<uint32_t> offsets;
        offsets.reserve(num_glyphs + 1);

        if (index_to_loc_format == 0)
        {
            for (int i = 0; i <= num_glyphs; ++i)
            {
                uint32_t offset { static_cast<uint32_t>(read_u16(loca_table + i * 2) * 2) };
                offsets.push_back(offset);
            }
        }
        else
        {
            for (int i = 0; i <= num_glyphs; ++i)
            {
                uint32_t offset { read_u32(loca_table + i * 4) };
                offsets.push_back(offset);
            }
        }
        return offsets;
    }

    std::shared_ptr<FontTTF::GlyphTTF> FontManagerTTF::parse_glyph(
        const uint8_t* glyf_table,
        const std::vector<uint32_t>& glyph_offsets,
        const uint16_t glyph_index
    )
    {
        auto glyph { std::make_shared<FontTTF::GlyphTTF>() };

        const uint32_t offset_start { glyph_offsets[glyph_index] };
        const uint32_t offset_end { glyph_offsets[glyph_index + 1] };
        if (offset_start == offset_end)
        {
            return glyph;
        }

        const uint8_t* glyph_ptr { glyf_table + offset_start };

        const int16_t number_of_contours { read_s16(glyph_ptr) };
        glyph->bbox = {
            { static_cast<double>(read_s16(glyph_ptr + 2)), static_cast<double>(read_s16(glyph_ptr + 4)) },
            { static_cast<double>(read_s16(glyph_ptr + 6)), static_cast<double>(read_s16(glyph_ptr + 8)) }
        };

        if (number_of_contours <= 0)
        {
            return glyph;
        }

        const uint8_t* ptr { glyph_ptr + 10 };
        std::vector<uint16_t> end_pts_of_contours;

        for (int i = 0; i < number_of_contours; ++i)
        {
            uint16_t end_pt { read_u16(ptr) };
            end_pts_of_contours.push_back(end_pt);
            ptr += 2;
        }

        const uint16_t instruction_length { read_u16(ptr) };
        ptr += 2 + instruction_length;

        const uint16_t num_points { static_cast<uint16_t>(end_pts_of_contours.back() + 1) };

        std::vector<uint8_t> flags;
        flags.reserve(num_points);

        while (flags.size() < num_points)
        {
            uint8_t flag { (*ptr++) };
            flags.push_back(flag);
            if (flag & 0x08)
            {
                const uint8_t repeat_count { (*ptr++) };
                for (int j = 0; j < repeat_count; ++j)
                {
                    flags.push_back(flag);
                }
            }
        }

        std::vector<int16_t> x_coords(num_points);
        int16_t x { 0 };
        for (int i = 0; i < num_points; ++i)
        {
            if (flags[i] & 0x02)
            {
                const uint8_t dx { (*ptr++) };
                x += flags[i] & 0x10 ? dx : -dx;
            }
            else if (!(flags[i] & 0x10))
            {
                const int16_t dx { read_s16(ptr) };
                ptr += 2;
                x += dx;
            }
            x_coords[i] = x;
        }

        std::vector<int16_t> y_coords(num_points);
        int16_t y { 0 };
        for (int i = 0; i < num_points; ++i)
        {
            if (flags[i] & 0x04)
            {
                const uint8_t dy { (*ptr++) };
                y += flags[i] & 0x20 ? dy : -dy;
            }
            else if (!(flags[i] & 0x20))
            {
                const int16_t dy { read_s16(ptr) };
                ptr += 2;
                y += dy;
            }
            y_coords[i] = y;
        }

        glyph->contours.resize(number_of_contours);

        int point_index { 0 };
        for (int c = 0; c < number_of_contours; ++c)
        {
            const int end_pt { end_pts_of_contours[c] };
            while (point_index <= end_pt)
            {
                glyph->contours[c].push_back(
                    {
                        static_cast<double>(x_coords[point_index]),
                        static_cast<double>(y_coords[point_index]),
                        static_cast<bool>(flags[point_index] & 0x01)
                    }
                );
                ++point_index;
            }
        }

        return glyph;
    }
}
