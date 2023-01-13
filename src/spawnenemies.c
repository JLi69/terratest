#include "world.h"
#include <stdlib.h>

#define CHICKEN_PROB 32

void spawnEnemies(struct World *world, float *worldheight, int worldwidth)
{
	//Spawn chickens
	for(int i = 0; i < worldwidth; i++)
	{
		float x = i * BLOCK_SIZE - BLOCK_SIZE * worldwidth / 2.0f,
			  y = (int)worldheight[i] * BLOCK_SIZE + BLOCK_SIZE;

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
