set(GFX_CORE_TYPES_SOURCES
        bitmap.cpp
        color4.cpp
        obb-2D.cpp
        obb-3D.cpp
        uuid.cpp
        frustum.cpp
        bounding-sphere.cpp
        barycentric-triangle.cpp
        triangle-mesh.cpp
)

add_library(gfx_core_types STATIC ${GFX_CORE_TYPES_SOURCES})

target_include_directories(gfx_core_types PUBLIC
        ${INCLUDE_DIR}
)

