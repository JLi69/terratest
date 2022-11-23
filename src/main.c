#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include "gl-func.h"
#include "window-func.h"

int main()
{	
	GLFWwindow* win = initWindow();

	struct ShaderProgram program = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/simple.frag");
	glUseProgram(program.id);
	struct Buffers rectangle = createRectangleBuffer();
	bindBuffers(rectangle);

	while(!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		int w, h;
		glfwGetWindowSize(win, &w, &h);

		glUniform4f(getUniformLocation("uColor", &program), 0.0f, 1.0f, 0.0f, 1.0f);		
		glUniform2f(getUniformLocation("uWindowDimensions", &program), (float)w, (float)h);	
		glUniform2f(getUniformLocation("uDimensions", &program), (float)40.0f, (float)40.0f);	
		glUniform2f(getUniformLocation("uPixPos", &program), (float)200.0f, (float)200.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniform4f(getUniformLocation("uColor", &program), 1.0f, 0.0f, 0.0f, 1.0f);		
		glUniform2f(getUniformLocation("uDimensions", &program), (float)80.0f, (float)40.0f);	
		glUniform2f(getUniformLocation("uPixPos", &program), (float)-300.0f, (float)200.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUniform4f(getUniformLocation("uColor", &program), 0.0f, 0.0f, 1.0f, 1.0f);		
		glUniform2f(getUniformLocation("uDimensions", &program), (float)80.0f, (float)40.0f);	
		glUniform2f(getUniformLocation("uPixPos", &program), (float)-w / 2.0f, (float)-h / 2.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		outputGLErrors();
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	//Clean up
	cleanupProgram(&program);
	cleanupBuffer(rectangle);
	glfwTerminate();
}
