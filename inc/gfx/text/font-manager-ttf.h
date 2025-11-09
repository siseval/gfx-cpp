#ifndef FONT_MANAGER_TTF_H
#define FONT_MANAGER_TTF_H

#include <gfx/text/font-manager.h>
#include <gfx/text/font-ttf.h>

namespace gfx::text
{

class FontManagerTTF
{

public:

    std::shared_ptr<FontTTF> load_from_file(const std::string &path);
    std::shared_ptr<FontTTF> load_from_memory(const uint8_t* data, const std::size_t size);

private:

    std::unordered_map<uint32_t, uint16_t> parse_cmap_format_4(const std::uint8_t* cmap_table, const uint32_t length);
    std::vector<uint32_t> parse_loca_table(const std::uint8_t* loca_table, const uint16_t num_glyphs, uint16_t index_to_loc_format);
    std::shared_ptr<GlyphTTF> parse_glyph(const std::uint8_t* glyf_table, const std::vector<uint32_t> &glyph_offsets, const uint16_t glyph_index, bool loca_long_format);

    std::vector<std::shared_ptr<FontTTF>> loaded_fonts;
};

}

#endif // FONT_MANAGER_TTF_H
