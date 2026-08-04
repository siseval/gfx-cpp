set(GFX_TEXT_SOURCES
        ../../2D-bak/font-manager-ttf.cpp
        ../../2D-bak/font-ttf.cpp
        ../../2D-bak/utf-8.cpp
)

add_library(gfx_text STATIC ${GFX_TEXT_SOURCES})

target_link_libraries(gfx_text PUBLIC
        gfx_core
)

target_include_directories(gfx_text PUBLIC
        ${INCLUDE_DIR}
)

