#include "savefile.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void readRectangle(struct Rectangle *rect, FILE *file)
{
	size_t ret;
	ret = fread(&rect->position.x, sizeof(float), 1, file);
	ret = fread(&rect->position.y, sizeof(float), 1, file);
	ret = fread(&rect->dimensions.w, sizeof(float), 1, file);
	ret = fread(&rect->dimensions.h, sizeof(float), 1, file);
}

void readBoundingRect(struct BoundingRect *rect, FILE *file)
{
	size_t ret;
	ret = fread(&rect->minX, sizeof(float), 1, file);
	ret = fread(&rect->maxX, sizeof(float), 1, file);
	ret = fread(&rect->minY, sizeof(float), 1, file);
	ret = fread(&rect->maxY, sizeof(float), 1, file);
}

void readVector(struct Vector2D *vec, FILE *file)
{
	size_t ret;
	ret = fread(&vec->x, sizeof(float), 1, file);
	ret = fread(&vec->y, sizeof(float), 1, file);
}

void readSprite(struct Sprite *spr, FILE *file)
{
	size_t ret;
	readRectangle(&spr->hitbox, file);
	readVector(&spr->vel, file);	
	ret = fread(&spr->falling, sizeof(int16_t), 1, file);
	ret = fread(&spr->canMove, sizeof(int16_t), 1, file); 
	ret = fread(&spr->flipped, sizeof(int16_t), 1, file); 
	ret = fread(&spr->animating, sizeof(int16_t), 1, file); 
	ret = fread(&spr->canJump, sizeof(int16_t), 1, file);
	ret = fread(&spr->animationFrame, sizeof(int32_t), 1, file);	
	ret = fread(&spr->timeSinceLastUpdate, sizeof(float), 1, file);
	ret = fread(&spr->type, sizeof(uint32_t), 1, file);
	ret = fread(&spr->animationState, sizeof(uint32_t), 1, file);
	ret = fread(&spr->timeExisted, sizeof(float), 1, file);
}

void readInventory(struct Inventory *inventory, FILE *file)
{
	size_t ret;
	int maxSize;
	ret = fread(&maxSize, sizeof(int32_t), 1, file);
	*inventory = createInventory(maxSize);
	ret = fread(&inventory->selected, sizeof(int32_t), 1, file);
	for(int i = 0; i < inventory->maxSize; i++)
	{
		ret = fread(&inventory->slots[i].amount, sizeof(int32_t), 1, file);
		ret = fread(&inventory->slots[i].item, sizeof(enum Item), 1, file);
		ret = fread(&inventory->slots[i].usesLeft, sizeof(int32_t), 1, file);
		ret = fread(&inventory->slots[i].maxUsesLeft, sizeof(int32_t), 1, file);
	}
}

void readDroppedItem(struct DroppedItem *droppedItem, FILE *file)
{
	size_t ret;
	readSprite(&droppedItem->itemSpr, file);
	ret = fread(&droppedItem->item.amount, sizeof(int32_t), 1, file);
	ret = fread(&droppedItem->item.item, sizeof(enum Item), 1, file);
	ret = fread(&droppedItem->item.usesLeft, sizeof(int32_t), 1, file);
	ret = fread(&droppedItem->item.maxUsesLeft, sizeof(int32_t), 1, file);
}

void readPlayerData(struct Player *player, FILE *file)
{
	size_t ret;
	//Player sprite data
	readSprite(&player->playerSpr, file);	
	//Read inventory data	
	readInventory(&player->inventory, file);
	//Player health and breath data
	ret = fread(&player->health, sizeof(int32_t), 1, file);
	ret = fread(&player->maxHealth, sizeof(int32_t), 1, file);
	ret = fread(&player->breath, sizeof(float), 1, file);
	ret = fread(&player->maxBreath, sizeof(float), 1, file);

	player->damageCooldown = DAMAGE_COOLDOWN + 1.0f;
	player->damageTaken = 0;
}

void readBlockData(struct Block *blocks, int sz, FILE *file)
{
	uint8_t* blockData = malloc(sizeof(uint8_t) * 3 * sz);
	size_t ret = fread(blockData, sz * 3, sizeof(uint8_t), file);
	for(int i = 0; i < sz; i++)
	{
		blocks[i].type = blockData[i * 3];
		blocks[i].mass = (float)blockData[i * 3 + 1] / 128.0f;
		blocks[i].visibility = blockData[i * 3 + 2];
	}
	free(blockData);
}

void writeRectangle(struct Rectangle *rect, FILE *file)
{
	fwrite(&rect->position.x, sizeof(float), 1, file);
	fwrite(&rect->position.y, sizeof(float), 1, file);
	fwrite(&rect->dimensions.w, sizeof(float), 1, file);
	fwrite(&rect->dimensions.h, sizeof(float), 1, file);
}

void writeBoundingRect(struct BoundingRect *rect, FILE *file)
{
	fwrite(&rect->minX, sizeof(float), 1, file);
	fwrite(&rect->maxX, sizeof(float), 1, file);
	fwrite(&rect->minY, sizeof(float), 1, file);
	fwrite(&rect->maxY, sizeof(float), 1, file);
}

