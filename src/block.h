#ifndef BLOCK_H
#include "inventory.h"

#define BLOCK_SIZE 32.0f

struct Block 
{
	uint8_t type;
	float mass; //a value between 0.0 and 1.0 though if it is under more
				//water then it could be slightly higher than 1.0
	uint8_t visibility;
};

enum Visibility
{
	REVEALED,
	DARK,
	HIDDEN
};

struct BoundingRect
{
	int minX, maxX, minY, maxY;
};

//Pass in X and Y as world coordinates, not grid coordinates
struct Block getBlock(struct Block *blocks,
						int x, int y,
						int maxIndex, struct BoundingRect boundRect);
void setBlockType(struct Block *blocks,
				   int x, int y,
				   int maxIndex,
				   enum BlockType type, struct BoundingRect boundRect);
void setBlockMass(struct Block *blocks, 
				   int x, int y,
				   int maxIndex,
				   float mass, struct BoundingRect boundRect);
void setBlockVisibility(struct Block *blocks, 
				   int x, int y,
				   int maxIndex,
				   enum Visibility visibility,
				   struct BoundingRect boundRect);
struct Block createBlock(enum BlockType type, float mass);

#endif
#define BLOCK_H
