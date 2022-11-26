#include "quadtree.h"
#include <stdlib.h>

struct SpriteQuadTree* createQuadTree(union Point botleft, union Point topright)
{
	struct SpriteQuadTree* tree = (struct SpriteQuadTree*)malloc(sizeof(struct SpriteQuadTree));

	tree->botLeftCorner = botleft;
	tree->topRightCorner = topright;
	tree->spriteCount = 0;
	tree->botLeft = tree->botRight = tree->topLeft = tree->topRight = NULL;

	return tree;
}

void destroyQuadTree(struct SpriteQuadTree *tree)
{
	if(tree == NULL)
		return;
	destroyQuadTree(tree->botLeft);
	destroyQuadTree(tree->botRight);
	destroyQuadTree(tree->topLeft);
	destroyQuadTree(tree->topRight);
	free(tree);
}

void insert(struct SpriteQuadTree *tree, struct Sprite sprite)
{
	//Invalid rectangle
	if(sprite.hitbox.dimensions.w <= 0 ||
	   sprite.hitbox.dimensions.h <= 0)
		return;
	struct Rectangle quadBound = createRectFromCorner(tree->botLeftCorner, tree->topRightCorner);
	if(!colliding(sprite.hitbox, quadBound))
		return;

	//Leaf node 
	if(tree->botLeft == NULL &&
	   tree->botRight == NULL &&
	   tree->topLeft == NULL &&
	   tree->topRight == NULL)
	{
		//Quad is too small, don't bother inserting
		if((quadBound.dimensions.w <= 0.0001f ||
		   quadBound.dimensions.h <= 0.0001f) &&
		   tree->spriteCount >= QUAD_CAPACITY)
			return;

		//Check if it already contains an object in that position	
		for(int i = 0; i < tree->spriteCount; i++)
		{
			//Don't insert it
			//TODO: perhaps add a way to keep track of multiple rectangles in
			//the same position
			if(tree->sprites[i].hitbox.position.x == sprite.hitbox.position.x &&
			   tree->sprites[i].hitbox.position.y == sprite.hitbox.position.y)
				return;
		}

		//It still has room
		if(tree->spriteCount < QUAD_CAPACITY)
			tree->sprites[tree->spriteCount++] = sprite;
		//Not enough room, split the node
		else
		{
			//Split the node
			union Point mid = midpoint(tree->botLeftCorner, tree->topRightCorner);
			tree->botLeft = createQuadTree(tree->botLeftCorner, mid);
			tree->topRight = createQuadTree(mid, tree->topRightCorner);

			tree->topLeft = createQuadTree(createPoint(tree->botLeftCorner.x, mid.y),
										   createPoint(mid.x, tree->topRightCorner.y));
			
			tree->botRight = createQuadTree(createPoint(mid.x, tree->botLeftCorner.y),
											createPoint(tree->topRightCorner.x, mid.y));
			
			//Insert all the objects into it
			for(int i = 0; i < tree->spriteCount; i++)
			{		
				insert(tree->botLeft, tree->sprites[i]);
				insert(tree->topLeft, tree->sprites[i]);
				insert(tree->botRight, tree->sprites[i]);
				insert(tree->topRight, tree->sprites[i]);
			}

			//Insert the sprite into the smaller quadtrees	
			insert(tree->botLeft, sprite);
			insert(tree->topLeft, sprite);
			insert(tree->botRight, sprite);
			insert(tree->topRight, sprite);
		}
	}
	//Not leaf node
	else
	{	
		union Point mid = midpoint(tree->botLeftCorner, tree->topRightCorner);
		insert(tree->botLeft, sprite);
		insert(tree->topLeft, sprite);
		insert(tree->botRight, sprite);
		insert(tree->topRight, sprite);
	}
}

int collisionSearch(
	struct SpriteQuadTree *tree,
	struct Sprite sprite,
	struct Sprite **collision)
{
	
	if(tree == NULL)
		return 0;

	struct Rectangle quadBound = createRectFromCorner(tree->botLeftCorner, tree->topRightCorner);
	if(!colliding(quadBound, sprite.hitbox))
		return 0;

	//Leaf node
	if(tree->botLeft == NULL &&
	   tree->botRight == NULL &&
	   tree->topLeft == NULL &&
	   tree->botRight == NULL)
	{
		for(int i = 0; i < tree->spriteCount; i++)
		{
			if(colliding(tree->sprites[i].hitbox, sprite.hitbox))
			{	
				*collision = &tree->sprites[i];
				return 1;
			}		
		}	
	}
	else
	{
		int found = collisionSearch(tree->botLeft, sprite, collision);
		if(found) return 1;	
		found = collisionSearch(tree->botRight, sprite, collision);			
		if(found) return 1;	
		found = collisionSearch(tree->topLeft, sprite, collision);
		if(found) return 1;	
		found = collisionSearch(tree->topRight, sprite, collision);
		if(found) return 1;	
	}
	return 0;
}

void destroyTree(struct SpriteQuadTree *tree)
{
	if(tree == NULL)
		return;
	destroyTree(tree->botLeft);
	destroyTree(tree->botRight);
	destroyTree(tree->topLeft);
	destroyTree(tree->topRight);
	free(tree);
}
