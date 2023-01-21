#include "world.h"
#include <stdlib.h>
#include <math.h>

#define CHICKEN_PROB 32

#define GREEN_SLIME_PROB 128
#define BLUE_SLIME_PROB 1024
#define RED_SLIME_PROB 64

void spawnEnemies(struct World *world, float *worldheight, int worldwidth, union Point playerPos)
{
	//Spawn chickens
	for(int i = 0; i < worldwidth; i++)
	{
		float x = i * BLOCK_SIZE - BLOCK_SIZE * worldwidth / 2.0f,
			  y = (int)worldheight[i] * BLOCK_SIZE + BLOCK_SIZE;

		if(sqrtf(powf(playerPos.x - x, 2.0f) + powf(playerPos.y - y, 2.0f)) < 16.0f * BLOCK_SIZE)
			continue;

		if(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type == GRASS &&
		   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
			rand() % CHICKEN_PROB == 0)
		{
			struct Enemy chicken = createEnemy(CHICKEN, x, y);
			chicken.spr.flipped = rand() % 2;
			insertEnemy(world->enemies, chicken);
		}	
	}
}

//Spawns zombies and slimes
void spawnEnemiesAtNight(struct World *world, struct Vector2D camPos, float range)
{
	struct Vector2D camPosGrid = createVector(floorf(camPos.x / BLOCK_SIZE) * BLOCK_SIZE, floorf(camPos.y / BLOCK_SIZE) * BLOCK_SIZE);
	
	//Spawn slimes
	for(float x = camPosGrid.x - range * BLOCK_SIZE; x <= camPosGrid.x + range * BLOCK_SIZE; x += BLOCK_SIZE)
	{
		for(float y = camPosGrid.y - range * BLOCK_SIZE; y <= camPosGrid.y + range * BLOCK_SIZE; y += BLOCK_SIZE)
		{
			if(sqrtf(powf(x - camPos.x, 2.0f) + powf(y - camPos.y, 2.0f)) < BLOCK_SIZE * 24.0f)
				continue;

			struct Enemy temp = createEnemy(DELETED, x, y);
			if(getCollision(world->enemies, temp, ROOT) != -1)
				continue;

			if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
			   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				rand() % GREEN_SLIME_PROB == 0)
			{
				struct Enemy slime = createEnemy(GREEN_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			}	
			else if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			    getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
				getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				rand() % BLUE_SLIME_PROB == 0)
			{
				struct Enemy slime = createEnemy(BLUE_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			}
		}
	}
}

//Spawns chickens during the day
void spawnChickens(struct World *world, struct Vector2D camPos, float range)
{
	struct Vector2D camPosGrid = createVector(floorf(camPos.x / BLOCK_SIZE) * BLOCK_SIZE, floorf(camPos.y / BLOCK_SIZE) * BLOCK_SIZE);
	
	for(float x = camPosGrid.x - range * BLOCK_SIZE; x <= camPosGrid.x + range * BLOCK_SIZE; x += BLOCK_SIZE)
	{
		for(float y = camPosGrid.y - range * BLOCK_SIZE; y <= camPosGrid.y + range * BLOCK_SIZE; y += BLOCK_SIZE)
		{
			if(sqrtf(powf(x - camPos.x, 2.0f) + powf(y - camPos.y, 2.0f)) < BLOCK_SIZE * 24.0f)
				continue;

			struct Enemy temp = createEnemy(DELETED, x, y);
			if(getCollision(world->enemies, temp, ROOT) != -1)
				continue;

			if(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type == GRASS &&
			   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				rand() % CHICKEN_PROB == 0)
			{
				struct Enemy chicken = createEnemy(CHICKEN, x, y);
				chicken.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, chicken);
			}	
		}
	}
}

//Spawn enemies in caves
void spawnEnemiesInCave(struct World *world, struct Vector2D camPos, float range)
{
	struct Vector2D camPosGrid = createVector(floorf(camPos.x / BLOCK_SIZE) * BLOCK_SIZE, floorf(camPos.y / BLOCK_SIZE) * BLOCK_SIZE);
	
	for(float x = camPosGrid.x - range * BLOCK_SIZE; x <= camPosGrid.x + range * BLOCK_SIZE; x += BLOCK_SIZE)
	{
		for(float y = camPosGrid.y - range * BLOCK_SIZE; y <= camPosGrid.y + range * BLOCK_SIZE; y += BLOCK_SIZE)
		{
			if(sqrtf(powf(x - camPos.x, 2.0f) + powf(y - camPos.y, 2.0f)) < BLOCK_SIZE * 24.0f ||
				y > BLOCK_SIZE * WATER_LEVEL)
				continue;

			struct Enemy temp = createEnemy(DELETED, x, y);
			if(getCollision(world->enemies, temp, ROOT) != -1)
				continue;
			
			if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			    getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
				getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				(rand() % RED_SLIME_PROB) == 0 && y < BLOCK_SIZE * 64.0f)
			{
				struct Enemy slime = createEnemy(RED_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			}
			else if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
			   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				rand() % GREEN_SLIME_PROB == 0)
			{
				struct Enemy slime = createEnemy(GREEN_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			}	
			else if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			    getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
				getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				rand() % BLUE_SLIME_PROB == 0 &&
				y < BLOCK_SIZE * 180.0f)
			{
				struct Enemy slime = createEnemy(BLUE_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			} 
		}
	}
}

void spawnWave(struct World *world, struct Vector2D camPos, float range)
{
	struct Vector2D camPosGrid = createVector(floorf(camPos.x / BLOCK_SIZE) * BLOCK_SIZE, floorf(camPos.y / BLOCK_SIZE) * BLOCK_SIZE);
	
	for(float x = camPosGrid.x - range * BLOCK_SIZE; x <= camPosGrid.x + range * BLOCK_SIZE; x += BLOCK_SIZE)
	{
		for(float y = camPosGrid.y - range / 8.0 * BLOCK_SIZE; y <= camPosGrid.y + range / 8.0f * BLOCK_SIZE; y += BLOCK_SIZE)
		{
			if(sqrtf(powf(x - camPos.x, 2.0f) + powf(y - camPos.y, 2.0f)) < BLOCK_SIZE * 32.0f)
				continue;

			struct Enemy temp = createEnemy(DELETED, x, y);
			if(getCollision(world->enemies, temp, ROOT) != -1)
				continue;
			
			if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			    getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
				getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				(rand() % (RED_SLIME_PROB * 16)) == 0)
			{
				struct Enemy slime = createEnemy(RED_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			}
			else if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
			   getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				(rand() % GREEN_SLIME_PROB) / 8 == 0)
			{
				struct Enemy slime = createEnemy(GREEN_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			}	
			else if(!canReplace(getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type) &&
			    getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE - 1, world->blockArea, world->worldBoundingRect).type != LEAF &&
				getBlock(world->blocks, x / BLOCK_SIZE, y / BLOCK_SIZE, world->blockArea, world->worldBoundingRect).type == NONE &&
				(rand() % BLUE_SLIME_PROB) / 8 == 0)
			{
				struct Enemy slime = createEnemy(BLUE_SLIME, x, y);
				slime.spr.flipped = rand() % 2;
				insertEnemy(world->enemies, slime);
			} 
		}
	}
}
