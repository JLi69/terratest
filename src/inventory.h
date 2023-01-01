#ifndef INVENTORY_H

#define EMPTY_INVENTORY_SLOT itemAmtWithUses(NOTHING, 0, 0, 0);

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
	LAVA,
	SAPLING,
	PLANK_BLOCK,
	STONE_BRICK,
	SMOOTH_STONE,
	GLASS,
	FARMLAND,
	WHEAT1,
	WHEAT2,
	WHEAT3,
	WHEAT4,
	COAL_BLOCK,
	IRON_BLOCK,
	DIAMOND_BLOCK,
	GOLD_BLOCK,
	RAINBOW_BLOCK,
	LADDER,
	MAGMA_BRICK,
	MAGMA_TILE,
	SAND_BRICK,
	SAND_TILE,
	PILLAR,
	DOOR_BOTTOM_CLOSED,
	DOOR_TOP_CLOSED,
	DOOR_BOTTOM_OPEN,
	DOOR_TOP_OPEN,
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
	IRON_INGOT,
	GOLD_INGOT,
	
	WOOD_PICKAXE, //Obtained from crafting
	STONE_PICKAXE,
	IRON_PICKAXE,
	GOLD_PICKAXE,
	DIAMOND_PICKAXE,
	RAINBOW_PICKAXE,
	
	WOOD_AXE, //Obtained from crafting
	STONE_AXE,
	IRON_AXE,
	GOLD_AXE,
	DIAMOND_AXE,
	RAINBOW_AXE,

	WOOD_HOE, //Obtained from crafting
	STONE_HOE,
	IRON_HOE,
	GOLD_HOE,
	DIAMOND_HOE,
	RAINBOW_HOE,

	WOOD_SWORD, //Obtained from crafting
	STONE_SWORD,
	IRON_SWORD,
	GOLD_SWORD,
	DIAMOND_SWORD,
	RAINBOW_SWORD,
	
	BUCKET,
	WATER_BUCKET,
	LAVA_BUCKET,

	BREAD,
	CAKE,
	WHEAT,

	STONE_BRICK_ITEM,
	SMOOTH_STONE_ITEM,
	GLASS_ITEM,
	COAL_BLOCK_ITEM,
	IRON_BLOCK_ITEM,
	DIAMOND_BLOCK_ITEM,
	GOLD_BLOCK_ITEM,
	RAINBOW_BLOCK_ITEM,
	LADDER_ITEM,
	MAGMA_BRICK_ITEM,
	MAGMA_TILE_ITEM,
	SAND_BRICK_ITEM,
	SAND_TILE_ITEM,
	PILLAR_ITEM,

	DOOR_ITEM,
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
int maxUses(enum Item item);
struct Inventory createInventory(int sz);
//Returns 1 if found slot to put item in, 0 otherwise
int pickup(enum Item item, int amt, int uses, int maxUses, struct Inventory *inventory);
//Completely deletes an item from the inventory
void removeSlot(int ind, struct Inventory *inventory);
void decrementSlot(int ind, struct Inventory *inventory);
//Returns the amount given to the player
int removeAmountItem(enum Item item, int amt, struct Inventory *inventory);
int maxStack(enum Item item);

//Display inventory to the screen
void displayInventoryItemIcons(struct Inventory inventory, float x, float y, float iconSz,
							   float spacing);
void displayInventoryOutline(struct Inventory inventory, float x, float y, float iconSz,
							 float spacing);
void displayInventoryNumbers(struct Inventory inventory, float x, float y, float numSz,
							 float spacing);

struct InventorySlot itemAmt(enum Item item, int amt);
struct InventorySlot itemAmtWithUses(enum Item item, int amt, int uses, int maxUses);
void use(int ind, struct Inventory *inventory);

int isPartOfDoor(enum BlockType type);
int canReplace(enum BlockType type);

#endif

#define INVENTORY_H
