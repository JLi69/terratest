#ifndef CRAFTING_H
#include "inventory.h"

#define MAX_ITEMS_IN_RECIPE 4
#define END_RECIPE itemAmt(NOTHING, 0)
#define RECIPE_COUNT 256

//Just a temporary macro, will not hardcode recipes later
//hopefully I get around to that
//
//Or maybe I won't
#define CREATE_RECIPE(ind, res, a, b, c, d) \
	recipes[ind][0] = res; \
	recipes[ind][1] = a; \
	recipes[ind][2] = b; \
	recipes[ind][3] = c; \
	recipes[ind][4] = d; \
	recipeCount++;


struct InventorySlot itemAmt(enum Item item, int amt);
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
