#include "world.h"
#include <stdlib.h>
#include "draw.h"
#include "sprite.h"
#include <stdio.h>
#include <math.h>

float interpolate(float a, float b, float weight)
{
	return (b - a) * weight + a;
}

struct World generateWorld(int seed, float amp, int interval)
{
	srand(seed);

	struct World world;
	world.solidBlocks = createQuadTree(createPoint(-WORLD_WIDTH * 32.0f * 2.0f, -4.0f * amp * 32.0f),
								  createPoint(WORLD_WIDTH * 32.0f * 2.0f, 16.0f * amp * 32.0f));
	world.transparentBlocks = createQuadTree(createPoint(-WORLD_WIDTH * 32.0f * 2.0f, -4.0f * amp * 32.0f),
								  createPoint(WORLD_WIDTH * 32.0f * 2.0f, 16.0f * amp * 32.0f));

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
			//Caves get bigger the deeper down you go
			if(caveValues[i + (int)y * WORLD_WIDTH] < (MAX_CAVE_VALUE - MIN_CAVE_VALUE) * (amp - y) / amp + MIN_CAVE_VALUE && y > 4.0f)
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

			//Place trees
			if(tempBlock.type == GRASS)
			{
				if(rand() % TREE_PROB == 0)
				{
					struct Sprite treeBlock = createSpriteWithType(
													createRect((float)i * BLOCK_SIZE - WORLD_WIDTH / 2.0f * BLOCK_SIZE,
													y * BLOCK_SIZE - amp * BLOCK_SIZE / 2.0f + BLOCK_SIZE,
													BLOCK_SIZE, BLOCK_SIZE), STUMP);
					insert(world.transparentBlocks, treeBlock);

					int height = TREE_MIN_HEIGHT + rand() % (TREE_MAX_HEIGHT - TREE_MIN_HEIGHT + 1);
					treeBlock.type = LOG;	
					for(int j = 0; j < height; j++)
					{
						treeBlock.hitbox.position.y += BLOCK_SIZE;
						struct Sprite* collision;
						if(collisionSearch(world.transparentBlocks, treeBlock, &collision))
						{	
							collision->type = LOG;
							continue;
						}
						insert(world.transparentBlocks, treeBlock);
					}

					struct Sprite leafBlock;
					int radius = sqrt(height * 3) > 6 ? 6 : sqrt(height * 3);
					for(int j = -radius; j <= radius; j++)
					{	
						for(int k = -radius; k <= radius; k++)
						{
							if(j * j + k * k < radius * radius)
							{
								leafBlock = createSpriteWithType(
													createRect(treeBlock.hitbox.position.x + j * BLOCK_SIZE,
													treeBlock.hitbox.position.y + k * BLOCK_SIZE,
													BLOCK_SIZE, BLOCK_SIZE), LEAF);
								insert(world.solidBlocks, leafBlock);
								
								//Vines
								if(rand() % VINE_PROB == 0)
								{
									int vineLength = rand() % (MAX_VINE_LEN - MIN_VINE_LEN + 1) + MIN_VINE_LEN;
									struct Sprite* collision;	
									for(int v = 0; v < vineLength; v++)
									{
										struct Sprite vineBlock = createSpriteWithType(
													createRect(leafBlock.hitbox.position.x,
													leafBlock.hitbox.position.y - v * BLOCK_SIZE,
													BLOCK_SIZE, BLOCK_SIZE), VINES);
										if(collisionSearch(world.solidBlocks, vineBlock, &collision))
											continue;

										insert(world.transparentBlocks, vineBlock);
									}
								}
							}
						}
					}
				}
				else if(rand() % STUMP_PROB == 0)
				{
					struct Sprite stumpBlock = createSpriteWithType(
													createRect((float)i * BLOCK_SIZE - WORLD_WIDTH / 2.0f * BLOCK_SIZE,
													y * BLOCK_SIZE - amp * BLOCK_SIZE / 2.0f + BLOCK_SIZE,
													BLOCK_SIZE, BLOCK_SIZE), STUMP);
					insert(world.transparentBlocks, stumpBlock);	
				}	
				else if(rand() % FLOWER_PROB == 0)
				{
					struct Sprite flowerBlock = createSpriteWithType(
													createRect((float)i * BLOCK_SIZE - WORLD_WIDTH / 2.0f * BLOCK_SIZE,
													y * BLOCK_SIZE - amp * BLOCK_SIZE / 2.0f + BLOCK_SIZE,
													BLOCK_SIZE, BLOCK_SIZE), FLOWER);
					insert(world.transparentBlocks, flowerBlock);	
				}
				else if(rand() % TALL_GRASS_PROB == 0)
				{
					struct Sprite tallGrassBlock = createSpriteWithType(
													createRect((float)i * BLOCK_SIZE - WORLD_WIDTH / 2.0f * BLOCK_SIZE,
													y * BLOCK_SIZE - amp * BLOCK_SIZE / 2.0f + BLOCK_SIZE,
													BLOCK_SIZE, BLOCK_SIZE), TALL_GRASS);
					insert(world.transparentBlocks, tallGrassBlock); 
				}
			}

			if(tempBlock.type == STONE)
			{
				if(rand() % COAL_PROB == 0)
					tempBlock.type = COAL;
				else if(rand() % IRON_PROB == 0 && y <= 128.0f)
					tempBlock.type = IRON;	
				else if(rand() % DIAMOND_PROB == 0 && y <= 32.0f)
					tempBlock.type = DIAMOND;	
			}

			insert(world.solidBlocks, tempBlock);
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

void drawQTreeOutline(struct SpriteQuadTree *tree, float x, float y, float width, float height)
{
	if(tree == NULL)
		return;

	setRectPos(x, y);
	setRectSize(width, height);
	drawRect();

	drawQTreeOutline(tree->botLeft, x - width / 4.0f, y - height / 4.0f, width / 2.0f, height / 2.0f);
	drawQTreeOutline(tree->topLeft, x - width / 4.0f, y + height / 4.0f, width / 2.0f, height / 2.0f);
	drawQTreeOutline(tree->topRight, x + width / 4.0f, y + height / 4.0f, width / 2.0f, height / 2.0f);
	drawQTreeOutline(tree->botRight, x + width / 4.0f, y - height / 4.0f, width / 2.0f, height / 2.0f);
}
