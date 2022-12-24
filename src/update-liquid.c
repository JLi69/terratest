#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include "draw.h"

#define MAX_LIQUID 1.0f
#define MAX_COMPRESS 0.0f
#define MIN_LIQUID 0.001f
#define MIN_FLOW 1.0f
#define MAX_SPEED 8.0f

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
				else
					setTransparency(1.0f);
			}

			setLevel(getLiquid(liquids, x, y, world, maxIndex).mass);
			setRectPos(x * BLOCK_SIZE - camPos.x,
					   y * BLOCK_SIZE - camPos.y);	
			drawRect();
		}
	}

	setLevel(1.0f);
	setTransparency(1.0f);
}

//Update liquid helper functions
float getStableState(float mass)
{
	if(mass <= 1.0f)
		return 1.0f;
	else if(mass < 2.0f * MAX_LIQUID + MAX_COMPRESS)
		return (MAX_LIQUID * MAX_LIQUID + mass * MAX_COMPRESS) / (MAX_LIQUID + MAX_COMPRESS);
	return (mass + MAX_COMPRESS) / 2.0f;
}

float constrain(float val, float min, float max)
{
	if(val < min)
		return min;
	if(val > max)
		return max;
	return val;
}

float minVal(float a, float b)
{
	return a > b ? b : a;
}

void updateLiquid(struct Liquid *liquids,
				  struct SpriteQuadTree *solidBlocks,
				  struct Vector2D camPos,
				  float timePassed, int updateDist,
				  int maxIndex)
{
	struct Liquid* newLiquids = (struct Liquid*)malloc((updateDist * 2 + 1) * (updateDist * 2 + 1) * sizeof(struct Liquid));
	int area = (updateDist * 2 + 1) * (updateDist * 2 + 1);

	for(int i = 0; i < area; i++)
		newLiquids[i] = createLiquid(EMPTY_LIQUID, 0.0f);

	float flow = 0.0f;
	float remainingMass = 0.0f;

	int minX = (int)(camPos.x / BLOCK_SIZE) - updateDist,
		minY = (int)(camPos.y / BLOCK_SIZE) - updateDist,
		sz = updateDist * 2 + 1;	

	for(int x = (int)(camPos.x / BLOCK_SIZE) - updateDist; x <= (int)(camPos.x / BLOCK_SIZE) + updateDist; x++)
	{
		for(int y = (int)(camPos.y / BLOCK_SIZE) + updateDist; y >= (int)(camPos.y / BLOCK_SIZE) - updateDist; y--)
		{	
			//Solid
			if(getLiquid(liquids, x, y, solidBlocks, maxIndex).type == SOLID)
			{
				newLiquids[x - minX + (y - minY) * sz].type = SOLID;	
				newLiquids[x - minX + (y - minY) * sz].mass = 1.0f;	
			}
			//Liquid
			else if(getLiquid(liquids, x, y, solidBlocks, maxIndex).type == WATER ||
					getLiquid(liquids, x, y, solidBlocks, maxIndex).type == LAVA)
			{
				//If it's lava, check neighbors and find if it is neighboring water, if it is, then turn it solid
				//and insert stone into the block quadtree
				if(getLiquid(liquids, x, y, solidBlocks, maxIndex).type == LAVA)
				{
					int xoff[] = { 1, -1, 0,  0 };
					int yoff[] = { 0,  0, 1, -1 };
					int found = 0;
					for(int i = 0; i < 4 && !found; i++)
					{
						if(getLiquid(liquids, x + xoff[i], y + yoff[i], solidBlocks, maxIndex).type == WATER)
						{	
							newLiquids[(x - minX) + (y - minY) * sz].type = SOLID;	
							newLiquids[(x - minX) + (y - minY) * sz].mass = 0.0f;
							insert(solidBlocks, createSpriteWithType(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE), STONE));	
							setLiquidType(liquids, x, y, solidBlocks, maxIndex, SOLID);
							found = 1;	
						}
					}

					if(found) continue;
				}

				flow = 0.0f;

				remainingMass = getLiquid(liquids, x, y, solidBlocks, maxIndex).mass;
				newLiquids[x - minX + (y - minY) * sz].type = getLiquid(liquids, x, y, solidBlocks, maxIndex).type;	
				newLiquids[x - minX + (y - minY) * sz].mass += remainingMass;

				if(getLiquid(liquids, x, y - 1, solidBlocks, maxIndex).type == EMPTY_LIQUID ||
				   getLiquid(liquids, x, y - 1, solidBlocks, maxIndex).type == getLiquid(liquids, x, y, solidBlocks, maxIndex).type)
				{
					flow = getStableState(remainingMass + getLiquid(liquids, x, y - 1, solidBlocks, maxIndex).mass) 
						   - getLiquid(liquids, x, y - 1, solidBlocks, maxIndex).mass;	
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, minVal(MAX_SPEED, remainingMass));
					if(y - minY - 1 >= 0)
					{
						newLiquids[x - minX + (y - minY) * sz].mass -= flow;	
						newLiquids[x - minX + (y - minY - 1) * sz].mass += flow;	
						newLiquids[x - minX + (y - minY - 1) * sz].type = getLiquid(liquids, x, y, solidBlocks, maxIndex).type;		
						remainingMass -= flow;
					} 
				}

				if(remainingMass <= 0) continue;

				if((getLiquid(liquids, x - 1, y, solidBlocks, maxIndex).type == EMPTY_LIQUID ||
					getLiquid(liquids, x - 1, y, solidBlocks, maxIndex).type == getLiquid(liquids, x, y, solidBlocks, maxIndex).type)
					&& getLiquid(liquids, x - 1, y, solidBlocks, maxIndex).mass >= 0.0f)
				{
					flow = (getLiquid(liquids, x, y, solidBlocks, maxIndex).mass - getLiquid(liquids, x - 1, y, solidBlocks, maxIndex).mass) / 4.0f;
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, remainingMass);
					if(x - minX - 1 >= 0)
					{
						newLiquids[(x - minX) + (y - minY) * sz].mass -= flow;	
						newLiquids[(x - minX - 1) + (y - minY) * sz].mass += flow;	
						newLiquids[x - minX - 1 + (y - minY) * sz].type = getLiquid(liquids, x, y, solidBlocks, maxIndex).type;			
						remainingMass -= flow;
					}  
				}

				if(remainingMass <= 0) continue;
			
				if((getLiquid(liquids, x + 1, y, solidBlocks, maxIndex).type == EMPTY_LIQUID ||
					getLiquid(liquids, x + 1, y, solidBlocks, maxIndex).type == getLiquid(liquids, x, y, solidBlocks, maxIndex).type)
					&& getLiquid(liquids, x + 1, y, solidBlocks, maxIndex).mass >= 0.0f)
				{
					flow = (getLiquid(liquids, x, y, solidBlocks, maxIndex).mass - getLiquid(liquids, x + 1, y, solidBlocks, maxIndex).mass) / 4.0f;
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, remainingMass);
					if(x - minX + 1 < sz)
					{
						newLiquids[(x - minX) + (y - minY) * sz].mass -= flow;	
						newLiquids[(x - minX + 1) + (y - minY) * sz].mass += flow;
						newLiquids[x - minX + 1 + (y - minY) * sz].type = getLiquid(liquids, x, y, solidBlocks, maxIndex).type;		
						remainingMass -= flow;
					} 
				}

				if(getLiquid(liquids, x, y + 1, solidBlocks, maxIndex).type == EMPTY_LIQUID ||
					getLiquid(liquids, x, y + 1, solidBlocks, maxIndex).type == getLiquid(liquids, x, y, solidBlocks, maxIndex).type)
				{
					flow = remainingMass - getStableState(remainingMass + getLiquid(liquids, x, y + 1, solidBlocks, maxIndex).mass);
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, minVal(MAX_SPEED, remainingMass));
					if(y - minY + 1 < sz)
					{
						newLiquids[x - minX + (y - minY) * sz].mass -= flow;	
						newLiquids[x - minX + (y - minY + 1) * sz].mass += flow;
						newLiquids[x - minX + (y - minY + 1) * sz].type = getLiquid(liquids, x, y, solidBlocks, maxIndex).type;		
						remainingMass -= flow;	
					}
				}

				if(remainingMass <= 0) continue;
			}
		}
	}

	for(int x = camPos.x / BLOCK_SIZE - updateDist; x <= camPos.x / BLOCK_SIZE + updateDist; x++)
	{
		for(int y = camPos.y / BLOCK_SIZE - updateDist; y <= camPos.y / BLOCK_SIZE + updateDist; y++)
		{
			if(x - minX + (y - minY) * sz >= area)
			{
				fprintf(stderr, "(%d, %d) (%d %d) %d %d OUT OF BOUNDS!\n", x, y, minX, minY, x - minX + (y - minY) * sz, area);
				continue;	
			}
			setLiquidType(liquids, x, y, solidBlocks, maxIndex, newLiquids[x - minX + (y - minY) * sz].type);  
			setLiquidMass(liquids, x, y, solidBlocks, maxIndex, newLiquids[x - minX + (y - minY) * sz].mass);
			if(getLiquid(liquids, x, y, solidBlocks, maxIndex).type == SOLID) continue;
			if(getLiquid(liquids, x, y, solidBlocks, maxIndex).mass <= MIN_LIQUID ||
			  newLiquids[(x - minX) + (y - minY) * sz].type == EMPTY_LIQUID)	
			{
				setLiquidType(liquids, x, y, solidBlocks, maxIndex, EMPTY_LIQUID);  		
				setLiquidMass(liquids, x, y, solidBlocks, maxIndex, 0.0f);
			}	
		}
	}

	free(newLiquids);
}

