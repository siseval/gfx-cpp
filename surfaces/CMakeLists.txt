cmake_minimum_required(VERSION 3.20)
project(surfaces LANGUAGES C CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(surfaces_glfw
        glfw/glfw-render-surface.cpp
        glfw/glfw-render-surface.h
        glfw/glad.c
        glfw/glad.h
        glfw/khrplatform.h
)

add_library(surfaces::glfw ALIAS surfaces_glfw)

target_include_directories(surfaces_glfw
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glfw>
)

target_link_libraries(surfaces_glfw
        PUBLIC
        gfx::gfx
        OpenGL::GL
        glfw
)