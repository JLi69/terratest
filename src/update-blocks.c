#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "draw.h"

#define MAX_LIQUID 1.0f
#define MAX_COMPRESS 0.0f
#define MIN_LIQUID 0.001f
#define MIN_FLOW 1.0f
#define MAX_SPEED 8.0f

void drawBlocks(struct Block *blocks,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  int maxIndex,
				  struct BoundingRect boundRect, float brightness)
{
	enum BlockType prev = NONE;
	for(int x = camPos.x / BLOCK_SIZE - viewDistX; x <= camPos.x / BLOCK_SIZE + viewDistX; x++)
	{
		for(int y = camPos.y / BLOCK_SIZE - viewDistY; y <= camPos.y / BLOCK_SIZE + viewDistY; y++)
		{
			if(getBlock(blocks, x, y, maxIndex, boundRect).type == NONE ||
				getBlock(blocks, x, y, maxIndex, boundRect).type == WATER ||
				getBlock(blocks, x, y, maxIndex, boundRect).type == LAVA)
				continue;

			if(getBlock(blocks, x, y, maxIndex, boundRect).type != prev)
			{	
				setTexOffset(1.0f / 16.0f * (float)((getBlock(blocks, x, y, maxIndex, boundRect).type - 1) % 16),
							 1.0f / 16.0f * (float)((getBlock(blocks, x, y, maxIndex, boundRect).type - 1) / 16));
				prev = getBlock(blocks, x, y, maxIndex, boundRect).type; 	
				if(prev == WATER) //Transparent water
					setTransparency(0.7f);
				else
					setTransparency(1.0f);
			}

			switch(getBlock(blocks, x, y, maxIndex, boundRect).visibility)
			{
			case REVEALED: setBrightness(brightness); break;
			case DARK: setBrightness(brightness * 0.2f); break;
			case HIDDEN: setBrightness(0.0f); break;
			}

			setRectPos(x * BLOCK_SIZE - camPos.x,
					   y * BLOCK_SIZE - camPos.y);	
			drawRect();
		}
	}

	setBrightness(1.0f);
	setTransparency(1.0f);
}

void drawLiquids(struct Block *blocks,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  int maxIndex,
				  struct BoundingRect boundRect, float brightness)
{
	enum BlockType prev = NONE;
	for(int x = camPos.x / BLOCK_SIZE - viewDistX; x <= camPos.x / BLOCK_SIZE + viewDistX; x++)
	{
		for(int y = camPos.y / BLOCK_SIZE - viewDistY; y <= camPos.y / BLOCK_SIZE + viewDistY; y++)
		{
			if(getBlock(blocks, x, y, maxIndex, boundRect).type != WATER &&
				getBlock(blocks, x, y, maxIndex, boundRect).type != LAVA)
				continue;
			
			switch(getBlock(blocks, x, y, maxIndex, boundRect).visibility)
			{
			case REVEALED: setBrightness(brightness); break;
			case DARK: setBrightness(brightness * 0.2f); break;
			case HIDDEN: setBrightness(0.0f); break;
			}

			if(getBlock(blocks, x, y, maxIndex, boundRect).type != prev && 
			   getBlock(blocks, x, y, maxIndex, boundRect).visibility != HIDDEN)
			{	
				setTexOffset(1.0f / 16.0f * (float)((getBlock(blocks, x, y, maxIndex, boundRect).type - 1) % 16),
							 1.0f / 16.0f * (float)((getBlock(blocks, x, y, maxIndex, boundRect).type - 1) / 16));
				prev = getBlock(blocks, x, y, maxIndex, boundRect).type; 	
				if(prev == WATER) //Transparent water
					setTransparency(0.7f);
				else
					setTransparency(1.0f);
			}	

			if(getBlock(blocks, x, y, maxIndex, boundRect).mass < 0.95f)
				setLevel(getBlock(blocks, x, y, maxIndex, boundRect).mass);
			else
				setLevel(1.0f);
			setRectPos(x * BLOCK_SIZE - camPos.x,
					   y * BLOCK_SIZE - camPos.y);	
			drawRect();
		}
	}

