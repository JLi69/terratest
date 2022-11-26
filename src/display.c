#include "gl-func.h"
#include "draw.h"
#include "game.h"
#include "window-func.h"
#include "world.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

static struct ShaderProgram shaders[MAX_SHADERS];
static struct Buffers buffers[MAX_BUFFERS];

void initGL(void)
{
	//Set up shaders
	shaders[0] = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/simple.frag");

	//Set up vertex buffers
	buffers[0] = createRectangleBuffer();

	useShader(&shaders[0]);	
	bindBuffers(buffers[0]);
}

void display(struct World world, struct Sprite player)
{		
	clear();	
	updateActiveShaderWindowSize();

	//Draw background
	setRectColor(0.0f, 40.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 0.0f);	
	setRectSize(10000.0f, 10000.0f);	
	drawRect();

	//Draw player
	setRectColor(255.0f, 255.0f, 0.0f, 255.0f);
	setRectSize(player.hitbox.dimensions.w, player.hitbox.dimensions.h);	
	setRectPos(0.0f, 0.0f);	
	drawRect();	

	//Draw blocks	
	setRectColor(0.0f, 255.0f, 0.0f, 255.0f);
	setRectSize(BLOCK_SIZE, BLOCK_SIZE);
	struct Vector2D camPos = createVector(player.hitbox.position.x, player.hitbox.position.y);
	drawSpriteTree(world.blocks, camPos);
}

void cleanup(void)
{
	for(int i = 0; i < MAX_SHADERS; i++)
		cleanupProgram(&shaders[i]);
	for(int i = 0; i < MAX_BUFFERS; i++)
		cleanupBuffer(buffers[i]);	
	glfwTerminate();
}
