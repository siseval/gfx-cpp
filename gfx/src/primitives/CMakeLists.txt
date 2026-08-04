set(GFX_PRIMITIVES_SOURCES
        #circle-2D.cpp
        cuboid-3D.cpp
        #ellipse-2D.cpp
        sphere-3D.cpp
        #polyline-2D.cpp
        #polygon-2D.cpp
        polygon-3D.cpp
        plane-3D.cpp
        #text-2D.cpp
        cone-3D.cpp
)

add_library(gfx_primitives STATIC ${GFX_PRIMITIVES_SOURCES})

target_link_libraries(gfx_primitives PUBLIC
        gfx_core
        gfx_utils
)

target_include_directories(gfx_primitives PUBLIC
        ${INCLUDE_DIR}
)

