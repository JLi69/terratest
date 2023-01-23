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
#include <stdlib.h>
#include "crafting.h"
#include "menu.h"

#if defined(__linux__)
#include <unistd.h>
#elif defined(WIN32) || defined(__MINGW64__)
#include <io.h>
#define F_OK 0
#define access _access
#endif

//Todo: move these to a struct (GLVars or something like that)
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
	textures[3] = loadTexture("res/textures/cloud.png");
	textures[4] = loadTexture("res/textures/items.png");
	textures[5] = loadTexture("res/textures/enemy1x1.png");
	textures[6] = loadTexture("res/textures/enemy1x2.png");
	textures[7] = loadTexture("res/textures/boss.png");

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
	setTexFrac(1.0f, 1.0f);
	setTexSize(16.0f, 16.0f);		
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
		setTexOffset(
				roundf(getBlockBreakTimer() * 4.0f / timeToBreakBlock(getSelectedBlock(), player.inventory.slots[player.inventory.selected].item)) / 16.0f + 1.0f / 16.0f,
				0.0f);
		setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
		setRectPos(roundf((cursorX + camPos.x) / BLOCK_SIZE) * BLOCK_SIZE - camPos.x, roundf((cursorY + camPos.y) / BLOCK_SIZE) * BLOCK_SIZE - camPos.y);	
		setRectSize(BLOCK_SIZE, BLOCK_SIZE);	
		drawRect();
	}	

	if(player.health > 0)
	{
		bindTexture(textures[0], GL_TEXTURE0);
		//Draw player	
		flip(player.playerSpr.flipped);
		setTexFrac(1.0f / 16.0f, 1.0f);
		setTexSize(256.0f, 32.0f);
		setTexOffset((float)player.playerSpr.animationFrame * 1.0f / 16.0f, 0.0f);
		setRectSize(player.playerSpr.hitbox.dimensions.w, player.playerSpr.hitbox.dimensions.h);	
		setRectPos(0.0f, 0.0f);	
		drawRect();	
	}

	//Draw items
	setRotationRad((-fabs(player.useItemTimer - USE_ANIMATION_LENGTH / 2.0f) + USE_ANIMATION_LENGTH / 2.0f) * 3.14159f * 1.0f / USE_ANIMATION_LENGTH);
	setRectSize(ITEM_SIZE * 1.5f, ITEM_SIZE * 1.5f);
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
	setRotationRad(0.0f);

	turnOffFlip();
	
	setRectSize(ITEM_SIZE, ITEM_SIZE);
	if(player.health > 0)
		drawItems(world, camPos, 32, 20);

	//Draw enemies
	setTransparency(1.0f);
	bindTexture(textures[5], GL_TEXTURE0);
	struct IntVec indices = createVec();		
	searchInRect(world.enemies, 
				 newpt(player.playerSpr.hitbox.position.x - BLOCK_SIZE * 32.0f, player.playerSpr.hitbox.position.y - BLOCK_SIZE * 20.0f),
				 newpt(player.playerSpr.hitbox.position.x + BLOCK_SIZE * 32.0f, player.playerSpr.hitbox.position.y + BLOCK_SIZE * 20.0f), &indices, ROOT);	
	//Draw enemies
	for(int i = 0; i < indices.sz; i++)
	{
		if(world.enemies->enemyArr[indices.values[i]].spr.type < ZOMBIE)
			drawEnemy1x1(world.enemies->enemyArr[indices.values[i]], camPos);
	}
	bindTexture(textures[6], GL_TEXTURE0);
	setTexFrac(1.0f / 16.0f, 1.0f / 8.0f);
	for(int i = 0; i < indices.sz; i++)
	{
		if(world.enemies->enemyArr[indices.values[i]].spr.type >= ZOMBIE)
			drawEnemy1x2(world.enemies->enemyArr[indices.values[i]], camPos);
	}
	flip(0); //Turn off flip
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);

	free(indices.values);

	//Draw boss
	if(world.boss.phase == 0)
	{
		//Intro text
		bindTexture(textures[2], GL_TEXTURE0);
		if(world.boss.timer < 2.5f) drawString("Who dares summon me?", world.boss.spr.hitbox.position.x - camPos.x, world.boss.spr.hitbox.position.y - camPos.y + 128.0f, 32.0f);	
		else if(world.boss.timer < 5.0f) drawString("Prepare to die!", world.boss.spr.hitbox.position.x - camPos.x, world.boss.spr.hitbox.position.y - camPos.y + 128.0f, 32.0f);
	}
	if(world.boss.phase >= 0)
	{
		//Draw boss health bar
		if(world.boss.phase > 0)
		{
			bindTexture(textures[2], GL_TEXTURE0);
			drawString("Skullface, Eater of Worlds", world.boss.spr.hitbox.position.x - camPos.x, world.boss.spr.hitbox.position.y - camPos.y + 128.0f, 16.0f);
		
			turnOffTexture();
			setRectColor(255.0f, 0.0f, 0.0f, 255.0f);
			setRectSize(256.0f, 16.0f);
			setRectPos(world.boss.spr.hitbox.position.x - camPos.x, world.boss.spr.hitbox.position.y - camPos.y + 96.0f);
			drawRect();
			setRectColor(0.0f, 255.0f, 0.0f, 255.0f);
			setRectSize(254.0f * (float)world.boss.health / (float)world.boss.maxHealth, 14.0f);
			setRectPos(world.boss.spr.hitbox.position.x - camPos.x - 254.0f * (0.5f - 0.5f * (float)world.boss.health / (float)world.boss.maxHealth), world.boss.spr.hitbox.position.y - camPos.y + 96.0f);
			drawRect();
			turnOnTexture();
		}	

		setTexFrac(1.0f / 8.0f, 1.0f);
		setTexSize(256.0f, 32.0f);
		bindTexture(textures[7], GL_TEXTURE0);
		drawBoss(world.boss, camPos);
		setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
		setTexSize(256.0f, 256.0f);
	}

	//Draw liquid blocks	
	setRectSize(BLOCK_SIZE, BLOCK_SIZE);	
	bindTexture(textures[1], GL_TEXTURE0);
	drawLiquids(world.blocks, camPos, 32, 20, world.blockArea, world.worldBoundingRect, 1.0f);

	if(!isPaused() && player.health > 0)
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

	setTransparency(0.7f);
	bindTexture(textures[2], GL_TEXTURE0);
	//Display health and breath bars
	if(player.health > 0)
	{
		setRectSize(ICON_SIZE, ICON_SIZE);
		setTexOffset(9.0f / 16.0f, 0.0f);
		if(player.health <= 0)
			setTexOffset(10.0f / 16.0f, 0.0f);
		for(int i = 0; i < player.maxHealth; i++)
		{
			if(i == player.health)
				setTexOffset(10.0f / 16.0f, 0.0f);
			setRectPos(i * ICON_SIZE - ICON_SIZE * 0.5f * (player.maxHealth), player.playerSpr.hitbox.dimensions.h / 2.0f + ICON_SIZE * 0.5f);
			drawRect();
		}
		if(player.breath < player.maxBreath)
		{
			setTexOffset(11.0f / 16.0f, 0.0f);
			for(int i = 0; i < (int)roundf(player.breath * BUBBLE_COUNT / BREATH); i++)
			{
				setRectPos(i * ICON_SIZE - ICON_SIZE * 0.5f * (int)roundf(player.breath * BUBBLE_COUNT / BREATH), player.playerSpr.hitbox.dimensions.h / 2.0f + ICON_SIZE * 2.0f);
				drawRect();
			}
		}
	}
	setTransparency(1.0f);

	//Crafting recipes
	if(craftingMenuShown())
	{
		displayCraftingRecipesDecorations(getMenuBegin(), getMenuEnd(), getMenuSelection(), -winWidth / 2.0f + (4.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (24.0f + 48.0f), 0.0f, 48.0f, 24.0f, 16.0f);
		bindTexture(textures[4], GL_TEXTURE0);	
		displayCraftingRecipesIcons(getMenuBegin(), getMenuEnd(), -winWidth / 2.0f + (4.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (24.0f + 48.0f), 0.0f, 48.0f, 24.0f, 16.0f);
		bindTexture(textures[2], GL_TEXTURE0);
		displayCraftingRecipesNumbers(getMenuBegin(), getMenuEnd(), -winWidth / 2.0f + (4.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (24.0f + 48.0f) + 24.0f, -24.0f, 48.0f, 24.0f, 16.0f, 16.0f);
	}
	
	//Inventory
	setTexSize(256.0f, 256.0f);		
	bindTexture(textures[4], GL_TEXTURE0);	
	displayInventoryItemIcons(player.inventory, -(float)winWidth / 2.0f + 32.0f, (float)winHeight / 2.0f - 32.0f, 28.0f, 20.0f);		
	bindTexture(textures[2], GL_TEXTURE0);	
	displayInventoryOutline(player.inventory, -(float)winWidth / 2.0f + 32.0f, (float)winHeight / 2.0f - 32.0f, 48.0f, 0.0f);
	displayInventoryNumbers(player.inventory, -(float)winWidth / 2.0f + 32.0f, (float)winHeight / 2.0f - 32.0f - 8.0f, 16.0f, 48.0f);

	//Coordinates
	float firstNumberEnd = drawInteger((int)(player.playerSpr.hitbox.position.x / BLOCK_SIZE), winWidth / 2.0f - 256.0f, winHeight / 2.0f - 48.0f, 32.0f);
	drawInteger((int)(player.playerSpr.hitbox.position.y / BLOCK_SIZE), firstNumberEnd + 32.0f, winHeight / 2.0f - 48.0f, 32.0f);
	drawString(",", firstNumberEnd - 32.0f, winHeight / 2.0f - 48.0f, 32.0f);	


	bindTexture(textures[2], GL_TEXTURE0);		

	//If there is a monster wave incoming, draw text onto screen
	if((world.moonPhase > 0.7f && world.moonPhase < 0.8f) &&
		(world.dayCycle > 0.4f || world.dayCycle < 0.6f))
		drawString("Monster Wave Incoming!", 0.0f, winHeight / 2.0f - 128.0f, 32.0f);

	if((player.damageCooldown > 0.0f && player.damageTaken > 0) ||
		player.health <= 0)
	{
		turnOffTexture();
		setRectPos(0.0f, 0.0f);
		setRectSize(1920.0f, 1080.0f);
		float alpha = (player.damageCooldown / DAMAGE_COOLDOWN * 128.0f * (1.0f - (float)player.health / (float)player.maxHealth));
		if(alpha > 128.0f)
			alpha = 128.0f;
		setRectColor(255.0f, 0.0f, 0.0f, alpha);
		drawRect();
		turnOnTexture();
	}

	//Menus
	if(player.health <= 0)
		drawMenu(RESPAWN);
	//Pause menu	
	if(isPaused())
	{
		turnOffTexture();
		setRectPos(0.0f, 0.0f);
		setRectSize(1920.0f, 1080.0f);
		setRectColor(64.0f, 64.0f, 64.0f, 192.0f);
		drawRect();
		turnOnTexture();

		drawMenu(PAUSED);
	}

	if(!isPaused())
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);

		//Draw cursor	
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

void displayMainMenu(float secondsPerFrame)
{
	//Fun falling block animation
	static struct Sprite fallingblocks[256];
	static int fallingblocksInit = 0;
	if(!fallingblocksInit)
	{
		for(int i = 0; i < 256; i++)
		{
			fallingblocks[i] = createSpriteWithType(
									createRect(
										(float)rand() / (float)RAND_MAX * 1920.0f - 960.0f,
										(float)rand() / (float)RAND_MAX * (1080.0f) + 540.0f,
										BLOCK_SIZE, BLOCK_SIZE),
									rand() % (SLIME_BLOCK) + 1); 
		}
		fallingblocksInit = 1;
	}

	clear();

	useShader(&shaders[0]);
	setTexSize(256.0f, 256.0f);
	setTexFrac(1.0f / 16.0f, 1.0f / 16.0f);
	updateActiveShaderWindowSize();	

	turnOffTexture();
	setRectPos(0.0f, 0.0f);
	setRectSize(1920.0f, 1080.0f);	
	setRectColor(64.0f, 120.0f, 255.0f, 255.0f);	
	drawRect();
	turnOnTexture();

	//Falling block animation	
	bindTexture(textures[1], GL_TEXTURE0);
	for(int i = 0; i < 256; i++)
	{
		setRectPos(fallingblocks[i].hitbox.position.x, fallingblocks[i].hitbox.position.y);
		setRectSize(fallingblocks[i].hitbox.dimensions.w, fallingblocks[i].hitbox.dimensions.h);
		setTexOffset(1.0f / 16.0f * (float)((fallingblocks[i].type - 1) % 16),
					 1.0f / 16.0f * (float)((fallingblocks[i].type - 1) / 16));
		drawRect();
		fallingblocks[i].hitbox.position.y -= 8.0f * BLOCK_SIZE * secondsPerFrame;
	
		if(fallingblocks[i].hitbox.position.y < -540.0f - BLOCK_SIZE / 2.0f)
		{
			fallingblocks[i] = createSpriteWithType(
									createRect(
										(float)rand() / (float)RAND_MAX * 1920.0f - 960.0f,
										(float)rand() / (float)RAND_MAX * 128.0f + 540.0f + BLOCK_SIZE,
										BLOCK_SIZE, BLOCK_SIZE),
									rand() % (SLIME_BLOCK) + 1);
		}
	}
	bindTexture(textures[2], GL_TEXTURE0);
		
	drawMenu(MAIN);
}

void displaySaveMenu(const char **savepaths, int savecount, int perColumn, int selected)
{
	int columns = (int)ceilf((float)savecount / (float)perColumn);

	clear();
	useShader(&shaders[0]);
	updateActiveShaderWindowSize();

	turnOffTexture();
	setRectColor(64.0f, 120.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 0.0f);	
	setRectSize(1920.0f, 1080.0f);	
	drawRect();
	turnOnTexture();

	bindTexture(textures[2], GL_TEXTURE0);

	for(int i = 0; i < columns; i++)
	{
		for(int j = 0; j < perColumn; j++)
		{
			int ind = i * perColumn + j;
			if(ind >= savecount)
				continue;

			int saveExists = access(savepaths[ind], F_OK) == 0;

			struct MenuObj tempbutton;

			if(saveExists)
			{
				tempbutton = createMenuObj(savepaths[ind],
										   -(float)columns / 2.0f * 480.0f + i * 480.0f + 240.0f,
											(float)perColumn / 2.0f * 96.0f - j * 96.0f - 48.0f,
											48.0f);
			}
			else
			{
				tempbutton = createMenuObj("[EMPTY]",
										   -(float)columns / 2.0f * 480.0f + i * 480.0f + 240.0f,
											(float)perColumn / 2.0f * 96.0f - j * 96.0f - 48.0f,
											48.0f);
			}

			if(buttonHover(tempbutton) || ind == selected)
				setBrightness(0.5f);
			else
				setBrightness(1.0f);
		
			if(saveExists)
				drawString(savepaths[ind], -(float)columns / 2.0f * 480.0f + i * 480.0f + 240.0f, (float)perColumn / 2.0f * 96.0f - j * 96.0f - 48.0f, 48.0f);  
			else
				drawString("[EMPTY]", -(float)columns / 2.0f * 480.0f + i * 480.0f + 240.0f, (float)perColumn / 2.0f * 96.0f - j * 96.0f - 48.0f, 48.0f);  	
		}
	}

	//Cancel button
	struct MenuObj cancel = createMenuObj("Cancel",
							   256.0f,
							   (float)perColumn / 2.0f * 96.0f - perColumn * 96.0f - 48.0f - 96.0f,
							   32.0f);
	drawTextButton(cancel); 	

	//Play button
	struct MenuObj play = createMenuObj("Play",
							   0.0f,
							   (float)perColumn / 2.0f * 96.0f - perColumn * 96.0f - 48.0f - 96.0f,
							   32.0f);
	drawTextButton(play);

	struct MenuObj deleteButton = createMenuObj("Delete",
							   -256.0f,
							   (float)perColumn / 2.0f * 96.0f - perColumn * 96.0f - 48.0f - 96.0f,
							   32.0f);
	drawTextButton(deleteButton);

	setBrightness(1.0f);
}

void displayDeletePrompt()
{
	clear();
	updateActiveShaderWindowSize();
	
	turnOffTexture();
	setRectColor(64.0f, 120.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 0.0f);	
	setRectSize(1920.0f, 1080.0f);	
	drawRect();
	turnOnTexture();

	drawMenu(DELETE_WORLD_PROMPT);
}

void displayCreatePrompt(unsigned int seed)
{
	clear();
	updateActiveShaderWindowSize();
	
	turnOffTexture();
	setRectColor(64.0f, 120.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 0.0f);	
	setRectSize(1920.0f, 1080.0f);	
	drawRect();

	//Text box for world seed
	setRectColor(255.0f, 255.0f, 255.0f, 255.0f);	
	setRectPos(0.0f, 128.0f);	
	setRectSize(650.0f, 74.0f);
	drawRect();
	setRectColor(0.0f, 0.0f, 0.0f, 255.0f);	
	setRectSize(640.0f, 64.0f);
	drawRect();	

	turnOnTexture();

	//Blank seed
	if(seed == 0)
		drawString("seed (leave blank for random)", 0.0f, 128.0f, 16.0f);
	else
	{
		drawUnsignedInteger(seed, 0.0f, 128.0f, 32.0f);
	}

	drawMenu(CREATE_WORLD_PROMPT);
}

void displayFPS(int fps)
{
	bindTexture(textures[2], GL_TEXTURE0);
	int winWidth, winHeight;
	getWindowSize(&winWidth, &winHeight);
	float end = drawString("FPS:", winWidth / 2.0f - 256.0f + 16.0f, winHeight / 2.0f - 80.0f, 16.0f);
	drawInteger((int)fps, end, winHeight / 2.0f - 80.0f, 16.0f);
}
