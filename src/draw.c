#include "gl-func.h"
#include "draw.h"
#include <glad/glad.h>

void clear(void)
{	
	glClear(GL_COLOR_BUFFER_BIT);
}

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

void turnOnTexture(void)
{	
	glUniform1i(getUniformLocation("uUseTexture", getActiveShader()), 1);
}

void turnOffTexture(void)
{	
	glUniform1i(getUniformLocation("uUseTexture", getActiveShader()), 0);
}

void setTexFrac(float fracX, float fracY)
{	
	glUniform2f(getUniformLocation("uTexFrac", getActiveShader()), fracX, fracY);
}

void setTexOffset(float x, float y)
{	
	glUniform2f(getUniformLocation("uTexOffset", getActiveShader()), x, y);
}

void setTexSize(float texWidth, float texHeight)
{	
	glUniform2f(getUniformLocation("uTexSize", getActiveShader()), texWidth, texHeight);
}

void turnOnFlip(void)
{	
	glUniform1i(getUniformLocation("uFlip", getActiveShader()), 1);
}

void turnOffFlip(void)
{	
	glUniform1i(getUniformLocation("uFlip", getActiveShader()), 0);
}

void flip(int flipped)
{
	glUniform1i(getUniformLocation("uFlip", getActiveShader()), flipped);
}

void setTransparency(float a)
{	
	glUniform1f(getUniformLocation("uTransparency", getActiveShader()), a);
}

void setOutline(float width)
{
	glUniform1f(getUniformLocation("uOutline", getActiveShader()), width);
}

void setLevel(float level)
{
	glUniform1f(getUniformLocation("uLevel", getActiveShader()), level);
}

void setBrightness(float brightness)
{
	glUniform1f(getUniformLocation("uBrightness", getActiveShader()), brightness);
}

void setRayCount(float rays)
{	
	glUniform1f(getUniformLocation("uRays", getActiveShader()), rays);
}

void setPhase(float phase)
{
	glUniform1f(getUniformLocation("uPhase", getActiveShader()), phase);
}