void writeVector(struct Vector2D *vec, FILE *file)
{
	fwrite(&vec->x, sizeof(float), 1, file);
	fwrite(&vec->y, sizeof(float), 1, file);
}

void writeSprite(struct Sprite *spr, FILE *file)
{
	writeRectangle(&spr->hitbox, file);
	writeVector(&spr->vel, file);	
	fwrite(&spr->falling, sizeof(int16_t), 1, file);
	fwrite(&spr->canMove, sizeof(int16_t), 1, file); 
	fwrite(&spr->flipped, sizeof(int16_t), 1, file); 
	fwrite(&spr->animating, sizeof(int16_t), 1, file); 
	fwrite(&spr->canJump, sizeof(int16_t), 1, file);
	fwrite(&spr->animationFrame, sizeof(int32_t), 1, file);	
	fwrite(&spr->timeSinceLastUpdate, sizeof(float), 1, file);
	fwrite(&spr->type, sizeof(uint32_t), 1, file);
	fwrite(&spr->animationState, sizeof(uint32_t), 1, file);
	fwrite(&spr->timeExisted, sizeof(float), 1, file);
}

void writeInventory(struct Inventory *inventory, FILE *file)
{
	fwrite(&inventory->maxSize, sizeof(int32_t), 1, file);
	fwrite(&inventory->selected, sizeof(int32_t), 1, file);
	for(int i = 0; i < inventory->maxSize; i++)
	{
		fwrite(&inventory->slots[i].amount, sizeof(int32_t), 1, file);
		fwrite(&inventory->slots[i].item, sizeof(enum Item), 1, file);
		fwrite(&inventory->slots[i].usesLeft, sizeof(int32_t), 1, file);
		fwrite(&inventory->slots[i].maxUsesLeft, sizeof(int32_t), 1, file);
	}
}

void writeDroppedItem(struct DroppedItem *droppedItem, FILE *file)
{
	writeSprite(&droppedItem->itemSpr, file);
	fwrite(&droppedItem->item.amount, sizeof(int32_t), 1, file);
	fwrite(&droppedItem->item.item, sizeof(enum Item), 1, file);
	fwrite(&droppedItem->item.usesLeft, sizeof(int32_t), 1, file);
	fwrite(&droppedItem->item.maxUsesLeft, sizeof(int32_t), 1, file);
}

void writePlayerData(struct Player *player, FILE *file)
{
	//Player sprite data
	writeSprite(&player->playerSpr, file);
	//Write inventory data
	writeInventory(&player->inventory, file);
	//Player health and breath data
	fwrite(&player->health, sizeof(int32_t), 1, file);
	fwrite(&player->maxHealth, sizeof(int32_t), 1, file);
	fwrite(&player->breath, sizeof(float), 1, file);
	fwrite(&player->maxBreath, sizeof(float), 1, file);
}

void writeBlockData(struct Block *blocks, int sz, FILE *file)
{
	uint8_t* blockData = malloc(sizeof(uint8_t) * 3 * sz);
	uint8_t* ptr = blockData;
	for(int i = 0; i < sz; i++)
	{
		*(ptr++) = blocks[i].type;
		uint8_t massIntVal = (uint8_t)(128.0f * blocks[i].mass);
		*(ptr++) = massIntVal;
		*(ptr++) = blocks[i].visibility;
	}
	fwrite(blockData, 3 * sz, sizeof(uint8_t), file);
	free(blockData);
}

void writeEnemyData(struct World *world, FILE *file)
{
	fwrite(&world->enemies->pointCount, sizeof(int32_t), 1, file);
	for(int i = 0; i < world->enemies->pointCount; i++)
	{
		writeSprite(&world->enemies->enemyArr[i].spr, file);
		uint8_t sprDataInt[] = 
		{
			world->enemies->enemyArr[i].health,
			world->enemies->enemyArr[i].maxHealth,
			world->enemies->enemyArr[i].attackmode	
		};
		float sprDataFloat[] =
		{
			world->enemies->enemyArr[i].walkDistance,
			world->enemies->enemyArr[i].timer,
			world->enemies->enemyArr[i].damageCooldown,
			world->enemies->enemyArr[i].despawnTimer
		};
		fwrite(sprDataInt, sizeof(uint8_t), 3, file);
		fwrite(sprDataFloat, sizeof(float), 4, file);
	}
}

void readEnemyData(struct World *world, FILE *file)
{
	size_t ret;
	int pointCount;
	ret = fread(&pointCount, sizeof(int32_t), 1, file);
	for(int i = 0; i < pointCount; i++)
	{
		struct Enemy enemy;
		readSprite(&enemy.spr, file);
		uint8_t sprDataInt[3];	
		float sprDataFloat[4];	
		ret = fread(sprDataInt, sizeof(uint8_t), 3, file);
		ret = fread(sprDataFloat, sizeof(float), 4, file);
		
		enemy.health = sprDataInt[0];
		enemy.maxHealth = sprDataInt[1];	
		enemy.attackmode = sprDataInt[2];	
	    
		enemy.walkDistance = sprDataFloat[0];
		enemy.timer = sprDataFloat[1];	
		enemy.damageCooldown = sprDataFloat[2];	
		enemy.despawnTimer = sprDataFloat[3];
		enemy.updated = 0;
		insertEnemy(world->enemies, enemy);
	}
}

