#include "gl-func.h"
#include "draw.h"
#include <glad/glad.h>

void setRectPos(float x, float y)
{
	glUniform2f(getUniformLocation("uPixPos", getActiveShader()), x, y);
}

void setRectSize(float x, float y)
{
	glUniform2f(getUniformLocation("uDimensions", getActiveShader()), x, y);
}

void setRectColor(float r, float g, float b, float a)
{
	glUniform4f(getUniformLocation("uColor", getActiveShader()),
				r / 255.0f,
				g / 255.0f,
				b / 255.0f,
				a / 255.0f);
}

void drawRect(void)
{
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
