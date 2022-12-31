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
#include "crafting.h"

static struct ShaderProgram shaders[MAX_SHADERS];
static struct Buffers buffers[MAX_BUFFERS];
static unsigned int textures[MAX_TEXTURES];

void initGL(void)
{
	//Set up shaders
	shaders[0] = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/texture.frag");
	shaders[1] = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/outline.frag");
	shaders[2] = createShaderProgram("res/shaders/transform-vert.vert", "res/shaders/sun.frag");

	//Set up vertex buffers
	buffers[0] = createRectangleBuffer();

	//Set up textures
	textures[0] = loadTexture("res/textures/player.png");
	textures[1] = loadTexture("res/textures/tiles.png");	
	textures[2] = loadTexture("res/textures/icons.png");
	textures[3] = loadTexture("res/textures/sky.png");
	textures[4] = loadTexture("res/textures/items.png");

	useShader(&shaders[0]);	
	bindBuffers(buffers[0]);

	//Turn on transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//0.0 = midnight, 1.0 = afternoon
void background(float dayCycleTime, float offsetx, float offsety, struct World world)
{
	//Draw background
	setBrightness((dayCycleTime - 1.0f) * (dayCycleTime) * -4.0f);
	turnOffTexture();
	setRectColor(64.0f, 120.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 0.0f);	
	setRectSize(10000.0f, 10000.0f);	
	drawRect();	

	setBrightness(1.0f);
	useShader(&shaders[2]);
	//Draw the sun	
	if(dayCycleTime > 0.25f && dayCycleTime < 0.75f)
	{
		setRayCount(12.0f);
		setPhase(2.0f);
		updateActiveShaderWindowSize();	
		turnOffTexture();
		setRectColor(255.0f, 255.0f, 0.0f, 255.0f);	
		setRectPos(-sinf(dayCycleTime * 2.0f * 3.14159f) * 1024.0f,
					-cosf(dayCycleTime * 2.0f * 3.14159f) * 960.0f - 500.0f);	
		setRectSize(128.0f, 128.0f);	
		drawRect();
	}
	//Draw the moon
	if(dayCycleTime < 0.25f || dayCycleTime > 0.75f)
	{
		setRayCount(0.0f);
		setPhase(world.moonPhase);
		updateActiveShaderWindowSize();	
		turnOffTexture();
		setRectColor(255.0f, 255.0f, 255.0f, 255.0f);	
		setRectPos(sinf(dayCycleTime * 2.0f * 3.14159f) * 1024.0f,
					cosf(dayCycleTime * 2.0f * 3.14159f) * 960.0f - 500.0f);	
		setRectSize(128.0f, 128.0f);	
		drawRect();
	}

	useShader(&shaders[0]);
	turnOnTexture();

	//Draw clouds		
	bindTexture(textures[3], GL_TEXTURE0);	
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);		
	setTexOffset(0.0f, 0.0f);
	setTransparency(0.8f);
	for(int i = 0; i < MAX_CLOUD; i++)
	{
		setRectSize(world.clouds[i].hitbox.dimensions.w, world.clouds[i].hitbox.dimensions.h);
		setRectPos(world.clouds[i].hitbox.position.x,
				   world.clouds[i].hitbox.position.y - offsety / BLOCK_SIZE * 4.0f + 960.0f);
		drawRect();
	}
	setTransparency(1.0f);
}

