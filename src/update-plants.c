#include "world.h"
#include <stdlib.h>
#include <math.h>

void updatePlants(struct World *world, struct Vector2D camPos, int simDist)
{
	for(int x = camPos.x / BLOCK_SIZE - simDist; x <= camPos.x / BLOCK_SIZE + simDist; x++)
	{
		for(int y = camPos.y / BLOCK_SIZE - simDist; y <= camPos.y / BLOCK_SIZE + simDist; y++)
		{
			if(getBlock(world->transparentBlocks, x, y, world->blockArea, world->worldBoundingRect).type >= WHEAT1 && 
				getBlock(world->transparentBlocks, x, y, world->blockArea, world->worldBoundingRect).type <= WHEAT3)
			{
				if(getBlock(world->blocks, x, y - 1, world->blockArea, world->worldBoundingRect).type != FARMLAND || 
					getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type != NONE)
				{
					setBlockType(world->transparentBlocks,
						x, y, world->blockArea,
						NONE, 
						world->worldBoundingRect);
					addItem(world, itemAmt(SEED_ITEM, 1), x * BLOCK_SIZE, y * BLOCK_SIZE);
				}	
				//Grow
				if(rand() % 3000 == 0)
					setBlockType(world->transparentBlocks,
						x, y, world->blockArea,
						getBlock(world->transparentBlocks,
								 x, y, world->blockArea,
								 world->worldBoundingRect).type + 1, 
						world->worldBoundingRect);
			}
			else if(getBlock(world->transparentBlocks, x, y, world->blockArea, world->worldBoundingRect).type == WHEAT4)
			{
				if(getBlock(world->blocks, x, y - 1, world->blockArea, world->worldBoundingRect).type != FARMLAND ||
					getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type != NONE)
				{
					setBlockType(world->transparentBlocks,
						x, y, world->blockArea,
						NONE, 
						world->worldBoundingRect);
					addItem(world, itemAmt(SEED_ITEM, 2), x * BLOCK_SIZE, y * BLOCK_SIZE);	
					addItem(world, itemAmt(WHEAT, 1), x * BLOCK_SIZE, y * BLOCK_SIZE);
				}
			}
			else if(getBlock(world->transparentBlocks, x, y, world->blockArea, world->worldBoundingRect).type == SAPLING)
			{
				if((getBlock(world->blocks, x, y - 1, world->blockArea, world->worldBoundingRect).type != DIRT &&
					getBlock(world->blocks, x, y - 1, world->blockArea, world->worldBoundingRect).type != GRASS) || 
					getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type != NONE)
				{
					setBlockType(world->transparentBlocks,
						x, y, world->blockArea,
						NONE, 
						world->worldBoundingRect);
					addItem(world, itemAmt(SAPLING_ITEM, 1), x * BLOCK_SIZE, y * BLOCK_SIZE);
				}

				int canGrow = 1;
				for(int i = 0; i < (TREE_MAX_HEIGHT + TREE_MIN_HEIGHT) / 2 && canGrow; i++)
					if(getBlock(world->blocks, x, y + i, world->blockArea, world->worldBoundingRect).type != NONE &&
						getBlock(world->blocks, x, y + i, world->blockArea, world->worldBoundingRect).type != LEAF)
						canGrow = 0;

				//Grow
				if(rand() % 20000 == 0 && canGrow)
				{
					struct Sprite* tempCollision;	

					int height = TREE_MIN_HEIGHT + rand() % (TREE_MAX_HEIGHT - TREE_MIN_HEIGHT + 1);
					for(int j = 1; j < height; j++)
					{
						if(getBlock(world->blocks, x, y + j, world->blockArea, world->worldBoundingRect).type == NONE || 
							getBlock(world->blocks, x, y + j, world->blockArea, world->worldBoundingRect).type == LEAF)
							setBlockType(world->transparentBlocks, x, y + j, world->blockArea, LOG, world->worldBoundingRect);
					}
					setBlockType(world->transparentBlocks, x, y, world->blockArea, STUMP, world->worldBoundingRect);

					struct Sprite leafBlock;
					int radius = sqrt(height * 3) > 6 ? 6 : sqrt(height * 3);
					for(int j = -radius; j <= radius; j++)
					{	
						for(int k = -radius; k <= radius; k++)
						{
							if(j * j + k * k < radius * radius && getBlock(world->blocks, x + k, y + height + j, world->blockArea, world->worldBoundingRect).type == NONE)
							{
								setBlockType(world->blocks, x + k, y + height + j, world->blockArea, LEAF, world->worldBoundingRect);
						
								//Vines
								if(rand() % VINE_PROB == 0)
								{
									int vineLength = rand() % (MAX_VINE_LEN - MIN_VINE_LEN + 1) + MIN_VINE_LEN;
									for(int v = 0; v < vineLength; v++)
									{
										if(getBlock(world->transparentBlocks, x + k, y + height + j - v, world->blockArea, world->worldBoundingRect).type != NONE &&
											getBlock(world->blocks, x + k, y + height + j - v, world->blockArea, world->worldBoundingRect).type == NONE)
											continue;
										setBlockType(world->transparentBlocks, x + k, y + height + j - v, world->blockArea, VINES, world->worldBoundingRect);	
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
