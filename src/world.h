#ifndef WORLD_H
#include "quadtree.h"

#define WORLD_WIDTH 16384
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

#define WATER_UPDATE_TIME 0.1f
#define LAVA_UPDATE_TIME 0.6f

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
	LAVA,
	WATER,
	SAND
};

struct World
{
	struct SpriteQuadTree *solidBlocks, //Blocks that can't be walked through
						  *transparentBlocks, //Blocks that can be walked through
						  *liquidBlocks; //Blocks that are liquids (water, lava)
};

void floodFill(float maxHeight, int type, float x, float y,
			   struct SpriteQuadTree *blocks,
			   struct SpriteQuadTree *solidBlocks,
			   float amp);
struct World generateWorld(
	int seed,
	float amp,
	int interval
	);
void drawSpriteTree(struct SpriteQuadTree *tree, struct Vector2D camPos);
void updateLiquid(struct SpriteQuadTree *liquids,
				  struct SpriteQuadTree *liquidRoot,
				  struct SpriteQuadTree *solidBlocks,
				  struct Vector2D camPos,
				  float timePassed);
#ifdef DEV_VERSION
void drawQTreeOutline(struct SpriteQuadTree *tree,
					  float x, float y, float width, float height);
#endif

#endif
#define WORLD_H
