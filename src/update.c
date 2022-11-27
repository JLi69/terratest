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
	const float height = 8.0f;
	*world = generateWorld(time(0), height, 256.0f);
	
	*player = createSprite(createRect(0.0f, 32.0f * 1.5f * height, 32.0f, 64.0f));
	player->animationState = IDLE;
	player->animating = 1;

	struct Sprite* collision;
	while(collisionSearch(world->blocks, *player, &collision))
		player->hitbox.position.y += 32.0f;
	player->canMove = 1;
}

void updateGameobjects(struct World *world, struct Sprite *player, float secondsPerFrame)
{	
	struct Sprite* collided = (void*)0;	

	//Move player in the x direction
	updateSpriteX(player, secondsPerFrame);
	//Check for collision
	if(collisionSearch(world->blocks, *player, &collided))
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
	if(collisionSearch(world->blocks, *player, &collided))
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
		if(collisionSearch(world->blocks, temp, &tempCollison))
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

	//Place blocks
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT))
	{
		double cursorX, cursorY;
		getCursorPos(&cursorX, &cursorY);
		cursorX = roundf((cursorX + player->hitbox.position.x) / BLOCK_SIZE) * BLOCK_SIZE;	
		cursorY = roundf((cursorY + player->hitbox.position.y) / BLOCK_SIZE) * BLOCK_SIZE;
		struct Sprite temp = createSpriteWithType(createRect(cursorX, cursorY, BLOCK_SIZE, BLOCK_SIZE), BRICK);
		struct Sprite* tempCollision;	
		if(!colliding(temp.hitbox, player->hitbox) && !collisionSearch(world->blocks, temp, &tempCollision))
		{
			insert(world->blocks, temp);
		}
	}
}
