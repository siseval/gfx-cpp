#ifndef UTF8_H
#define UTF8_H

#include <string>
#include <cstddef>

namespace gfx::text
{

bool decode_utf8(const std::string &s, size_t pos, uint32_t &out_codepoint, size_t &bytes);

}

#endif // UTF8_H
