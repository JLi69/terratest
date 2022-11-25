#include "world.h"
#include <stdlib.h>
#include "draw.h"

struct World generateTest(void)
{
	struct World world;

	//Create the blocks
	world.blocks = createQuadTree(createPoint(-32000000000.0f, -1000.0f),
									  createPoint(32000000000.0f, 1000.0f)); 
		
	insert(world.blocks, createSprite(createRect(16.0f, -32.0f, 32.0f, 32.0f)));		
	insert(world.blocks, createSprite(createRect(16.0f, 0.0f, 32.0f, 32.0f)));	
	for(float i = -6.0f; i <= 6.0f; i++)
		for(float j = 0.0f; j <= 3.0f; j++)
			insert(world.blocks, createSprite(createRect(i * 32.0f, -64.0f - j * 32.0f, 32.0f, 32.0f)));	
	
	insert(world.blocks, createSprite(createRect(-112.0f, 64.0f, 32.0f, 32.0f)));		
	insert(world.blocks, createSprite(createRect(-80.0f, 64.0f, 32.0f, 32.0f)));	
	insert(world.blocks, createSprite(createRect(-144.0f, 64.0f, 32.0f, 32.0f)));		
	insert(world.blocks, createSprite(createRect(-112.0f, 64.0f, 32.0f, 32.0f)));

	return world;
}

void drawSpriteTree(struct SpriteQuadTree *tree, struct Vector2D camPos)
{
	struct Rectangle camViewBox = createRect(camPos.x, camPos.y, 2000.0f, 1200.0f),
					 quadBound = createRectFromCorner(tree->botLeftCorner, tree->topRightCorner);
	if(!colliding(camViewBox, quadBound))
		return;

	//Leaf node
	if(tree->botLeft == NULL &&
	   tree->botRight == NULL &&
	   tree->topLeft == NULL &&
	   tree->botRight == NULL)
	{
		for(int i = 0; i < tree->spriteCount; i++)
		{
			setRectPos(tree->sprites[i].hitbox.position.x, tree->sprites[i].hitbox.position.y);
			drawRect();
		}	
	}
	else
	{
		drawSpriteTree(tree->botLeft, camPos);
		drawSpriteTree(tree->botRight, camPos);
		drawSpriteTree(tree->topLeft, camPos);
		drawSpriteTree(tree->topRight, camPos);
	}
}
