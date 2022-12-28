#include "world.h"

void revealVisible(struct World *world) 
{
	static const int neighborX[] = { 1, -1, 0, 0, 0 },
					 neighborY[] = { 0,  0, 1, -1, 0 };

	for(int x = world->worldBoundingRect.minX; x <= world->worldBoundingRect.maxX; x++)
	{
		for(int y = world->worldBoundingRect.minY; y <= world->worldBoundingRect.maxY; y++)
		{
			setBlockVisibility(world->backgroundBlocks, x, y, world->blockArea, REVEALED, world->worldBoundingRect);
			setBlockVisibility(world->transparentBlocks, x, y, world->blockArea, REVEALED, world->worldBoundingRect);	
		
			int foundEmpty = 0;
			for(int i = 0; i < 5 && !foundEmpty; i++)
			{
				if(getBlock(world->blocks, x + neighborX[i], y + neighborY[i], world->blockArea, world->worldBoundingRect).type == NONE ||
					getBlock(world->blocks, x + neighborX[i], y + neighborY[i], world->blockArea, world->worldBoundingRect).type == WATER ||
					getBlock(world->blocks, x + neighborX[i], y + neighborY[i], world->blockArea, world->worldBoundingRect).type == LAVA || 
					getBlock(world->blocks, x + neighborX[i], y + neighborY[i], world->blockArea, world->worldBoundingRect).type == LEAF)
					foundEmpty = 1;
			}
			if(foundEmpty)
				setBlockVisibility(world->blocks, x, y, world->blockArea, REVEALED, world->worldBoundingRect);	
		}
	}

	for(int x = world->worldBoundingRect.minX; x <= world->worldBoundingRect.maxX; x++)
	{
		for(int y = world->worldBoundingRect.minY; y <= world->worldBoundingRect.maxY; y++)
		{
			int foundVisible = 0;
			for(int i = 0; i < 4 && !foundVisible; i++)
			{
				if(getBlock(world->blocks, x + neighborX[i], y + neighborY[i], world->blockArea, world->worldBoundingRect).visibility == REVEALED)
					foundVisible = 1;
			}
			if(foundVisible && getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).visibility == HIDDEN)
				setBlockVisibility(world->blocks, x, y, world->blockArea, DARK, world->worldBoundingRect);	
		}
	}
}

void revealNeighbors(struct World *world, int x, int y)
{
	static const int neighborX[] = { 1, -1, 0, 0 },
					 neighborY[] = { 0,  0, 1, -1 };	
	static const int neighbor2X[] = { 2, 1, 0, -1, -2, -1, 0, 1 },
					 neighbor2Y[] = { 0, 1, 2, 1,  0,  -1, -2, -1 };
	setBlockVisibility(world->blocks, x, y, world->blockArea, REVEALED, world->worldBoundingRect);
	for(int i = 0; i < 4; i++)
	{
		if(getBlock(world->blocks, x + neighborX[i], y + neighborY[i], world->blockArea, world->worldBoundingRect).visibility != REVEALED)
			setBlockVisibility(world->blocks, x + neighborX[i], y + neighborY[i], world->blockArea, REVEALED, world->worldBoundingRect);
	}
	for(int i = 0; i < 8; i++)
	{
		if(getBlock(world->blocks, x + neighbor2X[i], y + neighbor2Y[i], world->blockArea, world->worldBoundingRect).visibility == HIDDEN)
			setBlockVisibility(world->blocks, x + neighbor2X[i], y + neighbor2Y[i], world->blockArea, DARK, world->worldBoundingRect);
	}
}
