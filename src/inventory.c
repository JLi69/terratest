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
	case DOOR_BOTTOM_CLOSED: //Fall through
	case DOOR_TOP_CLOSED: //Fall through
	case DOOR_BOTTOM_OPEN: //Fall through
	case DOOR_TOP_OPEN: //Fall through
	case MAGMA_BRICK: //Fall through
	case MAGMA_TILE: //Fall through
	case SAND_BRICK: //Fall through
	case SAND_TILE: //Fall through
	case PILLAR: //Fall through
	case LADDER: //Fall through
	case COAL_BLOCK: //Fall through
	case IRON_BLOCK: //Fall through
	case DIAMOND_BLOCK: //Fall through
	case GOLD_BLOCK: //Fall through 
	case RAINBOW_BLOCK: //Fall through
	case STONE_BRICK: //Fall through
	case SMOOTH_STONE: //Fall through
	case GLASS: //Fall through
	case FARMLAND: //Fall through
	case BRICK: //Fall through
	case PLANK_BLOCK: //Fall through
	case GRASS: //Fall through
	case SAND: //Fall through
	case DIRT: 	
		breakTime = 0.5f; 
		if(item >= WOOD_PICKAXE && item <= RAINBOW_PICKAXE)
		{
			breakTime *= powf(0.5f, (item - WOOD_PICKAXE + 1));
		}
		break;
	
	case LOG: //Fall through
	case STUMP:  
		breakTime = 2.0f;
		if(item >= WOOD_AXE && item <= RAINBOW_AXE)
		{
			breakTime *= 0.7f;
			breakTime *= powf(0.7f, (item - WOOD_AXE + 1));
		}
		if(item == RAINBOW_AXE)
			breakTime *= 0.1f;
		break;

	case LEAF: //Fall through
	case TALL_GRASS: //Fall through
	case VINES: breakTime = 0.1f; break;

	case WHEAT1: //Fall through
	case WHEAT2: //Fall through
	case WHEAT3: //Fall through
	case WHEAT4: //Fall through
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
	case MAGMA_STONE: //Fall through
	case COAL: //Fall through
		if(item >= WOOD_PICKAXE && item <= RAINBOW_PICKAXE)
			breakTime *= 0.4f;
	case IRON: //Fall through	
		if(item >= STONE_PICKAXE && item <= RAINBOW_PICKAXE)
			breakTime *= 0.5f;
	case DIAMOND: //Fall through
	case GOLD: //Fall through
		if(item >= IRON_PICKAXE && item <= RAINBOW_PICKAXE)
			breakTime *= 0.5f;
	case RAINBOW_ORE:
		if(item >= GOLD_PICKAXE && item <= RAINBOW_PICKAXE)
			breakTime *= 0.5f;
		if(item >= DIAMOND_PICKAXE && item <= RAINBOW_PICKAXE)
			breakTime *= 0.5f;
		if(item == RAINBOW_PICKAXE)
			breakTime *= 0.1f;
	default: break;	
	}

	return breakTime;
}

int maxStack(enum Item item)
{
	if(item >= WOOD_PICKAXE && item <= CAKE)
		return 1;
	return 99;
}

//Probably shouldn't hardcode this, move this to an external file
enum Item droppedItem(enum BlockType type, enum Item item)
{
	switch(type)
	{	
	case COAL_BLOCK: return COAL_BLOCK_ITEM;
	case IRON_BLOCK: return IRON_BLOCK_ITEM;
	case DIAMOND_BLOCK: return DIAMOND_BLOCK_ITEM;
	case GOLD_BLOCK: return GOLD_BLOCK_ITEM;
	case RAINBOW_BLOCK: return RAINBOW_BLOCK_ITEM;
	case SMOOTH_STONE: return SMOOTH_STONE_ITEM;
	case STONE_BRICK: return STONE_BRICK_ITEM;
	case LOG: //Fall through
	case STUMP: return LOG_ITEM;
	case PLANK_BLOCK: return PLANK;
	case FARMLAND: //Fall through
	case DIRT: return DIRT_ITEM;
	case GRASS: 
		if(item >= WOOD_PICKAXE && item <= RAINBOW_PICKAXE)
			return GRASS_ITEM;
		return DIRT_ITEM;
	case LEAF:
		if(rand() % 32 == 0) return STICK;
		else if(rand() % 32 <= 4) return SAPLING_ITEM;
		break;	
	case STONE: 
		if(item >= WOOD_PICKAXE && item <= RAINBOW_PICKAXE)	
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
		if(item >= STONE_PICKAXE && item <= RAINBOW_PICKAXE)
			return IRON_ITEM;
		break;
	case DIAMOND: 
		if(item >= IRON_PICKAXE && item <= RAINBOW_PICKAXE)	
			return DIAMOND_ITEM;
		break;
	case GOLD: 
		if(item >= IRON_PICKAXE && item <= RAINBOW_PICKAXE)
			return GOLD_ITEM;
		break;
	case RAINBOW_ORE: 
		if(item >= GOLD_PICKAXE && item <= RAINBOW_PICKAXE)	
			return RAINBOW_ITEM;
		break;
	case MAGMA_STONE: 
		if(item >= WOOD_PICKAXE && item <= RAINBOW_PICKAXE)
			return MAGMA_ITEM;
		break;
	case SAND: return SAND_ITEM;
	case SAPLING: return SAPLING_ITEM;
	case WHEAT1: //Fall through
	case WHEAT2: //Fall through
	case WHEAT3: //Fall through
		return SEED_ITEM;
	case WHEAT4:
		return WHEAT;
	case LADDER:
		return LADDER_ITEM;
	case MAGMA_BRICK: return MAGMA_BRICK_ITEM;
	case MAGMA_TILE: return MAGMA_TILE_ITEM;
	case SAND_BRICK: return SAND_BRICK_ITEM;
	case SAND_TILE: return SAND_TILE_ITEM;
	case PILLAR: return PILLAR_ITEM;
	
	case DOOR_BOTTOM_CLOSED: //Fall through
	case DOOR_TOP_CLOSED: //Fall through
	case DOOR_BOTTOM_OPEN: //Fall through
	case DOOR_TOP_OPEN: //Fall through
		return DOOR_ITEM;
	
	default: break;
	}
	return NOTHING;
}

