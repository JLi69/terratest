#include "inventory.h"
#include <stdlib.h>
#include "draw.h"

float timeToBreakBlock(enum BlockType type, enum Item item)
{
	return 999999.0f;
}

int maxStack(enum Item item)
{
	return 99;
}

//Probably shouldn't hardcode this, move this to an external file
enum Item droppedItem(enum BlockType type, enum Item item)
{
	switch(type)
	{
	case LOG: //Fall through
	case STUMP: return LOG_ITEM;
	case PLANK_BLOCK: return PLANK;
	case DIRT: return DIRT_ITEM;
	case GRASS: return DIRT_ITEM;
	case LEAF:
		if(rand() % 5 == 0) return SAPLING_ITEM;
		else if(rand() % 5 <= 3) return STICK;
		break;	
	case STONE: return STONE_ITEM;
	case BRICK: return BRICK_ITEM;
	case FLOWER: return FLOWER_ITEM;
	case TALL_GRASS:
		if(rand() % 3 == 0) return SEED_ITEM;
		break;
	case VINES: return VINES_ITEM;
	case COAL: return COAL_ITEM;
	case IRON: return IRON_ITEM;
	case DIAMOND: return DIAMOND_ITEM;
	case GOLD: return GOLD_ITEM;
	case RAINBOW_ORE: return RAINBOW_ITEM;
	case MAGMA_STONE: return MAGMA_ITEM;
	case SAND: return SAND_ITEM;
	case SAPLING: return SAPLING_ITEM;
	default: break;
	}
	return NOTHING;
}

//Probably shouldn't hardcode this, move to an external file
enum BlockType placeBlock(enum Item item)
{
	switch(item)
	{
	case LOG_ITEM: return LOG;
	case DIRT_ITEM: return DIRT;
	case FLOWER_ITEM: return FLOWER;
	case STONE_BLOCK: return STONE;
	case BRICK_ITEM: return BRICK;
	case VINES_ITEM: return VINES;
	case GRASS_ITEM: return GRASS;
	case MAGMA_ITEM: return MAGMA_STONE;
	case SAND_ITEM: return SAND;
	case INDESTRUCTABLE_ITEM: return INDESTRUCTABLE;
	case SAPLING_ITEM: return SAPLING;
	case PLANK: return PLANK_BLOCK;
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
		inventory.slots[i].maxUsesLeft = 0;
		inventory.slots[i].usesLeft = 0;	
	}
	return inventory;
}

int pickup(enum Item item, int amt, struct Inventory *inventory)
{
	for(int i = 0; i < inventory->maxSize; i++)
	{
		if(amt <= 0)
			return 1;
		if(inventory->slots[i].amount >= maxStack(inventory->slots[i].item))
			continue;

		if(inventory->slots[i].item == item &&
			inventory->slots[i].amount + amt <= maxStack(inventory->slots[i].item))
		{
			inventory->slots[i].amount += amt;
			return amt; //Found slot to put something in
		}
		else if(inventory->slots[i].item == item &&
				inventory->slots[i].amount + amt > maxStack(inventory->slots[i].item))
		{
			amt -= (maxStack(inventory->slots[i].item) - inventory->slots[i].amount);
			inventory->slots[i].amount = maxStack(inventory->slots[i].item);
		}
	}

	for(int i = 0; i < inventory->maxSize; i++)
	{
		if(inventory->slots[i].item == NOTHING)
		{
			inventory->slots[i].item = item;
			inventory->slots[i].amount = amt;
			return amt; //Found slot
		}
	}

	return amt;
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
