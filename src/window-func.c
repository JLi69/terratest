//GLFW website: https://glfw.org
//GLFW github: https://github.com/glfw/glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "window-func.h"
#include "gl-func.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

//if this is set to 1, quit
static int canExit = 0;
static GLFWwindow* win;

//Handle resizing of the window
void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
}

void initWindow(void)
{
	//Initialize glfw
	if(!glfwInit())
		crash("Failed to initialize glfw!\n");

	//Create window
	win = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "TerraTest", NULL, NULL);
	if(!win)
		crash("Failed to create window!");
	glfwMakeContextCurrent(win);
	glfwSetWindowSizeCallback(win, handleWindowResize);

	//Initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		crash("Failed to load glad!");
}

void updateActiveShaderWindowSize(void)
{
	int w, h;
	glfwGetWindowSize(win, &w, &h);
	glUniform2f(getUniformLocation("uWindowDimensions", getActiveShader()), (float)w, (float)h);	
}

int canQuit(void)
{
	return canExit || glfwWindowShouldClose(win);
}

void updateWindow(void)
{
	glfwPollEvents();
	glfwSwapBuffers(win);
}

void crash(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	glfwTerminate();	
	exit(EXIT_FAILURE);
}

void quit(void)
{
	canExit = 1;
}
