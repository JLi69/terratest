#include "enemy.h"
#include "draw.h"
#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
	case GREEN_SLIME: return 8;
	case BLUE_SLIME: return 16;
	case RED_SLIME: return 32;
	case PINK_SLIME: return 24;
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
			   struct Block *blocks, struct BoundingRect boundRect, int maxBlockInd,
			   struct Player *player)
{
	if(enemy->health <= 0)
		return;
	
	if(enemy->attackmode == KNOCKBACK && enemy->timer >= 0.2f)
		enemy->spr.vel.x = -fabs(enemy->spr.vel.x) / enemy->spr.vel.x * 3.0f * BLOCK_SIZE;

	if(enemy->timer > 0.0f && (enemy->attackmode == PASSIVE || enemy->attackmode == KNOCKBACK))
		enemy->timer -= timePassed;
	else if(enemy->timer <= 0.0f && (enemy->attackmode == PASSIVE || enemy->attackmode == KNOCKBACK))
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
	else if(enemy->attackmode == CHASE)
	{
		if(player->playerSpr.hitbox.position.x < enemy->spr.hitbox.position.x - BLOCK_SIZE / 2.0f)
		{
			enemy->spr.vel.x = -BLOCK_SIZE * 2.0f;
			enemy->spr.flipped = 1;
		}
		else if(player->playerSpr.hitbox.position.x > enemy->spr.hitbox.position.x + BLOCK_SIZE / 2.0f)
		{
			enemy->spr.vel.x = BLOCK_SIZE * 2.0f;
			enemy->spr.flipped = 0;
		} 

		if(fabsf(player->playerSpr.hitbox.position.x - enemy->spr.hitbox.position.x) < 
				BLOCK_SIZE * 4.0f && enemy->spr.vel.y == 0.0f &&
				enemy->spr.hitbox.position.y < player->playerSpr.hitbox.position.y)
			enemy->spr.vel.y = 8.0f * BLOCK_SIZE;
	}
	else if(enemy->attackmode == RUN_AWAY)
	{
		if(player->playerSpr.hitbox.position.x < enemy->spr.hitbox.position.x - BLOCK_SIZE / 2.0f)
		{
			enemy->spr.vel.x = BLOCK_SIZE * 3.0f;
			enemy->spr.flipped = 0;
		}
		else if(player->playerSpr.hitbox.position.x > enemy->spr.hitbox.position.x + BLOCK_SIZE / 2.0f)
		{
			enemy->spr.vel.x = -BLOCK_SIZE * 3.0f;
			enemy->spr.flipped = 1;
		} 

		if(fabsf(player->playerSpr.hitbox.position.x - enemy->spr.hitbox.position.x) < 
				BLOCK_SIZE * 4.0f && enemy->spr.vel.y == 0.0f &&
				enemy->spr.hitbox.position.y < player->playerSpr.hitbox.position.y)
			enemy->spr.vel.y = 8.0f * BLOCK_SIZE;
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
	float dist = sqrtf(
					powf(enemy->spr.hitbox.position.x - player->playerSpr.hitbox.position.x, 2.0f) +
					powf(enemy->spr.hitbox.position.y - player->playerSpr.hitbox.position.y, 2.0f));
	if(dist < 8.0f * BLOCK_SIZE && enemy->health > 1 && enemy->attackmode != KNOCKBACK)
		enemy->attackmode = CHASE;
	else if(enemy->attackmode != KNOCKBACK && enemy->attackmode != PASSIVE)
		enemy->attackmode = WANDER;

	//Colliding with player
	if(colliding(player->playerSpr.hitbox, enemy->spr.hitbox))
	{	
		damagePlayer(player, 1);	
	}

	if(getBlock(blocks, 
				enemy->spr.hitbox.position.x / BLOCK_SIZE,
				enemy->spr.hitbox.position.y / BLOCK_SIZE,
				maxBlockInd, boundRect).type == LAVA)
		damageEnemy(enemy, 3);
	//Swim
	if(getBlock(blocks, 
				enemy->spr.hitbox.position.x / BLOCK_SIZE,
				(int)roundf(enemy->spr.hitbox.position.y / BLOCK_SIZE + 0.5f),
				maxBlockInd, boundRect).type == WATER &&
		enemy->spr.vel.y <= 0.0f)
		enemy->spr.vel.y = 8.0f * BLOCK_SIZE;
	if(getBlock(blocks, 
				enemy->spr.hitbox.position.x / BLOCK_SIZE,
				enemy->spr.hitbox.position.y / BLOCK_SIZE - 1,
				maxBlockInd, boundRect).type == MAGMA_STONE)
		damageEnemy(enemy, 1);

	//If we are on low health, run away and try to stay away from the player
	if(enemy->health <= 1 && enemy->attackmode != KNOCKBACK && dist <= 8.0f * BLOCK_SIZE)
		enemy->attackmode = RUN_AWAY;
	else if(dist >= 8.0f * BLOCK_SIZE && enemy->health <= 1 && enemy->attackmode != KNOCKBACK)
		enemy->attackmode = WANDER;	
	
	enemy->damageCooldown -= timePassed;	
}

void slimeAI(struct Enemy *enemy, float timePassed,
			   struct Block *blocks, struct BoundingRect boundRect, int maxBlockInd,
			   struct Player *player, int damageAmt, int immuneToLava, int healAmount)
{
	if(enemy->health <= 0)
		return;
	
