cmake_minimum_required(VERSION 3.10)

project(gfx LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(gfx
        inc/gfx/geometry/types/triangle.h
        inc/gfx/core/transform.h
        inc/gfx/geometry/types/oriented-box.h
        inc/gfx/core/view/view-bounds.h
        inc/gfx/core/view/view.h
        inc/gfx/core/view/projection.h
        inc/gfx/projections/perspective-projection.h
        inc/gfx/core/view/viewport.h
        inc/gfx/core/view/projection.h
)

file(GLOB_RECURSE GFX_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(gfx PRIVATE ${GFX_SOURCES})

target_include_directories(gfx PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)
