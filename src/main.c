#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include "gl-func.h"
#include "window-func.h"
#include "draw.h"

int main(void)
{	
	initWindow();

	struct ShaderProgram program = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/simple.frag");
	useShader(&program);	
	struct Buffers rectangle = createRectangleBuffer();
	bindBuffers(rectangle);

	while(!canQuit())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		updateActiveShaderWindowSize();
		setRectColor(255.0f, 255.0f, 0.0f, 255.0f);
		setRectSize(20.0f, 40.0f);	
		setRectPos(0.0f, 0.0f);	
		drawRect();

		outputGLErrors();
		updateWindow();		
	}

	//Clean up
	cleanupProgram(&program);
	cleanupBuffer(rectangle);
	glfwTerminate();
}
