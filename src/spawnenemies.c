#include "world.h"
#include <stdlib.h>
#include <math.h>

#define CHICKEN_PROB 32

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
