#include "savefile.h"
#include <stdio.h>
#include <stdlib.h>

void saveWorld(struct World *world, struct Player *player, const char *path)
{
	FILE* savefile = fopen(path, "w");

	if(savefile == NULL)
	{
		fprintf(stderr, "Error: cannot open file: %s\n", path);
		return;
	}

	//Write player data
	fwrite(&player->playerSpr, sizeof(player->playerSpr), 1, savefile);
	fwrite(&player->inventory.maxSize, sizeof(int), 1, savefile);	
	fwrite(&player->inventory.selected, sizeof(int), 1, savefile);
	fwrite(player->inventory.slots, sizeof(struct InventorySlot), player->inventory.maxSize, savefile);

	fwrite(&player->health, sizeof(int), 1, savefile); 
	fwrite(&player->maxHealth, sizeof(int), 1, savefile); 
	fwrite(&player->breath, sizeof(int), 1, savefile); 
	fwrite(&player->maxBreath, sizeof(int), 1, savefile);

	//Write world data
	fwrite(&world->blockArea, sizeof(int), 1, savefile);
	//Write block data
	fwrite(world->blocks, sizeof(struct Block), world->blockArea, savefile);
	fwrite(world->transparentBlocks, sizeof(struct Block), world->blockArea, savefile);
	fwrite(world->backgroundBlocks, sizeof(struct Block), world->blockArea, savefile);
	
	//Misc. world data
	fwrite(&world->dayCycle, sizeof(float), 1, savefile);
	fwrite(&world->moonPhase, sizeof(float), 1, savefile); 

	fwrite(&world->clouds, sizeof(struct Sprite), MAX_CLOUD, savefile);
	fwrite(&world->worldBoundingRect, sizeof(world->worldBoundingRect), 1, savefile);

	fwrite(&world->totalItems, sizeof(int), 1, savefile);
	fwrite(&world->droppedItems, sizeof(struct DroppedItem), world->totalItems, savefile);

	fclose(savefile);
}

int readSave(struct World *world, struct Player *player, const char *path)
{
	FILE* savefile = fopen(path, "r");

	if(savefile == NULL)
	{
		fprintf(stderr, "File not found: %s\n", path);
		return 0;
	}
	
	size_t ret;

	//Write player data
	ret = fread(&player->playerSpr, sizeof(struct Sprite), 1, savefile);
	if(ret < 1)
	{
		fprintf(stderr, "Error: corrupted world: %s\n", path);
		return 0;
	}

	int inventorySz;
	ret = fread(&inventorySz, sizeof(int), 1, savefile);	
	player->inventory = createInventory(inventorySz);
	ret = fread(&player->inventory.selected, sizeof(int), 1, savefile);
	ret = fread(player->inventory.slots, sizeof(struct InventorySlot), player->inventory.maxSize, savefile);

	ret = fread(&player->health, sizeof(int), 1, savefile); 
	ret = fread(&player->maxHealth, sizeof(int), 1, savefile); 
	ret = fread(&player->breath, sizeof(int), 1, savefile); 
	ret = fread(&player->maxBreath, sizeof(int), 1, savefile);

	//Write world data
	ret = fread(&world->blockArea, sizeof(int), 1, savefile);
	if(ret < 1)
	{
		fprintf(stderr, "Error: corrupted world: %s\n", path);
		return 0;
	}
	world->blocks = (struct Block*)malloc(sizeof(struct Block) * world->blockArea);
	world->transparentBlocks = (struct Block*)malloc(sizeof(struct Block) * world->blockArea);
	world->backgroundBlocks = (struct Block*)malloc(sizeof(struct Block) * world->blockArea);
	//Write block data
	ret = fread(world->blocks, sizeof(struct Block), world->blockArea, savefile);
	if(ret < world->blockArea)
	{
		fprintf(stderr, "Error: corrupted world: %s\n", path);
		free(world->blocks);
		free(world->transparentBlocks);
		free(world->backgroundBlocks);
		return 0;
	}
	ret = fread(world->transparentBlocks, sizeof(struct Block), world->blockArea, savefile);
	if(ret < world->blockArea)
	{
		fprintf(stderr, "Error: corrupted world: %s\n", path);
		free(world->blocks);
		free(world->transparentBlocks);
		free(world->backgroundBlocks);
		return 0;
	}
	ret = fread(world->backgroundBlocks, sizeof(struct Block), world->blockArea, savefile);
	if(ret < world->blockArea)
	{
		fprintf(stderr, "Error: corrupted world: %s\n", path);
		free(world->blocks);
		free(world->transparentBlocks);
		free(world->backgroundBlocks);
		return 0;
	}

	//Misc. world data
	ret = fread(&world->dayCycle, sizeof(float), 1, savefile);	
	ret = fread(&world->moonPhase, sizeof(float), 1, savefile); 
	ret = fread(&world->clouds, sizeof(struct Sprite), MAX_CLOUD, savefile);
	ret = fread(&world->worldBoundingRect, sizeof(world->worldBoundingRect), 1, savefile);
	ret = fread(&world->totalItems, sizeof(int), 1, savefile);
	ret = fread(&world->droppedItems, sizeof(struct DroppedItem), world->totalItems, savefile);

	return 1;
}
