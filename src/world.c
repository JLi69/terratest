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

void floodFill(int x, int y, int maxY, enum BlockType type,
			   struct Block *blocks, 
			   int worldArea, int depth, struct BoundingRect boundRect)
{
	if(depth > 16000)
		return;

	//printf("%d %d\n", x, y);

	if(x >= boundRect.maxX + 1 ||
	   x <= boundRect.minX - 1)
		return;
	if(y > maxY || y <= boundRect.minY)
		return;
	if(getBlock(blocks, x, y, worldArea, boundRect).type != NONE)
		return;

	setBlockType(blocks, x, y, worldArea, type, boundRect);
	setBlockMass(blocks, x, y, worldArea, 1.0f, boundRect);

	floodFill(x - 1, y, maxY, type, blocks, worldArea, depth + 1, boundRect);
	floodFill(x + 1, y, maxY, type, blocks, worldArea, depth + 1, boundRect);
	floodFill(x, y - 1, maxY, type, blocks, worldArea, depth + 1, boundRect);
	floodFill(x, y + 1, maxY, type, blocks, worldArea, depth + 1, boundRect);
}

struct World generateWorld(int seed, float amp, int interval)
{	
	updateWindow();

	srand(seed);

	struct World world;

	world.worldBoundingRect.minX = -WORLD_WIDTH / 2;
	world.worldBoundingRect.maxX = WORLD_WIDTH / 2;
	world.worldBoundingRect.minY = -4.0f * amp;
	world.worldBoundingRect.maxY = 4.0f * amp;
	printf("World bounds: [x: %d %d, y: %d %d]\n", world.worldBoundingRect.minX, world.worldBoundingRect.maxX, world.worldBoundingRect.minY, world.worldBoundingRect.maxY);

	world.blockArea = WORLD_WIDTH * 8 * (int)amp;

	world.dayCycle = 0.4f;	
	world.moonPhase = 0.2f;

	world.totalItems = 0;

	world.blocks = (struct Block*)malloc(sizeof(struct Block) * world.blockArea);
	world.backgroundBlocks = (struct Block*)malloc(sizeof(struct Block) * world.blockArea); 
	world.transparentBlocks = (struct Block*)malloc(sizeof(struct Block) * world.blockArea);
	//Set all liquid blocks as empty
	for(int i = 0; i < world.blockArea; i++)
		world.blocks[i] = world.backgroundBlocks[i] = world.transparentBlocks[i] = createBlock(NONE, 0.0f); //Fill world with empty blocks 
	world.enemies = createQuadTree(
			newpt(world.worldBoundingRect.minX * BLOCK_SIZE, world.worldBoundingRect.minY * BLOCK_SIZE),
			newpt(world.worldBoundingRect.maxX * BLOCK_SIZE, world.worldBoundingRect.maxY * BLOCK_SIZE));

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
		if(i % 512 == 0)	
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
			enum BlockType type = STONE;
			//Insert background blocks	
			if(y + 8.0f > worldHeight[i] && y <= WATER_LEVEL)
				type = SAND;	
			else if(y + 7.0f > worldHeight[i])
				type = DIRT;
			setBlockType(world.backgroundBlocks, i - WORLD_WIDTH / 2, y, world.blockArea, type, world.worldBoundingRect);

			//Caves get bigger the deeper down you go
			if(y > 0 && caveValues[i + (int)y * WORLD_WIDTH] <
					(MAX_CAVE_VALUE - MIN_CAVE_VALUE) * (amp - y) / amp + MIN_CAVE_VALUE && y > 4.0f)
			{
				//Place lava into the world
				if(y < 16.0f)
				{
					setBlockType(world.blocks, i - WORLD_WIDTH / 2, y,
								  world.blockArea, LAVA, world.worldBoundingRect);
					setBlockMass(world.blocks, i - WORLD_WIDTH / 2, y,
								  world.blockArea, 1.0f, world.worldBoundingRect);	
				}
				continue;
			}

			if(y + 1.0f > worldHeight[i])
				type = GRASS;	
			if(y <= WATER_LEVEL && y + 7.0f > worldHeight[i])
				type = SAND;	
			
			//Bottom of world
			if((y < 0.0f) || (y <= 4.0f && (float)rand() / (float)RAND_MAX <= 1.0f / sqrtf(y + 1.0f)))
				type = INDESTRUCTABLE;

