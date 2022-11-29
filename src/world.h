#ifndef WORLD_H
#include "quadtree.h"

#define WORLD_WIDTH 1024
#define MIN_CAVE_VALUE -0.2f
#define MAX_CAVE_VALUE 0.0f

#define BLOCK_SIZE 32.0f

#define TREE_MIN_HEIGHT 8
#define TREE_MAX_HEIGHT 12
#define TREE_PROB 8
#define FLOWER_PROB 4
#define STUMP_PROB 16
#define TALL_GRASS_PROB 2

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
};

struct World
{
	struct SpriteQuadTree* blocks;
};

struct World generateTest(void);
struct World generateWorld(
	int seed,
	float amp,
	int interval
	);
void drawSpriteTree(struct SpriteQuadTree *tree, struct Vector2D camPos);

#endif
#define WORLD_H
