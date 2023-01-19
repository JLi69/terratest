#include "crafting.h"
#include <stdlib.h>
#include "draw.h"

static int recipeCount = 0; 
static struct InventorySlot recipes[RECIPE_COUNT][MAX_ITEMS_IN_RECIPE + 2];

//Just a temporary macro, will not hardcode recipes later
//hopefully I get around to that
//
//Or maybe I won't
#define CREATE_RECIPE(res, a, b, c, d) \
	recipes[recipeCount][0] = res; \
	recipes[recipeCount][1] = a; \
	recipes[recipeCount][2] = b; \
	recipes[recipeCount][3] = c; \
	recipes[recipeCount][4] = d; \
	recipes[recipeCount][5] = d; \
	recipeCount++;

//Probably shouldn't be hardcoded, maybe move to an external file later
void initRecipes()
{
	CREATE_RECIPE(itemAmt(PLANK, 4), //Result
				  itemAmt(LOG_ITEM, 1), //Ingredients
				  END_RECIPE, END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(STICK, 8), //Result
				  itemAmt(LOG_ITEM, 1), //Ingredients
				  END_RECIPE, END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(WOOD_PICKAXE, 1), //Result
				  itemAmt(PLANK, 4), //Ingredients
				  itemAmt(STICK, 4), END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(BRICK_ITEM, 8), //Result
				  itemAmt(DIRT_ITEM, 1),
				  itemAmt(STONE_ITEM, 1),
				  itemAmt(COAL_ITEM, 1),
				  END_RECIPE);
	CREATE_RECIPE(itemAmt(STONE_BLOCK, 1), //Result
				  itemAmt(STONE_ITEM, 2), //Ingredients
				  END_RECIPE, END_RECIPE, END_RECIPE);	
	CREATE_RECIPE(itemAmt(STONE_PICKAXE, 1), //Result
				  itemAmt(STONE_ITEM, 4), //Ingredients
				  itemAmt(STONE_BLOCK, 1), 
				  itemAmt(STICK, 4), END_RECIPE);
	CREATE_RECIPE(itemAmt(IRON_INGOT, 2), //Result
				  itemAmt(IRON_ITEM, 1), //Ingredients
				  itemAmt(COAL_ITEM, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(IRON_PICKAXE, 1), //Result
				  itemAmt(IRON_INGOT, 4), //Ingredients
				  itemAmt(STICK, 3), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GOLD_INGOT, 1), //Result
				  itemAmt(GOLD_ITEM, 1), //Ingredients
				  itemAmt(COAL_ITEM, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GOLD_PICKAXE, 1), //Result
				  itemAmt(GOLD_INGOT, 6), //Ingredients
				  itemAmt(STICK, 3), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(DIAMOND_PICKAXE, 1), //Result
				  itemAmt(DIAMOND_ITEM, 6), //Ingredients
				  itemAmt(STICK, 3), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(RAINBOW_PICKAXE, 1), //Result
				  itemAmt(RAINBOW_ITEM, 5), //Ingredients
				  itemAmt(STICK, 3), 
				  END_RECIPE, END_RECIPE);
	//Axe recipes
	CREATE_RECIPE(itemAmt(WOOD_AXE, 1), //Result
				  itemAmt(PLANK, 3), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(STONE_AXE, 1), //Result
				  itemAmt(STONE_ITEM, 3), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(IRON_AXE, 1), //Result
				  itemAmt(IRON_INGOT, 3), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GOLD_AXE, 1), //Result
				  itemAmt(GOLD_INGOT, 3), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(DIAMOND_AXE, 1), //Result
				  itemAmt(DIAMOND_ITEM, 3), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(RAINBOW_AXE, 1), //Result
				  itemAmt(RAINBOW_ITEM, 3), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);

	//Hoe recipes
	CREATE_RECIPE(itemAmt(WOOD_HOE, 1), //Result
				  itemAmt(PLANK, 2), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(STONE_HOE, 1), //Result
				  itemAmt(STONE_ITEM, 2), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(IRON_HOE, 1), //Result
				  itemAmt(IRON_INGOT, 2), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GOLD_HOE, 1), //Result
				  itemAmt(GOLD_INGOT, 2), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(DIAMOND_HOE, 1), //Result
				  itemAmt(DIAMOND_ITEM, 2), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(RAINBOW_HOE, 1), //Result
				  itemAmt(RAINBOW_ITEM, 2), //Ingredients
				  itemAmt(STICK, 2), 
				  END_RECIPE, END_RECIPE);

	//Sword recipes
	CREATE_RECIPE(itemAmt(WOOD_SWORD, 1), //Result
				  itemAmt(PLANK, 4), //Ingredients
				  itemAmt(STICK, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(STONE_SWORD, 1), //Result
				  itemAmt(STONE_BLOCK, 4), //Ingredients
				  itemAmt(STICK, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(IRON_SWORD, 1), //Result
				  itemAmt(IRON_INGOT, 4), //Ingredients
				  itemAmt(STICK, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GOLD_SWORD, 1), //Result
				  itemAmt(GOLD_INGOT, 4), //Ingredients
				  itemAmt(STICK, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(DIAMOND_SWORD, 1), //Result
				  itemAmt(DIAMOND_ITEM, 4), //Ingredients
				  itemAmt(STICK, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(RAINBOW_SWORD, 1), //Result
				  itemAmt(RAINBOW_ITEM, 4), //Ingredients
				  itemAmt(STICK, 1), 
				  END_RECIPE, END_RECIPE);

	CREATE_RECIPE(itemAmt(BUCKET, 1), //Result
				  itemAmt(IRON_INGOT, 2), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);

	CREATE_RECIPE(itemAmt(BREAD, 1), //Result
				  itemAmt(WHEAT, 4), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);

	CREATE_RECIPE(itemAmt(CAKE, 1), //Result
				  itemAmt(WHEAT, 16), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	
	CREATE_RECIPE(itemAmt(COOKED_MEAT, 1), //Result
				  itemAmt(RAW_MEAT, 1), //Ingredients
				  itemAmt(COAL_ITEM, 1), 
				  END_RECIPE, END_RECIPE);

	CREATE_RECIPE(itemAmt(DOOR_ITEM, 1), //Result
				  itemAmt(PLANK, 2), //Ingredients
				  END_RECIPE, 
				  END_RECIPE,
				  END_RECIPE);
	CREATE_RECIPE(itemAmt(STONE_BRICK_ITEM, 8), //Result
				  itemAmt(STONE_ITEM, 3), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(SMOOTH_STONE_ITEM, 1), //Result
				  itemAmt(STONE_BRICK_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GLASS_ITEM, 16), //Result
				  itemAmt(SAND_ITEM, 1), //Ingredients
				  itemAmt(COAL_ITEM, 1), 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(LADDER_ITEM, 16), //Result
				  itemAmt(STICK, 8), //Ingredients
				  END_RECIPE,	
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(SAND_BRICK_ITEM, 8), //Result
				  itemAmt(SAND_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(SAND_TILE_ITEM, 1), //Result
				  itemAmt(SAND_BRICK_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(MAGMA_BRICK_ITEM, 8), //Result
				  itemAmt(MAGMA_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(MAGMA_TILE_ITEM, 1), //Result
				  itemAmt(MAGMA_BRICK_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(PILLAR_ITEM, 1), //Result
				  itemAmt(STONE_BRICK_ITEM, 1), //Ingredients
				  itemAmt(SMOOTH_STONE_ITEM, 1), 
				  itemAmt(STONE_BLOCK, 1),
				  itemAmt(STONE_ITEM, 4));

	//Ore blocks to flex your wealth
	CREATE_RECIPE(itemAmt(COAL_BLOCK_ITEM, 1), //Result
				  itemAmt(COAL_ITEM, 10), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(IRON_BLOCK_ITEM, 1), //Result
				  itemAmt(IRON_INGOT, 10), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(DIAMOND_BLOCK_ITEM, 1), //Result
				  itemAmt(DIAMOND_ITEM, 10), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GOLD_BLOCK_ITEM, 1), //Result
				  itemAmt(GOLD_INGOT, 10), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(RAINBOW_BLOCK_ITEM, 1), //Result
				  itemAmt(RAINBOW_ITEM, 10), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(COAL_ITEM, 10), //Result
				  itemAmt(COAL_BLOCK_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(IRON_INGOT, 10), //Result
				  itemAmt(IRON_BLOCK_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(DIAMOND_ITEM, 10), //Result
				  itemAmt(DIAMOND_BLOCK_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(GOLD_INGOT, 10), //Result
				  itemAmt(GOLD_INGOT, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
	CREATE_RECIPE(itemAmt(RAINBOW_ITEM, 10), //Result
				  itemAmt(RAINBOW_BLOCK_ITEM, 1), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);

	CREATE_RECIPE(itemAmt(SLIME_BLOCK_ITEM, 1), //Result
				  itemAmt(SLIMEBALL, 10), //Ingredients
				  END_RECIPE, 
				  END_RECIPE, END_RECIPE);
}

struct InventorySlot craft(struct Inventory *inventory, int recipeId)
{
	int canCraft = 1;

	for(int i = 1; i <= MAX_ITEMS_IN_RECIPE && recipes[recipeId][i].item != END_RECIPE.item && canCraft; i++)
	{
		//Check if there is enough stuff for that item
		int total = 0;
		for(int j = 0; j < inventory->maxSize; j++)	
			if(inventory->slots[j].item == recipes[recipeId][i].item)
				total += inventory->slots[j].amount;
		if(total < recipes[recipeId][i].amount)
			canCraft = 0;
	}

	if(!canCraft)
		return itemAmt(NOTHING, 0);	
	
	for(int i = 1; i <= MAX_ITEMS_IN_RECIPE && recipes[recipeId][i].item != END_RECIPE.item; i++)
	{
		removeAmountItem(recipes[recipeId][i].item, recipes[recipeId][i].amount, inventory);
	}

	return recipes[recipeId][0];
}

void displayCraftingRecipesIcons(int start, int end,
							float x, float y, float iconSz, float spacingX,
							float spacingY)
{
	float offset = - (1.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (iconSz + spacingX);
	setRectSize(iconSz, iconSz);
	for(int i = start; i < end; i++)
	{
		if(i < 0 || i >= recipeCount)
			continue;
	
		for(int j = 1; j <= MAX_ITEMS_IN_RECIPE && recipes[i][j].item != END_RECIPE.item; j++)
		{
			setRectPos(x + (j - 1) * (iconSz + spacingX) + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - 
						  (i - start + 0.5f) * (iconSz + spacingY));
			setTexOffset(1.0f / 16.0f * (float)((recipes[i][j].item - 1) % 16),
						 1.0f / 16.0f * (float)((recipes[i][j].item - 1) / 16));
			drawRect();
		}
		setRectPos(x + (iconSz + spacingX) * 4.0f + iconSz + spacingX + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - 
						  (i - start + 0.5f) * (iconSz + spacingY + 0.5f));
		setTexOffset(1.0f / 16.0f * (float)((recipes[i][0].item - 1) % 16),
					 1.0f / 16.0f * (float)((recipes[i][0].item - 1) / 16));
		drawRect();
	}
}

void displayCraftingRecipesNumbers(int start, int end, 
							float x, float y, float iconSz, 
							float spacingX, float spacingY, float digitSz)
{
	float offset = - (1.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (iconSz + spacingX);
	for(int i = start; i < end; i++)
	{
		if(i < 0 || i >= recipeCount)
			continue;
	
		for(int j = 1; j <= MAX_ITEMS_IN_RECIPE && recipes[i][j].item != END_RECIPE.item; j++)
		{
			drawInteger(recipes[i][j].amount, x + (j - 1) * (iconSz + spacingX) + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - (i - start + 0.5f) * (iconSz + spacingY), digitSz);	
		}

		drawInteger(recipes[i][0].amount, x + (iconSz + spacingX) * 4.0f + iconSz + spacingX + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - (i - start + 0.5f) * (iconSz + spacingY), digitSz);
	}
}

void displayCraftingRecipesDecorations(int start, int end, int highlighted, 
							float x, float y, float iconSz, 
							float spacingX, float spacingY)
{
	float offset = - (1.0f + MAX_ITEMS_IN_RECIPE) / 2.0f * (iconSz + spacingX);
	turnOffTexture();
	setRectPos(x, y);
	
	setRectSize((MAX_ITEMS_IN_RECIPE + 3.0f) * (iconSz + spacingX),
				((float)(end - start)) * (iconSz + spacingY));
	setRectColor(64.0f, 32.0f, 0.0f, 255.0f);
	drawRect();
	
	setRectSize((MAX_ITEMS_IN_RECIPE + 3.0f) * (iconSz + spacingX) - 8.0f,
				((float)(end - start)) * (iconSz + spacingY) - 8.0f);
	setRectColor(128.0f, 64.0f, 0.0f, 255.0f);
	drawRect();

	setRectColor(192.0f, 96.0f, 0.0f, 255.0f);
	setRectPos(x, y - (highlighted - start + 0.5f) * (iconSz + spacingY) + (end - start) / 2.0f * (iconSz + spacingY));
	setRectSize((MAX_ITEMS_IN_RECIPE + 3.0f) * (iconSz + spacingX) - 8.0f,
				((iconSz + spacingY)));
	drawRect();

	turnOnTexture();

	drawString("Crafting", x, y + ((end - start) / 2) * (iconSz + spacingY) + (iconSz + spacingY) / 2.0f, iconSz);

	//Draw arrows
	setRectSize(iconSz, iconSz);	
	setTexOffset(8.0f / 16.0f, 0.0f);
	for(int i = start; i < end; i++)
	{
		if(i < 0 || i >= recipeCount)
			continue;
		setRectPos(x + (iconSz + spacingX) * 4.0f + offset,
				   y + (end - start) / 2.0f * (iconSz + spacingY) - (i - start + 0.5f) * (iconSz + spacingY));
		drawRect();
	}	
}

int numberOfCraftingRecipes()
{
	return recipeCount; 
}
