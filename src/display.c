#include "gl-func.h"
#include "draw.h"
#include "game.h"
#include "window-func.h"
#include "world.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

static struct ShaderProgram shaders[MAX_SHADERS];
static struct Buffers buffers[MAX_BUFFERS];
static unsigned int textures[MAX_TEXTURES];

void initGL(void)
{
	//Set up shaders
	shaders[0] = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/texture.frag");

	//Set up vertex buffers
	buffers[0] = createRectangleBuffer();

	//Set up textures
	textures[0] = loadTexture("res/textures/player.png");
	textures[1] = loadTexture("res/textures/tiles.png");

	useShader(&shaders[0]);	
	bindBuffers(buffers[0]);

	//Turn on transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void display(struct World world, struct Sprite player)
{		
	clear();	
	updateActiveShaderWindowSize();

	//Draw background
	turnOffTexture();
	setRectColor(0.0f, 40.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 0.0f);	
	setRectSize(10000.0f, 10000.0f);	
	drawRect();

	bindTexture(textures[0], GL_TEXTURE0);
	//Draw player	
	flip(player.flipped);
	setTexFrac(1.0f / 16.0f, 1.0f);
	setTexSize(256.0f, 32.0f);
	setTexOffset((float)player.animationFrame * 1.0f / 16.0f, 0.0f);
	turnOnTexture();
	setRectSize(player.hitbox.dimensions.w, player.hitbox.dimensions.h);	
	setRectPos(0.0f, 0.0f);	
	drawRect();	
	turnOffFlip();

	bindTexture(textures[1], GL_TEXTURE0);
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);		
	setTexOffset(0.0f, 0.0f);
	//Draw blocks
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
