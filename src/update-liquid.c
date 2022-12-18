#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include "draw.h"

int getWorldGridWidth(struct SpriteQuadTree *world)
{
	return (int)((world->topRightCorner.x - world->botLeftCorner.x) / BLOCK_SIZE);
}

int getWorldGridHeight(struct SpriteQuadTree *world)
{
	return (int)((world->topRightCorner.y - world->botLeftCorner.y) / BLOCK_SIZE);
}

int translateWorldXToGridX(int x, struct SpriteQuadTree *world)
{
	return x - (int)(world->botLeftCorner.x / BLOCK_SIZE);
}

int translateWorldYToGridY(int y, struct SpriteQuadTree *world)
{
	return y - (int)(world->botLeftCorner.y / BLOCK_SIZE);
}

void drawLiquid(struct Liquid *liquids,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  struct SpriteQuadTree *world,
				  int maxIndex)
{	
	enum LiquidType prev = EMPTY_LIQUID;
	for(int x = camPos.x / BLOCK_SIZE - viewDistX; x <= camPos.x / BLOCK_SIZE + viewDistX; x++)
	{
		for(int y = camPos.y / BLOCK_SIZE - viewDistY; y <= camPos.y / BLOCK_SIZE + viewDistY; y++)
		{
			if(getLiquid(liquids, x, y, world, maxIndex).type == EMPTY_LIQUID ||
			   getLiquid(liquids, x, y, world, maxIndex).type == SOLID)
				continue;

			if(getLiquid(liquids, x, y, world, maxIndex).type != prev)
			{	
				setTexOffset(1.0f / 16.0f * (float)((getLiquid(liquids, x, y, world, maxIndex).type - 2) % 16),
							 1.0f / 16.0f * (float)((getLiquid(liquids, x, y, world, maxIndex).type - 2) / 16));
				prev = getLiquid(liquids, x, y, world, maxIndex).type; 	
				if(prev == WATER) //Transparent water
					setTransparency(0.7f);
			}

			setRectPos(x * BLOCK_SIZE - camPos.x,
					   y * BLOCK_SIZE - camPos.y);	
			drawRect();
		}
	}

	setTransparency(1.0f);
}

void updateLiquid(struct Liquid *liquids,
				  struct SpriteQuadTree *solidBlocks,
				  struct Vector2D camPos,
				  float timePassed, int updateDist)
{
	
}

struct Liquid getLiquid(struct Liquid *liquids,
						int x, int y, struct SpriteQuadTree *world, int maxIndex)
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, world);
	int gridY = translateWorldYToGridY(y, world);
	if(gridY * getWorldGridWidth(world) + gridX >= maxIndex || gridY * getWorldGridWidth(world) + gridX < 0)
		return createLiquid(EMPTY_LIQUID, -1.0f);
	return liquids[gridY * getWorldGridWidth(world) + gridX];
}

void setLiquidType(struct Liquid *liquids,
				   int x, int y, struct SpriteQuadTree *world, int maxIndex,
				   enum LiquidType type)
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, world);
	int gridY = translateWorldYToGridY(y, world);
	if(gridY * getWorldGridWidth(world) + gridX >= maxIndex || gridY * getWorldGridWidth(world) + gridX < 0)
		return;
	liquids[gridY * getWorldGridWidth(world) + gridX].type = type;
}

void setLiquidMass(struct Liquid *liquids, 
				   int x, int y, struct SpriteQuadTree *world,
				   int maxIndex,
				   float mass)
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, world);
	int gridY = translateWorldYToGridY(y, world);
	if(gridY * getWorldGridWidth(world) + gridX >= maxIndex || gridY * getWorldGridWidth(world) + gridX < 0)
		return;
	liquids[gridY * getWorldGridWidth(world) + gridX].mass = mass;
}

struct Liquid createLiquid(enum LiquidType type, float mass)
{
	struct Liquid block;
	block.type = type;
	block.mass = mass;
	return block;
}
