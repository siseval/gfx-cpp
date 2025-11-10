#include <gfx/text/utf-8.h>

namespace gfx::text
{

bool decode_utf8(const std::string &s, size_t pos, uint32_t &out_codepoint, size_t &bytes)
{
    if (pos >= s.size()) 
    {
        return false;
    }

    unsigned char c = static_cast<unsigned char>(s[pos]);

    if (c <= 0x7F) 
    {
        out_codepoint = c;
        bytes = 1;

    } 
    else if ((c & 0xE0) == 0xC0) 
    {
        if (pos + 1 >= s.size()) 
        {
            return false;
        }
        out_codepoint = ((c & 0x1F) << 6) | (s[pos+1] & 0x3F);
        bytes = 2;
    } 
    else if ((c & 0xF0) == 0xE0) 
    {
        if (pos + 2 >= s.size()) 
        {
            return false;
        }
        out_codepoint = ((c & 0x0F) << 12) |
                        ((s[pos+1] & 0x3F) << 6) |
                        (s[pos+2] & 0x3F);
        bytes = 3;
    } 
    else if ((c & 0xF8) == 0xF0) 
    {
        if (pos + 3 >= s.size()) 
        {
            return false;
        }
        out_codepoint = ((c & 0x07) << 18) |
                        ((s[pos+1] & 0x3F) << 12) |
                        ((s[pos+2] & 0x3F) << 6) |
                        (s[pos+3] & 0x3F);
        bytes = 4;
    } 
    else 
    {
        return false;
    }
    return true;
}

}
