#include "game.h"
#include "world.h"
#include "window-func.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

void initGame(struct World *world, struct Sprite *player)
{
	const float height = 128.0f;
	*world = generateWorld(time(0), height, 256.0f);

	*player = createSprite(createRect(0.0f, 32.0f * 1.5f * height, 32.0f, 64.0f));
	player->animationState = IDLE;
	player->animating = 1;

	struct Sprite* collision;
	while(collisionSearch(world->solidBlocks, *player, &collision))
		player->hitbox.position.y += 32.0f;
	player->canMove = 1;
}

void updateGameobjects(struct World *world, struct Sprite *player, float secondsPerFrame)
{
	static float liquidUpdateTimer = 0.0f;
	liquidUpdateTimer += secondsPerFrame;

	struct Sprite* collided = (void*)0;	

	//printf("%f %f\n", player->hitbox.position.x / BLOCK_SIZE, player->hitbox.position.y / BLOCK_SIZE);

	//Move player in the x direction
	updateSpriteX(player, secondsPerFrame);
	//Check for collision
	if(collisionSearch(world->solidBlocks, *player, &collided))
	{				
		//Uncollide the player	
		if(player->vel.x != 0.0f)
		{
			if(player->hitbox.position.x >=
			   collided->hitbox.position.x + collided->hitbox.dimensions.w / 2.0f)
			{
				player->vel.x = 0.0f;
				player->hitbox.position.x =
					collided->hitbox.position.x +
					collided->hitbox.dimensions.w / 2.0f +
					player->hitbox.dimensions.w / 2.0f;	
			}	
			else if(player->hitbox.position.x <= 
					collided->hitbox.position.x - collided->hitbox.dimensions.x / 2.0f)
			{
				player->vel.x = 0.0f;
				player->hitbox.position.x =
					collided->hitbox.position.x -
					collided->hitbox.dimensions.w / 2.0f -
					player->hitbox.dimensions.w / 2.0f;	
			}	
		}
	}

	//Move player in y direction
	updateSpriteY(player, secondsPerFrame);	
	//Check for collision	
	if(collisionSearch(world->solidBlocks, *player, &collided))
	{		
		//Uncollide the player
		if(player->vel.y != 0.0f)
		{
			if(player->hitbox.position.y >=
			   collided->hitbox.position.y + collided->hitbox.dimensions.h / 2.0f)
			{
				player->vel.y = 0.0f;
				player->hitbox.position.y =
					collided->hitbox.position.y +
					collided->hitbox.dimensions.h / 2.0f +
					player->hitbox.dimensions.h / 2.0f;	
				//The player is supported by a block	
				player->falling = 0;	
			}	
			else if(player->hitbox.position.y <= 
					collided->hitbox.position.y - collided->hitbox.dimensions.h / 2.0f)
			{
				player->falling = 1;	
				player->vel.y = -0.5f;
				player->hitbox.position.y =
					collided->hitbox.position.y -
					collided->hitbox.dimensions.h / 2.0f -
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
		struct Sprite* tempCollison;	
		if(collisionSearch(world->solidBlocks, temp, &tempCollison))
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
	//Jump
	if(!player->falling && isPressed(GLFW_KEY_SPACE))
		player->vel.y = JUMP_SPEED;

	//Update liquid blocks	
	struct Vector2D camPos = createVector(player->hitbox.position.x, player->hitbox.position.y);
	if(liquidUpdateTimer > 0.03f)
	{
		updateLiquid(world->liquidBlocks, world->solidBlocks, camPos, secondsPerFrame, 64, world->blockArea);
		liquidUpdateTimer = 0.0f;	
	}

	//Place blocks
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT))
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);	

		cursorX = roundf((cursorX + player->hitbox.position.x) / BLOCK_SIZE) * BLOCK_SIZE;	
		cursorY = roundf((cursorY + player->hitbox.position.y) / BLOCK_SIZE) * BLOCK_SIZE;	

		struct Sprite temp = createSpriteWithType(createRect(cursorX, cursorY, BLOCK_SIZE, BLOCK_SIZE), BRICK);
		struct Sprite* tempCollision;	
		if(!colliding(temp.hitbox, player->hitbox) && !collisionSearch(world->solidBlocks, temp, &tempCollision))
		{
			setLiquidMass(world->liquidBlocks, cursorX / BLOCK_SIZE, cursorY / BLOCK_SIZE, world->solidBlocks, world->blockArea, 0.0f);
			setLiquidType(world->liquidBlocks, cursorX / BLOCK_SIZE, cursorY / BLOCK_SIZE, world->solidBlocks, world->blockArea, SOLID);
			insert(world->solidBlocks, temp);	
		}
	}
	//Destroy blocks
	else if(mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);
		cursorX = roundf((cursorX + player->hitbox.position.x) / BLOCK_SIZE) * BLOCK_SIZE;	
		cursorY = roundf((cursorY + player->hitbox.position.y) / BLOCK_SIZE) * BLOCK_SIZE;	

		struct Sprite selected = createSprite(createRect(cursorX, cursorY, BLOCK_SIZE, BLOCK_SIZE));
		struct Sprite* tempCollision = NULL;
		
		//Delete solid block
		collisionSearch(world->solidBlocks, selected, &tempCollision);
		if(tempCollision != NULL && tempCollision->type != INDESTRUCTABLE)
		{
			setLiquidMass(world->liquidBlocks, cursorX / BLOCK_SIZE, cursorY / BLOCK_SIZE, world->solidBlocks, world->blockArea, 0.0f);
			setLiquidType(world->liquidBlocks, cursorX / BLOCK_SIZE, cursorY / BLOCK_SIZE, world->solidBlocks, world->blockArea, EMPTY_LIQUID);
			deleteSprite(world->solidBlocks, selected);	
		}

		//Delete transparent block
		if(tempCollision == NULL) //Only delete if you haven't already broken a block already
								  //TODO: Add cooldown for breaking blocks
		{
			collisionSearch(world->transparentBlocks, selected, &tempCollision);
			if(tempCollision != NULL && tempCollision->type != INDESTRUCTABLE)
			{
				deleteSprite(world->transparentBlocks, selected);
			}
		}
	}
}
