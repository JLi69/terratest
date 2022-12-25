#include "world.h"
#include <stdlib.h>
#include "draw.h"
#include "sprite.h"
#include <stdio.h>
#include <math.h>
#include "window-func.h"

float interpolate(float a, float b, float weight)
{
	return (b - a) * weight + a;
}

void floodFill(int x, int y, int maxY, enum LiquidType liquid,
			   struct Liquid *liquidBlocks, 
			   struct SpriteQuadTree *world,
			   int worldArea, float amp, int depth)
{
	if(depth > 32000)
		return;

	if(x < world->botLeftCorner.x / BLOCK_SIZE + 1 ||
	   x > world->topRightCorner.x / BLOCK_SIZE - 1)
		return;
	if(y > maxY || y < world->botLeftCorner.y / BLOCK_SIZE)
		return;
	if(getLiquid(liquidBlocks, x, y - (int)(amp / 2.0f), world, worldArea).type != EMPTY_LIQUID)
		return;
	setLiquidType(liquidBlocks, x, y - (int)(amp / 2.0f), world, worldArea, liquid);
	setLiquidMass(liquidBlocks, x, y - (int)(amp / 2.0f), world, worldArea, 1.0f);

	floodFill(x - 1, y, maxY, liquid, liquidBlocks, world, worldArea, amp, depth + 1);
	floodFill(x + 1, y, maxY, liquid, liquidBlocks, world, worldArea, amp, depth + 1);
	floodFill(x, y - 1, maxY, liquid, liquidBlocks, world, worldArea, amp, depth + 1);
	floodFill(x, y + 1, maxY, liquid, liquidBlocks, world, worldArea, amp, depth + 1);
}

struct World generateWorld(int seed, float amp, int interval)
{	
	updateWindow();

	srand(seed);

	struct World world;
	world.dayCycle = 0.25f;	

	world.solidBlocks = createQuadTree(
								  createPoint(-WORLD_WIDTH * BLOCK_SIZE, -4.0f * amp * BLOCK_SIZE),
								  createPoint(WORLD_WIDTH * BLOCK_SIZE, 4.0f * amp * BLOCK_SIZE));
	world.backgroundBlocks = createQuadTree(
								  createPoint(-WORLD_WIDTH * BLOCK_SIZE, -4.0f * amp * BLOCK_SIZE),
								  createPoint(WORLD_WIDTH * BLOCK_SIZE, 4.0f * amp * BLOCK_SIZE));
	world.transparentBlocks = createQuadTree(
								  createPoint(-WORLD_WIDTH * BLOCK_SIZE, -4.0f * amp * BLOCK_SIZE),
								  createPoint(WORLD_WIDTH * BLOCK_SIZE, 4.0f * amp * BLOCK_SIZE));

	world.blockArea = WORLD_WIDTH * 2 * 8 * (int)amp;
	world.liquidBlocks = (struct Liquid*)malloc(sizeof(struct Liquid) * world.blockArea);
	//Set all liquid blocks as empty
	for(int i = 0; i < world.blockArea; i++)
		world.liquidBlocks[i] = createLiquid(EMPTY_LIQUID, 0.0f); //Fill world with empty liquid

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
		if(canQuit())
			exit(EXIT_SUCCESS);
		//Loading bar
		if(i % 128 == 0)	
		{
			clear();
			updateActiveShaderWindowSize();
			turnOffTexture();
			setRectColor(255.0f, 0.0f, 0.0f, 255.0f);
			setRectPos(0.0f, 0.0f);
			setRectSize(640.0f, 32.0f);	
			drawRect();
			setRectColor(0.0f, 255.0f, 0.0f, 255.0f);
			setRectPos(-320.0f + 320.0f * (float)i / (float)WORLD_WIDTH, 0.0f);
			setRectSize(640.0f * (float)i / (float)WORLD_WIDTH, 32.0f);
			drawRect();			
			updateWindow();
		}