	if(enemy->attackmode == KNOCKBACK && enemy->timer >= 0.2f)
		enemy->spr.vel.x = -fabs(enemy->spr.vel.x) / enemy->spr.vel.x * 2.0f * BLOCK_SIZE;

	if(enemy->timer > 0.0f && (enemy->attackmode == PASSIVE || enemy->attackmode == KNOCKBACK))
		enemy->timer -= timePassed;
	else if(enemy->timer <= 0.0f && (enemy->attackmode == PASSIVE || enemy->attackmode == KNOCKBACK))
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
	else if(enemy->attackmode == CHASE)
	{
		if(player->playerSpr.hitbox.position.x < enemy->spr.hitbox.position.x - BLOCK_SIZE / 2.0f)
		{
			enemy->spr.vel.x = -BLOCK_SIZE * 3.0f;
			enemy->spr.flipped = 1;
		}
		else if(player->playerSpr.hitbox.position.x > enemy->spr.hitbox.position.x + BLOCK_SIZE / 2.0f)
		{
			enemy->spr.vel.x = BLOCK_SIZE * 3.0f;
			enemy->spr.flipped = 0;
		} 

		if(fabsf(player->playerSpr.hitbox.position.x - enemy->spr.hitbox.position.x) < 
				BLOCK_SIZE * 4.0f && enemy->spr.vel.y == 0.0f &&
				enemy->spr.hitbox.position.y < player->playerSpr.hitbox.position.y)
			enemy->spr.vel.y = 12.0f * BLOCK_SIZE;
	}

