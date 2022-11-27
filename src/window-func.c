//GLFW website: https://glfw.org
//GLFW github: https://github.com/glfw/glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "window-func.h"
#include "gl-func.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
//Maximum number of keys that can be detected as pressed
#define MAX_KEY_PRESSED 4
#define MOUSE_BUTTON_COUNT 3
#define UNPRESSED -1

//if this is set to 1, quit
static int canExit = 0;
//The window
static GLFWwindow* win;
//Store the keys that are pressed
static int pressed[MAX_KEY_PRESSED];
static int mouse[MOUSE_BUTTON_COUNT];

//Handle resizing of the window
void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
}

//Handle key input
void handleKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		//Check all the keys to see if the key is already pressed
		for(int i = 0; i < MAX_KEY_PRESSED; i++)
			if(pressed[i] == key)
				return;
		//Mark the key down as pressed
		for(int i = 0; i < MAX_KEY_PRESSED; i++)
		{	
			if(pressed[i] == UNPRESSED)
			{
				pressed[i] = key;
				return;
			}
		}
	}
	else if(action == GLFW_RELEASE)
	{
		//Mark the key as unpressed
		for(int i = 0; i < MAX_KEY_PRESSED; i++)
		{	
			if(pressed[i] == key)
			{
				pressed[i] = UNPRESSED;
				return;
			}
		}
	}
}

void handleMouseInput(GLFWwindow *window, int button, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		for(int i = 0; i < MOUSE_BUTTON_COUNT; i++)
			if(mouse[i] == button)
				return;
		for(int i = 0; i < MOUSE_BUTTON_COUNT; i++)
		{
			if(mouse[i] == UNPRESSED)
			{
				mouse[i] = button;
				return;
			}
		}
	}
	else if(action == GLFW_RELEASE)
	{
		for(int i = 0; i < MOUSE_BUTTON_COUNT; i++)
		{
			if(mouse[i] == button)
			{
				mouse[i] = UNPRESSED;
				return;
			}
		}
	}
}

int isPressed(int key)
{
	for(int i = 0; i < MAX_KEY_PRESSED; i++)
		if(key == pressed[i])
			return 1;
	return 0;
}

int mouseButtonHeld(int button)
{
	for(int i = 0; i < MOUSE_BUTTON_COUNT; i++)
		if(button == mouse[i])
			return 1;
	return 0;
}

void initWindow(void)
{
	for(int i = 0; i < MAX_KEY_PRESSED; i++)
		pressed[i] = UNPRESSED;
	for(int i = 0; i < MOUSE_BUTTON_COUNT; i++)
		mouse[i] = UNPRESSED;

	//Initialize glfw
	if(!glfwInit())
		crash("Failed to initialize glfw!\n");

	//Create window
	win = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "TerraTest", NULL, NULL);
	if(!win)
		crash("Failed to create window!");
	glfwMakeContextCurrent(win);

	glfwSetWindowSizeCallback(win, handleWindowResize);
	glfwSetKeyCallback(win, handleKeyInput);
	glfwSetMouseButtonCallback(win, handleMouseInput);

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
	outputGLErrors();
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

void getWindowSize(int *w, int *h)
{
	glfwGetWindowSize(win, w, h);
}

void getCursorPos(double *x, double *y)
{
	int winWidth, winHeight;
	glfwGetWindowSize(win, &winWidth, &winHeight);
	glfwGetCursorPos(win, x, y);

	*y = (double)winHeight - *y;
	*x -= winWidth / 2.0f;
	*y -= winHeight / 2.0f;
}
