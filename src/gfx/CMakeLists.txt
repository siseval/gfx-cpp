add_subdirectory(core)
add_subdirectory(primitives)
add_subdirectory(shaders)
add_subdirectory(surfaces)
add_subdirectory(utils)
add_subdirectory(text)
add_subdirectory(geometry)
add_subdirectory(debug)

add_library(gfx INTERFACE)

target_link_libraries(gfx INTERFACE
    gfx_core
    gfx_primitives
    gfx_shaders
    gfx_surfaces
    gfx_utils
    gfx_text
    gfx_geometry
    gfx_debug
)

target_include_directories(gfx INTERFACE
    ${INCLUDE_DIR}
)