	if(enemy->attackmode != PASSIVE && enemy->spr.vel.y >= 0.0f)
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
					enemy->spr.vel.y = 12.0f * BLOCK_SIZE;
				enemy->spr.hitbox.position.x =
					collided.hitbox.position.x +
					collided.hitbox.dimensions.w / 2.0f +
					enemy->spr.hitbox.dimensions.w / 2.0f;	
			}	
			else if(enemy->spr.hitbox.position.x <= 
					collided.hitbox.position.x - collided.hitbox.dimensions.x / 2.0f)
			{
				if(enemy->spr.vel.y == 0.0f)
					enemy->spr.vel.y = 12.0f * BLOCK_SIZE; //Attempt to jump over
				enemy->spr.hitbox.position.x =
					collided.hitbox.position.x -
					collided.hitbox.dimensions.w / 2.0f -
					enemy->spr.hitbox.dimensions.w / 2.0f;	
			}	
		}
	}

	updateSpriteY(&enemy->spr, timePassed);
	//Check for collision	
	if(blockCollisionSearch(enemy->spr, 3, 3, blocks, maxBlockInd, boundRect, &collided))
	{	
		//Apply fall damage
		//Slime's weakness is fall damage
		if(enemy->spr.vel.y / BLOCK_SIZE <= -16.0f && 
			!(getBlock(blocks, enemy->spr.hitbox.position.x / BLOCK_SIZE, enemy->spr.hitbox.position.y / BLOCK_SIZE, maxBlockInd, boundRect).type == WATER) && 
			!(getBlock(blocks, enemy->spr.hitbox.position.x / BLOCK_SIZE, enemy->spr.hitbox.position.y / BLOCK_SIZE - 1, maxBlockInd, boundRect).type == SLIME_BLOCK))
		{
			damageEnemy(enemy, 2 * (int)sqrtf(-enemy->spr.vel.y / BLOCK_SIZE - 16.0f));
		}

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
	float dist = sqrtf(
					powf(enemy->spr.hitbox.position.x - player->playerSpr.hitbox.position.x, 2.0f) +
					powf(enemy->spr.hitbox.position.y - player->playerSpr.hitbox.position.y, 2.0f));
	if(dist < 16.0f * BLOCK_SIZE && enemy->attackmode != KNOCKBACK)
		enemy->attackmode = CHASE;
	else if(enemy->attackmode != KNOCKBACK && enemy->attackmode != PASSIVE)
		enemy->attackmode = WANDER;

	//Colliding with player
	if(colliding(player->playerSpr.hitbox, enemy->spr.hitbox))
	{	
		//Enemy heals whenever damaging player (if healAmount > 0)
		if(damagePlayer(player, damageAmt))	
			enemy->health += healAmount;	
		if(enemy->health > enemy->maxHealth)
			enemy->health = enemy->maxHealth;
	}

	if(!immuneToLava)
	{
		if(getBlock(blocks, 
					enemy->spr.hitbox.position.x / BLOCK_SIZE,
					enemy->spr.hitbox.position.y / BLOCK_SIZE,
					maxBlockInd, boundRect).type == LAVA)
		{
			damageEnemy(enemy, 3);
			enemy->attackmode = WANDER;	
		}
		if(getBlock(blocks, 
					enemy->spr.hitbox.position.x / BLOCK_SIZE,
					enemy->spr.hitbox.position.y / BLOCK_SIZE - 1,
					maxBlockInd, boundRect).type == MAGMA_STONE)
		{	
			damageEnemy(enemy, 1);
			enemy->attackmode = WANDER;	
		}
	}
	//Swim in lava if immune
	else if(getBlock(blocks, 
			enemy->spr.hitbox.position.x / BLOCK_SIZE,
			(int)roundf(enemy->spr.hitbox.position.y / BLOCK_SIZE + 0.5f),
			maxBlockInd, boundRect).type == LAVA &&
			enemy->spr.vel.y <= 0.0f)
		enemy->spr.vel.y = 8.0f * BLOCK_SIZE;

	enemy->damageCooldown -= timePassed;	
}

void updateEnemy(struct Enemy *enemy, 
				 float timePassed,
				 struct Block *blocks, 
				 struct BoundingRect boundRect,
				 int maxBlockInd,
				 struct Player *player)
{
	switch(enemy->spr.type)
	{
	case CHICKEN: chickenAI(enemy, timePassed, blocks, boundRect, maxBlockInd, player); break;
	case GREEN_SLIME: slimeAI(enemy, timePassed, blocks, boundRect, maxBlockInd, player, 1, 0, 0); break;
	case BLUE_SLIME: slimeAI(enemy, timePassed, blocks, boundRect, maxBlockInd, player, 2, 0, 0); break;
	case RED_SLIME: slimeAI(enemy, timePassed, blocks, boundRect, maxBlockInd, player, 3, 1, 0); break;
	case PINK_SLIME: slimeAI(enemy, timePassed, blocks, boundRect, maxBlockInd, player, 1, 0, 4); break;
	default: return;
	}
}

int damageEnemy(struct Enemy *enemy, int amt)
{
	if(enemy->damageCooldown > 0.0f)
		return 0;
	enemy->health -= amt;
	enemy->damageCooldown = ENEMY_DAMAGE_COOLDOWN;
	enemy->attackmode = KNOCKBACK;
	enemy->timer = 0.2f;
	return amt;
}

enum Item droppedLoot(enum EnemyType enemy)
{
	switch(enemy)
	{
	case CHICKEN:
		if(rand() % 2 == 0)
			return RAW_MEAT;
		return NOTHING;
	case GREEN_SLIME: //Fall through
	case BLUE_SLIME:
		return SLIMEBALL;
	case RED_SLIME:
		if(rand() % 2 == 0)
			return SLIMEBALL;
		else if(rand() % 2 == 0)
			return MAGMA_ITEM;
		else if(rand() % 2 == 0)
			return COAL_ITEM;
		else if(rand() % 8 == 0)
			return IRON_ITEM;
		return SLIMEBALL;
	case PINK_SLIME:
		if(rand() % 3 == 0)
			return HEART_ITEM;
		return SLIMEBALL;
	default: break;
	}

	return NOTHING;
}
