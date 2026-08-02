set(GFX_CORE_SOURCES
        primitive-2D.cpp
        render-2D.cpp
        render-3D.cpp
        render-engine.cpp
        render-surface.cpp
        scene-graph-2D.cpp
        scene-graph.cpp
        thread-pool.cpp
        camera.cpp
)

add_library(gfx_core STATIC ${GFX_CORE_SOURCES})

add_subdirectory(types)
add_subdirectory(material)

target_link_libraries(gfx_core PUBLIC
        gfx_core_types
        gfx_core_material
)

target_include_directories(gfx_core PUBLIC
        ${INCLUDE_DIR}
)

