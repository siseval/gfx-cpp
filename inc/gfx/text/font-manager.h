// #ifndef FONT_MANAGER_H
// #define FONT_MANAGER_H
//
// #include <string>
// #include <memory.h>
// #include <gfx/text/font.h>
//
// namespace gfx::text
// {
//
// class FontManager
// {
//
// public:
//
//     virtual std::shared_ptr<Font> load_from_file(const std::string &path, const std::string &name = "") = 0;
//     virtual std::shared_ptr<Font> load_from_memory(const std::vector<std::byte> &data, const std::size_t size, const std::string &name = "") = 0;
//
//     virtual std::shared_ptr<Font> get_font(const std::string &name)
//     {
//         auto it = loaded_fonts.find(name);
//         if (it != loaded_fonts.end())
//         {
//             return it->second;
//         }
//         return nullptr;
//     }
//
//     virtual bool is_font_loaded(const std::string &name) const
//     {
//         return loaded_fonts.find(name) != loaded_fonts.end();
//     }
//
//     virtual void add_font(const std::string &name, const std::shared_ptr<Font> font)
//     {
//         loaded_fonts[name] = font;
//     }
//
//     virtual void unload_font(const std::string &name)
//     {
//         loaded_fonts.erase(name);
//     }
//
//     virtual void unload_all_fonts()
//     {
//         loaded_fonts.clear();
//     }
//
//
//
//
//     std::unordered_map<std::string, std::shared_ptr<Font>> loaded_fonts;
// };
//
// }
//
// #endif // FONT_MANAGER_H
