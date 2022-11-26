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
	const float height = 256.0f;
	*world = generateWorld(time(0), height, 256.0f);
	*player = createSprite(createRect(0.0f, height * 32.0f * 2.0f, 32.0f, 64.0f));
	
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
		//The player is not supported by a block
		player->falling = 1;

	//Move character with arrow keys
	if(isPressed(GLFW_KEY_A))
		player->vel.x = -PLAYER_SPEED;
	else if(isPressed(GLFW_KEY_D))
		player->vel.x = PLAYER_SPEED;
	else
		player->vel.x = 0.0f;
	//Jump
	if(!player->falling && isPressed(GLFW_KEY_SPACE))
		player->vel.y = JUMP_SPEED;
}
