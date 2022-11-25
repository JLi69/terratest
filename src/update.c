#include "game.h"
#include "world.h"
#include "window-func.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>

void initGame(struct World *world, struct Sprite *player)
{
	*world = generateTest();
	*player = createSprite(createRect(0.0f, 128.0f, 32.0f, 64.0f));
	player->canMove = 1;
}

void updateGameobjects(struct World *world, struct Sprite *player, float secondsPerFrame)
{	
	struct Sprite* collided = (void*)0;	

	updateSpriteX(player, secondsPerFrame);	
	if(collisionSearch(world->blocks, *player, &collided))
	{				
		//Uncollide the player
		/*if(player->vel.x != 0.0f)
		{
			player->hitbox.position.x -= player->vel.x * secondsPerFrame;
			player->vel.x = 0.0f;
		}*/	
	
		//Player is moving in the y direction		
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

	updateSpriteY(player, secondsPerFrame);	
	if(collisionSearch(world->blocks, *player, &collided))
	{
		//Player is moving in the y direction		
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
		//The player is not supported by a block
		player->falling = 1;

	//Move character with arrow keys
	if(isPressed(GLFW_KEY_A))
		player->vel.x = -PLAYER_SPEED;
	else if(isPressed(GLFW_KEY_D))
		player->vel.x = PLAYER_SPEED;
	else
		player->vel.x = 0.0f;

	if(!player->falling && isPressed(GLFW_KEY_SPACE))
		player->vel.y = JUMP_SPEED;
}
