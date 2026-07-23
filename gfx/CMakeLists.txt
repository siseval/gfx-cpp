add_library(gfx
        inc/gfx/geometry/types/triangle.h)

file(GLOB_RECURSE GFX_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(gfx PRIVATE ${GFX_SOURCES})

target_include_directories(gfx PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)
