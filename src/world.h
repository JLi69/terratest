//TODO: IMPROVE MEMORY USAGE
//Don't have the entire world loaded into memory

#ifndef WORLD_H
#include "sprite.h"
#include "inventory.h"

#define WORLD_WIDTH 8192
#define MIN_CAVE_VALUE -0.2f
#define MAX_CAVE_VALUE 0.1f

#define BLOCK_SIZE 32.0f

#define TREE_MIN_HEIGHT 8
#define TREE_MAX_HEIGHT 12
#define TREE_PROB 8
#define FLOWER_PROB 4
#define STUMP_PROB 16
#define TALL_GRASS_PROB 2
#define VINE_PROB 12
#define MIN_VINE_LEN 4
#define MAX_VINE_LEN 8

#define COAL_PROB 128
#define IRON_PROB 256
#define DIAMOND_PROB 640
#define GOLD_PROB 384
#define RAINBOW_PROB 2048

#define WATER_LEVEL 230.0f

#define MAX_CLOUD 32

#define MAX_ITEMS 8192
#define ITEM_SIZE 16.0f
#define TIME_TO_DESPAWN 300.0f

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

struct Block 
{
	enum BlockType type;
	float mass; //a value between 0.0 and 1.0 though if it is under more
				//water then it could be slightly higher than 1.0
	enum Visibility visibility;
};

struct World
{
	int blockArea; //Maximum number of blocks that can fit into the world
				   //if they are all arranged in a grid pattern
	struct Block *blocks, //Solid blocks and liquid blocks 
				 *transparentBlocks, //Blocks that can be walked through
				 *backgroundBlocks; 
	float dayCycle; //Current time of day in the world
	struct Sprite clouds[MAX_CLOUD];
	struct BoundingRect worldBoundingRect;
	
	struct Sprite droppedItems[MAX_ITEMS];
	int totalItems; 
};

struct World generateWorld(
	int seed,
	float amp,
	int interval
	);

//Run this function once
void revealVisible(struct World *world);
//Reveal neighbors of (x, y) along with (x, y)
void revealNeighbors(struct World *world, int x, int y);

//Liquid functions
void updateBlocks(struct Block *blocks,
				  struct Vector2D camPos,
				  float timePassed, int updateDist,
				  int maxIndex, struct BoundingRect boundRect);
void drawBlocks(struct Block *blocks,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  int maxIndex, struct BoundingRect boundRect, float brightness);
void drawLiquids(struct Block *blocks,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  int maxIndex, struct BoundingRect boundRect, float brightness);
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

int blockCollisionSearch(struct Sprite spr, int distX, int distY, struct Block *blocks,
						 int maxIndex, struct BoundingRect boundRect, struct Sprite *collided);

//returns 1 if the object is touching a block of certain type
//returns 0 if not
int touching(struct World world, int x, int y, enum BlockType type);

void addItem(struct World *world, enum Item item, float x, float y);
void drawItems(struct World world, struct Vector2D camPos,
				int viewDistX, int viewDistY);
void updateItems(struct World *world, struct Vector2D camPos, int simDist,
				 float timePassed, struct Sprite player);

#endif
#define WORLD_H
