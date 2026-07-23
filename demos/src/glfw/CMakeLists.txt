find_package(glfw3 REQUIRED)

add_subdirectory(animations)

add_library(demos_glfw INTERFACE)

target_link_libraries(demos_glfw INTERFACE
        glfw
)

target_include_directories(demos_glfw INTERFACE
        ${INCLUDE_DIR}
)
