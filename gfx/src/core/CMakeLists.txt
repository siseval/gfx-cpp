set(GFX_CORE_SOURCES
        #render-2D.cpp
        render-surface.cpp
        thread-pool.cpp
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

