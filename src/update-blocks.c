#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include "draw.h"

#define MAX_LIQUID 1.0f
#define MAX_COMPRESS 0.0f
#define MIN_LIQUID 0.001f
#define MIN_FLOW 1.0f
#define MAX_SPEED 8.0f

int translateWorldXToGridX(int x, int minX)
{
	return x - minX;
}

int translateWorldYToGridY(int y, int minY)
{
	return y - minY;
}

int getWorldGridWidth(struct BoundingRect boundRect)
{
	return boundRect.maxX - boundRect.minX + 1;
}

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

			setLevel(getBlock(blocks, x, y, maxIndex, boundRect).mass);
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
			//Solid
			if(getBlock(blocks, x, y, maxIndex, boundRect).type != WATER &&
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
				//If it's lava, check neighbors and find if it is neighboring water, if it is, then turn it solid
				//and insert stone into the block quadtree
				if(getBlock(blocks, x, y, maxIndex, boundRect).type == LAVA)
				{
					int xoff[] = { 1, -1, 0,  0 };
					int yoff[] = { 0,  0, 1, -1 };
					int found = 0;
					for(int i = 0; i < 4 && !found; i++)
					{
						if(getBlock(blocks, x + xoff[i], y + yoff[i], maxIndex, boundRect).type == WATER)
						{	
							newBlocks[(x - minX) + (y - minY) * sz].type = STONE;	
							newBlocks[(x - minX) + (y - minY) * sz].mass = 1.0f;
							setBlockType(blocks, x, y, maxIndex, STONE, boundRect);
							found = 1;	
						}
					}

					if(found) continue;
				}

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

struct Block getBlock(struct Block *blocks,
						int x, int y, int maxIndex, struct BoundingRect boundRect) 
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, boundRect.minX);
	int gridY = translateWorldYToGridY(y, boundRect.minY);
	if(x < boundRect.minX ||
	   x > boundRect.maxX ||
	   y < boundRect.minY ||
	   y > boundRect.maxY)
		return createBlock(NONE, -1.0f);
	if(gridY * getWorldGridWidth(boundRect) + gridX >= maxIndex || gridY * getWorldGridWidth(boundRect) + gridX < 0)
		return createBlock(NONE, -1.0f);
	return blocks[gridY * getWorldGridWidth(boundRect) + gridX];
}

void setBlockType(struct Block *blocks,
				   int x, int y, int maxIndex,
				   enum BlockType type,
				   struct BoundingRect boundRect)
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, boundRect.minX);
	int gridY = translateWorldYToGridY(y, boundRect.minY);
	if(x < boundRect.minX ||
	   x > boundRect.maxX ||
	   y < boundRect.minY ||
	   y > boundRect.maxY)
		return;
	if(gridY * getWorldGridWidth(boundRect) + gridX >= maxIndex || gridY * getWorldGridWidth(boundRect) + gridX < 0)
		return;
	blocks[gridY * getWorldGridWidth(boundRect) + gridX].type = type;
}

void setBlockMass(struct Block *blocks, 
				   int x, int y,
				   int maxIndex,
				   float mass,
				   struct BoundingRect boundRect)
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, boundRect.minX);
	int gridY = translateWorldYToGridY(y, boundRect.minY);
	if(x < boundRect.minX ||
	   x > boundRect.maxX ||
	   y < boundRect.minY ||
	   y > boundRect.maxY)
		return;
	if(gridY * getWorldGridWidth(boundRect) + gridX >= maxIndex || gridY * getWorldGridWidth(boundRect) + gridX < 0)
		return;
	blocks[gridY * getWorldGridWidth(boundRect) + gridX].mass = mass;
}

void setBlockVisibility(struct Block *blocks, 
				   int x, int y,
				   int maxIndex,
				   enum Visibility visibility,
				   struct BoundingRect boundRect)
{
	//Translate x and y to be grid coordinates
	int gridX = translateWorldXToGridX(x, boundRect.minX);
	int gridY = translateWorldYToGridY(y, boundRect.minY);
	if(x < boundRect.minX ||
	   x > boundRect.maxX ||
	   y < boundRect.minY ||
	   y > boundRect.maxY)
		return;
	if(gridY * getWorldGridWidth(boundRect) + gridX >= maxIndex || gridY * getWorldGridWidth(boundRect) + gridX < 0)
		return;
	blocks[gridY * getWorldGridWidth(boundRect) + gridX].visibility = visibility;
}

int blockCollisionSearch(struct Sprite spr, int distX, int distY, struct Block *blocks,
						 int maxIndex, struct BoundingRect boundRect, struct Sprite *collided)
{
	for(int x = (int)(spr.hitbox.position.x / BLOCK_SIZE) - distX; x <= (int)(spr.hitbox.position.x / BLOCK_SIZE) + distX; x++)
	{
		for(int y = (int)(spr.hitbox.position.y / BLOCK_SIZE) - distY; y <= (int)(spr.hitbox.position.y / BLOCK_SIZE) + distY; y++)
		{
			if(getBlock(blocks, x, y, maxIndex, boundRect).type != NONE &&
				getBlock(blocks, x, y, maxIndex, boundRect).type != WATER &&
				getBlock(blocks, x, y, maxIndex, boundRect).type != LAVA)
			{
				struct Sprite block = createSprite(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
				if(colliding(block.hitbox, spr.hitbox))
				{
					*collided = block;
					return 1;
				}
			}
		}
	}

	return 0;
}

struct Block createBlock(enum BlockType type, float mass)
{
	struct Block block;
	block.type = type;
	block.mass = mass;
	block.visibility = HIDDEN;
	return block;
}