void writeBoss(struct Boss *boss, FILE *file)
{
	writeSprite(&boss->spr, file);
	int bossIntData[] = 
	{
		boss->phase,
		boss->health,
		boss->maxHealth,
		boss->attackmode
	};
	float bossFloatData[] =
	{
		boss->timer,
		boss->spawnX,
		boss->spawnY,
		boss->damageCooldown
	};
	fwrite(bossIntData, sizeof(int32_t), 4, file);
	fwrite(bossFloatData, sizeof(float), 4, file);
}

void readBoss(struct Boss *boss, FILE *file)
{
	readSprite(&boss->spr, file);
	int bossIntData[4];
	float bossFloatData[4];
	size_t ret;
	ret = fread(bossIntData, sizeof(int32_t), 4, file);
	ret = fread(bossFloatData, sizeof(float), 4, file);
	boss->phase = bossIntData[0];
	boss->health = bossIntData[1];
	boss->maxHealth = bossIntData[2];
	boss->attackmode = bossIntData[3];
	boss->timer = bossFloatData[0];
	boss->spawnX = bossFloatData[1];
	boss->spawnY = bossFloatData[2];
	boss->damageCooldown = bossFloatData[3];
	boss->fireballTimer = 0.0f;
}

void saveWorld(struct World *world, struct Player *player, const char *path)
{
	FILE* savefile = fopen(path, "wb");

	if(savefile == NULL)
	{
		fprintf(stderr, "Error: cannot open file: %s\n", path);
		return;
	}

	//Write player data
	writePlayerData(player, savefile);

	//Write world data
	fwrite(&world->blockArea, sizeof(int32_t), 1, savefile);
	//Write block data
	writeBlockData(world->blocks, world->blockArea, savefile);
	writeBlockData(world->transparentBlocks, world->blockArea, savefile);
	writeBlockData(world->backgroundBlocks, world->blockArea, savefile);

	//Misc. world data
	fwrite(&world->dayCycle, sizeof(float), 1, savefile);
	fwrite(&world->moonPhase, sizeof(float), 1, savefile); 

	for(int i = 0; i < MAX_CLOUD; i++)
		writeSprite(&world->clouds[i], savefile);

	writeBoundingRect(&world->worldBoundingRect, savefile);

	fwrite(&world->totalItems, sizeof(int32_t), 1, savefile);
	for(int i = 0; i < world->totalItems; i++)
		writeDroppedItem(&world->droppedItems[i], savefile);

	writeEnemyData(world, savefile);
	writeBoss(&world->boss, savefile);
	
	fclose(savefile);
}

int readSave(struct World *world, struct Player *player, const char *path)
{
	size_t ret;
	FILE* savefile = fopen(path, "rb");

	if(savefile == NULL)
	{
		fprintf(stderr, "File not found: %s\n", path);
		return 0;
	}
	
	//Read player data
	readPlayerData(player, savefile);
	player->useItemTimer = 0.0f;

	//Read world data
	ret = fread(&world->blockArea, sizeof(int32_t), 1, savefile);
	world->blocks = (struct Block*)malloc(sizeof(struct Block) * world->blockArea);
	world->transparentBlocks = (struct Block*)malloc(sizeof(struct Block) * world->blockArea);
	world->backgroundBlocks = (struct Block*)malloc(sizeof(struct Block) * world->blockArea);	

	//Read block data
	readBlockData(world->blocks, world->blockArea, savefile);
	readBlockData(world->transparentBlocks, world->blockArea, savefile);
	readBlockData(world->backgroundBlocks, world->blockArea, savefile);

	//Misc. world data
	ret = fread(&world->dayCycle, sizeof(float), 1, savefile);	
	ret = fread(&world->moonPhase, sizeof(float), 1, savefile); 	
	
	for(int i = 0; i < MAX_CLOUD; i++)
		readSprite(&world->clouds[i], savefile);

	readBoundingRect(&world->worldBoundingRect, savefile);
	ret = fread(&world->totalItems, sizeof(int32_t), 1, savefile);
	for(int i = 0; i < world->totalItems; i++)
		readDroppedItem(&world->droppedItems[i], savefile);

	world->enemies = createQuadTree(
			newpt(world->worldBoundingRect.minX * BLOCK_SIZE, world->worldBoundingRect.minY * BLOCK_SIZE),
			newpt(world->worldBoundingRect.maxX * BLOCK_SIZE, world->worldBoundingRect.maxY * BLOCK_SIZE));
	readEnemyData(world, savefile);	

	readBoss(&world->boss, savefile);

	fclose(savefile);

	return 1;
}
