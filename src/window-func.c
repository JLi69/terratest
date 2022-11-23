#include "window-func.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
}

GLFWwindow* initWindow()
{
	//Initialize glfw
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize glfw!\n");
		exit(EXIT_FAILURE);
	}

	//Create window
	GLFWwindow* win = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "TerraTest", NULL, NULL);
	if(!win)
	{
		fprintf(stderr, "Failed to create window!\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(win);
	glfwSetWindowSizeCallback(win, handleWindowResize);

	//Initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to load glad!\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	return win;
}
