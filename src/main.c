#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>

#define DEFAULT_WIDTH 960
#define DEFAULT_HEIGHT 600

int main()
{
	//Initialize glfw
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize glfw!\n");
		return -1;
	}

	//Create window
	GLFWwindow* win = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Terraria Clone", NULL, NULL);
	if(!win)
	{
		fprintf(stderr, "Failed to create window!\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);

	//Initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to load glad!\n");
		glfwTerminate();
		return -1;
	}

	while(!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
		glfwSwapBuffers(win);	
	}

	glfwTerminate();
}
