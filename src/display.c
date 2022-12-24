#include "gl-func.h"
#include "draw.h"
#include "game.h"
#include "window-func.h"
#include "world.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>
#include <stdio.h>

static struct ShaderProgram shaders[MAX_SHADERS];
static struct Buffers buffers[MAX_BUFFERS];
static unsigned int textures[MAX_TEXTURES];

void initGL(void)
{
	//Set up shaders
	shaders[0] = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/texture.frag");
	shaders[1] = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/outline.frag");

	//Set up vertex buffers
	buffers[0] = createRectangleBuffer();

	//Set up textures
	textures[0] = loadTexture("res/textures/player.png");
	textures[1] = loadTexture("res/textures/tiles.png");	
	textures[2] = loadTexture("res/textures/icons.png");
	textures[3] = loadTexture("res/textures/liquids.png");

	useShader(&shaders[0]);	
	bindBuffers(buffers[0]);

	//Turn on transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void display(struct World world, struct Sprite player)
{		
	clear();	

	setTransparency(1.0f);
	useShader(&shaders[0]);		
	updateActiveShaderWindowSize();
	//Draw background
	turnOffTexture();
	setRectColor(64.0f, 120.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 0.0f);	
	setRectSize(10000.0f, 10000.0f);	
	drawRect();		
	
	turnOnTexture();
	bindTexture(textures[1], GL_TEXTURE0);
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);		
	setTexOffset(0.0f, 0.0f);
	//Draw blocks
	setRectSize(BLOCK_SIZE, BLOCK_SIZE);
	struct Vector2D camPos = createVector(player.hitbox.position.x, player.hitbox.position.y);
	setBrightness(0.6f);
	drawSpriteTree(world.backgroundBlocks, camPos);	
	setBrightness(1.0f);
	drawSpriteTree(world.transparentBlocks, camPos);		

	bindTexture(textures[0], GL_TEXTURE0);
	//Draw player	
	flip(player.flipped);
	setTexFrac(1.0f / 16.0f, 1.0f);
	setTexSize(256.0f, 32.0f);
	setTexOffset((float)player.animationFrame * 1.0f / 16.0f, 0.0f);
	setRectSize(player.hitbox.dimensions.w, player.hitbox.dimensions.h);	
	setRectPos(0.0f, 0.0f);	
	drawRect();	
	turnOffFlip();

	setRectSize(BLOCK_SIZE, BLOCK_SIZE);	
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);		
	setTexOffset(0.0f, 0.0f);
	//Draw liquid blocks			
	//Draw blocks	
	bindTexture(textures[1], GL_TEXTURE0);
	drawSpriteTree(world.solidBlocks, camPos);		
	//Draw liquid blocks			
	bindTexture(textures[3], GL_TEXTURE0);
	drawLiquid(world.liquidBlocks, camPos, 32, 20, world.solidBlocks, world.blockArea);

	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);
	//Highlight block
	if(!isPaused())
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);
	
		useShader(&shaders[1]);						
		updateActiveShaderWindowSize();
		turnOffTexture();	
		setRectColor(255.0f, 255.0f, 255.0f, 255.0f);
		setRectPos(roundf((cursorX + camPos.x) / BLOCK_SIZE) * BLOCK_SIZE - camPos.x, roundf((cursorY + camPos.y) / BLOCK_SIZE) * BLOCK_SIZE - camPos.y);	
		setRectSize(BLOCK_SIZE, BLOCK_SIZE);	
		drawRect();

		//Draw cursor	
		bindTexture(textures[2], GL_TEXTURE0);	
		useShader(&shaders[0]);
		setRectPos(cursorX, cursorY);		
		setTexOffset(0.0f, 0.0f);
		setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
		setRectSize(CURSOR_SIZE, CURSOR_SIZE);	
		drawRect();	
	}

	//Display quadtree
#ifdef DEV_VERSION
	if(isPressed(GLFW_KEY_F2))
	{
		useShader(&shaders[1]);	
		setRectColor(0.0f, 255.0f, 0.0f, 255.0f);
		turnOffTexture();
		updateActiveShaderWindowSize();
		drawQTreeOutline(world.solidBlocks, 0.0f, 0.0f, 512.0f, 512.0f);
	}
#endif
}

void cleanup(void)
{
	for(int i = 0; i < MAX_SHADERS; i++)
		cleanupProgram(&shaders[i]);
	for(int i = 0; i < MAX_BUFFERS; i++)
		cleanupBuffer(buffers[i]);	
	glfwTerminate();
}
