#include "world.h"
#include "draw.h"
#include <stdio.h>

void addItem(struct World *world, enum Item item, float x, float y)
{
	if(item == NOTHING)
		return;
	if(world->totalItems >= MAX_ITEMS)
		return;
	struct Sprite temp = createSpriteWithType(
			createRect(x, y, ITEM_SIZE, ITEM_SIZE),
			item
		);
	temp.canMove = 1;
	world->droppedItems[world->totalItems++] = temp;	
}

void drawItems(struct World world, struct Vector2D camPos, int viewDistX, int viewDistY)
{
	enum Item prev = NOTHING;
	for(int i = 0; i < world.totalItems; i++)
	{
		if(world.droppedItems[i].hitbox.position.x / BLOCK_SIZE < camPos.x / BLOCK_SIZE - viewDistX ||
			world.droppedItems[i].hitbox.position.x / BLOCK_SIZE > camPos.x / BLOCK_SIZE + viewDistX ||
			world.droppedItems[i].hitbox.position.y / BLOCK_SIZE < camPos.y / BLOCK_SIZE - viewDistY ||
			world.droppedItems[i].hitbox.position.y / BLOCK_SIZE > camPos.y / BLOCK_SIZE + viewDistY)
			continue;

		if(world.droppedItems[i].type != prev)
		{	
			setTexOffset(1.0f / 16.0f * (float)((world.droppedItems[i].type - 1) % 16),
						 1.0f / 16.0f * (float)((world.droppedItems[i].type - 1) / 16));
			prev = world.droppedItems[i].type;
		}

		setTransparency((TIME_TO_DESPAWN - world.droppedItems[i].timeExisted) / TIME_TO_DESPAWN);
		setRectPos(world.droppedItems[i].hitbox.position.x - camPos.x,
					world.droppedItems[i].hitbox.position.y - camPos.y);
		drawRect();
	}
}

void updateItems(struct World *world, struct Vector2D camPos, int simDist, float timePassed, struct Player *player)
{
	struct Sprite temp[MAX_ITEMS];

	//Remove items
	int ind = 0;
	for(int i = 0; i < world->totalItems; i++)
	{
		if(colliding(player->playerSpr.hitbox, world->droppedItems[i].hitbox) &&
			pickup(world->droppedItems[i].type, 1, &player->inventory))
			continue;
		if(world->droppedItems[i].timeExisted > TIME_TO_DESPAWN) //Get deleted after 5 minutes
			continue;
		temp[ind++] = world->droppedItems[i];
	}

	for(int i = 0; i < ind; i++)
	{
		if(temp[i].hitbox.position.x / BLOCK_SIZE < camPos.x / BLOCK_SIZE - simDist ||
			temp[i].hitbox.position.x / BLOCK_SIZE > camPos.x / BLOCK_SIZE + simDist ||
			temp[i].hitbox.position.y / BLOCK_SIZE < camPos.y / BLOCK_SIZE - simDist ||
			temp[i].hitbox.position.y / BLOCK_SIZE > camPos.y / BLOCK_SIZE + simDist)
		{
			continue;
		}

		temp[i].timeExisted += timePassed;

		if(temp[i].falling)
			updateSpriteY(&temp[i], timePassed);

		struct Sprite collided;
		if(blockCollisionSearch(temp[i], 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided) && 
				temp[i].falling)
		{	
			temp[i].falling = 0;
			temp[i].hitbox.position.y = collided.hitbox.position.y + collided.hitbox.dimensions.h / 2.0f + ITEM_SIZE / 2.0f;
			temp[i].vel.y = 0.0f;	
		}
		else
		{
			temp[i].falling = 1;
		}
	}

	world->totalItems = ind;
	for(int i = 0; i < ind; i++)
	{
		world->droppedItems[i] = temp[i];
	}
}
