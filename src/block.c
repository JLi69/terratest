#include "block.h"
#include "sprite.h"

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
				getBlock(blocks, x, y, maxIndex, boundRect).type != LAVA &&
				getBlock(blocks, x, y, maxIndex, boundRect).type != DOOR_TOP_OPEN &&
				getBlock(blocks, x, y, maxIndex, boundRect).type != DOOR_BOTTOM_OPEN)
			{
				struct Sprite block = createSpriteWithType(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE), 
															getBlock(blocks, x, y, maxIndex, boundRect).type);
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