struct Liquid getLiquid(struct Liquid *liquids,
						int x, int y, struct SpriteQuadTree *world, int maxIndex)
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, world);
	int gridY = translateWorldYToGridY(y, world);
	if(x < (int)(world->botLeftCorner.x / BLOCK_SIZE) ||
	   x > (int)(world->topRightCorner.x / BLOCK_SIZE) ||
	   y < (int)(world->botLeftCorner.y / BLOCK_SIZE) ||
	   y > (int)(world->topRightCorner.y / BLOCK_SIZE))
		return createLiquid(EMPTY_LIQUID, -1.0f);
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
	if(x < (int)(world->botLeftCorner.x / BLOCK_SIZE) ||
	   x > (int)(world->topRightCorner.x / BLOCK_SIZE) ||
	   y < (int)(world->botLeftCorner.y / BLOCK_SIZE) ||
	   y > (int)(world->topRightCorner.y / BLOCK_SIZE))
		return;
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
	if(x < (int)(world->botLeftCorner.x / BLOCK_SIZE) ||
	   x > (int)(world->topRightCorner.x / BLOCK_SIZE) ||
	   y < (int)(world->botLeftCorner.y / BLOCK_SIZE) ||
	   y > (int)(world->topRightCorner.y / BLOCK_SIZE))
		return;
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
