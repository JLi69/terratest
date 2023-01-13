#include "enemy.h"
#include "draw.h"
#include "world.h"

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
	enemy.attackmode = WANDER;
	enemy.health = enemy.maxHealth = maxHealthEnemy(type);
	return enemy;
}

void chickenAI(struct Enemy *enemy, float timePassed)
{
	//Choose random amount to wander and wander that
	//if we hit a block, attempt to jump over it
	//Once we walk that distance, generate new amount
	//to wander and wander in the opposite direction
	
	//if we are close enough to the player, enter chase mode and attempt
	//to get to the player
	
	//If we are on low health, run away and try to stay away from the player
}

void updateEnemy(struct Enemy *enemy, float timePassed)
{
	switch(enemy->spr.type)
	{
	case CHICKEN: 
		chickenAI(enemy, timePassed); 
		break;
	default: return;
	}
}
