#include "world.h"

void updateDoor(struct World *world, int x, int y, struct Enemy *enemyArr, struct IntVec indices)
{
	struct Sprite doorPart1, doorPart2;	
	if(getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type == DOOR_BOTTOM_CLOSED)
	{
		if(getBlock(world->blocks, x, y + 1, world->blockArea, world->worldBoundingRect).type == NONE ||
			getBlock(world->blocks, x, y + 1, world->blockArea, world->worldBoundingRect).type == WATER ||
			getBlock(world->blocks, x, y + 1, world->blockArea, world->worldBoundingRect).type == LAVA)
		{	
			doorPart1 = createSprite(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
			doorPart2 = createSprite(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE + BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
			for(int i = 0; i < indices.sz; i++)
			{			
				if(colliding(doorPart1.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox) ||
					colliding(doorPart2.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox))
				{
					setBlockType(world->blocks, x, y, world->blockArea, NONE, world->worldBoundingRect);
					addItem(world, itemAmt(DOOR_ITEM, 1), x * BLOCK_SIZE, y * BLOCK_SIZE);
					return;
				}
			}
			setBlockType(world->blocks, x, y + 1, world->blockArea, DOOR_TOP_CLOSED, world->worldBoundingRect);	
		}	
		else if(getBlock(world->blocks, x, y - 1, world->blockArea, world->worldBoundingRect).type == NONE ||
			getBlock(world->blocks, x, y - 1, world->blockArea, world->worldBoundingRect).type == WATER ||
			getBlock(world->blocks, x, y - 1, world->blockArea, world->worldBoundingRect).type == LAVA)
		{
			doorPart1 = createSprite(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
			doorPart2 = createSprite(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE - BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
			for(int i = 0; i < indices.sz; i++)
			{			
				if(colliding(doorPart1.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox) ||
					colliding(doorPart2.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox))
				{
					setBlockType(world->blocks, x, y, world->blockArea, NONE, world->worldBoundingRect);
					addItem(world, itemAmt(DOOR_ITEM, 1), x * BLOCK_SIZE, y * BLOCK_SIZE);
					return;
				}
			}
			setBlockType(world->blocks, x, y, world->blockArea, DOOR_TOP_CLOSED, world->worldBoundingRect);		
			setBlockType(world->blocks, x, y - 1, world->blockArea, DOOR_BOTTOM_CLOSED, world->worldBoundingRect);	
		}
		else
		{
			setBlockType(world->blocks, x, y, world->blockArea, NONE, world->worldBoundingRect);
			addItem(world, itemAmt(DOOR_ITEM, 1), x * BLOCK_SIZE, y * BLOCK_SIZE);
		}
	}
}

int toggleDoor(struct World *world, int x, int y, struct Sprite playerSpr, struct Enemy *enemyArr, struct IntVec indices)
{
	struct Sprite doorPart1, doorPart2;

	doorPart1 = createSprite(createRect(x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	
	if(getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type == DOOR_BOTTOM_CLOSED)
	{
		doorPart2 = createSprite(createRect(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
		if(colliding(playerSpr.hitbox, doorPart1.hitbox) || colliding(playerSpr.hitbox, doorPart2.hitbox))
			return 0;

		for(int i = 0; i < indices.sz; i++)
			if(colliding(doorPart1.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox) ||
				colliding(doorPart2.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox))
				return 0;

		setBlockType(world->blocks, x, y, world->blockArea, DOOR_BOTTOM_OPEN, world->worldBoundingRect);
		setBlockType(world->blocks, x, y + 1, world->blockArea, DOOR_TOP_OPEN, world->worldBoundingRect);
		return 1;	
	}
	else if(getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type == DOOR_TOP_CLOSED)
	{
		doorPart2 = createSprite(createRect(x * BLOCK_SIZE, (y - 1) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
		if(colliding(playerSpr.hitbox, doorPart1.hitbox) || colliding(playerSpr.hitbox, doorPart2.hitbox))
			return 0;
		
		for(int i = 0; i < indices.sz; i++)
			if(colliding(doorPart1.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox) ||
				colliding(doorPart2.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox))
				return 0;
		
		setBlockType(world->blocks, x, y - 1, world->blockArea, DOOR_BOTTOM_OPEN, world->worldBoundingRect);
		setBlockType(world->blocks, x, y, world->blockArea, DOOR_TOP_OPEN, world->worldBoundingRect);
		return 1;	
	}
	else if(getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type == DOOR_TOP_OPEN)
	{
		doorPart2 = createSprite(createRect(x * BLOCK_SIZE, (y - 1) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
		if(colliding(playerSpr.hitbox, doorPart1.hitbox) || colliding(playerSpr.hitbox, doorPart2.hitbox))
			return 0;
		
		for(int i = 0; i < indices.sz; i++)
			if(colliding(doorPart1.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox) ||
				colliding(doorPart2.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox))
				return 0;

		setBlockType(world->blocks, x, y - 1, world->blockArea, DOOR_BOTTOM_CLOSED, world->worldBoundingRect);
		setBlockType(world->blocks, x, y, world->blockArea, DOOR_TOP_CLOSED, world->worldBoundingRect);
		return 1;	
	}
	else if(getBlock(world->blocks, x, y, world->blockArea, world->worldBoundingRect).type == DOOR_BOTTOM_OPEN)
	{
		doorPart2 = createSprite(createRect(x * BLOCK_SIZE, (y + 1) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
		if(colliding(playerSpr.hitbox, doorPart1.hitbox) || colliding(playerSpr.hitbox, doorPart2.hitbox))
			return 0;
		
		for(int i = 0; i < indices.sz; i++)
			if(colliding(doorPart1.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox) ||
				colliding(doorPart2.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox))
				return 0;

		setBlockType(world->blocks, x, y, world->blockArea, DOOR_BOTTOM_CLOSED, world->worldBoundingRect);
		setBlockType(world->blocks, x, y + 1, world->blockArea, DOOR_TOP_CLOSED, world->worldBoundingRect);
		return 1;	
	}
	return 0;
}
