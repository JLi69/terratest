#include "world.h"
#include <stdlib.h>
#include "draw.h"
#include "sprite.h"
#include <stdio.h>
#include <math.h>

struct World generateTest(void)
{
	struct World world;
	world.blocks = createQuadTree(createPoint(-32000000000.0f, -1000.0f),
									  createPoint(3200000000.0f, 1000.0f)); 
		
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

float interpolate(float a, float b, float weight)
{
	return (b - a) * weight + a;
}

struct World generateWorld(int seed, float amp, int interval)
{
	srand(seed);

	struct World world;
	world.blocks = createQuadTree(createPoint(-WORLD_WIDTH * 32.0f, -4.0f * amp * 32.0f),
								  createPoint(WORLD_WIDTH * 32.0f, 4.0f * amp * 32.0f));

	float worldHeight[WORLD_WIDTH];
	struct Vector2D randVecs1[WORLD_WIDTH + 1],
					randVecs2[WORLD_WIDTH + 1];
	for(int i = 0; i < WORLD_WIDTH; i++)
		worldHeight[i] = 0.0f;		

	const struct Vector2D corners[] = 
	{
		createVector(0.0f, 0.0f),
		createVector(1.0f, 0.0f),
		createVector(0.0f, 1.0f),
		createVector(1.0f, 1.0f),	
	};

	//Generate hills
	float amplitude = amp;
	for(int val = interval; val >= 2; val /= 2)
	{
		for(int i = 0; i < WORLD_WIDTH + 1; i++)
		{
			randVecs1[i] = generateUnit((float)(rand()) / (float)RAND_MAX * 2.0f * PI);
			randVecs2[i] = generateUnit((float)(rand()) / (float)RAND_MAX * 2.0f * PI);
		}

		for(int i = 0; i < WORLD_WIDTH; i++)
		{
			int ind1 = (i / val),
				ind2 = (i / val) + 1;	

			struct Vector2D vecs[] =
			{
				randVecs1[ind1],
				randVecs1[ind2],
				randVecs2[ind1],
				randVecs2[ind2]
			};

			struct Vector2D pt = createVector((float)(i % val) / (float)val, 0.5f);
			float dotProds[4];

			for(int j = 0; j < 4; j++)
			{
				struct Vector2D diff = createVector(pt.x - corners[j].x, pt.y - corners[j].y);
				dotProds[j] = dotProduct(vecs[j], diff);
			}

			float int1, int2, value;
			int1 = interpolate(dotProds[0], dotProds[1], pt.x);	
			int2 = interpolate(dotProds[2], dotProds[3], pt.x);
			value = interpolate(int1, int2, pt.y);
			worldHeight[i] += (amplitude + (value) * amplitude);
		}
		amplitude /= 2.0f;
	}

	//Generate cave systems
	int tileCount = (int)amp * 8 * 2 * WORLD_WIDTH;
	struct Vector2D* randVecsCaves = (struct Vector2D*)malloc(sizeof(struct Vector2D) * tileCount);	
	float* caveValues = (float*)malloc(sizeof(float) * tileCount);	
	for(int i = 0; i < tileCount; i++)
	{	
		randVecsCaves[i] = generateUnit((float)(rand()) / (float)RAND_MAX * 2.0f * PI);
		caveValues[i] = 0.0f;	
	}

	float mag = 1.0f;
	for(float val = 16.0f; val >= 4.0f; val /= 2.0f)
	{		
		for(int i = 0; i < (int)tileCount; i++)
		{
			int x = i % WORLD_WIDTH,
				y = (i - i % WORLD_WIDTH) / WORLD_WIDTH;
			int lowerX = x / val,
				lowerY = y / val,
				upperX = lowerX + 1,
				upperY = lowerY + 1;
			
			struct Vector2D vecs[] =
			{
				randVecsCaves[lowerX + WORLD_WIDTH * lowerY],
				randVecsCaves[upperX + WORLD_WIDTH * lowerY],
				randVecsCaves[lowerX + WORLD_WIDTH * upperY],
				randVecsCaves[upperX + WORLD_WIDTH * upperY]
			};
	
			struct Vector2D pt = createVector((float)x / val - floorf((float)x / val),
											  (float)y / val - floorf((float)y / val));
			float dotProds[4];

			for(int j = 0; j < 4; j++)
			{
				struct Vector2D diff = createVector(pt.x - corners[j].x, pt.y - corners[j].y);
				dotProds[j] = dotProduct(vecs[j], diff);
			}

			float int1, int2, value;
			int1 = interpolate(dotProds[0], dotProds[1], pt.x);	
			int2 = interpolate(dotProds[2], dotProds[3], pt.x);
			value = interpolate(int1, int2, pt.y);

			caveValues[y * WORLD_WIDTH + x] += value * mag;		
		}
		mag /= 2.0f;
	}
	free(randVecsCaves);
	
	int total = 0;
	for(int i = 0; i < WORLD_WIDTH; i++)
	{
		for(float y = 0.0f; y <= (worldHeight[i]); y += 1.0f)
		{
			if(caveValues[i + (int)y * WORLD_WIDTH] < -0.2f && y > 4.0f)
				continue;

			struct Sprite tempBlock = createSpriteWithType(createRect((float)i * BLOCK_SIZE - WORLD_WIDTH / 2.0f * BLOCK_SIZE,
														 y * BLOCK_SIZE - amp * BLOCK_SIZE / 2.0f,
														 BLOCK_SIZE, BLOCK_SIZE), STONE);
			if(y + 1.0f > worldHeight[i])
				tempBlock.type = GRASS;
			else if(y + 7.0f > worldHeight[i])
				tempBlock.type = DIRT;
			
			//Bottom of world
			if(y <= 4.0f && (float)rand() / (float)RAND_MAX <= 1.0f / sqrtf(y + 1.0f))
				tempBlock.type = INDESTRUCTABLE;

			insert(world.blocks, tempBlock);
			total++;
		}	
	}

	printf("Inserted %d blocks!\n", total);
	free(caveValues);

	return world;
}

//Draws out the world
void drawSpriteTree(struct SpriteQuadTree *tree, struct Vector2D camPos)
{
	static int prevSpriteType = NONE;

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
			if(!colliding(tree->sprites[i].hitbox, camViewBox))
				continue;
			if(tree->sprites[i].type != prevSpriteType)
			{
				setTexOffset(1.0f / 16.0f * (float)((tree->sprites[i].type - 1) % 16), 1.0f / 16.0f * (float)((tree->sprites[i].type - 1) / 16));
				prevSpriteType = tree->sprites[i].type; 
			}
			setRectPos(tree->sprites[i].hitbox.position.x - camPos.x,
					   tree->sprites[i].hitbox.position.y - camPos.y);
			drawRect();
		}	
		prevSpriteType = NONE;
	}
	else
	{
		drawSpriteTree(tree->botLeft, camPos);
		drawSpriteTree(tree->botRight, camPos);
		drawSpriteTree(tree->topLeft, camPos);
		drawSpriteTree(tree->topRight, camPos);
	}
}
