#ifndef QUADTREE_H
#include "sprite.h"
#define QUAD_CAPACITY 32

//Rectangle quadtree
struct SpriteQuadTree
{	
	union Point botLeftCorner, topRightCorner;
	int spriteCount;
	struct Sprite sprites[QUAD_CAPACITY];	
	struct SpriteQuadTree *botLeft,
						*botRight,
						*topLeft,
						*topRight;
};

//Creates an empty quadtree and returns the pointer to it
struct SpriteQuadTree* createQuadTree(
		union Point botleft, 
		union Point topright);
void destroyQuadTree(struct SpriteQuadTree *tree);
//Searches for collisions in the quadtree and then gives the
//pointer to the sprite that it first finds colliding with
//
//Returns 0 if found no collision, 1 if found
int collisionSearch(
		struct SpriteQuadTree *tree,
		struct Sprite sprite,
		struct Sprite **collision);
//Inserts a rectangle into the quadtree
void insert(
		struct SpriteQuadTree *tree,
		struct Sprite sprite);
void destroyTree(struct SpriteQuadTree *tree);

#endif
#define QUADTREE_H
