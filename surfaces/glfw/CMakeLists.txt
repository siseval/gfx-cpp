find_package(glfw3 REQUIRED)

set(GFX_SURFACES_GLFW_SOURCES
        glad.c
        glfw-render-surface.cpp
)

add_library(gfx_surfaces_glfw STATIC ${GFX_SURFACES_GLFW_SOURCES})

target_link_libraries(gfx_surfaces_glfw PRIVATE
        gfx
        glfw
)

if(APPLE)
    target_link_libraries(gfx_surfaces_glfw PRIVATE "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(gfx_surfaces_glfw PRIVATE GL)
endif()

target_include_directories(gfx_surfaces_glfw PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)



