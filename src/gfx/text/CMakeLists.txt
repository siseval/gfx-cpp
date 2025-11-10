set(GFX_TEXT_SOURCES
    font-manager-ttf.cpp
    font-ttf.cpp
    utf-8.cpp
)

add_library(gfx_text STATIC ${GFX_TEXT_SOURCES})

target_link_libraries(gfx_text PUBLIC
    gfx_core
)

target_include_directories(gfx_text PUBLIC
        ${INCLUDE_DIR}
)

