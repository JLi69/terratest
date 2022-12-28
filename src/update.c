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

void initGame(struct World *world, struct Sprite *player)
{
	const float height = 128.0f;
	*world = generateWorld(time(0), height, 256.0f);
	revealVisible(world);

	*player = createSprite(createRect(0.0f, 32.0f * 4.0f * height, 32.0f, 64.0f));
	player->animationState = IDLE;
	player->animating = 1;

	struct Sprite collision;
	while(!blockCollisionSearch(*player, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collision))
		player->hitbox.position.y -= 32.0f;
	player->hitbox.position.y += 32.0f;
	player->canMove = 1;
}

void updateGameobjects(struct World *world, struct Sprite *player, float secondsPerFrame)
{
	struct Vector2D camPos = createVector(player->hitbox.position.x, player->hitbox.position.y);
	static float blockUpdateTimer = 0.0f;
	blockUpdateTimer += secondsPerFrame;

	struct Sprite collided;	

	//printf("%f %f\n", player->hitbox.position.x / BLOCK_SIZE, player->hitbox.position.y / BLOCK_SIZE);

	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, WATER))
	{
		player->vel.x *= 0.4f;
		if(player->vel.y > 0.0f)
			player->vel.y *= 0.6f;
	}
	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, LAVA))
	{	
		player->vel.x *= 0.2f;
		if(player->vel.y > 0.0f)
			player->vel.y *= 0.4f;	
	}

	//Move player in the x direction
	updateSpriteX(player, secondsPerFrame);
	//Check for collision
	if(blockCollisionSearch(*player, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided))
	{				
		//Uncollide the player	
		if(player->vel.x != 0.0f)
		{
			if(player->hitbox.position.x >=
			   collided.hitbox.position.x + collided.hitbox.dimensions.w / 2.0f)
			{
				player->vel.x = 0.0f;
				player->hitbox.position.x =
					collided.hitbox.position.x +
					collided.hitbox.dimensions.w / 2.0f +
					player->hitbox.dimensions.w / 2.0f;	
			}	
			else if(player->hitbox.position.x <= 
					collided.hitbox.position.x - collided.hitbox.dimensions.x / 2.0f)
			{
				player->vel.x = 0.0f;
				player->hitbox.position.x =
					collided.hitbox.position.x -
					collided.hitbox.dimensions.w / 2.0f -
					player->hitbox.dimensions.w / 2.0f;	
			}	
		}
	}

	//Move player in y direction
	updateSpriteY(player, secondsPerFrame);	
	//Check for collision	
	if(blockCollisionSearch(*player, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided))
	{		
		//Uncollide the player
		if(player->vel.y != 0.0f)
		{
			if(player->hitbox.position.y >=
			   collided.hitbox.position.y + collided.hitbox.dimensions.h / 2.0f)
			{
				player->vel.y = 0.0f;
				player->hitbox.position.y =
					collided.hitbox.position.y +
					collided.hitbox.dimensions.h / 2.0f +
					player->hitbox.dimensions.h / 2.0f;	
				//The player is supported by a block	
				player->falling = 0;	
			}	
			else if(player->hitbox.position.y <= 
					collided.hitbox.position.y - collided.hitbox.dimensions.h / 2.0f)
			{
				player->falling = 1;	
				player->vel.y = -0.5f;
				player->hitbox.position.y =
					collided.hitbox.position.y -
					collided.hitbox.dimensions.h / 2.0f -
					player->hitbox.dimensions.h / 2.0f;	
			}	
		}	
	}
	else
	{
		//The player is not supported by a block
		player->falling = 1;
		player->animationState = FALLING;	
	
		struct Sprite temp = createSprite(
							 createRect(player->hitbox.position.x, player->hitbox.position.y - 0.01f,
										player->hitbox.dimensions.w, player->hitbox.dimensions.h));
		struct Sprite tempCollison;	
		if(blockCollisionSearch(temp, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &tempCollison))
			player->animationState = IDLE;
	}

	//Move character with arrow keys
	if(isPressed(GLFW_KEY_A))
	{
		player->flipped = 1;
		player->vel.x = -PLAYER_SPEED;
		if(player->animationState == IDLE)
			player->animationState = WALKING;	
	}
	else if(isPressed(GLFW_KEY_D))
	{
		player->flipped = 0;	
		player->vel.x = PLAYER_SPEED;
		if(player->animationState == IDLE)
			player->animationState = WALKING;
	}
	else
	{
		player->vel.x = 0.0f;
		if(player->animationState == WALKING)
			player->animationState = IDLE;
	}
	
	//Check if player can jump
	if(!player->falling || 
	   touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, WATER) || 
	   touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, VINES))
		player->canJump = 1;
	else
		player->canJump = 0;

	//Jump
	if(player->canJump && isPressed(GLFW_KEY_SPACE))
		player->vel.y = JUMP_SPEED;

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
		cursorX = roundf((cursorX + player->hitbox.position.x) / BLOCK_SIZE);	
		cursorY = roundf((cursorY + player->hitbox.position.y) / BLOCK_SIZE);	

		struct Sprite temp = createSprite(createRect(cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
		
		if(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT))
		{
			//TODO: Add inventory so that player can place blocks other than brick
			if(getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE)		
				setBlockType(world->backgroundBlocks, cursorX, cursorY, world->blockArea, BRICK, world->worldBoundingRect);
		}	
		else if(!colliding(temp.hitbox, player->hitbox) &&
				(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE ||
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER ||
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA))
		{
			setBlockType(world->blocks, cursorX, cursorY, world->blockArea, BRICK, world->worldBoundingRect);	
		}
	}
	//Destroy blocks
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);
		cursorX = roundf((cursorX + player->hitbox.position.x) / BLOCK_SIZE);	
		cursorY = roundf((cursorY + player->hitbox.position.y) / BLOCK_SIZE);
	
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
					setBlockType(world->backgroundBlocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);	
			}
			else
			{
				if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE)	
					setBlockType(world->transparentBlocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);	
				else if((getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != WATER &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != LAVA &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != INDESTRUCTABLE) &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).visibility == REVEALED)
				{	
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
}

float getBlockBreakTimer()
{
	return breakBlockTimer;
}