		for(float y = -32.0f; y <= (worldHeight[i]); y += 1.0f)
		{
			struct Sprite tempBlock = createSpriteWithType(createRect(
														 (float)i * BLOCK_SIZE - WORLD_WIDTH / 2.0f * BLOCK_SIZE,
														 y * BLOCK_SIZE - amp * BLOCK_SIZE / 2.0f,
														 BLOCK_SIZE, BLOCK_SIZE), STONE);

			//Insert background blocks	
			if(y + 1.0f > worldHeight[i] && y <= WATER_LEVEL)
				tempBlock.type = SAND;	
			else if(y + 7.0f > worldHeight[i])
				tempBlock.type = DIRT;
			insert(world.backgroundBlocks, tempBlock);

			//Caves get bigger the deeper down you go
			if(y > 0 && caveValues[i + (int)y * WORLD_WIDTH] <
					(MAX_CAVE_VALUE - MIN_CAVE_VALUE) * (amp - y) / amp + MIN_CAVE_VALUE && y > 4.0f)
			{
				//Place lava into the world
				if(y < 16.0f)
				{
					setLiquidType(world.liquidBlocks, i - WORLD_WIDTH / 2, y - amp / 2.0f, world.solidBlocks,
								  world.blockArea, LAVA);
					setLiquidMass(world.liquidBlocks, i - WORLD_WIDTH / 2, y - amp / 2.0f, world.solidBlocks,
								  world.blockArea, 1.0f);	
				}
				continue;
			}

			if(y + 1.0f > worldHeight[i])
			{	
				tempBlock.type = GRASS;
				if(y <= WATER_LEVEL)
				{
					tempBlock.type = SAND;	
				}
			}
			
			//Bottom of world
			if((y < 0.0f) || (y <= 4.0f && (float)rand() / (float)RAND_MAX <= 1.0f / sqrtf(y + 1.0f)))
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
					struct Sprite* tempCollision;
					if(collisionSearch(world.transparentBlocks, treeBlock, &tempCollision))
						deleteSprite(world.transparentBlocks, *tempCollision);	
					insert(world.transparentBlocks, treeBlock);

					int height = TREE_MIN_HEIGHT + rand() % (TREE_MAX_HEIGHT - TREE_MIN_HEIGHT + 1);
					treeBlock.type = LOG;	
					for(int j = 0; j < height; j++)
					{
						treeBlock.hitbox.position.y += BLOCK_SIZE;
						struct Sprite* collision;	
						if(collisionSearch(world.transparentBlocks, treeBlock, &collision))	
							deleteSprite(world.transparentBlocks, *collision);	
						
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
							
								//Leaf is solid
								setLiquidType(world.liquidBlocks,
										treeBlock.hitbox.position.x / BLOCK_SIZE + j,
										treeBlock.hitbox.position.y / BLOCK_SIZE + k, world.solidBlocks,
										world.blockArea, SOLID);
						
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
				else if(rand() % IRON_PROB == 0 && y <= 200.0f)
					tempBlock.type = IRON;	
				else if(rand() % GOLD_PROB == 0 && y <= 128.0f)
					tempBlock.type = GOLD;		
				else if(rand() % DIAMOND_PROB == 0 && y <= 48.0f)
					tempBlock.type = DIAMOND;
				else if(rand() % RAINBOW_PROB == 0 && y <= 32.0f)
					tempBlock.type = RAINBOW_ORE;
				else if(y <= 32.0f && rand() % (int)(y * y * 0.05f + 1.0f) <= sqrtf(y))
					tempBlock.type = MAGMA_STONE;
			}

			insert(world.solidBlocks, tempBlock);	

			total++;

			setLiquidType(world.liquidBlocks, i - WORLD_WIDTH / 2, y - amp / 2.0f, world.solidBlocks,
								  world.blockArea, SOLID);
		}	
	}

	//Place water into the world
	for(int i = 0; i < WORLD_WIDTH; i++)
	{
		if(WATER_LEVEL < worldHeight[i])
			continue;
		floodFill(i - WORLD_WIDTH / 2.0f, WATER_LEVEL, WATER_LEVEL, WATER, 
				  world.liquidBlocks, world.solidBlocks, world.blockArea, amp, 0);
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
			if(!colliding(tree->sprites[i].hitbox, camViewBox) || 
			   (tree->sprites[i].hitbox.position.x >= tree->topRightCorner.x ||
			    tree->sprites[i].hitbox.position.y >= tree->topRightCorner.y))
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

#ifdef DEV_VERSION
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
#endif
