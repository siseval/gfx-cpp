#ifndef FONT_MANAGER_TTF_H
#define FONT_MANAGER_TTF_H

#include <filesystem>
#include <gfx/text/font-manager.h>
#include <gfx/text/font-ttf.h>

namespace gfx::text
{

class FontManagerTTF
{

public:

    std::shared_ptr<FontTTF> load_from_file(const std::string &path, const std::string &name = "");
    std::shared_ptr<FontTTF> load_from_memory(const uint8_t* data, const std::size_t size, const std::string &name);

    void load_font_directory(const std::filesystem::path &path = "");

    std::unordered_map<std::string, std::shared_ptr<FontTTF>> get_loaded_fonts() const
    {
        return loaded_fonts;
    }

    std::shared_ptr<FontTTF> get_font(const std::string &name)
    {
        auto it = loaded_fonts.find(name);
        if (it != loaded_fonts.end())
        {
            return it->second;
        }
        return nullptr;
    }

    bool is_font_loaded(const std::string &name) const
    {
        return loaded_fonts.find(name) != loaded_fonts.end();
    }

    void add_font(const std::string &name, const std::shared_ptr<FontTTF> font)
    {
        loaded_fonts[name] = font;
    }

    void unload_font(const std::string &name)
    {
        loaded_fonts.erase(name);
    }

    void unload_all_fonts()
    {
        loaded_fonts.clear();
    }

    void set_font_directory_path(const std::filesystem::path &path)
    {
        font_directory_path = path;
    }

    std::filesystem::path get_font_directory_path() const
    {
        return font_directory_path;
    }


private:

    std::unordered_map<uint32_t, uint16_t> parse_cmap_format_4(const std::uint8_t* cmap_table, const uint32_t length);
    std::vector<uint32_t> parse_loca_table(const std::uint8_t* loca_table, const uint16_t num_glyphs, uint16_t index_to_loc_format);
    std::shared_ptr<GlyphTTF> parse_glyph(const std::uint8_t* glyf_table, const std::vector<uint32_t> &glyph_offsets, const uint16_t glyph_index, bool loca_long_format);

    std::filesystem::path font_directory_path;

    std::unordered_map<std::string, std::shared_ptr<FontTTF>> loaded_fonts;
};

}

#endif // FONT_MANAGER_TTF_H
