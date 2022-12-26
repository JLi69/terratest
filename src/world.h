//TODO: IMPROVE MEMORY USAGE
//Don't have the entire world loaded into memory

#ifndef WORLD_H
#include "quadtree.h"

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

#define SIM_DIST 3000.0f

#define MAX_CLOUD 32

enum SpriteType
{
	NONE = 0,
	GRASS,
	DIRT,
	STONE,
	INDESTRUCTABLE,
	BRICK,
	LEAF,
	LOG,
	STUMP,
	FLOWER,
	TALL_GRASS,
	VINES,
	COAL,
	IRON,
	DIAMOND,
	GOLD,
	RAINBOW_ORE,
	MAGMA_STONE,
	SAND
};

enum LiquidType
{
	EMPTY_LIQUID = 0,
	SOLID,
	WATER,
	LAVA
};

struct Liquid
{
	enum LiquidType type;
	float mass; //a value between 0.0 and 1.0 though if it is under more
				//water then it could be slightly higher than 1.0
};

struct World
{
	struct SpriteQuadTree *solidBlocks, //Blocks that can't be walked through
						  *transparentBlocks, //Blocks that can be walked through
						  *backgroundBlocks;	
	int blockArea; //Maximum number of blocks that can fit into the world
				   //if they are all arranged in a grid pattern
	struct Liquid* liquidBlocks; //Liquid blocks
	float dayCycle; //Current time of day in the world
	struct Sprite clouds[MAX_CLOUD];
};

struct World generateWorld(
	int seed,
	float amp,
	int interval
	);
void drawSpriteTree(struct SpriteQuadTree *tree, struct Vector2D camPos);

//Liquid functions
void updateLiquid(struct Liquid *liquids,
				  struct SpriteQuadTree *solidBlocks,
				  struct Vector2D camPos,
				  float timePassed, int updateDist,
				  int maxIndex);
void drawLiquid(struct Liquid *liquids,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  struct SpriteQuadTree *world, int maxIndex);
//Pass in X and Y as world coordinates, not grid coordinates
struct Liquid getLiquid(struct Liquid *liquids,
						int x, int y, struct SpriteQuadTree *world,
						int maxIndex);
void setLiquidType(struct Liquid *liquids,
				   int x, int y, struct SpriteQuadTree *world,
				   int maxIndex,
				   enum LiquidType type);
void setLiquidMass(struct Liquid *liquids, 
				   int x, int y, struct SpriteQuadTree *world,
				   int maxIndex,
				   float mass);
struct Liquid createLiquid(enum LiquidType type, float mass);

#ifdef DEV_VERSION
void drawQTreeOutline(struct SpriteQuadTree *tree,
					  float x, float y, float width, float height);
#endif

#endif
#define WORLD_H
