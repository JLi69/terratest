#ifndef WORLD_H
#include "quadtree.h"

struct World
{
	struct SpriteQuadTree* blocks;
};

struct World generateTest(void);
void drawSpriteTree(struct SpriteQuadTree *tree, struct Vector2D camPos);

#endif
#define WORLD_H
