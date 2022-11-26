#ifndef WORLD_H
#include "quadtree.h"

#define WORLD_WIDTH 1024
#define CAVE_VALUE 0.25f

#define BLOCK_SIZE 32.0f

enum SpriteType
{
	NONE,
	GRASS,
	DIRT,
	STONE,
	COAL,
	IRON,
	INDESTRUCTABLE,
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
