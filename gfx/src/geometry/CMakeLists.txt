set(GFX_GEOMETRY_SOURCES
        flatten.cpp
        ../../2D-bak/rasterize.cpp
        ../../2D-bak/triangulate.cpp
)

add_library(gfx_geometry STATIC ${GFX_GEOMETRY_SOURCES})

add_subdirectory(types)

target_link_libraries(gfx_geometry PUBLIC
        gfx_core
        gfx_utils
)

target_include_directories(gfx_geometry PUBLIC
        ${INCLUDE_DIR}
)

