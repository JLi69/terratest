//TODO: IMPROVE MEMORY USAGE
//Don't have the entire world loaded into memory

#ifndef WORLD_H
#include "sprite.h"
#include "inventory.h"
#include "quadtree.h"
#include <stdint.h>
#include "block.h"
#include "boss.h"

#define WORLD_WIDTH 8192
#define MIN_CAVE_VALUE -0.2f
#define MAX_CAVE_VALUE 0.1f

#define TREE_MIN_HEIGHT 8
#define TREE_MAX_HEIGHT 12
#define TREE_PROB 8
#define FLOWER_PROB 4
#define STUMP_PROB 16
#define TALL_GRASS_PROB 2
#define VINE_PROB 12
#define MIN_VINE_LEN 4
#define MAX_VINE_LEN 8

#define COAL_PROB 128
#define IRON_PROB 256
#define DIAMOND_PROB 640
#define GOLD_PROB 384
#define RAINBOW_PROB 2048

#define WATER_LEVEL 230.0f

#define MAX_CLOUD 32

#define MAX_ITEMS 8192
#define ITEM_SIZE 16.0f
#define TIME_TO_DESPAWN 300.0f

struct DroppedItem
{
	struct Sprite itemSpr;
	struct InventorySlot item;
};

struct World
{
	int blockArea; //Maximum number of blocks that can fit into the world
				   //if they are all arranged in a grid pattern
	struct Block *blocks, //Solid blocks and liquid blocks 
				 *transparentBlocks, //Blocks that can be walked through
				 *backgroundBlocks; 
	float dayCycle; //Current time of day in the world
	float moonPhase;
	struct Sprite clouds[MAX_CLOUD];
	struct BoundingRect worldBoundingRect;
	
	int totalItems; 
	struct DroppedItem droppedItems[MAX_ITEMS];

	struct QuadTree* enemies;
	
	struct Boss boss; 
};

struct World generateWorld(
	int seed,
	float amp,
	int interval
	);

//Run this function once
void revealVisible(struct World *world);
//Reveal neighbors of (x, y) along with (x, y)
void revealNeighbors(struct World *world, int x, int y);

//Liquid functions
void updateBlocks(struct Block *blocks,
				  struct Vector2D camPos,
				  float timePassed, int updateDist,
				  int maxIndex, struct BoundingRect boundRect);
void drawBlocks(struct Block *blocks,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  int maxIndex, struct BoundingRect boundRect, float brightness);
void drawLiquids(struct Block *blocks,
				  struct Vector2D camPos,
				  int viewDistX, int viewDistY,
				  int maxIndex, struct BoundingRect boundRect, float brightness);

int blockCollisionSearch(struct Sprite spr, int distX, int distY, struct Block *blocks,
						 int maxIndex, struct BoundingRect boundRect, struct Sprite *collided);

//returns 1 if the object is touching a block of certain type
//returns 0 if not
int touching(struct World world, float x, float y, enum BlockType type, float massThreshold);

void addItem(struct World *world, struct InventorySlot item, float x, float y);
void drawItems(struct World world, struct Vector2D camPos,
				int viewDistX, int viewDistY);
void updateItems(struct World *world, struct Vector2D camPos, int simDist,
				 float timePassed, struct Player *player);

void updatePlants(struct World *world, struct Vector2D camPos, int simDist);
//Pass in coordinates of the bottom of the door
void updateDoor(struct World *world, int x, int y, struct Enemy *enemyArr, struct IntVec indices);
//Returns 1 if it detected a door
int toggleDoor(struct World *world, int x, int y, struct Sprite playerSpr, struct Enemy *enemyArr, struct IntVec indices);
//Spawn enemies upon creation of the world
void spawnEnemies(struct World *world, float *worldheight, int worldwidth, union Point playerPos);
void spawnEnemiesAtNight(struct World *world, struct Vector2D camPos, float range);
void spawnEnemiesInCave(struct World *world, struct Vector2D camPos, float range);
void spawnChickens(struct World *world, struct Vector2D camPos, float range);
void spawnWave(struct World *world, struct Vector2D camPos, float range);

#endif
#define WORLD_H
