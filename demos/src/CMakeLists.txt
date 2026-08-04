set(GLFW_DEMO_SOURCES
        glfw/animations/main.cpp
        common/core/demo-player.cpp
#        common/animations/video/video-demo.cpp
        common/animations/test-3D/test-3D-demo.cpp
#        common/animations/fireworks/fireworks-demo.cpp
#        common/animations/fireworks/firework.cpp
#        common/animations/fireworks/particle.cpp
#        common/animations/fractal/fractal-demo.cpp
#        common/animations/boids/boids-demo.cpp
#        common/animations/star/star-demo.cpp
#        common/animations/shader/shader-demo.cpp
#        common/animations/text/text-demo.cpp
#        common/animations/physics/physics-demo.cpp
#        common/animations/space/space-demo.cpp
#        common/animations/space/camera.cpp
#        common/animations/space/body.cpp
#        common/animations/space/simulate.cpp
        common/animations/construct/construct-demo.cpp
#        common/animations/snake/snake-demo.cpp
)

#set(CURSES_DEMO_SOURCES
#        curses/animations/main.cpp
#        common/core/demo-player.cpp
#        common/animations/video/video-demo.cpp
#        common/animations/test-3D/test-3D-demo.cpp
#        common/animations/fireworks/fireworks-demo.cpp
#        common/animations/fireworks/firework.cpp
#        common/animations/fireworks/particle.cpp
#        common/animations/fractal/fractal-demo.cpp
#        common/animations/boids/boids-demo.cpp
#        common/animations/star/star-demo.cpp
#        common/animations/shader/shader-demo.cpp
#        common/animations/text/text-demo.cpp
#        common/animations/physics/physics-demo.cpp
#        common/animations/space/space-demo.cpp
#        common/animations/space/camera.cpp
#        common/animations/space/body.cpp
#        common/animations/space/simulate.cpp
#        common/animations/construct/construct-demo.cpp
#        common/animations/snake/snake-demo.cpp
#)

add_executable(glfw_main ${GLFW_DEMO_SOURCES})
target_link_libraries(glfw_main PRIVATE gfx_surfaces_glfw)
target_include_directories(glfw_main PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/gfx/inc"
)

#add_executable(curses_main ${CURSES_DEMO_SOURCES})
#target_link_libraries(curses_main PRIVATE gfx_surfaces_curses)
#target_include_directories(curses_main PRIVATE
#        "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
#        "${CMAKE_CURRENT_SOURCE_DIR}"
#        "${CMAKE_SOURCE_DIR}/gfx/inc"
#)