//Probably shouldn't hardcode this, move to an external file
enum BlockType placeBlock(enum Item item)
{
	switch(item)
	{
	case DOOR_ITEM: return DOOR_BOTTOM_CLOSED;
	case MAGMA_BRICK_ITEM: return MAGMA_BRICK;
	case MAGMA_TILE_ITEM: return MAGMA_TILE;
	case SAND_BRICK_ITEM: return SAND_BRICK;
	case SAND_TILE_ITEM: return SAND_TILE;
	case PILLAR_ITEM: return PILLAR;
	case COAL_BLOCK_ITEM: return COAL_BLOCK;
	case IRON_BLOCK_ITEM: return IRON_BLOCK;
	case DIAMOND_BLOCK_ITEM: return DIAMOND_BLOCK;
	case GOLD_BLOCK_ITEM: return GOLD_BLOCK;
	case RAINBOW_BLOCK_ITEM: return RAINBOW_BLOCK;
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
	case SEED_ITEM: return WHEAT1;
	case WATER_BUCKET: return WATER;
	case LAVA_BUCKET: return LAVA;
	case STONE_BRICK_ITEM: return STONE_BRICK;
	case SMOOTH_STONE_ITEM: return SMOOTH_STONE;
	case GLASS_ITEM: return GLASS;
	case LADDER_ITEM: return LADDER;

	default: break;
	}
	return NONE;
}

int maxUses(enum Item item)
{
	switch(item)
	{
	case WOOD_SWORD: //Fall through
	case WOOD_AXE: //Fall through
	case WOOD_HOE: //Fall through
	case WOOD_PICKAXE: return 16;
	
	case STONE_SWORD: //Fall through
	case STONE_AXE: //Fall through
	case STONE_HOE: //Fall through
	case STONE_PICKAXE: return 64;
	
	case IRON_SWORD: //Fall through
	case IRON_AXE: //Fall through
	case IRON_HOE: //Fall through
	case IRON_PICKAXE: return 128;
	
	case GOLD_SWORD: //Fall through
	case GOLD_AXE: //Fall through
	case GOLD_HOE: //Fall through
	case GOLD_PICKAXE: return 256;

	case DIAMOND_SWORD: //Fall through
	case DIAMOND_AXE: //Fall through
	case DIAMOND_HOE: //Fall through
	case DIAMOND_PICKAXE: return 2048;

	case RAINBOW_SWORD: //Fall through
	case RAINBOW_AXE: //Fall through
	case RAINBOW_HOE: //Fall through
	case RAINBOW_PICKAXE: return 16384;	
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
	int total = 0;
	for(int i = 0; i < inventory->maxSize; i++)
	{
		if(amt <= 0)
			return total;
		if(inventory->slots[i].amount >= maxStack(inventory->slots[i].item))
			continue;

		if(inventory->slots[i].item == item &&
			inventory->slots[i].amount + amt <= maxStack(inventory->slots[i].item))
		{
			total += amt;
			inventory->slots[i].amount += amt;
			inventory->slots[i].maxUsesLeft = maxUses;
			inventory->slots[i].usesLeft = uses;
			return total; //Found slot to put something in
		}
		else if(inventory->slots[i].item == item &&
				inventory->slots[i].amount + amt > maxStack(inventory->slots[i].item))
		{
			total += (maxStack(inventory->slots[i].item) - inventory->slots[i].amount);
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
			total += amt;
			inventory->slots[i].item = item;
			inventory->slots[i].amount = amt;
			inventory->slots[i].maxUsesLeft = maxUses;
			inventory->slots[i].usesLeft = uses;
			return total; //Found slot
		}
	}

	return total;
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

int isPartOfDoor(enum BlockType type)
{
	return type == DOOR_TOP_OPEN ||
		   type == DOOR_TOP_CLOSED ||
		   type == DOOR_BOTTOM_CLOSED ||
		   type == DOOR_BOTTOM_OPEN;
}

int canReplace(enum BlockType type)
{
	return type == WATER ||
		   type == LAVA ||
		   type == NONE;
}

int healAmount(enum Item item)
{
	switch(item)
	{
	case BREAD: return 1;
	case CAKE: return 3;
	default: break;
	}
	return 0;
}
