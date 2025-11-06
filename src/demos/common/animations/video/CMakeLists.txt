set(DEMOS_COMMON_ANIMATIONS_VIDEO_SOURCES
    video-demo.cpp
)

add_library(demos_common_animations_video STATIC
    ${DEMOS_COMMON_ANIMATIONS_VIDEO_SOURCES}
)

target_link_libraries(demos_common_animations_video PRIVATE
    gfx
)

target_include_directories(demos_common_animations_video PUBLIC
    ${INCLUDE_DIR}
)
