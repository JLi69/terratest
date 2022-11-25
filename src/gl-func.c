#include "gl-func.h"
#include "window-func.h"
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <string.h>

static struct ShaderProgram* active = NULL;

void readShaderFile(const char *path, char *output, unsigned int maxSize)
{
	FILE* shaderFile = fopen(path, "r");
	
	//Cannot open file
	if(!shaderFile)
	{
		fprintf(stderr, "Failed to open %s!\n", path);
		return;
	}

	//Read the file
	int ind = 0, ch;
	while((ch = fgetc(shaderFile)) != EOF && ind < maxSize)
		output[ind++] = ch;
	output[ind] = '\0';

	fclose(shaderFile);
}

void outputCompileErrors(unsigned int shader)
{	
	int compileStatus;
	//Check to see if vertex shader compiled
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if(!compileStatus)
	{
		char log[MAX_LOG_LEN + 1];
		int len;
		glGetShaderInfoLog(shader, MAX_LOG_LEN, &len, log);
		log[len] = '\0';

		//Output shader type
		int shaderType;
		glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
		switch(shaderType)
		{
		case GL_VERTEX_SHADER: fprintf(stderr, "Vertex shader failed to compile!\n"); break;
		case GL_FRAGMENT_SHADER: fprintf(stderr, "Fragment shader failed to compile!\n"); break;
		default: fprintf(stderr, "Shader failed to compile!\n");	
		}

		fprintf(stderr, "%s\n", log);
	}
}

struct ShaderProgram createShaderProgram(const char *vertpath, const char *fragpath)
{
	struct ShaderProgram program;
	program.id = glCreateProgram();
	
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER),
				 fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Read the shaders
	//1 extra character for space for the null terminator
	char vertSrc[MAX_SRC_LEN + 1],
		 fragSrc[MAX_SRC_LEN + 1];
	readShaderFile(vertpath, vertSrc, MAX_SRC_LEN);
	readShaderFile(fragpath, fragSrc, MAX_SRC_LEN);

	//Send the source code to the shaders
	const char *vertSrcBegin = &vertSrc[0],
			   *fragSrcBegin = &fragSrc[0];
	glShaderSource(vertShader, 1, &vertSrcBegin, NULL);
	glShaderSource(fragShader, 1, &fragSrcBegin, NULL);

	//Compile the shaders
	fprintf(stderr, "Compiling %s...\n", vertpath);
	glCompileShader(vertShader);	
	outputCompileErrors(vertShader);

	fprintf(stderr, "Compiling %s...\n", fragpath);
	glCompileShader(fragShader);	
	outputCompileErrors(fragShader);

	//Attach shaders
	glAttachShader(program.id, vertShader);
	glAttachShader(program.id, fragShader);
	glLinkProgram(program.id);
	
	//output link status
	int linked;
	glGetProgramiv(program.id, GL_LINK_STATUS, &linked);
	if(!linked)
	{
		fprintf(stderr, "Program failed to link!\n");	
	}

	glValidateProgram(program.id);

	//Clean up
	glDetachShader(program.id, vertShader);
	glDetachShader(program.id, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	program.uniforms = (struct Uniform*)malloc(sizeof(struct Uniform));
	program._maxUniformCount = 1;
	program._uniformCount = 0;

	return program;
}

void cleanupProgram(struct ShaderProgram *program)
{
	free(program->uniforms);
	program->uniforms = NULL;
	program->_uniformCount = 0;
	program->_maxUniformCount = 0;
	glDeleteProgram(program->id);
	program->id = EMPTY_SHADER;
}

void outputGLErrors(void)
{
	GLenum err;
	int errorCount = 0;	
	while((err = glGetError()) != GL_NO_ERROR)
	{
		fprintf(stderr, "OpenGL Error: %d\n", err);
		errorCount++;	
	}

	//if it's a dev version, kill the game when we get an OpenGL error
#ifdef DEV_VERSION
	if(errorCount > 0)
	{
		fprintf(stderr, "Found %d errors!\n", errorCount);
		crash("OpenGL Errors -- exiting");	
	}
#endif
}

