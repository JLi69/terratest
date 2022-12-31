#include "inventory.h"
#include <stdlib.h>
#include "draw.h"
#include <math.h>

float timeToBreakBlock(enum BlockType type, enum Item item)
{
	float breakTime = 999999.0f;
	//Break time by hand
	switch(type)
	{
	case PLANK_BLOCK: //Fall through
	case GRASS: //Fall through
	case SAND: //Fall through
	case DIRT: 
		if(item >= WOOD_PICKAXE)
		{
			breakTime *= 0.5f;
			breakTime *= powf(0.5f, (item - WOOD_PICKAXE + 1));
		}
		breakTime = 0.5f; break;
	
	case LOG: //Fall through
	case STUMP:  breakTime = 1.0f; break;

	case LEAF: //Fall through
	case TALL_GRASS: //Fall through
	case VINES: breakTime = 0.1f; break;

	case SAPLING: //Fall through
	case FLOWER: breakTime = 0.01f; break;

	case STONE: //Fall through
	case COAL: //Fall through
	case IRON: //Fall through
	case DIAMOND: //Fall through
	case GOLD: //Fall through
	case RAINBOW_ORE: //Fall through
	case MAGMA_STONE:
		breakTime = 4.0f;	
		break;

	default: break;
	}

	switch(type)
	{
	case STONE: //Fall through
	case COAL: //Fall through
		if(item >= WOOD_PICKAXE)
			breakTime *= 0.4f;
	case IRON: //Fall through	
		if(item >= STONE_PICKAXE)
			breakTime *= 0.5f;
	case DIAMOND: //Fall through
	case GOLD: //Fall through
	case RAINBOW_ORE: //Fall through
	case MAGMA_STONE:	
	default: break;	
	}

	return breakTime;
}

int maxStack(enum Item item)
{
	if(item >= WOOD_PICKAXE)
		return 1;
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
	case GRASS: 
		if(item >= WOOD_PICKAXE)
			return GRASS_ITEM;
		return DIRT_ITEM;
	case LEAF:
		if(rand() % 32 == 0) return STICK;
		else if(rand() % 32 <= 4) return SAPLING_ITEM;
		break;	
	case STONE: 
		if(item >= WOOD_PICKAXE)	
			return STONE_ITEM;
		break;
	case BRICK: return BRICK_ITEM;
	case FLOWER: return FLOWER_ITEM;
	case TALL_GRASS:
		if(rand() % 3 == 0) return SEED_ITEM;
		break;
	case VINES: return VINES_ITEM;
	case COAL: 
		if(item >= WOOD_PICKAXE)	
			return COAL_ITEM;
		break;
	case IRON: 
		if(item >= STONE_PICKAXE)
			return IRON_ITEM;
	case DIAMOND: 
		return DIAMOND_ITEM;
	case GOLD: return GOLD_ITEM;
	case RAINBOW_ORE: return RAINBOW_ITEM;
	case MAGMA_STONE: 
		if(item >= WOOD_PICKAXE)
			return MAGMA_ITEM;
		break;
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

int maxUses(enum Item item)
{
	switch(item)
	{
	case WOOD_PICKAXE:
		return 16;
	case STONE_PICKAXE:
		return 64;
	default: break;
	}
	return 0;
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

int pickup(enum Item item, int amt, int uses, int maxUses, struct Inventory *inventory)
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
			inventory->slots[i].maxUsesLeft = maxUses;
			inventory->slots[i].usesLeft = uses;
			return amt; //Found slot to put something in
		}
		else if(inventory->slots[i].item == item &&
				inventory->slots[i].amount + amt > maxStack(inventory->slots[i].item))
		{
			amt -= (maxStack(inventory->slots[i].item) - inventory->slots[i].amount);
			inventory->slots[i].amount = maxStack(inventory->slots[i].item);
			inventory->slots[i].maxUsesLeft = maxUses;
			inventory->slots[i].usesLeft = uses;	
		}
	}

	for(int i = 0; i < inventory->maxSize; i++)
	{
		if(inventory->slots[i].item == NOTHING)
		{
			inventory->slots[i].item = item;
			inventory->slots[i].amount = amt;
			inventory->slots[i].maxUsesLeft = maxUses;
			inventory->slots[i].usesLeft = uses;
			return amt; //Found slot
		}
	}

	return amt;
}

void removeSlot(int ind, struct Inventory *inventory)
{
	if(ind >= inventory->maxSize)
		return;
	inventory->slots[ind] = EMPTY_INVENTORY_SLOT;
}

void decrementSlot(int ind, struct Inventory *inventory)
{
	if(ind >= inventory->maxSize)
		return;
	if(inventory->slots[ind].amount >= 0)
		inventory->slots[ind].amount--;
	if(inventory->slots[ind].amount <= 0)
		inventory->slots[ind] = EMPTY_INVENTORY_SLOT;
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

	turnOffTexture();
	for(int i = 0; i < inventory.maxSize; i++)
	{
		//Draw the uses left bar
		if(inventory.slots[i].maxUsesLeft > 0)
		{
			setRectColor(0.0f, 0.0f, 0.0f, 255.0f);
			setRectPos(x + (iconSz + spacing) * i, y - iconSz / 2.0f);		
			setRectSize(iconSz, iconSz / 8.0f);
			drawRect();
		}
	}
	for(int i = 0; i < inventory.maxSize; i++)
	{
		//Draw the uses left bar
		if(inventory.slots[i].maxUsesLeft > 0)
		{
			float percent = (float)inventory.slots[i].usesLeft /
							(float)inventory.slots[i].maxUsesLeft;
			setRectColor((1.0f - percent) * 255.0f, percent * 255.0f, 0.0f, 255.0f);
			setRectPos(x + (iconSz + spacing) * i - (iconSz / 2.0f) * (1.0f - percent), y - iconSz / 2.0f);		
			setRectSize(iconSz * percent, iconSz / 8.0f);
			drawRect();
		}
	}
	turnOnTexture();
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

struct InventorySlot itemAmt(enum Item item, int amt)
{
	struct InventorySlot requirement;
	requirement.item = item;
	requirement.amount = amt;
	return requirement;
}

struct InventorySlot itemAmtWithUses(enum Item item, int amt, int uses, int maxUses)
{
	struct InventorySlot requirement;
	requirement.item = item;
	requirement.amount = amt;
	requirement.usesLeft = uses;
	requirement.maxUsesLeft = maxUses;
	return requirement;
}

void use(int ind, struct Inventory *inventory)
{
	if(inventory->slots[ind].maxUsesLeft <= 0)
		return;
	inventory->slots[ind].usesLeft--;
	//Used up
	if(inventory->slots[ind].usesLeft < 0)
		inventory->slots[ind] = itemAmtWithUses(NOTHING, 0, 0, 0);
}
