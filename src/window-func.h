#ifndef WINDOW_UTIL_H
//GLFW website: https://glfw.org
//GLFW github: https://github.com/glfw/glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define DEFAULT_WIDTH 960
#define DEFAULT_HEIGHT 600

//Handle resizing of the window
void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight);
//Initialize window
GLFWwindow* initWindow();

#endif
#define WINDOW_UTIL_H
