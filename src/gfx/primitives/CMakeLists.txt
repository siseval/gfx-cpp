set(GFX_PRIMITIVES_SOURCES
    circle-2D.cpp
    ellipse-2D.cpp
    polyline-2D.cpp
    polygon-2D.cpp
    bitmap-2D.cpp
    text-2D.cpp
)

add_library(gfx_primitives STATIC ${GFX_PRIMITIVES_SOURCES})

target_link_libraries(gfx_primitives PUBLIC
    gfx_core
    gfx_utils
)

target_include_directories(gfx_primitives PUBLIC
        ${INCLUDE_DIR}
)

