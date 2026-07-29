set(DEMOS_COMMON_ANIMATIONS_CONSTRUCT_SOURCES
        construct-demo.cpp
)

add_library(demos_common_animations_construct STATIC
        ${DEMOS_COMMON_ANIMATIONS_CONSTRUCT_SOURCES}
)

target_link_libraries(demos_common_animations_construct PRIVATE
        gfx
)

target_include_directories(demos_common_animations_construct PUBLIC
        ${INCLUDE_DIR}
)
