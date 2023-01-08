#include "savefile.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Some notes:
 * - I didn't particularly optimize the saving world code so it might be beneficial to have
 *   some of the following optimizations:
 *		- Chunking to save the world in smaller portions (and to consume less memory)
 *		- Write all binary data to a character array in memory first before writing
 *		that all to disk so that fwrite/fread doesn't need to do a bunch of syscalls
 *		(not sure how much that will increase speed but I imagine it could help a bit)
 * */

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
	ret = fread(&spr->falling, sizeof(short int), 1, file);
	ret = fread(&spr->canMove, sizeof(short int), 1, file); 
	ret = fread(&spr->flipped, sizeof(short int), 1, file); 
	ret = fread(&spr->animating, sizeof(short int), 1, file); 
	ret = fread(&spr->canJump, sizeof(short int), 1, file);
	ret = fread(&spr->animationFrame, sizeof(int), 1, file);	
	ret = fread(&spr->timeSinceLastUpdate, sizeof(float), 1, file);
	ret = fread(&spr->type, sizeof(unsigned int), 1, file);
	ret = fread(&spr->animationState, sizeof(unsigned int), 1, file);
	ret = fread(&spr->timeExisted, sizeof(float), 1, file);
}

void readInventory(struct Inventory *inventory, FILE *file)
{
	size_t ret;
	int maxSize;
	ret = fread(&maxSize, sizeof(int), 1, file);
	*inventory = createInventory(maxSize);
	ret = fread(&inventory->selected, sizeof(int), 1, file);
	for(int i = 0; i < inventory->maxSize; i++)
	{
		ret = fread(&inventory->slots[i].amount, sizeof(int), 1, file);
		ret = fread(&inventory->slots[i].item, sizeof(enum Item), 1, file);
		ret = fread(&inventory->slots[i].usesLeft, sizeof(int), 1, file);
		ret = fread(&inventory->slots[i].maxUsesLeft, sizeof(int), 1, file);
	}
}

void readDroppedItem(struct DroppedItem *droppedItem, FILE *file)
{
	size_t ret;
	readSprite(&droppedItem->itemSpr, file);
	ret = fread(&droppedItem->item.amount, sizeof(int), 1, file);
	ret = fread(&droppedItem->item.item, sizeof(enum Item), 1, file);
	ret = fread(&droppedItem->item.usesLeft, sizeof(int), 1, file);
	ret = fread(&droppedItem->item.maxUsesLeft, sizeof(int), 1, file);
}

void readPlayerData(struct Player *player, FILE *file)
{
	size_t ret;
	//Player sprite data
	readSprite(&player->playerSpr, file);	
	//Read inventory data	
	readInventory(&player->inventory, file);
	//Player health and breath data
	ret = fread(&player->health, sizeof(int), 1, file);
	ret = fread(&player->maxHealth, sizeof(int), 1, file);
	ret = fread(&player->breath, sizeof(float), 1, file);
	ret = fread(&player->maxBreath, sizeof(float), 1, file);

	player->damageCooldown = DAMAGE_COOLDOWN + 1.0f;
	player->damageTaken = 0;
}

void readBlockData(struct Block *blocks, int sz, FILE *file)
{
	size_t ret;
	for(int i = 0; i < sz; i++)
	{
		ret = fread(&blocks[i].type, sizeof(enum BlockType), 1, file);
		ret = fread(&blocks[i].mass, sizeof(float), 1, file);
		ret = fread(&blocks[i].visibility, sizeof(enum Visibility), 1, file);
	}
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
	fwrite(&spr->falling, sizeof(short int), 1, file);
	fwrite(&spr->canMove, sizeof(short int), 1, file); 
	fwrite(&spr->flipped, sizeof(short int), 1, file); 
	fwrite(&spr->animating, sizeof(short int), 1, file); 
	fwrite(&spr->canJump, sizeof(short int), 1, file);
	fwrite(&spr->animationFrame, sizeof(int), 1, file);	
	fwrite(&spr->timeSinceLastUpdate, sizeof(float), 1, file);
	fwrite(&spr->type, sizeof(unsigned int), 1, file);
	fwrite(&spr->animationState, sizeof(unsigned int), 1, file);
	fwrite(&spr->timeExisted, sizeof(float), 1, file);
}

void writeInventory(struct Inventory *inventory, FILE *file)
{
	fwrite(&inventory->maxSize, sizeof(int), 1, file);
	fwrite(&inventory->selected, sizeof(int), 1, file);
	for(int i = 0; i < inventory->maxSize; i++)
	{
		fwrite(&inventory->slots[i].amount, sizeof(int), 1, file);
		fwrite(&inventory->slots[i].item, sizeof(enum Item), 1, file);
		fwrite(&inventory->slots[i].usesLeft, sizeof(int), 1, file);
		fwrite(&inventory->slots[i].maxUsesLeft, sizeof(int), 1, file);
	}
}

void writeDroppedItem(struct DroppedItem *droppedItem, FILE *file)
{
	writeSprite(&droppedItem->itemSpr, file);
	fwrite(&droppedItem->item.amount, sizeof(int), 1, file);
	fwrite(&droppedItem->item.item, sizeof(enum Item), 1, file);
	fwrite(&droppedItem->item.usesLeft, sizeof(int), 1, file);
	fwrite(&droppedItem->item.maxUsesLeft, sizeof(int), 1, file);
}

void writePlayerData(struct Player *player, FILE *file)
{
	//Player sprite data
	writeSprite(&player->playerSpr, file);
	//Write inventory data
	writeInventory(&player->inventory, file);
	//Player health and breath data
	fwrite(&player->health, sizeof(int), 1, file);
	fwrite(&player->maxHealth, sizeof(int), 1, file);
	fwrite(&player->breath, sizeof(float), 1, file);
	fwrite(&player->maxBreath, sizeof(float), 1, file);
}

void writeBlockData(struct Block *blocks, int sz, FILE *file)
{
	for(int i = 0; i < sz; i++)
	{
		fwrite(&blocks[i].type, sizeof(enum BlockType), 1, file);
		fwrite(&blocks[i].mass, sizeof(float), 1, file);
		fwrite(&blocks[i].visibility, sizeof(enum Visibility), 1, file);
	}
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
	fwrite(&world->blockArea, sizeof(int), 1, savefile);
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

	fwrite(&world->totalItems, sizeof(int), 1, savefile);
	for(int i = 0; i < world->totalItems; i++)
		writeDroppedItem(&world->droppedItems[i], savefile);

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

	//Read world data
	ret = fread(&world->blockArea, sizeof(int), 1, savefile);
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
	ret = fread(&world->totalItems, sizeof(int), 1, savefile);
	for(int i = 0; i < world->totalItems; i++)
		readDroppedItem(world->droppedItems, savefile);
	fclose(savefile);
	return 1;
}
