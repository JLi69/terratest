#include "enemy.h"
#include "draw.h"
#include "world.h"
#include <stdlib.h>
#include <stdio.h>

void drawEnemy1x1(struct Enemy enemy, struct Vector2D camPos)
{
	flip(enemy.spr.flipped);
	setRectPos(enemy.spr.hitbox.position.x - camPos.x, enemy.spr.hitbox.position.y - camPos.y);
	setRectSize(enemy.spr.hitbox.dimensions.w, enemy.spr.hitbox.dimensions.h);	
	int ind = enemy.spr.type * 4 + enemy.spr.animationFrame;
	setTexOffset(1.0f / 16.0f * (ind % 16), 1.0f / 16.0f * (ind / 16));
	drawRect();

	//Draw health bar
	turnOffTexture();
	setRectPos(enemy.spr.hitbox.position.x - camPos.x,
			   enemy.spr.hitbox.position.y - camPos.y + enemy.spr.hitbox.dimensions.h / 2.0f * 1.5f);
	setRectSize(BLOCK_SIZE * 1.5f, 6.0f);
	setRectColor(255.0f, 0.0f, 0.0f, 255.0f);
	drawRect();

	setRectPos(enemy.spr.hitbox.position.x - camPos.x - (1.0f - (float)enemy.health / (float)enemy.maxHealth) * 0.5f * (BLOCK_SIZE * 1.5f - 2.0f),
			   enemy.spr.hitbox.position.y - camPos.y + enemy.spr.hitbox.dimensions.h / 2.0f * 1.5f);
	setRectSize((float)enemy.health / (float)enemy.maxHealth * BLOCK_SIZE * 1.5f - 2.0f, 4.0f);
	setRectColor(0.0f, 255.0f, 0.0f, 255.0f);
	drawRect();

	turnOnTexture();
}

int maxHealthEnemy(enum EnemyType type)
{
	switch(type)
	{
	case CHICKEN: return 5;
	default: return 0;
	}
}

struct Enemy createEnemy(enum EnemyType type, float x, float y)
{
	struct Enemy enemy;
	enemy.spr = createSpriteWithType(
					createRect(x, y, BLOCK_SIZE, BLOCK_SIZE),
					type);
	enemy.spr.canMove = 1;
	enemy.spr.animating = 1;
	enemy.attackmode = WANDER;
	enemy.health = enemy.maxHealth = maxHealthEnemy(type);
	enemy.walkDistance = 0;
	enemy.damageCooldown = enemy.timer = 0.0f;
	return enemy;
}

void chickenAI(struct Enemy *enemy, float timePassed,
			   struct Block *blocks, struct BoundingRect boundRect, int maxBlockInd)
{
	if(enemy->timer > 0.0f && enemy->attackmode == PASSIVE)
		enemy->timer -= timePassed;
	else if(enemy->timer <= 0.0f && enemy->attackmode == PASSIVE)
		enemy->attackmode = WANDER;

	//Choose random amount to wander and wander that
	//if we hit a block, attempt to jump over it
	//Once we walk that distance, generate new amount
	//to wander and wander in the opposite direction
	if(enemy->attackmode == WANDER && enemy->walkDistance <= 0.0f)
	{
		enemy->walkDistance = BLOCK_SIZE * (rand() % 8 + 8.0f);
		//If enemy is still, have it start moving
		if(enemy->spr.vel.x == 0.0f)
		{
			enemy->spr.vel.x = enemy->spr.flipped ? -BLOCK_SIZE : BLOCK_SIZE;
		}
		enemy->attackmode = PASSIVE;
		enemy->timer = 2.0f;
		enemy->spr.vel.x *= -1.0f;
		enemy->spr.flipped = !enemy->spr.flipped;
	}

	if(enemy->attackmode != PASSIVE)
		updateSpriteX(&enemy->spr, timePassed);
	//Check for collision and uncollide
	struct Sprite collided;
	if(blockCollisionSearch(enemy->spr, 3, 3, blocks, maxBlockInd, boundRect, &collided))
	{				
		//Uncollide the enemy	
		if(enemy->spr.vel.x != 0.0f)
		{
			if(enemy->spr.hitbox.position.x >=
			   collided.hitbox.position.x + collided.hitbox.dimensions.w / 2.0f)
			{
				if(enemy->spr.vel.y == 0.0f)
					enemy->spr.vel.y = 8.0f * BLOCK_SIZE;
				enemy->spr.hitbox.position.x =
					collided.hitbox.position.x +
					collided.hitbox.dimensions.w / 2.0f +
					enemy->spr.hitbox.dimensions.w / 2.0f;	
			}	
			else if(enemy->spr.hitbox.position.x <= 
					collided.hitbox.position.x - collided.hitbox.dimensions.x / 2.0f)
			{
				if(enemy->spr.vel.y == 0.0f)
					enemy->spr.vel.y = 8.0f * BLOCK_SIZE; //Attempt to jump over
				enemy->spr.hitbox.position.x =
					collided.hitbox.position.x -
					collided.hitbox.dimensions.w / 2.0f -
					enemy->spr.hitbox.dimensions.w / 2.0f;	
			}	
		}
	}

	if(enemy->spr.vel.y < -BLOCK_SIZE * 4.0f)
		enemy->spr.vel.y = -BLOCK_SIZE * 4.0f;
	updateSpriteY(&enemy->spr, timePassed);
	//Check for collision	
	if(blockCollisionSearch(enemy->spr, 3, 3, blocks, maxBlockInd, boundRect, &collided))
	{		
		//Uncollide the enemy
		if(enemy->spr.vel.y != 0.0f)
		{
			if(enemy->spr.hitbox.position.y >=
			   collided.hitbox.position.y + collided.hitbox.dimensions.h / 2.0f)
			{
				enemy->spr.vel.y = 0.0f;
				enemy->spr.hitbox.position.y =
					collided.hitbox.position.y +
					collided.hitbox.dimensions.h / 2.0f +
					enemy->spr.hitbox.dimensions.h / 2.0f;	
				//The player is supported by a block	
				enemy->spr.falling = 0;	
			}	
			else if(enemy->spr.hitbox.position.y <= 
					collided.hitbox.position.y - collided.hitbox.dimensions.h / 2.0f)
			{
				enemy->spr.falling = 1;	
				enemy->spr.vel.y = -0.5f;
				enemy->spr.hitbox.position.y =
					collided.hitbox.position.y -
					collided.hitbox.dimensions.h / 2.0f -
					enemy->spr.hitbox.dimensions.h / 2.0f;	
			}	
		}	
	}
	else
	{
		//The enemy is not supported by a block
		enemy->spr.falling = 1;
	}
	
	if(enemy->attackmode == WANDER)
	{
		enemy->walkDistance -= (enemy->spr.vel.x * timePassed > 0.0f) ? 
								enemy->spr.vel.x * timePassed :
								-enemy->spr.vel.x * timePassed;
	}

	//if we are close enough to the player, enter chase mode and attempt
	//to get to the player
	
	//If we are on low health, run away and try to stay away from the player
	
}

void updateEnemy(struct Enemy *enemy, float timePassed,
				 struct Block *blocks, struct BoundingRect boundRect, int maxBlockInd)
{
	switch(enemy->spr.type)
	{
	case CHICKEN: chickenAI(enemy, timePassed, blocks, boundRect, maxBlockInd); break;
	default: return;
	}
}
