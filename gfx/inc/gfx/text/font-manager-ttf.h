#pragma once

#include <filesystem>

#include "gfx/text/font-ttf.h"

namespace gfx
{
    class FontManagerTTF
    {
    public:

        std::shared_ptr<FontTTF> load_from_file(const std::string& path, const std::string& name = "");
        std::shared_ptr<FontTTF> load_from_memory(const uint8_t* data, std::size_t size, const std::string& name);

        void load_font_directory(const std::filesystem::path& path = "");

        std::unordered_map<std::string, std::shared_ptr<FontTTF>> get_loaded_fonts() const;

        std::shared_ptr<FontTTF> get_font(const std::string& name);
        bool is_font_loaded(const std::string& name) const;

        void add_font(const std::string& name, std::shared_ptr<FontTTF> font);
        void unload_font(const std::string& name);
        void unload_all_fonts();

        void set_font_directory_path(const std::filesystem::path& path);
        std::filesystem::path get_font_directory_path() const;

    private:

        static std::unordered_map<uint32_t, uint16_t> parse_cmap_format_4(
            const std::uint8_t* cmap_table,
            uint32_t length
        );

        static std::vector<uint32_t> parse_loca_table(
            const std::uint8_t* loca_table,
            uint16_t num_glyphs,
            uint16_t index_to_loc_format
        );

        static std::shared_ptr<FontTTF::GlyphTTF> parse_glyph(
            const std::uint8_t* glyf_table,
            const std::vector<uint32_t>& glyph_offsets,
            uint16_t glyph_index
        );

        std::unordered_map<std::string, std::shared_ptr<FontTTF>> _loaded_fonts;
        std::filesystem::path _font_directory_path;
    };
}
