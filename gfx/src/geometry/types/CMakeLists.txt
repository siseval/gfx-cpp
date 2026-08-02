set(GFX_CORE_TYPES_SOURCES
        frustum.cpp
)

add_library(gfx_geometry_types STATIC ${GFX_GEOMETRY_TYPES_SOURCES})

target_include_directories(gfx_geometry_types PUBLIC
        ${INCLUDE_DIR}
)

