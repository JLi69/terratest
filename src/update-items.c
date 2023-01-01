#include "world.h"
#include "draw.h"
#include <stdio.h>
#include <math.h>

void addItem(struct World *world, struct InventorySlot item, float x, float y)
{
	if(item.item == NOTHING || item.amount <= 0)
		return;
	if(world->totalItems >= MAX_ITEMS)
		return;
	struct Sprite temp = createSprite(
			createRect(x, y, ITEM_SIZE, ITEM_SIZE)
		);
	temp.canMove = 1;
	world->droppedItems[world->totalItems].item = item;	
	world->droppedItems[world->totalItems++].itemSpr = temp;	
}

void drawItems(struct World world, struct Vector2D camPos, int viewDistX, int viewDistY)
{
	enum Item prev = NOTHING;
	for(int i = 0; i < world.totalItems; i++)
	{
		if(world.droppedItems[i].itemSpr.hitbox.position.x / BLOCK_SIZE < camPos.x / BLOCK_SIZE - viewDistX ||
			world.droppedItems[i].itemSpr.hitbox.position.x / BLOCK_SIZE > camPos.x / BLOCK_SIZE + viewDistX ||
			world.droppedItems[i].itemSpr.hitbox.position.y / BLOCK_SIZE < camPos.y / BLOCK_SIZE - viewDistY ||
			world.droppedItems[i].itemSpr.hitbox.position.y / BLOCK_SIZE > camPos.y / BLOCK_SIZE + viewDistY)
			continue;

		if(world.droppedItems[i].item.item != prev)
		{	
			setTexOffset(1.0f / 16.0f * (float)((world.droppedItems[i].item.item - 1) % 16),
						 1.0f / 16.0f * (float)((world.droppedItems[i].item.item - 1) / 16));
			prev = world.droppedItems[i].itemSpr.type;
		}

		setTransparency((TIME_TO_DESPAWN - world.droppedItems[i].itemSpr.timeExisted) / TIME_TO_DESPAWN);
		setRectPos(world.droppedItems[i].itemSpr.hitbox.position.x - camPos.x,
					world.droppedItems[i].itemSpr.hitbox.position.y - camPos.y);
		drawRect();
	}
}

void updateItems(struct World *world, struct Vector2D camPos, int simDist, float timePassed, struct Player *player)
{
	struct DroppedItem temp[MAX_ITEMS];

	//Remove items
	int ind = 0;
	for(int i = 0; i < world->totalItems; i++)
	{
		int pickedup = 0;
		if(powf(player->playerSpr.hitbox.position.x - world->droppedItems[i].itemSpr.hitbox.position.x, 2.0f) +
		   powf(player->playerSpr.hitbox.position.y - world->droppedItems[i].itemSpr.hitbox.position.y, 2.0f) < 1.5f * 1.5f * BLOCK_SIZE * BLOCK_SIZE &&
			(pickedup = pickup(world->droppedItems[i].item.item, world->droppedItems[i].item.amount,
				   world->droppedItems[i].item.usesLeft, world->droppedItems[i].item.maxUsesLeft, &player->inventory)) >= world->droppedItems[i].item.amount)
			continue;
		else if(colliding(player->playerSpr.hitbox, world->droppedItems[i].itemSpr.hitbox) &&
			pickedup < world->droppedItems[i].item.amount)
			world->droppedItems[i].item.amount -= pickedup;
		if(world->droppedItems[i].itemSpr.timeExisted > TIME_TO_DESPAWN) //Get deleted after 5 minutes
			continue;
		//Destroyed by lava
		if(touching(*world, world->droppedItems[i].itemSpr.hitbox.position.x / BLOCK_SIZE, world->droppedItems[i].itemSpr.hitbox.position.y / BLOCK_SIZE, LAVA))
			continue;
		temp[ind++] = world->droppedItems[i];
	}

	for(int i = 0; i < ind; i++)
	{
		if(temp[i].itemSpr.hitbox.position.x / BLOCK_SIZE < camPos.x / BLOCK_SIZE - simDist ||
			temp[i].itemSpr.hitbox.position.x / BLOCK_SIZE > camPos.x / BLOCK_SIZE + simDist ||
			temp[i].itemSpr.hitbox.position.y / BLOCK_SIZE < camPos.y / BLOCK_SIZE - simDist ||
			temp[i].itemSpr.hitbox.position.y / BLOCK_SIZE > camPos.y / BLOCK_SIZE + simDist)
		{
			continue;
		}

		temp[i].itemSpr.timeExisted += timePassed;

		if(temp[i].itemSpr.falling)
			updateSpriteY(&temp[i].itemSpr, timePassed);

		struct Sprite collided;
		if(blockCollisionSearch(temp[i].itemSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided) && 
				temp[i].itemSpr.falling)
		{	
			temp[i].itemSpr.falling = 0;
			temp[i].itemSpr.hitbox.position.y = collided.hitbox.position.y + collided.hitbox.dimensions.h / 2.0f + ITEM_SIZE / 2.0f;
			temp[i].itemSpr.vel.y = 0.0f;	
		}
		else
		{
			temp[i].itemSpr.falling = 1;
		}
	}

	world->totalItems = ind;
	for(int i = 0; i < ind; i++)
	{
		world->droppedItems[i] = temp[i];
	}
}
