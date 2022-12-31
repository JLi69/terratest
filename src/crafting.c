#include "crafting.h"
#include <stdlib.h>
#include "draw.h"

static int recipeCount = 0; 
static struct InventorySlot recipes[RECIPE_COUNT][MAX_ITEMS_IN_RECIPE + 1];

struct InventorySlot itemAmt(enum Item item, int amt)
{
	struct InventorySlot requirement;
	requirement.item = item;
	requirement.amount = amt;
	return requirement;
}

//Probably shouldn't be hardcoded, maybe move to an external file later
void initRecipes()
{
	CREATE_RECIPE(0, 
				  itemAmt(PLANK, 4), //Result
				  itemAmt(LOG_ITEM, 1), //Ingredients
				  END_RECIPE, END_RECIPE, END_RECIPE);
	CREATE_RECIPE(1, 
				  itemAmt(STICK, 8), //Result
				  itemAmt(LOG_ITEM, 1), //Ingredients
				  END_RECIPE, END_RECIPE, END_RECIPE);
	CREATE_RECIPE(2, 
				  itemAmt(WOOD_PICKAXE, 1), //Result
				  itemAmt(PLANK, 4), //Ingredients
				  itemAmt(STICK, 4), END_RECIPE, END_RECIPE);
	CREATE_RECIPE(3, 
				  itemAmt(STONE_BLOCK, 1), //Result
				  itemAmt(STONE_ITEM, 3), //Ingredients
				  END_RECIPE, END_RECIPE, END_RECIPE);
}

struct InventorySlot craft(struct Inventory *inventory, int recipeId)
{
	return itemAmt(NOTHING, 0);	
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
	
		for(int j = 1; j <= MAX_ITEMS_IN_RECIPE; j++)
		{
			if(recipes[i][j].item == END_RECIPE.item)
				break;
			setRectPos(x + (j - 1) * (iconSz + spacingX) + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - 
						  (i - start + 1.0f) * (iconSz + spacingY));
			setTexOffset(1.0f / 16.0f * (float)((recipes[i][j].item - 1) % 16),
						 1.0f / 16.0f * (float)((recipes[i][j].item - 1) / 16));
			drawRect();
		}
		setRectPos(x + (iconSz + spacingX) * 4.0f + iconSz + spacingX + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - 
						  (i - start + 1.0f) * (iconSz + spacingY));
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
	
		for(int j = 1; j <= MAX_ITEMS_IN_RECIPE; j++)
		{
			if(recipes[i][j].item == END_RECIPE.item)
				break;
			drawInteger(recipes[i][j].amount, x + (j - 1) * (iconSz + spacingX) + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - (i - start + 1.0f) * (iconSz + spacingY), digitSz);	
		}

		drawInteger(recipes[i][0].amount, x + (iconSz + spacingX) * 4.0f + iconSz + spacingX + offset, y + (end - start) / 2.0f * (iconSz + spacingY) - (i - start + 1.0f) * (iconSz + spacingY), digitSz);
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
				((float)(end - start) - 0.5f) * (iconSz + spacingY));
	setRectColor(64.0f, 32.0f, 0.0f, 255.0f);
	drawRect();
	
	setRectSize((MAX_ITEMS_IN_RECIPE + 3.0f) * (iconSz + spacingX) - 8.0f,
				((float)(end - start) - 0.5f) * (iconSz + spacingY) - 8.0f);
	setRectColor(128.0f, 64.0f, 0.0f, 255.0f);
	drawRect();

	setRectColor(192.0f, 96.0f, 0.0f, 255.0f);
	setRectPos(x, y - (highlighted - start + 1) * (iconSz + spacingY) + (end - start) / 2.0f * (iconSz + spacingY));
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
				   y + (end - start) / 2.0f * (iconSz + spacingY) - (i - start + 1.0f) * (iconSz + spacingY));
		drawRect();
	}	
}

int numberOfCraftingRecipes()
{
	return recipeCount; 
}
