add_subdirectory(fireworks)
add_subdirectory(snake)
add_subdirectory(star)
add_subdirectory(space)
add_subdirectory(fractal)
add_subdirectory(video)
add_subdirectory(physics)
add_subdirectory(boids)
add_subdirectory(shader)
add_subdirectory(text)

add_library(demos_common_animations INTERFACE)

target_link_libraries(demos_common_animations INTERFACE
    demos_common_animations_fireworks
    demos_common_animations_snake
    demos_common_animations_star
    demos_common_animations_space
    demos_common_animations_fractal
    demos_common_animations_video
    demos_common_animations_physics
    demos_common_animations_boids
    demos_common_animations_shader
    demos_common_animations_text
)

target_include_directories(demos_common_animations INTERFACE
    ${INCLUDE_DIR}
)
