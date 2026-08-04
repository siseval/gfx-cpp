#include "glfw/glfw-demo-player.h"

int main()
{
    demos::GLFWDemoPlayer player;
    player.init();
    player.run();
}
