set(GFX_PROJECTIONS_SOURCES
        perspective-projection.cpp
        orthographic-projection.cpp
)

add_library(gfx_projections INTERFACE ${GFX_PROJECTIONS_SOURCES})

target_include_directories(gfx_projections INTERFACE
        ${INCLUDE_DIR}
)

