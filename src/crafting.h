#ifndef CRAFTING_H
#include "inventory.h"

#define MAX_ITEMS_IN_RECIPE 4
#define END_RECIPE itemAmt(NOTHING, 0)
#define RECIPE_COUNT 256


//TODO: better crafting menu?

void initRecipes();
//Returns result of crafting recipe
struct InventorySlot craft(struct Inventory *inventory, int recipeId);
void displayCraftingRecipesIcons(int start, int end, 
							float x, float y, float iconSz, 
							float spacingX, float spacingY);
void displayCraftingRecipesNumbers(int start, int end, 
							float x, float y, float iconSz, 
							float spacingX, float spacingY, float digitSz);
void displayCraftingRecipesDecorations(int start, int end, int highlighted, 
							float x, float y, float iconSz, 
							float spacingX, float spacingY);
int numberOfCraftingRecipes();

#endif

#define CRAFTING_H
