#include "game.h"
#include "world.h"
#include "window-func.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static float breakBlockTimer = 0.0f;

static const enum BlockType transparent[] = { LOG,
											  STUMP,
											  FLOWER,
											  TALL_GRASS,
											  VINES };

void initGame(struct World *world, struct Player *player)
{
	const float height = 128.0f;
	*world = generateWorld(time(0), height, 256.0f);
	revealVisible(world);

	player->playerSpr = createSprite(createRect(0.0f, 32.0f * 4.0f * height, 32.0f, 64.0f));
	player->playerSpr.animationState = IDLE;
	player->playerSpr.animating = 1;
	player->inventory = createInventory(16); //inventory of size 16

	struct Sprite collision;
	while(!blockCollisionSearch(player->playerSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collision))
		player->playerSpr.hitbox.position.y -= 32.0f;
	player->playerSpr.hitbox.position.y += 32.0f;
	player->playerSpr.canMove = 1;
}

void updateGameobjects(struct World *world, struct Player *player, float secondsPerFrame)
{
	struct Vector2D camPos = createVector(player->playerSpr.hitbox.position.x, player->playerSpr.hitbox.position.y);
	static float blockUpdateTimer = 0.0f;
	blockUpdateTimer += secondsPerFrame;

	struct Sprite collided;	

	//printf("%f %f\n", player->hitbox.position.x / BLOCK_SIZE, player->hitbox.position.y / BLOCK_SIZE);

	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, WATER))
	{
		player->playerSpr.vel.x *= 0.4f;
		if(player->playerSpr.vel.y > 0.0f)
			player->playerSpr.vel.y *= 0.6f;
	}
	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, LAVA))
	{	
		player->playerSpr.vel.x *= 0.2f;
		if(player->playerSpr.vel.y > 0.0f)
			player->playerSpr.vel.y *= 0.4f;	
	}

	//Move player in the x direction
	updateSpriteX(&player->playerSpr, secondsPerFrame);
	//Check for collision
	if(blockCollisionSearch(player->playerSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided))
	{				
		//Uncollide the player	
		if(player->playerSpr.vel.x != 0.0f)
		{
			if(player->playerSpr.hitbox.position.x >=
			   collided.hitbox.position.x + collided.hitbox.dimensions.w / 2.0f)
			{
				player->playerSpr.vel.x = 0.0f;
				player->playerSpr.hitbox.position.x =
					collided.hitbox.position.x +
					collided.hitbox.dimensions.w / 2.0f +
					player->playerSpr.hitbox.dimensions.w / 2.0f;	
			}	
			else if(player->playerSpr.hitbox.position.x <= 
					collided.hitbox.position.x - collided.hitbox.dimensions.x / 2.0f)
			{
				player->playerSpr.vel.x = 0.0f;
				player->playerSpr.hitbox.position.x =
					collided.hitbox.position.x -
					collided.hitbox.dimensions.w / 2.0f -
					player->playerSpr.hitbox.dimensions.w / 2.0f;	
			}	
		}
	}

	//Move player in y direction
	updateSpriteY(&player->playerSpr, secondsPerFrame);	
	//Check for collision	
	if(blockCollisionSearch(player->playerSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided))
	{		
		//Uncollide the player
		if(player->playerSpr.vel.y != 0.0f)
		{
			if(player->playerSpr.hitbox.position.y >=
			   collided.hitbox.position.y + collided.hitbox.dimensions.h / 2.0f)
			{
				player->playerSpr.vel.y = 0.0f;
				player->playerSpr.hitbox.position.y =
					collided.hitbox.position.y +
					collided.hitbox.dimensions.h / 2.0f +
					player->playerSpr.hitbox.dimensions.h / 2.0f;	
				//The player is supported by a block	
				player->playerSpr.falling = 0;	
			}	
			else if(player->playerSpr.hitbox.position.y <= 
					collided.hitbox.position.y - collided.hitbox.dimensions.h / 2.0f)
			{
				player->playerSpr.falling = 1;	
				player->playerSpr.vel.y = -0.5f;
				player->playerSpr.hitbox.position.y =
					collided.hitbox.position.y -
					collided.hitbox.dimensions.h / 2.0f -
					player->playerSpr.hitbox.dimensions.h / 2.0f;	
			}	
		}	
	}
	else
	{
		//The player is not supported by a block
		player->playerSpr.falling = 1;
		player->playerSpr.animationState = FALLING;	
	
		struct Sprite temp = createSprite(
							 createRect(player->playerSpr.hitbox.position.x, player->playerSpr.hitbox.position.y - 0.01f,
										player->playerSpr.hitbox.dimensions.w, player->playerSpr.hitbox.dimensions.h));
		struct Sprite tempCollison;	
		if(blockCollisionSearch(temp, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &tempCollison))
			player->playerSpr.animationState = IDLE;
	}

	//Move character with arrow keys
	if(isPressed(GLFW_KEY_A))
	{
		player->playerSpr.flipped = 1;
		player->playerSpr.vel.x = -PLAYER_SPEED;
		if(player->playerSpr.animationState == IDLE)
			player->playerSpr.animationState = WALKING;	
	}
	else if(isPressed(GLFW_KEY_D))
	{
		player->playerSpr.flipped = 0;	
		player->playerSpr.vel.x = PLAYER_SPEED;
		if(player->playerSpr.animationState == IDLE)
			player->playerSpr.animationState = WALKING;
	}
	else
	{
		player->playerSpr.vel.x = 0.0f;
		if(player->playerSpr.animationState == WALKING)
			player->playerSpr.animationState = IDLE;
	}
	
	//Check if player can jump
	if(!player->playerSpr.falling || 
	   touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, WATER) || 
	   touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, VINES))
		player->playerSpr.canJump = 1;
	else
		player->playerSpr.canJump = 0;

	//Jump
	if(player->playerSpr.canJump && isPressed(GLFW_KEY_SPACE))
		player->playerSpr.vel.y = JUMP_SPEED;

	//Update liquid blocks	
	if(blockUpdateTimer > 0.03f)
	{
		updateBlocks(world->blocks, camPos, blockUpdateTimer, 64, world->blockArea, world->worldBoundingRect);
		blockUpdateTimer = 0.0f;	
	}

	//Place blocks
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT))
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);	
		cursorX = roundf((cursorX + player->playerSpr.hitbox.position.x) / BLOCK_SIZE);	
		cursorY = roundf((cursorY + player->playerSpr.hitbox.position.y) / BLOCK_SIZE);	

		struct Sprite temp = createSprite(createRect(cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
		
		if(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT))
		{
			if(getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE)		
			{
				setBlockType(world->backgroundBlocks, cursorX, cursorY, world->blockArea, placeBlock(player->inventory.slots[player->inventory.selected].item), world->worldBoundingRect);
				if(placeBlock(player->inventory.slots[player->inventory.selected].item) != NONE)	
					decrementSlot(player->inventory.selected, &player->inventory);	
			}	
		}
		else if(!colliding(temp.hitbox, player->playerSpr.hitbox) &&
				(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE ||
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER ||
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA))
		{
			//Check if block is transparent
			int found = 0, ableToPlace = 0;
			for(int i = 0; i < sizeof(transparent) / sizeof(enum BlockType) && !found; i++)
			{	
				if(transparent[i] == placeBlock(player->inventory.slots[player->inventory.selected].item))
				{	
					found = 1;
					if(getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE)
					{
						setBlockType(world->transparentBlocks, cursorX, cursorY, world->blockArea, placeBlock(player->inventory.slots[player->inventory.selected].item), world->worldBoundingRect);			
						ableToPlace = 1;	
					}	
				}
			}
			if(!found && placeBlock(player->inventory.slots[player->inventory.selected].item) != NONE)
				setBlockType(world->blocks, cursorX, cursorY, world->blockArea, placeBlock(player->inventory.slots[player->inventory.selected].item), world->worldBoundingRect);			
			if(placeBlock(player->inventory.slots[player->inventory.selected].item) != NONE && (!found || (found && ableToPlace)))	
				decrementSlot(player->inventory.selected, &player->inventory);	
		}
	}
	//Destroy blocks
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);
		cursorX = roundf((cursorX + player->playerSpr.hitbox.position.x) / BLOCK_SIZE);	
		cursorY = roundf((cursorY + player->playerSpr.hitbox.position.y) / BLOCK_SIZE);
	
		if((isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT)) &&
			(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE ||
			 getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER ||
			 getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA) &&
			getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != NONE)
			breakBlockTimer += secondsPerFrame;
		else if((getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != WATER &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != LAVA &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != INDESTRUCTABLE &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != NONE) &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).visibility == REVEALED &&
				!(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT)))
			breakBlockTimer += secondsPerFrame;
		else if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE &&
				getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != NONE)
			breakBlockTimer += secondsPerFrame;
		else
			breakBlockTimer = 0.0f;

		if(breakBlockTimer > 1.0f)
		{
			if(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT))
			{
				if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE ||
					getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER ||
					getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA	)		
				{	
					addItem(world, droppedItem(getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type, NOTHING), cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE);	
					setBlockType(world->backgroundBlocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);
				}	
			}
			else
			{
				if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE)	
				{	
					addItem(world, droppedItem(getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type, NOTHING), cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE);	
					setBlockType(world->transparentBlocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);	
				}	
				else if((getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != WATER &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != LAVA &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != INDESTRUCTABLE) &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).visibility == REVEALED)
				{	
					addItem(world, droppedItem(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type, NOTHING), cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE);	
					setBlockType(world->blocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);
					setBlockMass(world->blocks, cursorX, cursorY, world->blockArea, 0.0f, world->worldBoundingRect);				
					revealNeighbors(world, cursorX, cursorY);	
				}	
			}
			
			breakBlockTimer = 0.0f;
		}
	}
	else
		breakBlockTimer = 0.0f;

	//Inventory keys
	static const int inventoryHotKeys[] = { GLFW_KEY_1,
											GLFW_KEY_2,
											GLFW_KEY_3,
											GLFW_KEY_4,
											GLFW_KEY_5,
											GLFW_KEY_6,
											GLFW_KEY_7,
											GLFW_KEY_8,
											GLFW_KEY_9 };
	for(int i = 0; i < 9; i++)
		if(isPressed(inventoryHotKeys[i]))
			player->inventory.selected = i;
	
	double scroll = getMouseScroll();
	if(scroll > 0.0)
	{
		player->inventory.selected++;
		player->inventory.selected %= player->inventory.maxSize;	
	}
	else if(scroll < 0.0)
	{
		player->inventory.selected--;
		player->inventory.selected += player->inventory.maxSize;
		player->inventory.selected %= player->inventory.maxSize;	
	}

	//Update time in the world
	world->dayCycle += secondsPerFrame * 1.0f / 60.0f * 1.0f / 20.0f;
	if(world->dayCycle > 1.0f)
		world->dayCycle = 0.0f;

	//Update clouds
	for(int i = 0; i < MAX_CLOUD; i++)
	{
		world->clouds[i].hitbox.position.x += 16.0f * secondsPerFrame;
		if(world->clouds[i].hitbox.position.x > 960.0f + world->clouds[i].hitbox.dimensions.w / 2.0f)
		{
			float sz = (float)rand() / (float)RAND_MAX * 32.0f + 64.0f;
			world->clouds[i].hitbox.position.x = -960.0f - world->clouds[i].hitbox.dimensions.w / 2.0f;
			world->clouds[i].hitbox.position.y = (float)rand() / (float)RAND_MAX * 960.0f;
			world->clouds[i].hitbox.dimensions.w = world->clouds[i].hitbox.dimensions.h = sz;	
		}
	}

	updateItems(world, camPos, 32, secondsPerFrame, player);
}

float getBlockBreakTimer()
{
	return breakBlockTimer;
}
