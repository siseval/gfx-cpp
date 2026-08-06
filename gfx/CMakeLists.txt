cmake_minimum_required(VERSION 3.20)
project(gfx VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(gfx)


# === PUBLIC HEADERS ===

# --- CORE ---
target_sources(gfx PRIVATE
        inc/gfx/core/primitive.h
        inc/gfx/core/render-layer.h
        inc/gfx/core/render-surface.h
        inc/gfx/core/scene-graph.h
        inc/gfx/core/transform.h
)

# --- CORE/MATERIAL ---
target_sources(gfx PRIVATE
        inc/gfx/core/material/fragment-shader.h
        inc/gfx/core/material/material.h
        inc/gfx/core/material/texture.h
        inc/gfx/core/material/vertex-shader.h
)

# --- CORE/TYPES ---
target_sources(gfx PRIVATE
        inc/gfx/core/types/bitmap.h
        inc/gfx/core/types/color4.h
        inc/gfx/core/triangle-mesh.h
        inc/gfx/core/types/uuid.h
)

# --- CORE/VIEW ---
target_sources(gfx PRIVATE
        inc/gfx/core/view/projection.h
        inc/gfx/core/view/view-bounds.h
        inc/gfx/core/view/view.h
        inc/gfx/core/view/viewport.h
)

# --- GEOMETRY ---
target_sources(gfx PRIVATE
        inc/gfx/geometry/flatten.h
        inc/gfx/geometry/triangulate.h
)

# --- GEOMETRY/TYPES ---
target_sources(gfx PRIVATE
        inc/gfx/geometry/types/frustum.h
        inc/gfx/geometry/types/oriented-box.h
        inc/gfx/geometry/types/polygon.h
        inc/gfx/geometry/types/triangle.h
        inc/gfx/geometry/types/bounding-ball.h
        inc/gfx/geometry/types/aligned-box.h
)

# --- MATH ---
target_sources(gfx PRIVATE
        inc/gfx/math/matrix.h
        inc/gfx/math/vec2.h
        inc/gfx/math/vec3.h
)

# --- PRIMITIVES ---
target_sources(gfx PRIVATE
        inc/gfx/primitives/cone-3D.h
        inc/gfx/primitives/cuboid-3D.h
        inc/gfx/primitives/plane-3D.h
        inc/gfx/primitives/polygon-3D.h
        inc/gfx/primitives/sphere-3D.h
        inc/gfx/primitives/text-2D.h
)

# --- PROJECTIONS ---
target_sources(gfx PRIVATE
        inc/gfx/projections/orthographic-projection.h
        inc/gfx/projections/perspective-projection.h
        inc/gfx/projections/viewport-projection.h
)

# --- SHADERS ---
target_sources(gfx PRIVATE
        inc/gfx/shaders/default-fragment-shader.h
        inc/gfx/shaders/default-shader-2D.h
        inc/gfx/shaders/default-vertex-shader.h
        inc/gfx/shaders/diffuse-fragment-shader.h
)

# --- TEXT ---
target_sources(gfx PRIVATE
        inc/gfx/text/font-manager-ttf.h
        inc/gfx/text/font-ttf.h
        inc/gfx/text/utf-8.h
)


# === SOURCE ===

# --- CORE ---
target_sources(gfx PRIVATE
        src/core/render-surface.cpp
        inc/gfx/core/thread-pool.h
        src/core/thread-pool.cpp
)

# --- CORE/MATERIAL ---
target_sources(gfx PRIVATE
        src/core/material/fragment-shader.cpp
        src/core/material/material.cpp
        src/core/material/texture.cpp
)

# --- CORE/TYPES ---
target_sources(gfx PRIVATE
        src/core/types/bitmap.cpp
        src/core/types/color4.cpp
        src/core/types/uuid.cpp
)

# --- GEOMETRY ---
target_sources(gfx PRIVATE
        src/geometry/flatten.cpp
        src/geometry/triangulate.cpp
)

# --- CORE/TYPES ---
target_sources(gfx PRIVATE
        src/geometry/types/frustum.cpp
)

# --- PRIMITIVES ---
target_sources(gfx PRIVATE
        src/primitives/cone-3D.cpp
        src/primitives/cuboid-3D.cpp
        src/primitives/plane-3D.cpp
        src/primitives/polygon-3D.cpp
        src/primitives/sphere-3D.cpp
        src/primitives/text-2D.cpp
)

# --- PROJECTIONS ---
target_sources(gfx PRIVATE
        src/projections/orthographic-projection.cpp
        src/projections/perspective-projection.cpp
        src/projections/viewport-projection.cpp
)


# --- TEXT ---
target_sources(gfx PRIVATE
        src/text/font-manager-ttf.cpp
        src/text/font-ttf.cpp
        src/text/utf-8.cpp
)

# --- UTILS ---
target_sources(gfx PRIVATE
        src/utils/log.cpp
)

add_library(gfx::gfx ALIAS gfx)

target_compile_features(gfx PUBLIC cxx_std_23)
set_target_properties(gfx PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_include_directories(gfx
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)