cmake_minimum_required(VERSION 3.20)
project(demos LANGUAGES C CXX)

add_library(demos_common STATIC
        # Core Framework
        src/common/core/demo-player.cpp
        src/common/core/demo-player.h
        src/common/core/demo-utils.h
        src/common/core/gfx-demo.h

        # Animations: Boids
#        src/common/animations/boids/boids-demo.cpp
#        src/common/animations/boids/boids-demo.h

        # Animations: Construct
        src/common/animations/construct/construct-demo.cpp
        src/common/animations/construct/construct-demo.h

        # Animations: Fireworks
#        src/common/animations/fireworks/firework.cpp
#        src/common/animations/fireworks/firework.h
#        src/common/animations/fireworks/fireworks-demo.cpp
#        src/common/animations/fireworks/fireworks-demo.h
#        src/common/animations/fireworks/particle.cpp
#        src/common/animations/fireworks/particle.h

        # Animations: Fractal
#        src/common/animations/fractal/fractal-demo.cpp
#        src/common/animations/fractal/fractal-demo.h
#        src/common/animations/fractal/fractal.h
#        src/common/animations/fractal/julia.h
#        src/common/animations/fractal/mandelbrot.h

        # Animations: Physics
#        src/common/animations/physics/physics-demo.cpp
#        src/common/animations/physics/physics-demo.h

        # Animations: Shader
#        src/common/animations/shader/shader-demo.cpp
#        src/common/animations/shader/shader-demo.h

        # Animations: Snake
#        src/common/animations/snake/snake-demo.cpp
#        src/common/animations/snake/snake-demo.h

        # Animations: Space
#        src/common/animations/space/body.cpp
#        src/common/animations/space/body.h
#        src/common/animations/space/camera.cpp
#        src/common/animations/space/camera.h
#        src/common/animations/space/physics.h
#        src/common/animations/space/simulate.cpp
#        src/common/animations/space/simulate.h
#        src/common/animations/space/space-demo.cpp
#        src/common/animations/space/space-demo.h
#        src/common/animations/space/units.h

        # Animations: Star
#        src/common/animations/star/star-demo.cpp
#        src/common/animations/star/star-demo.h

        # Animations: Test-3D
        src/common/animations/test-3D/test-3D-demo.cpp
        src/common/animations/test-3D/test-3D-demo.h

        # Animations: Text
#        src/common/animations/text/text-demo.cpp
#        src/common/animations/text/text-demo.h

        # Animations: Video
#        src/common/animations/video/video-demo.cpp
#        src/common/animations/video/video-demo.h
)

target_include_directories(demos_common
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/common
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(demos_common PUBLIC
        ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)


target_link_libraries(demos_common PUBLIC gfx::gfx)

add_executable(demo_glfw
        src/glfw/animations/main.cpp
        src/glfw/glfw-demo-player.h
)

target_include_directories(demo_glfw
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/glfw
)

target_link_libraries(demo_glfw
        PRIVATE
        demos_common
        surfaces::glfw
)