	setLevel(1.0f);
	setBrightness(1.0f);
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

void updateBlocks(struct Block *blocks,
				  struct Vector2D camPos,
				  float timePassed, int updateDist,
				  int maxIndex, struct BoundingRect boundRect)
{
	struct Block* newBlocks = (struct Block*)malloc((updateDist * 2 + 1) * (updateDist * 2 + 1) * sizeof(struct Block));
	int area = (updateDist * 2 + 1) * (updateDist * 2 + 1);

	for(int i = 0; i < area; i++)
		newBlocks[i] = createBlock(NONE, 0.0f);

	float flow = 0.0f;
	float remainingMass = 0.0f;

	int minX = (int)(camPos.x / BLOCK_SIZE) - updateDist,
		minY = (int)(camPos.y / BLOCK_SIZE) - updateDist,
		sz = updateDist * 2 + 1;	

	for(int x = (int)(camPos.x / BLOCK_SIZE) - updateDist; x <= (int)(camPos.x / BLOCK_SIZE) + updateDist; x++)
	{
		for(int y = (int)(camPos.y / BLOCK_SIZE) + updateDist; y >= (int)(camPos.y / BLOCK_SIZE) - updateDist; y--)
		{	
			//If it's lava, check neighbors and find if it is neighboring water, if it is, then turn it stone
			if(getBlock(blocks, x, y, maxIndex, boundRect).type == LAVA)
			{
				int xoff[] = { 1, -1, 0,  0 };
				int yoff[] = { 0,  0, 1, -1 };
				for(int i = 0; i < 4; i++)
				{
					if(getBlock(blocks, x + xoff[i], y + yoff[i], maxIndex, boundRect).type == WATER &&
						getBlock(blocks, x + xoff[i], y + yoff[i], maxIndex, boundRect).mass >= 0.2f)
					{	
						newBlocks[(x - minX) + (y - minY) * sz].type = STONE;	
						newBlocks[(x - minX) + (y - minY) * sz].mass = 1.0f;
						setBlockType(blocks, x, y, maxIndex, STONE, boundRect);
					}
					else if(getBlock(blocks, x + xoff[i], y + yoff[i], maxIndex, boundRect).type == WATER && 
							getBlock(blocks, x + xoff[i], y + yoff[i], maxIndex, boundRect).mass < 0.2f)
					{
						if(x - minX + xoff[i] >= 0 && x - minX + xoff[i] < sz && y - minY + yoff[i] >= 0 && y - minY + yoff[i] < sz)
						{
							newBlocks[(x - minX + xoff[i]) + (y - minY + yoff[i]) * sz].type = NONE;	
							newBlocks[(x - minX + xoff[i]) + (y - minY + yoff[i]) * sz].mass = 0.0f;		
							setBlockType(blocks, x + xoff[i], y + yoff[i], maxIndex, NONE, boundRect);	
						}
					}
				}
			}	
		}
	}

	for(int x = (int)(camPos.x / BLOCK_SIZE) - updateDist; x <= (int)(camPos.x / BLOCK_SIZE) + updateDist; x++)
	{
		for(int y = (int)(camPos.y / BLOCK_SIZE) + updateDist; y >= (int)(camPos.y / BLOCK_SIZE) - updateDist; y--)
		{	
			//Check if grass is under a block, if it is, try to die
			if(getBlock(blocks, x, y, maxIndex, boundRect).type == GRASS)
			{
				if(getBlock(blocks, x, y + 1, maxIndex, boundRect).type != NONE &&
					getBlock(blocks, x, y + 1, maxIndex, boundRect).type != WATER &&
					getBlock(blocks, x, y + 1, maxIndex, boundRect).type != DOOR_BOTTOM_OPEN && 
					getBlock(blocks, x, y + 1, maxIndex, boundRect).type != DOOR_BOTTOM_CLOSED && 
					rand() % 16 == 0)
					newBlocks[x - minX + (y - minY) * sz].type = DIRT;	
				else
					newBlocks[x - minX + (y - minY) * sz].type = GRASS;	
				newBlocks[x - minX + (y - minY) * sz].mass = 1.0f;
			}
			//Dirt, try to spread grass
			else if(getBlock(blocks, x, y, maxIndex, boundRect).type == DIRT)
			{
				if(getBlock(blocks, x, y + 1, maxIndex, boundRect).type != NONE)
				{
					newBlocks[x - minX + (y - minY) * sz].type = DIRT;		
					newBlocks[(x - minX) + (y - minY) * sz].mass = 1.0f;
					continue;
				}
				int xoff[] = { 1, -1, 1, -1, 1, -1 };
				int yoff[] = { 0,  0, 1, 1, -1, -1 };
				int found = 0;
				for(int i = 0; i < 6 && !found; i++)
				{
					if(getBlock(blocks, x + xoff[i], y + yoff[i], maxIndex, boundRect).type == GRASS && rand() % 4096 == 0)
					{	
						newBlocks[(x - minX) + (y - minY) * sz].type = GRASS;	
						newBlocks[(x - minX) + (y - minY) * sz].mass = 1.0f;
						found = 1;	
					}
				}

				if(!found)
				{
					newBlocks[x - minX + (y - minY) * sz].type = DIRT;		
					newBlocks[(x - minX) + (y - minY) * sz].mass = 1.0f;
				}
			}	
			//Solid
			else if(getBlock(blocks, x, y, maxIndex, boundRect).type != WATER &&
			   getBlock(blocks, x, y, maxIndex, boundRect).type != LAVA &&
			   getBlock(blocks, x, y, maxIndex, boundRect).type != NONE)
			{
				newBlocks[x - minX + (y - minY) * sz].type = getBlock(blocks, x, y, maxIndex, boundRect).type;	
				newBlocks[x - minX + (y - minY) * sz].mass = 1.0f;	
			}
			//Liquid
			else if(getBlock(blocks, x, y, maxIndex, boundRect).type == WATER ||
					getBlock(blocks, x, y, maxIndex, boundRect).type == LAVA)
			{	
				flow = 0.0f;

				remainingMass = getBlock(blocks, x, y, maxIndex, boundRect).mass;
				newBlocks[x - minX + (y - minY) * sz].type = getBlock(blocks, x, y, maxIndex, boundRect).type;	
				newBlocks[x - minX + (y - minY) * sz].mass += remainingMass;

				if(getBlock(blocks, x, y - 1, maxIndex, boundRect).type == NONE ||
				   getBlock(blocks, x, y - 1, maxIndex, boundRect).type == getBlock(blocks, x, y, maxIndex, boundRect).type)
				{
					flow = getStableState(remainingMass + getBlock(blocks, x, y - 1, maxIndex, boundRect).mass) 
						   - getBlock(blocks, x, y - 1, maxIndex, boundRect).mass;	
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, minVal(MAX_SPEED, remainingMass));
					if(y - minY - 1 >= 0)
					{
						newBlocks[x - minX + (y - minY) * sz].mass -= flow;	
						newBlocks[x - minX + (y - minY - 1) * sz].mass += flow;	
						newBlocks[x - minX + (y - minY - 1) * sz].type = getBlock(blocks, x, y, maxIndex, boundRect).type;		
						remainingMass -= flow;
					} 
				}

				if(remainingMass <= 0) continue;

				if((getBlock(blocks, x - 1, y, maxIndex, boundRect).type == NONE ||
					getBlock(blocks, x - 1, y, maxIndex, boundRect).type == getBlock(blocks, x, y, maxIndex, boundRect).type)
					&& getBlock(blocks, x - 1, y, maxIndex, boundRect).mass >= 0.0f)
				{
					flow = (getBlock(blocks, x, y, maxIndex, boundRect).mass - getBlock(blocks, x - 1, y, maxIndex, boundRect).mass) / 4.0f;
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, remainingMass);
					if(x - minX - 1 >= 0)
					{
						newBlocks[(x - minX) + (y - minY) * sz].mass -= flow;	
						newBlocks[(x - minX - 1) + (y - minY) * sz].mass += flow;	
						newBlocks[x - minX - 1 + (y - minY) * sz].type = getBlock(blocks, x, y, maxIndex, boundRect).type;			
						remainingMass -= flow;
					}  
				}

				if(remainingMass <= 0) continue;
			
				if((getBlock(blocks, x + 1, y, maxIndex, boundRect).type == NONE ||
					getBlock(blocks, x + 1, y, maxIndex, boundRect).type == getBlock(blocks, x, y, maxIndex, boundRect).type)
					&& getBlock(blocks, x + 1, y, maxIndex, boundRect).mass >= 0.0f)
				{
					flow = (getBlock(blocks, x, y, maxIndex, boundRect).mass - getBlock(blocks, x + 1, y, maxIndex, boundRect).mass) / 4.0f;
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, remainingMass);
					if(x - minX + 1 < sz)
					{
						newBlocks[(x - minX) + (y - minY) * sz].mass -= flow;	
						newBlocks[(x - minX + 1) + (y - minY) * sz].mass += flow;
						newBlocks[x - minX + 1 + (y - minY) * sz].type = getBlock(blocks, x, y, maxIndex, boundRect).type;		
						remainingMass -= flow;
					} 
				}

				if(getBlock(blocks, x, y + 1, maxIndex, boundRect).type == NONE ||
					getBlock(blocks, x, y + 1, maxIndex, boundRect).type == getBlock(blocks, x, y, maxIndex, boundRect).type)
				{
					flow = remainingMass - getStableState(remainingMass + getBlock(blocks, x, y + 1, maxIndex, boundRect).mass);
					if(flow > MIN_FLOW)
						flow *= 0.5f;
					flow = constrain(flow, 0.0f, minVal(MAX_SPEED, remainingMass));
					if(y - minY + 1 < sz)
					{
						newBlocks[x - minX + (y - minY) * sz].mass -= flow;	
						newBlocks[x - minX + (y - minY + 1) * sz].mass += flow;
						newBlocks[x - minX + (y - minY + 1) * sz].type = getBlock(blocks, x, y, maxIndex, boundRect).type;		
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
			setBlockType(blocks, x, y, maxIndex, newBlocks[x - minX + (y - minY) * sz].type, boundRect);  
			setBlockMass(blocks, x, y, maxIndex, newBlocks[x - minX + (y - minY) * sz].mass, boundRect);
			//if(getBlock(blocks, x, y, maxIndex, boundRect).type != SOLID) continue;
			if(getBlock(blocks, x, y, maxIndex, boundRect).mass <= MIN_LIQUID ||
			  newBlocks[(x - minX) + (y - minY) * sz].type == NONE)	
			{
				setBlockType(blocks, x, y, maxIndex, NONE, boundRect);  		
				setBlockMass(blocks, x, y, maxIndex, 0.0f, boundRect);
			}	
		}
	}

	free(newBlocks);
}

int touching(struct World world, float x, float y, enum BlockType type, float massThreshold)
{
	int xVals[] = { floorf(x), ceilf(x), floorf(x), ceilf(x) };
	int yVals[] = { floorf(y), floorf(y), ceilf(y), ceilf(y) };
	for(int i = 0; i < 4; i++)
	{
		if(getBlock(world.blocks, xVals[i], yVals[i], world.blockArea, world.worldBoundingRect).type == type &&
			getBlock(world.blocks, xVals[i], yVals[i], world.blockArea, world.worldBoundingRect).mass >= massThreshold ||
			getBlock(world.transparentBlocks, xVals[i], yVals[i], world.blockArea, world.worldBoundingRect).type == type &&
			getBlock(world.transparentBlocks, xVals[i], yVals[i], world.blockArea, world.worldBoundingRect).mass >= massThreshold)
			return 1;
	}
	return 0;
}
