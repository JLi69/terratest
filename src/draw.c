#include "gl-func.h"
#include "draw.h"
#include <glad/glad.h>
#include <math.h>
#include <string.h>

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

float drawInteger(int value, float x, float y, float digitSz)
{
	setRectSize(digitSz, digitSz);
	
	if(value == 0)
	{
		setTexOffset(0.0f, 1.0f / 16.0f);
		setRectPos(x, y);
		drawRect();
		return x + digitSz * 2.0f;
	}

	float xOffset = 0.0f;	
	int digitCount = (int)log10((double)(value > 0 ? value : -value)) + 1;
	if(value < 0)
	{
		value *= -1;
		setTexOffset(1.0f / 16.0f * 10.0f, 1.0f / 16.0f);
		setRectPos(xOffset + x - (digitCount - 1) / 2.0f * digitSz, y);	
		drawRect();
		xOffset += digitSz;
	}	

	for(int i = digitCount - 1; i >= 0; i--)
	{
		int digit = value / (pow(10, i));
		value -= pow(10, i) * digit;
		setTexOffset(1.0f / 16.0f * digit, 1.0f / 16.0f);
		setRectPos(xOffset + x - (digitCount - 1) / 2.0f * digitSz, y);
		xOffset += digitSz;
		drawRect();
	}

	return x + xOffset + digitSz - (digitCount - 1) / 2.0f * digitSz;
}

float drawUnsignedInteger(unsigned int value, float x, float y, float digitSz)
{
	setRectSize(digitSz, digitSz);
	
	if(value == 0)
	{
		setTexOffset(0.0f, 1.0f / 16.0f);
		setRectPos(x, y);
		drawRect();
		return x + digitSz * 2.0f;
	}

	float xOffset = 0.0f;	
	int digitCount = (int)log10((double)(value > 0 ? value : -value)) + 1;	

	for(int i = digitCount - 1; i >= 0; i--)
	{
		int digit = value / (pow(10, i));
		value -= pow(10, i) * digit;
		setTexOffset(1.0f / 16.0f * digit, 1.0f / 16.0f);
		setRectPos(xOffset + x - (digitCount - 1) / 2.0f * digitSz, y);
		xOffset += digitSz;
		drawRect();
	}

	return x + xOffset + digitSz - (digitCount - 1) / 2.0f * digitSz;
}

int convertChar(char ch)
{
	if(ch >= 'a' && ch <= 'z')
		return ch - 'a' + 'A' - ' ';
	if(ch > 'z')
		return ch - 'z' + 'a' - ' ' - 1;
	return ch - ' ';
}

float drawString(const char *str, float x, float y, float charSz)
{	
	setRectSize(charSz, charSz);
	const char *ptr = str;

	int strSz = strlen(str);

	float xOffset = 0.0f;
	for(int i = 0; i < strSz; i++)
	{
		setRectPos(x + xOffset - (strSz - 1) / 2.0f * charSz, y);
		setTexOffset(1.0f / 16.0f * (convertChar(str[i]) % 16), 
					 1.0f / 16.0f * (convertChar(str[i]) / 16) + 2.0f / 16.0f); 
		drawRect();
		xOffset += charSz;
	}

	return x + xOffset - (strSz - 1) / 2.0f * charSz + charSz;
}
