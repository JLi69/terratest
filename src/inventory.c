#include "inventory.h"
#include <stdlib.h>
#include "draw.h"

float timeToBreakBlock(enum BlockType type, enum Item item)
{
	return 999999.0f;
}

enum Item droppedItem(enum BlockType type, enum Item item)
{
	switch(type)
	{
	case LOG: //Fall through
	case STUMP:
		return LOG_ITEM;
	case DIRT:
		return DIRT_ITEM;
	case GRASS:
		return DIRT_ITEM;
	case LEAF:
		if(rand() % 5 == 0) return SAPLING_ITEM;
		else if(rand() % 5 <= 3) return STICK;
		break;	
	case STONE:
		return STONE_ITEM;
	case BRICK:
		return BRICK_ITEM;
	case FLOWER:
		return FLOWER_ITEM;
	case TALL_GRASS:
		if(rand() % 3 == 0) return SEED_ITEM;
		break;
	case VINES:
		return VINES_ITEM;
	default: break;
	}
	return NOTHING;
}

enum BlockType placeBlock(enum Item item)
{
	switch(item)
	{
	case LOG_ITEM:
		return LOG;
	case DIRT_ITEM:
		return DIRT;
	default: break;
	}
	return NONE;
}

struct Inventory createInventory(int sz)
{
	struct Inventory inventory;
	inventory.slots = (struct InventorySlot*)malloc(sz * sizeof(struct InventorySlot));
	inventory.maxSize = sz;
	inventory.selected = 0;
	for(int i = 0; i < sz; i++)
	{
		inventory.slots[i].amount = 0;
		inventory.slots[i].item = NOTHING;
	}
	return inventory;
}

int pickup(enum Item item, int amt, struct Inventory *inventory)
{
	for(int i = 0; i < inventory->maxSize; i++)
	{
		if(inventory->slots[i].item == item)
		{
			inventory->slots[i].amount += amt;
			return 1; //Found slot to put something in
		}
	}

	for(int i = 0; i < inventory->maxSize; i++)
	{
		if(inventory->slots[i].item == NOTHING)
		{
			inventory->slots[i].item = item;
			inventory->slots[i].amount = amt;
			return 1; //Found slot
		}
	}

	return 0;
}

void removeSlot(int ind, struct Inventory *inventory)
{
	if(ind >= inventory->maxSize)
		return;
	inventory->slots[ind].amount = 0;
	inventory->slots[ind].item = NOTHING;
}

void decrementSlot(int ind, struct Inventory *inventory)
{
	if(ind >= inventory->maxSize)
		return;
	if(inventory->slots[ind].amount >= 0)
		inventory->slots[ind].amount--;
	if(inventory->slots[ind].amount <= 0)
		inventory->slots[ind].item = NOTHING;
}

int removeAmountItem(enum Item item, int amt, struct Inventory *inventory)
{
	//Count total
	int total = 0;
	for(int i = 0; i < inventory->maxSize; i++)
		if(inventory->slots[i].item == item)
			total += inventory->slots[i].amount;

	if(total < amt)
		return 0;

	int totalRemoved = 0;
	for(int i = 0; i < inventory->maxSize; i++)
	{
		if(totalRemoved >= amt)
			return 1;

		if(inventory->slots[i].item == item &&
			inventory->slots[i].amount <= amt - totalRemoved)
		{
			totalRemoved += inventory->slots[i].amount;
			inventory->slots[i].amount = 0;
			inventory->slots[i].item = NOTHING;
		}
		else if(inventory->slots[i].item == item &&
				inventory->slots[i].amount > amt - totalRemoved)
		{
			totalRemoved += amt - totalRemoved;
			inventory->slots[i].amount -= (amt - totalRemoved);
			return 1;
		}
	}

	return 1;
}

void displayInventoryItemIcons(struct Inventory inventory, float x, float y, float iconSz,
								float spacing)
{
	setRectSize(iconSz, iconSz);

	for(int i = 0; i < inventory.maxSize; i++)
	{
		if(inventory.slots[i].item == NOTHING)
			continue;
		setRectPos(x + (iconSz + spacing) * i, y);
		setTexOffset(1.0f / 16.0f * (float)((inventory.slots[i].item - 1) % 16),
					 1.0f / 16.0f * (float)((inventory.slots[i].item - 1) / 16));
		drawRect();	
	}
}

void displayInventoryOutline(struct Inventory inventory, float x, float y, float iconSz,
								float spacing)
{
	setRectSize(iconSz, iconSz);	
	setTexOffset(1.0f / 16.0f * 6.0f, 0.0f);
	for(int i = 0; i < inventory.maxSize; i++)
	{
		if(i == inventory.selected)
			continue;
		setRectPos(x + (iconSz + spacing) * i, y);
		drawRect();		
	}

	setTexOffset(1.0f / 16.0f * 7.0f, 0.0f);
	setRectPos(x + (iconSz + spacing) * inventory.selected, y);
	drawRect();
}

void displayInventoryNumbers(struct Inventory inventory, float x, float y, float numSz,
							 float spacing)
{
	for(int i = 0; i < inventory.maxSize; i++)
	{
		if(inventory.slots[i].amount <= 1)
			continue;
		drawInteger(inventory.slots[i].amount, x + spacing * i, y, numSz);
	}
}
