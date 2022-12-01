#ifndef WORLD_H
#include "quadtree.h"

#define WORLD_WIDTH 8192
#define MIN_CAVE_VALUE -0.2f
#define MAX_CAVE_VALUE 0.0f

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
#define IRON_PROB 512
#define DIAMOND_PROB 4096

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
	DIAMOND
};

struct World
{
	struct SpriteQuadTree *solidBlocks, //Blocks that can't be walked through
						  *transparentBlocks; //Blocks that can be walked through
};

struct World generateWorld(
	int seed,
	float amp,
	int interval
	);
void drawSpriteTree(struct SpriteQuadTree *tree, struct Vector2D camPos);
void drawQTreeOutline(struct SpriteQuadTree *tree,
					  float x, float y, float width, float height);

#endif
#define WORLD_H