			//Place trees
			if(type == GRASS)
			{	

				if(rand() % TREE_PROB == 0)
				{	
					struct Sprite* tempCollision;	

					int height = TREE_MIN_HEIGHT + rand() % (TREE_MAX_HEIGHT - TREE_MIN_HEIGHT + 1);
					for(int j = 1; j < height; j++)
					{
						setBlockType(world.transparentBlocks, i - WORLD_WIDTH / 2, y + j, world.blockArea, LOG, world.worldBoundingRect);
					}
					setBlockType(world.transparentBlocks, i - WORLD_WIDTH / 2, y + 1, world.blockArea, STUMP, world.worldBoundingRect);

					struct Sprite leafBlock;
					int radius = sqrt(height * 3) > 6 ? 6 : sqrt(height * 3);
					for(int j = -radius; j <= radius; j++)
					{	
						for(int k = -radius; k <= radius; k++)
						{
							if(j * j + k * k < radius * radius)
							{
								setBlockType(world.blocks, i - WORLD_WIDTH / 2 + k, y + height + j, world.blockArea, LEAF, world.worldBoundingRect);
						
								//Vines
								if(rand() % VINE_PROB == 0)
								{
									int vineLength = rand() % (MAX_VINE_LEN - MIN_VINE_LEN + 1) + MIN_VINE_LEN;
									for(int v = 0; v < vineLength; v++)
									{
										if(getBlock(world.transparentBlocks, i - WORLD_WIDTH / 2 + k, y + height + j - v, world.blockArea, world.worldBoundingRect).type != NONE &&
											getBlock(world.blocks, i - WORLD_WIDTH / 2 + k, y + height + j - v, world.blockArea, world.worldBoundingRect).type == NONE)
											continue;
										setBlockType(world.transparentBlocks, i - WORLD_WIDTH / 2 + k, y + height + j - v, world.blockArea, VINES, world.worldBoundingRect);	
									}
								}
							}
						}
					}
				}
				else if(rand() % STUMP_PROB == 0)
				{
					setBlockType(world.transparentBlocks, i - WORLD_WIDTH / 2, y + 1, world.blockArea, STUMP, world.worldBoundingRect);
				}	
				else if(rand() % FLOWER_PROB == 0)
				{	
					setBlockType(world.transparentBlocks, i - WORLD_WIDTH / 2, y + 1, world.blockArea, FLOWER, world.worldBoundingRect);		
				}
				else if(rand() % TALL_GRASS_PROB == 0)
				{
					setBlockType(world.transparentBlocks, i - WORLD_WIDTH / 2, y + 1, world.blockArea, TALL_GRASS, world.worldBoundingRect);
				}	
			}

			if(type == STONE)
			{
				if(rand() % COAL_PROB <= (world.worldBoundingRect.maxY - y) / 256)
					type = COAL;
				else if(rand() % IRON_PROB <= (200 - y) / 64 && y <= 200.0f)
					type = IRON;	
				else if(rand() % GOLD_PROB <= (128 - y) / 80 && y <= 128.0f)
					type = GOLD;		
				else if(rand() % DIAMOND_PROB <= (128 - y) / 80 && y <= 48.0f)
					type = DIAMOND;
				else if(rand() % RAINBOW_PROB <= (96 - y) / 80 && y <= 32.0f)
					type = RAINBOW_ORE;
				else if(y <= 32.0f && rand() % (int)(y * y * 0.05f + 1.0f) <= sqrtf(y))
					type = MAGMA_STONE;
			}

			setBlockType(world.blocks, i - WORLD_WIDTH / 2, y, world.blockArea, type, world.worldBoundingRect);
			setBlockType(world.transparentBlocks, i - WORLD_WIDTH / 2, y, world.blockArea, NONE, world.worldBoundingRect);
			total++;
		}	
	}

	//Place water into the world
	for(int i = 0; i < WORLD_WIDTH; i++)
	{
		if(WATER_LEVEL < worldHeight[i])
			continue;
		int y;
		for(y = WATER_LEVEL; y > worldHeight[i]; y--)
		{
			setBlockType(world.blocks, i - WORLD_WIDTH / 2.0f, y, world.blockArea, WATER, world.worldBoundingRect);
			setBlockMass(world.blocks, i - WORLD_WIDTH / 2.0f, y, world.blockArea, 1.0f, world.worldBoundingRect); 
		}
		floodFill(i - WORLD_WIDTH / 2.0f, y, WATER_LEVEL, WATER, 
				  world.blocks, world.blockArea, 0, world.worldBoundingRect);
	}

	//Spawn enemies
	spawnEnemies(&world, worldHeight, WORLD_WIDTH, newpt(0.0f, worldHeight[WORLD_WIDTH / 2] * BLOCK_SIZE));

	printf("Inserted %d blocks!\n", total);
	free(caveValues);

	//Insert clouds into the world
	for(int i = 0; i < MAX_CLOUD; i++)
	{
		float sz = (float)rand() / (float)RAND_MAX * 32.0f + 64.0f;
		world.clouds[i] = createSprite(createRect(
						(float)rand() / (float)RAND_MAX * 1920.0f - 960.0f,
						(float)rand() / (float)RAND_MAX * 960.0f,
						sz, sz));
	}

	return world;
}