void display(struct World world, struct Player player)
{		
	clear();	

	setTransparency(1.0f);
	useShader(&shaders[0]);		
	updateActiveShaderWindowSize();	

	background(world.dayCycle, player.playerSpr.hitbox.position.x, player.playerSpr.hitbox.position.y, world);

	turnOnTexture();
	bindTexture(textures[1], GL_TEXTURE0);
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);		
	setTexOffset(0.0f, 0.0f);
	//Draw blocks
	setRectSize(BLOCK_SIZE, BLOCK_SIZE);
	struct Vector2D camPos = createVector(player.playerSpr.hitbox.position.x, player.playerSpr.hitbox.position.y);
	drawBlocks(world.backgroundBlocks, camPos, 32, 20, world.blockArea, world.worldBoundingRect, 0.6f);	
	drawBlocks(world.transparentBlocks, camPos, 32, 20, world.blockArea, world.worldBoundingRect, 1.0f);		

	setRectSize(BLOCK_SIZE, BLOCK_SIZE);	
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);		
	setTexOffset(0.0f, 0.0f);
	//Draw solid blocks	
	bindTexture(textures[1], GL_TEXTURE0);
	drawBlocks(world.blocks, camPos, 32, 20, world.blockArea, world.worldBoundingRect, 1.0f);		

	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);
	//Highlight block
	if(!isPaused())
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);

		//Break block animation	
		bindTexture(textures[2], GL_TEXTURE0);	
		setTexOffset(roundf(getBlockBreakTimer() * 4.0f) / 16.0f + 1.0f / 16.0f, 0.0f);
		setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
		setRectPos(roundf((cursorX + camPos.x) / BLOCK_SIZE) * BLOCK_SIZE - camPos.x, roundf((cursorY + camPos.y) / BLOCK_SIZE) * BLOCK_SIZE - camPos.y);	
		setRectSize(BLOCK_SIZE, BLOCK_SIZE);	
		drawRect();
	}	

	bindTexture(textures[0], GL_TEXTURE0);
	//Draw player	
	flip(player.playerSpr.flipped);
	setTexFrac(1.0f / 16.0f, 1.0f);
	setTexSize(256.0f, 32.0f);
	setTexOffset((float)player.playerSpr.animationFrame * 1.0f / 16.0f, 0.0f);
	setRectSize(player.playerSpr.hitbox.dimensions.w, player.playerSpr.hitbox.dimensions.h);	
	setRectPos(0.0f, 0.0f);	
	drawRect();	

	//Draw items
	setRectSize(ITEM_SIZE, ITEM_SIZE);
	bindTexture(textures[4], GL_TEXTURE0);
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	setTexSize(256.0f, 256.0f);		
	setTexOffset(0.0f, 0.0f);

	switch(player.playerSpr.animationState)
	{
	case IDLE:
		setRectPos(player.playerSpr.flipped ? 8.0f : -8.0f, -16.0f);
		break;
	case FALLING:
		setRectPos(0.0f, 24.0f);
		break;
	case WALKING:
		setRectPos(player.playerSpr.flipped ? -4.0f : 4.0f, -16.0f);
		break;
	}
	//Draw currently held item in player's hand
	setTexOffset(1.0f / 16.0f * (float)((player.inventory.slots[player.inventory.selected].item - 1) % 16),
				1.0f / 16.0f * (float)((player.inventory.slots[player.inventory.selected].item - 1) / 16));	
	if(player.inventory.slots[player.inventory.selected].item != NOTHING)
		drawRect();

	turnOffFlip();
	drawItems(world, camPos, 32, 20);

	//Draw liquid blocks	
	setRectSize(BLOCK_SIZE, BLOCK_SIZE);	
	bindTexture(textures[1], GL_TEXTURE0);
	drawLiquids(world.blocks, camPos, 32, 20, world.blockArea, world.worldBoundingRect, 1.0f);

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
		useShader(&shaders[0]);
	}

	int winWidth, winHeight;
	getWindowSize(&winWidth, &winHeight);

	//Crafting recipes
	if(craftingMenuShown())
	{
		bindTexture(textures[2], GL_TEXTURE0);
		displayCraftingRecipesDecorations(0, 8, getMenuSelection(), -winWidth / 2.0f + (4.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (24.0f + 48.0f), 0.0f, 48.0f, 24.0f, 16.0f);
		bindTexture(textures[4], GL_TEXTURE0);	
		displayCraftingRecipesIcons(0, 8, -winWidth / 2.0f + (4.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (24.0f + 48.0f), 0.0f, 48.0f, 24.0f, 16.0f);
		bindTexture(textures[2], GL_TEXTURE0);
		displayCraftingRecipesNumbers(0, 8, -winWidth / 2.0f + (4.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (24.0f + 48.0f) + 24.0f, -24.0f, 48.0f, 24.0f, 16.0f, 16.0f);
	}
	
	//Inventory
	setTexSize(256.0f, 256.0f);		
	bindTexture(textures[4], GL_TEXTURE0);	
	displayInventoryItemIcons(player.inventory, -(float)winWidth / 2.0f + 32.0f, (float)winHeight / 2.0f - 32.0f, 28.0f, 20.0f);		
	bindTexture(textures[2], GL_TEXTURE0);	
	displayInventoryOutline(player.inventory, -(float)winWidth / 2.0f + 32.0f, (float)winHeight / 2.0f - 32.0f, 48.0f, 0.0f);
	displayInventoryNumbers(player.inventory, -(float)winWidth / 2.0f + 32.0f, (float)winHeight / 2.0f - 32.0f - 8.0f, 16.0f, 48.0f);

	//Coordinates
	float firstNumberEnd = drawInteger((int)(player.playerSpr.hitbox.position.x / BLOCK_SIZE), winWidth / 2.0f - 256.0f, -winHeight / 2.0f + 48.0f, 32.0f);
	drawInteger((int)(player.playerSpr.hitbox.position.y / BLOCK_SIZE), firstNumberEnd + 32.0f, -winHeight / 2.0f + 48.0f, 32.0f);
	drawString(",", firstNumberEnd - 32.0f, -winHeight / 2.0f + 48.0f, 32.0f);	

	if(!isPaused())
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);

		//Draw cursor	
		bindTexture(textures[2], GL_TEXTURE0);	
		setRectPos(cursorX, cursorY);		
		setTexOffset(0.0f, 0.0f);
		setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
		setRectSize(CURSOR_SIZE, CURSOR_SIZE);	
		drawRect();
	}
}

void cleanup(void)
{
	for(int i = 0; i < MAX_SHADERS; i++)
		cleanupProgram(&shaders[i]);
	for(int i = 0; i < MAX_BUFFERS; i++)
		cleanupBuffer(buffers[i]);	
	glfwTerminate();
}
