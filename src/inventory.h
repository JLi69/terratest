#ifndef INVENTORY_H

//Blocks types
enum BlockType 
{
	NONE = 0,
	GRASS,
	DIRT,
	STONE,
	INDESTRUCTABLE,
	BRICK,
	LEAF,
	LOG,
	STUMP,
	FLOWER,
	TALL_GRASS,
	VINES,
	COAL,
	IRON,
	DIAMOND,
	GOLD,
	RAINBOW_ORE,
	MAGMA_STONE,
	SAND,
	WATER,
	LAVA
};

//item types
enum Item
{
	NOTHING = 0,
	//"Natural blocks"
	GRASS_ITEM, //Obtained when breaking grass with a pickaxe
	DIRT_ITEM, //Obtained when breaking dirt or grass without a pickaxe
	STONE_ITEM, //Obtained when mining stone with a pickaxe
	STONE_BLOCK, //Obtained from crafting
	INDESTRUCTABLE_ITEM, //You are not supposed to have this
	BRICK_ITEM, //Obtained from crafting or mining brick with pickaxe
	SAPLING_ITEM, //Obtained from breaking leaves
	STICK, //Obtained from breaking leaves or breaking wood
	LOG_ITEM, //Obtained from breaking wood
	PLANK,
	FLOWER_ITEM, //Obtained from breaking flowers
	SEED_ITEM, //Obtained from breaking tall grass
	VINES_ITEM, //Obtained from breaking leaves
	COAL_ITEM, //Obtained from mining coal with a pickaxe
	IRON_ITEM, //Obtained from mining iron with a pickaxe
	DIAMOND_ITEM, //Obtained from mining diamond with a pickaxe
	GOLD_ITEM, //Obtained from mining gold with a pickaxe
	RAINBOW_ITEM, //Obtained from mining rainbow ore with a pickaxe
	MAGMA_ITEM, //Obtained from mining a magma block with a pickaxe
	SAND_ITEM, //Obtained from breaking sand
	BUCKET, //Obtained from crafting
	WATER_BUCKET, //Obtained from collecting water in a bucket
	LAVA_BUCKET, //Obtained from collecting lava in a bucket
	WOOD_PICKAXE, //Obtained from crafting
};

struct InventorySlot
{
	enum Item item;
	int amount;
	int usesLeft, maxUsesLeft;
};

struct Inventory
{
	struct InventorySlot* slots;
	int maxSize;
	int selected;
};

//How long it takes for a block to be broken by an item in seconds
float timeToBreakBlock(enum BlockType type, enum Item item);
//Item dropped by a block
enum Item droppedItem(enum BlockType type, enum Item item);
enum BlockType placeBlock(enum Item item);
struct Inventory createInventory(int sz);
//Returns 1 if found slot to put item in, 0 otherwise
int pickup(enum Item item, int amt, struct Inventory *inventory);
//Completely deletes an item from the inventory
void removeSlot(int ind, struct Inventory *inventory);
void decrementSlot(int ind, struct Inventory *inventory);
//Returns 1 if it can remove the amount of items from inventory, 0 otherwise
int removeAmountItem(enum Item item, int amt, struct Inventory *inventory);

//Display inventory to the screen
void displayInventoryItemIcons(struct Inventory inventory, float x, float y, float iconSz,
							   float spacing);
void displayInventoryOutline(struct Inventory inventory, float x, float y, float iconSz,
							 float spacing);
void displayInventoryNumbers(struct Inventory inventory, float x, float y, float numSz,
							 float spacing);

#endif

#define INVENTORY_H
