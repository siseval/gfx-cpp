set(GFX_DEBUG_SOURCES
    debug-viewer.cpp
)

add_library(gfx_debug STATIC ${GFX_DEBUG_SOURCES})

target_link_libraries(gfx_debug PUBLIC
    gfx_core
    gfx_utils
    gfx_primitives
)

target_include_directories(gfx_debug PUBLIC
        ${INCLUDE_DIR}
)