int getUniformLocation(const char *uniformName, struct ShaderProgram *program)
{
	//Uniform name too long
	if(strlen(uniformName) > MAX_UNIFORM_LEN)
		return -1;

	if(program->uniforms == NULL || program->_maxUniformCount == 0)
	{
		program->uniforms = (struct Uniform*)malloc(sizeof(struct Uniform));
		program->_maxUniformCount = 1;

		//Create the uniform location entry
		program->uniforms[0].location = glGetUniformLocation(program->id, uniformName);
		strcpy(program->uniforms[0].name, uniformName); //Do not enter a name of length more than 128

		program->_uniformCount = 1;
	
		return program->uniforms[0].location;
	}

	//Not the most efficient way we can do this but
	//we shouldn't have too many uniforms so it should
	//not be a big deal for now
	//
	//TODO: if this turns out to be too slow, come up with
	//a hash function for uniform names and use a hash table instead
	for(int i = 0; i < program->_uniformCount; i++)
	{
		if(strcmp(uniformName, program->uniforms[i].name) == 0)
			return program->uniforms[i].location;
	}
	
	//Did not find uniform!
	//Still enough space left	
	if(program->_uniformCount < program->_maxUniformCount)
	{
		program->uniforms[program->_uniformCount++].location = glGetUniformLocation(program->id, uniformName);	
		strcpy(program->uniforms[program->_uniformCount - 1].name, uniformName); //Do not enter a name of length more than 128
		return program->uniforms[program->_uniformCount - 1].location;	
	}
	else if(program->_uniformCount >= program->_maxUniformCount)
	{
		//Temporary array	
		struct Uniform* temp = (struct Uniform*)malloc(sizeof(struct Uniform) * program->_uniformCount);
		for(int i = 0; i < program->_uniformCount; i++)
			temp[i] = program->uniforms[i];

		free(program->uniforms);
		program->_maxUniformCount *= 2;
		program->uniforms = (struct Uniform*)malloc(sizeof(struct Uniform) * program->_maxUniformCount);

		for(int i = 0; i < program->_uniformCount; i++)
			program->uniforms[i] = temp[i];
		//Create new uniform entry
		program->uniforms[program->_uniformCount++].location = glGetUniformLocation(program->id, uniformName);	
		strcpy(program->uniforms[program->_uniformCount - 1].name, uniformName); //Do not enter a name of length more than 128

		free(temp);
	}

	return -1;
}

struct Buffers createRectangleBuffer(void)
{
	struct Buffers buff;

	glGenVertexArrays(1, &buff.arr);
	glBindVertexArray(buff.arr);

	buff.bufferCount = 1;
	buff.buffers = (unsigned int*)malloc(sizeof(unsigned int) * buff.bufferCount);

	buff.types = (unsigned int*)malloc(sizeof(unsigned int) * buff.bufferCount); 
	buff.vertSize = (unsigned int*)malloc(sizeof(unsigned int) * buff.bufferCount); 
	buff.vertElementCount = (unsigned int*)malloc(sizeof(unsigned int) * buff.bufferCount); 
	
	glGenBuffers(buff.bufferCount, buff.buffers);

	//Set up rectangle vertices
	const float rectVerts[] = 
	{
		 1.0f,  1.0f,
		-1.0f,  1.0f,
		-1.0f, -1.0f,

		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};
	glGenBuffers(1, &buff.buffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buff.buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVerts), rectVerts, GL_STATIC_DRAW);	
	buff.types[0] = GL_FLOAT;
	buff.vertSize[0] = 2 * sizeof(float);
	buff.vertElementCount[0] = 2;

	return buff;
}

void bindBuffers(struct Buffers buff)
{
	glBindVertexArray(buff.arr);

	for(int i = 0; i < buff.bufferCount; i++)
	{
		glVertexAttribPointer(i, 
							  buff.vertElementCount[i],
							  buff.types[i],
							  GL_FALSE,
							  buff.vertSize[i],
							  NULL);
		glEnableVertexAttribArray(i);
	}
}

void cleanupBuffer(struct Buffers buff)
{
	glDeleteVertexArrays(1, &buff.arr);
	glDeleteBuffers(buff.bufferCount, buff.buffers);
	free(buff.buffers);
	free(buff.vertSize);
	free(buff.types);
	free(buff.vertElementCount);
}

void useShader(struct ShaderProgram *shaderPtr)
{
	active = shaderPtr;
	glUseProgram(active->id);
}

struct ShaderProgram* getActiveShader(void)
{
	return active;
}
