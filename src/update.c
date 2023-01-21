#include "game.h"
#include "world.h"
#include "window-func.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "crafting.h"
#include "menu.h"

static float breakBlockTimer = 0.0f;
static int menuShown = 0;
static int menuSelection = 0;
static enum BlockType selectedBlock = NONE;
static int menuBegin = 0, menuEnd = 8;

#define BUCKET_DELAY_TIME 0.5f
static float bucketDelay = BUCKET_DELAY_TIME + 1.0f;

static const enum BlockType transparent[] = { LOG,
											  STUMP,
											  FLOWER,
											  TALL_GRASS,
											  VINES,
											  SAPLING,
											  WHEAT1,
											  WHEAT2,
											  WHEAT3,
											  WHEAT4,
											  LADDER,
											  PILLAR };

void initGame(struct World *world, struct Player *player, int seed)
{
	const float height = 128.0f;
	//TODO: save system where we can load worlds from disk
	//Perhaps also implement chunking to reduce memory usage?
	*world = generateWorld(seed, height, 256.0f);
	revealVisible(world);

	player->playerSpr = createSprite(createRect(0.0f, 32.0f * 4.0f * height, 32.0f, 64.0f));
	player->playerSpr.animationState = IDLE;
	player->playerSpr.animating = 1;
	player->inventory = createInventory(20); //inventory of size 20	

	struct Sprite collision;
	while(!blockCollisionSearch(player->playerSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collision))
		player->playerSpr.hitbox.position.y -= 32.0f;
	player->playerSpr.hitbox.position.y += 32.0f;
	player->playerSpr.canMove = 1;
	player->health = START_HEALTH;
	player->maxHealth = START_HEALTH;
	player->breath = BREATH;
	player->maxBreath = BREATH;
	player->damageCooldown = DAMAGE_COOLDOWN + 1.0f;
	player->damageTaken = 0;
	player->useItemTimer = 0.0f;

#ifdef DEV_VERSION 
	player->inventory.slots[0] = itemAmt(BREAD, 99);
	player->inventory.slots[1] = itemAmt(CAKE, 99);
	player->inventory.slots[2] = itemAmt(BRICK_ITEM, 99);
	player->inventory.slots[3] = itemAmt(MAGMA_ITEM, 99);
	player->inventory.slots[4] = itemAmt(LAVA_BUCKET, 1);
	player->inventory.slots[5] = itemAmt(RAINBOW_SWORD, 1);
	player->inventory.slots[6] = itemAmt(WATER_BUCKET, 1);
	player->inventory.slots[7] = itemAmt(DIAMOND_PICKAXE, 1);
	player->inventory.slots[8] = itemAmt(HEART_ITEM, 99);

	//world->dayCycle = 0.8f;
	//world->moonPhase = 0.75f;
#endif
	srand(time(0));
}

void updateGameobjects(struct World *world, struct Player *player, float secondsPerFrame)
{
	struct Vector2D camPos = createVector(player->playerSpr.hitbox.position.x, player->playerSpr.hitbox.position.y);

	if(isPaused())
	{
		//Unpause the game
		if(buttonClicked(PAUSED, 0, GLFW_MOUSE_BUTTON_LEFT))
		{
			setPaused(0);		
			toggleCursor();
		}
	}

	//Pause/Unpause the game
	if(!craftingMenuShown() && isPressedOnce(GLFW_KEY_ESCAPE))
	{
		setPaused(!isPaused());
		toggleCursor();
		return;	
	}

	//Don't update the game if paused
	if(isPaused())
		return;

	//Dead, don't update the world
	if(player->health <= 0)
	{	
		//Respawn
		if(isPressed(GLFW_KEY_R) || buttonClicked(RESPAWN, 0, GLFW_MOUSE_BUTTON_LEFT))
		{
			player->playerSpr = createSprite(createRect(0.0f, BLOCK_SIZE * world->worldBoundingRect.maxY, 32.0f, 64.0f));
			player->playerSpr.animationState = IDLE;
			player->playerSpr.animating = 1;

			player->damageCooldown = 0.0f;
			struct Sprite collision;
			while(!blockCollisionSearch(player->playerSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collision))
				player->playerSpr.hitbox.position.y -= 32.0f;
			player->playerSpr.hitbox.position.y += 32.0f;
			player->playerSpr.canMove = 1;
			player->health = START_HEALTH;
			player->maxHealth = START_HEALTH;
			player->breath = BREATH;
			player->maxBreath = BREATH;
		}

		return;
	}

	if(isPressedOnce(GLFW_KEY_TAB))
		toggleCraftingMenu();
	if(craftingMenuShown())
	{
		if(isPressedOnce(GLFW_KEY_UP))
			menuSelection--;
		if(isPressedOnce(GLFW_KEY_DOWN))
			menuSelection++;	
		if(isPressedOnce(GLFW_KEY_ESCAPE))
			toggleCraftingMenu();
		//Craft
		int enterPressed = isPressedOnce(GLFW_KEY_ENTER);
		if(enterPressed && (isPressedOnce(GLFW_KEY_LEFT_SHIFT) || isPressedOnce(GLFW_KEY_RIGHT_SHIFT)))
		{
			struct InventorySlot crafted = craft(&player->inventory, menuSelection);
			crafted.maxUsesLeft = crafted.usesLeft = maxUses(crafted.item);
			while(crafted.item != NOTHING)
			{
				int pickedup = pickup(crafted.item, crafted.amount, crafted.usesLeft, crafted.maxUsesLeft, &player->inventory);
				addItem(world, itemAmtWithUses(crafted.item, crafted.amount - pickedup, crafted.usesLeft, crafted.maxUsesLeft), camPos.x, camPos.y + BLOCK_SIZE); 
				crafted = craft(&player->inventory, menuSelection);	
			}	
		}
		if(enterPressed)
		{
			struct InventorySlot crafted = craft(&player->inventory, menuSelection);
			crafted.maxUsesLeft = crafted.usesLeft = maxUses(crafted.item);
			if(crafted.item != NOTHING)
			{
				int pickedup = pickup(crafted.item, crafted.amount, crafted.usesLeft, crafted.maxUsesLeft, &player->inventory);
				addItem(world, itemAmtWithUses(crafted.item, crafted.amount - pickedup, crafted.usesLeft, crafted.maxUsesLeft), camPos.x, camPos.y + BLOCK_SIZE); 
			} 
		}

		//Wrap around
		if(menuSelection < 0)
			menuSelection = numberOfCraftingRecipes() - 1;
		else if(menuSelection >= numberOfCraftingRecipes())
			menuSelection = 0;
		menuBegin = (menuSelection / 8) * 8;
		menuEnd = (menuSelection / 8 + 1) * 8;

		//return; //Pause game when on crafting menu
	}	

	static float blockUpdateTimer = 0.0f;
	blockUpdateTimer += secondsPerFrame;
	bucketDelay += secondsPerFrame;
	player->damageCooldown -= secondsPerFrame;

	//Take Damage
	//Drown if the player is under water
	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE + 1, WATER, 0.5f))
	{	
		player->breath -= secondsPerFrame;
		if(player->breath <= 0.0f)
			damagePlayer(player, 1);
	}
	else player->breath = player->maxBreath;
	
	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE - 1.0f, MAGMA_STONE, 0.0f))
		damagePlayer(player, 1);

	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, LAVA, 0.1f))
		damagePlayer(player, 3);

	struct Sprite collided;	

	//printf("%f %f\n", player->hitbox.position.x / BLOCK_SIZE, player->hitbox.position.y / BLOCK_SIZE);

	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, WATER, 0.2f))
	{
		player->playerSpr.vel.x *= 0.4f;
		if(player->playerSpr.vel.y > 0.0f)
			player->playerSpr.vel.y *= 0.6f;
	}
	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, LAVA, 0.2f))
	{	
		player->playerSpr.vel.x *= 0.2f;
		if(player->playerSpr.vel.y > 0.0f)
			player->playerSpr.vel.y *= 0.1f;	
	}

	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE - 1, SLIME_BLOCK, -1.0f))
		player->playerSpr.vel.x *= 0.5f;
	//Move player in the x direction
	updateSpriteX(&player->playerSpr, secondsPerFrame);
	//Check for collision
	if(blockCollisionSearch(player->playerSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided))
	{				
		//Uncollide the player	
		if(player->playerSpr.vel.x != 0.0f)
		{
			if(player->playerSpr.hitbox.position.x >=
			   collided.hitbox.position.x + collided.hitbox.dimensions.w / 2.0f)
			{
				player->playerSpr.vel.x = 0.0f;
				player->playerSpr.hitbox.position.x =
					collided.hitbox.position.x +
					collided.hitbox.dimensions.w / 2.0f +
					player->playerSpr.hitbox.dimensions.w / 2.0f;	
			}	
			else if(player->playerSpr.hitbox.position.x <= 
					collided.hitbox.position.x - collided.hitbox.dimensions.x / 2.0f)
			{
				player->playerSpr.vel.x = 0.0f;
				player->playerSpr.hitbox.position.x =
					collided.hitbox.position.x -
					collided.hitbox.dimensions.w / 2.0f -
					player->playerSpr.hitbox.dimensions.w / 2.0f;	
			}	
		}
	}

	//Ladder, climb
	if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, LADDER, 0.0f))
	{
		player->playerSpr.falling = 0;
		player->playerSpr.canJump = 1;
	
		if(isPressed(GLFW_KEY_W) || isPressed(GLFW_KEY_SPACE))
			player->playerSpr.vel.y = JUMP_SPEED;
		else if(isPressed(GLFW_KEY_S))
			player->playerSpr.vel.y = -JUMP_SPEED;
		else
			player->playerSpr.vel.y = 0.0f;
	}

	//Move player in y direction
	updateSpriteY(&player->playerSpr, secondsPerFrame);
	//Check for collision	
	if(blockCollisionSearch(player->playerSpr, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &collided))
	{		
		//Apply fall damage
		if(player->playerSpr.vel.y / BLOCK_SIZE <= -21.0f && !touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, WATER, 0.5f) 
			 && !touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE - 1, SLIME_BLOCK, -1.0f))
		{
			damagePlayer(player, (int)sqrtf(-player->playerSpr.vel.y / BLOCK_SIZE - 20.0f));	
		}

		if(touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE - 1, SLIME_BLOCK, -1.0f))
			player->playerSpr.vel.y *= -0.5f;

		//Uncollide the player
		if(player->playerSpr.vel.y != 0.0f)
		{
			if(player->playerSpr.hitbox.position.y >=
			   collided.hitbox.position.y + collided.hitbox.dimensions.h / 2.0f)
			{
				if((!touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE - 1, SLIME_BLOCK, -1.0f) ||
					player->playerSpr.vel.y < BLOCK_SIZE * 2.0f))	
					player->playerSpr.vel.y = 0.0f;
				player->playerSpr.hitbox.position.y =
					collided.hitbox.position.y +
					collided.hitbox.dimensions.h / 2.0f +
					player->playerSpr.hitbox.dimensions.h / 2.0f;	
				//The player is supported by a block	
				player->playerSpr.falling = 0;	
			}	
			else if(player->playerSpr.hitbox.position.y <= 
					collided.hitbox.position.y - collided.hitbox.dimensions.h / 2.0f)
			{
				player->playerSpr.falling = 1;	
				player->playerSpr.vel.y = -0.5f;
				player->playerSpr.hitbox.position.y =
					collided.hitbox.position.y -
					collided.hitbox.dimensions.h / 2.0f -
					player->playerSpr.hitbox.dimensions.h / 2.0f;	
			}	
		}	
	}
	else
	{
		//The player is not supported by a block
		player->playerSpr.falling = 1;
		player->playerSpr.animationState = FALLING;	
	
		struct Sprite temp = createSprite(
							 createRect(player->playerSpr.hitbox.position.x, player->playerSpr.hitbox.position.y - 0.01f,
										player->playerSpr.hitbox.dimensions.w, player->playerSpr.hitbox.dimensions.h));
		struct Sprite tempCollison;	
		if(blockCollisionSearch(temp, 3, 3, world->blocks, world->blockArea, world->worldBoundingRect, &tempCollison))
			player->playerSpr.animationState = IDLE;
	}

	//Move character with arrow keys
	if(isPressed(GLFW_KEY_A))
	{
		player->playerSpr.flipped = 1;
		player->playerSpr.vel.x = -PLAYER_SPEED;
		if(player->playerSpr.animationState == IDLE)
			player->playerSpr.animationState = WALKING;	
	}
	else if(isPressed(GLFW_KEY_D))
	{
		player->playerSpr.flipped = 0;	
		player->playerSpr.vel.x = PLAYER_SPEED;
		if(player->playerSpr.animationState == IDLE)
			player->playerSpr.animationState = WALKING;
	}
	else
	{
		player->playerSpr.vel.x = 0.0f;
		if(player->playerSpr.animationState == WALKING)
			player->playerSpr.animationState = IDLE;
	}
	
	//Check if player can jump
	if(!player->playerSpr.falling || 
	   touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, WATER, 0.0f) || 
	   touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, VINES, 0.0f) || 
	   touching(*world, camPos.x / BLOCK_SIZE, camPos.y / BLOCK_SIZE, LAVA, 0.0f))
		player->playerSpr.canJump = 1;
	else
		player->playerSpr.canJump = 0;	

	//Jump
	if(player->playerSpr.canJump && isPressed(GLFW_KEY_SPACE))
		player->playerSpr.vel.y = JUMP_SPEED;

	//Update liquid blocks
	//This seems to be taking a lot of time, try to optimize this later
	if(blockUpdateTimer > 0.05f)
	{
		updatePlants(world, camPos, 48);
		updateBlocks(world->blocks, camPos, blockUpdateTimer, 32, world->blockArea, world->worldBoundingRect);
		blockUpdateTimer = 0.0f;	
	}

	//Get selected block
	double cursorX, cursorY;
	getCursorPos(&cursorX, &cursorY);
	cursorX = roundf((cursorX + player->playerSpr.hitbox.position.x) / BLOCK_SIZE);	
	cursorY = roundf((cursorY + player->playerSpr.hitbox.position.y) / BLOCK_SIZE);
	if(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT))
	{
		if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE ||
			getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER ||
			getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA)		
		{	
			selectedBlock = getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type;
		}	
	}
	else
	{
		if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE ||
		   getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER ||
		   getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA)	
		{		
			selectedBlock = getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type;
		}	
		else if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).visibility == REVEALED)
		{		
			selectedBlock = getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type;
		}	
	}

	struct IntVec indices = createVec();
	struct IntVec nodes = createVec();
	searchInRectAndGetNodes(world->enemies,
				 newpt(camPos.x - 64.0f * BLOCK_SIZE, camPos.y - 64.0f * BLOCK_SIZE),
				 newpt(camPos.x + 64.0f * BLOCK_SIZE, camPos.y + 64.0f * BLOCK_SIZE),
				 &indices, &nodes, ROOT);

	//Place blocks
	int placed = 0;
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT) && cursorInBounds())
	{
		struct Sprite temp = createSprite(createRect(cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));	
		
		int collidingWithEnemy = 0;
		for(int i = 0; i < indices.sz && !collidingWithEnemy; i++)
			if(colliding(temp.hitbox, world->enemies->enemyArr[indices.values[i]].spr.hitbox))
				collidingWithEnemy = 1;

		if(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT))
		{
			if(getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE)		
			{
				setBlockType(world->backgroundBlocks, cursorX, cursorY, world->blockArea, placeBlock(player->inventory.slots[player->inventory.selected].item), world->worldBoundingRect);
				if(!canReplace(placeBlock(player->inventory.slots[player->inventory.selected].item)))
				{
					placed = 1;
					decrementSlot(player->inventory.selected, &player->inventory);	
				}
			}	
		}
		else if(!colliding(temp.hitbox, player->playerSpr.hitbox) &&
				canReplace(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type) &&
				!collidingWithEnemy)
		{	
			//Check if block is transparent
			int found = 0, ableToPlace = 0;
			for(int i = 0; i < sizeof(transparent) / sizeof(enum BlockType) && !found; i++)
			{	
				if(transparent[i] == placeBlock(player->inventory.slots[player->inventory.selected].item))
				{	
					found = 1;
					if(getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE)
					{
						setBlockType(world->transparentBlocks, cursorX, cursorY, world->blockArea, placeBlock(player->inventory.slots[player->inventory.selected].item), world->worldBoundingRect);			
						ableToPlace = 1;	
					}	
				}
			}
			
			if(!found && placeBlock(player->inventory.slots[player->inventory.selected].item) != NONE)
			{
				if(((player->inventory.slots[player->inventory.selected].item == LAVA_BUCKET ||
					player->inventory.slots[player->inventory.selected].item == WATER_BUCKET) &&
					bucketDelay >= BUCKET_DELAY_TIME) ||
					(player->inventory.slots[player->inventory.selected].item != LAVA_BUCKET &&
					player->inventory.slots[player->inventory.selected].item != WATER_BUCKET))
				{
					setBlockType(world->blocks, cursorX, cursorY, world->blockArea, placeBlock(player->inventory.slots[player->inventory.selected].item), world->worldBoundingRect);				
					setBlockMass(world->blocks, cursorX, cursorY, world->blockArea, 1.0f, world->worldBoundingRect);			
				
					if(placeBlock(player->inventory.slots[player->inventory.selected].item) == DOOR_BOTTOM_CLOSED)
						updateDoor(world, cursorX, cursorY, world->enemies->enemyArr, indices);
				}	
			}	

			if(placeBlock(player->inventory.slots[player->inventory.selected].item) != NONE && (!found || (found && ableToPlace)))	
			{	
				placed = 1;
				if((player->inventory.slots[player->inventory.selected].item == LAVA_BUCKET ||
					player->inventory.slots[player->inventory.selected].item == WATER_BUCKET) &&
					bucketDelay >= BUCKET_DELAY_TIME)
				{
					player->inventory.slots[player->inventory.selected].item = BUCKET;		
					bucketDelay = 0.0f;	
				}
				else if(player->inventory.slots[player->inventory.selected].item != LAVA_BUCKET &&
						 player->inventory.slots[player->inventory.selected].item != WATER_BUCKET)
					decrementSlot(player->inventory.selected, &player->inventory);	
			}	
		}	
	}

	if(placed) activateUseAnimation(player);

	//Destroy blocks
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT) && cursorInBounds())
	{
		if((isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT)) &&
			(canReplace(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type) ||
			 isPartOfDoor(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type)) &&
			getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != NONE)
			breakBlockTimer += secondsPerFrame;
		else if(canReplace(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type) &&
				getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != NONE)
			breakBlockTimer += secondsPerFrame;
		else if((getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != WATER &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != LAVA &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != INDESTRUCTABLE &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != NONE) &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).visibility == REVEALED &&
				!(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT)))
			breakBlockTimer += secondsPerFrame;	
		else
			breakBlockTimer = 0.0f;

		if(breakBlockTimer > 0.0f)
			activateUseAnimation(player);

		if(breakBlockTimer > timeToBreakBlock(getSelectedBlock(), player->inventory.slots[player->inventory.selected].item))
		{
			if(isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT))
			{
				if(canReplace(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type) ||
					isPartOfDoor(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type))		
				{	
					addItem(world, itemAmt(droppedItem(getBlock(world->backgroundBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type, 
														player->inventory.slots[player->inventory.selected].item), 1), cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE);	
					setBlockType(world->backgroundBlocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);	
					use(player->inventory.selected, &player->inventory);
				}	
			}
			else
			{
				if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == NONE ||
					getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER ||
					getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA)	
				{	
					addItem(world, itemAmt(droppedItem(getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type, 
												player->inventory.slots[player->inventory.selected].item), 1), cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE);	
					if(getBlock(world->transparentBlocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WHEAT4)
						addItem(world, itemAmt(SEED_ITEM, 2), cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE);
					setBlockType(world->transparentBlocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);		
					use(player->inventory.selected, &player->inventory);
				}	
				else if((getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != WATER &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != LAVA &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type != INDESTRUCTABLE) &&
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).visibility == REVEALED)
				{	
					//Door
					if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == DOOR_BOTTOM_CLOSED ||
						getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == DOOR_BOTTOM_OPEN)
						setBlockType(world->blocks, cursorX, cursorY + 1, world->blockArea, NONE, world->worldBoundingRect);
					else if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == DOOR_TOP_CLOSED ||
							getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == DOOR_TOP_OPEN)
						setBlockType(world->blocks, cursorX, cursorY - 1, world->blockArea, NONE, world->worldBoundingRect);

					addItem(world, itemAmt(droppedItem(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type, 
														player->inventory.slots[player->inventory.selected].item), 1), cursorX * BLOCK_SIZE, cursorY * BLOCK_SIZE);	
					setBlockType(world->blocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);	
					setBlockMass(world->blocks, cursorX, cursorY, world->blockArea, 0.0f, world->worldBoundingRect);				
					revealNeighbors(world, cursorX, cursorY);
					use(player->inventory.selected, &player->inventory); 
				}	
			}
			
			breakBlockTimer = 0.0f;
		}
	}
	else
		breakBlockTimer = 0.0f;	

	//Interact with blocks
	if(mouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT) && cursorInBounds())
	{
		if(player->inventory.slots[player->inventory.selected].item >= WOOD_HOE &&
			player->inventory.slots[player->inventory.selected].item <= RAINBOW_HOE)
		{
			if((getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == GRASS ||
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == DIRT) &&
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).visibility == REVEALED)
			{
				use(player->inventory.selected, &player->inventory);
				setBlockType(world->blocks, cursorX, cursorY, world->blockArea, FARMLAND, world->worldBoundingRect);	
			}
		}
		else if(player->inventory.slots[player->inventory.selected].item == BUCKET && bucketDelay >= BUCKET_DELAY_TIME)
		{
			if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == WATER && 
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).mass > 0.2f)
			{
				player->inventory.slots[player->inventory.selected].item = WATER_BUCKET; 
				setBlockType(world->blocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);
				setBlockMass(world->blocks, cursorX, cursorY, world->blockArea, 0.0f, world->worldBoundingRect);	
			}
			else if(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type == LAVA && 
				getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).mass > 0.2f)
			{
				player->inventory.slots[player->inventory.selected].item = LAVA_BUCKET; 
				setBlockType(world->blocks, cursorX, cursorY, world->blockArea, NONE, world->worldBoundingRect);		
				setBlockMass(world->blocks, cursorX, cursorY, world->blockArea, 0.0f, world->worldBoundingRect);	
			}
			bucketDelay = 0.0f;
		}
		//Heal
		else if(healAmount(player->inventory.slots[player->inventory.selected].item) > 0 && player->health < player->maxHealth && player->damageCooldown <= 0.0f)
		{
			int heal = healAmount(player->inventory.slots[player->inventory.selected].item);
			if(heal < 0) damagePlayer(player, -heal);
			else player->health += heal;	

			if(player->health > player->maxHealth)
				player->health = player->maxHealth;
			decrementSlot(player->inventory.selected, &player->inventory);
			releaseMouseButton(GLFW_MOUSE_BUTTON_RIGHT);	
		}
		//Increase maximum health
		else if(player->inventory.slots[player->inventory.selected].item == HEALTH_BOOST)
		{
			player->maxHealth++;
			decrementSlot(player->inventory.selected, &player->inventory);
		}
	
		if((!placed && toggleDoor(world, cursorX, cursorY, player->playerSpr, world->enemies->enemyArr, indices)) ||
			(placed && isPartOfDoor(getBlock(world->blocks, cursorX, cursorY, world->blockArea, world->worldBoundingRect).type)))
			releaseMouseButton(GLFW_MOUSE_BUTTON_RIGHT);	
	}

	if(player->useItemTimer > 0.0f)
		player->useItemTimer -= secondsPerFrame;
	if(player->useItemTimer < 0.0f)
		player->useItemTimer = 0.0f;

	//Inventory keys
	static const int inventoryHotKeys[] = { GLFW_KEY_1,
											GLFW_KEY_2,
											GLFW_KEY_3,
											GLFW_KEY_4,
											GLFW_KEY_5,
											GLFW_KEY_6,
											GLFW_KEY_7,
											GLFW_KEY_8,
											GLFW_KEY_9,
											GLFW_KEY_0 };
	for(int i = 0; i < 10; i++)
		if(isPressed(inventoryHotKeys[i]))
			player->inventory.selected = i;
	
	double scroll = getMouseScroll();
	if(scroll > 0.0)
	{
		player->inventory.selected++;
		player->inventory.selected %= player->inventory.maxSize;	
	}
	else if(scroll < 0.0)
	{
		player->inventory.selected--;
		player->inventory.selected += player->inventory.maxSize;
		player->inventory.selected %= player->inventory.maxSize;	
	}

	//Drop item
	int qPressed = isPressedOnce(GLFW_KEY_Q);	
	if(qPressed && (isPressed(GLFW_KEY_LEFT_SHIFT) || isPressed(GLFW_KEY_RIGHT_SHIFT)) && player->inventory.slots[player->inventory.selected].item != NOTHING)
	{
		float offset = (player->playerSpr.flipped ? -BLOCK_SIZE : BLOCK_SIZE) * 2.0f;

		addItem(world, itemAmtWithUses(player->inventory.slots[player->inventory.selected].item,
							   player->inventory.slots[player->inventory.selected].amount, 
							   player->inventory.slots[player->inventory.selected].usesLeft,
							   player->inventory.slots[player->inventory.selected].maxUsesLeft), 
							   camPos.x + offset, camPos.y);
		removeSlot(player->inventory.selected, &player->inventory);	
	}
	else if(qPressed && player->inventory.slots[player->inventory.selected].item != NOTHING)
	{
		float offset = (player->playerSpr.flipped ? -BLOCK_SIZE : BLOCK_SIZE) * 2.0f;
		addItem(world, itemAmtWithUses(player->inventory.slots[player->inventory.selected].item, 1,
							   player->inventory.slots[player->inventory.selected].usesLeft,
							   player->inventory.slots[player->inventory.selected].maxUsesLeft),
							   camPos.x + offset, camPos.y);
		decrementSlot(player->inventory.selected, &player->inventory);
	}

	//Update time in the world
	world->dayCycle += secondsPerFrame * 1.0f / 60.0f * 1.0f / 20.0f;
	if(world->dayCycle > 1.0f)
	{
		//Update moon phases	
		world->moonPhase += 1.0f / 28.0f;
		if(world->moonPhase > 1.0f) world->moonPhase = 0.0f;

		world->dayCycle = 0.0f;
	}

	//Update enemies	
	//If it's a new moon and night, spawn a wave of enemies
	if((world->moonPhase > 0.7f && world->moonPhase < 0.8f) &&
		(world->dayCycle > 0.8f || world->dayCycle < 0.2f) &&
		(rand() % SPAWN_WAVE == 0) && indices.sz < 64)
		spawnWave(world, camPos, 64.0f);
	//If it's night, attempt to spawn enemies
	else if((world->dayCycle > 0.8f || world->dayCycle < 0.2f) &&
		rand() % SPAWN_AT_NIGHT == 0 &&
		indices.sz < 24)
		spawnEnemiesAtNight(world, camPos, 64.0f);
	//If it's day, attempt to spawn chickens
	else if((world->dayCycle > 0.4f || world->dayCycle < 0.6f) &&
		rand() % SPAWN_CHICKEN == 0 &&
		indices.sz < 8)
		spawnChickens(world, camPos, 64.0f);

	if(player->playerSpr.hitbox.position.x < 256.0f * BLOCK_SIZE &&
		rand() % SPAWN_IN_CAVE == 0 &&
		indices.sz < 16)
		spawnEnemiesInCave(world, camPos, 64.0f);

	int attacked = 0; //Did the player hit any enemy?
	for(int i = 0; i < indices.sz; i++)
	{
		int ind = indices.values[i];
		updateEnemy(&world->enemies->enemyArr[ind], secondsPerFrame, world->blocks, world->worldBoundingRect, world->blockArea, player);
		updatePoint(world->enemies, ind, nodes.values[i]);
		
		//Attack enemies
		if(mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT) && 
		   sqrtf(powf(world->enemies->enemyArr[ind].spr.hitbox.position.x - player->playerSpr.hitbox.position.x, 2.0f) +
			     powf(world->enemies->enemyArr[ind].spr.hitbox.position.y - player->playerSpr.hitbox.position.y, 2.0f)) < BLOCK_SIZE * 3.0f)
		{
			if(((world->enemies->enemyArr[ind].spr.hitbox.position.x > player->playerSpr.hitbox.position.x &&
				!player->playerSpr.flipped) ||
				(world->enemies->enemyArr[ind].spr.hitbox.position.x < player->playerSpr.hitbox.position.x &&
				player->playerSpr.flipped)) &&
				breakBlockTimer <= 0.0f &&
				damageEnemy(&world->enemies->enemyArr[ind], damageAmount(player->inventory.slots[player->inventory.selected].item)) > 0)
			{
				world->enemies->enemyArr[ind].despawnTimer = 0.0f;
				activateUseAnimation(player);
				attacked = 1;
				use(player->inventory.selected, &player->inventory);
			}	
		}
	}
	//Delete enemies that are dead
	for(int i = 0; i < indices.sz; i++)
	{	
		int ind = indices.values[i];
		if(world->enemies->enemyArr[ind].health <= 0 ||
		   world->enemies->enemyArr[ind].despawnTimer > 300.0f) //Delete enemies after five minutes
		{
			addItem(world, itemAmt(droppedLoot(world->enemies->enemyArr[ind].spr.type), 1), world->enemies->enemyArr[ind].spr.hitbox.position.x, world->enemies->enemyArr[ind].spr.hitbox.position.y); 
			deletePoint(world->enemies, ind);
		}	
	}
	//Rebuild quadtree if it gets too empty
	if(world->enemies->pointCount * 2 < world->enemies->maxPointCount &&
		world->enemies->maxPointCount > DEFAULT_SZ * CAPACITY * 2)
	{		
		struct QuadTree* newTree = rebuildQTree(world->enemies);
		destroyQuadtree(world->enemies);
		world->enemies = newTree;
	}

	//If the player attacked an enemy, release the left mouse button	
	if(attacked) releaseMouseButton(GLFW_MOUSE_BUTTON_LEFT);	

	free(indices.values);
	free(nodes.values);

	//Update clouds
	for(int i = 0; i < MAX_CLOUD; i++)
	{
		world->clouds[i].hitbox.position.x += 16.0f * secondsPerFrame;
		if(world->clouds[i].hitbox.position.x > 960.0f + world->clouds[i].hitbox.dimensions.w / 2.0f)
		{
			float sz = (float)rand() / (float)RAND_MAX * 32.0f + 64.0f;
			world->clouds[i].hitbox.position.x = -960.0f - world->clouds[i].hitbox.dimensions.w / 2.0f;
			world->clouds[i].hitbox.position.y = (float)rand() / (float)RAND_MAX * 960.0f;
			world->clouds[i].hitbox.dimensions.w = world->clouds[i].hitbox.dimensions.h = sz;	
		}
	}	

	updateItems(world, camPos, 32, secondsPerFrame, player);

	//If player dies, they drop all their stuff
	if(player->health <= 0)
	{
		for(int i = 0; i < player->inventory.maxSize; i++)
		{
			addItem(world, player->inventory.slots[i], camPos.x, camPos.y);
			player->inventory.slots[i] = itemAmt(NOTHING, 0);
		}
	}
}

float getBlockBreakTimer()
{
	return breakBlockTimer;
}

void toggleCraftingMenu()
{
	menuShown = !menuShown;
}

int craftingMenuShown()
{
	return menuShown;
}

int getMenuSelection()
{
	return menuSelection;
}

void setMenuSelection(int selection)
{
	menuSelection = selection;
}

enum BlockType getSelectedBlock()
{
	return selectedBlock;
}

int getMenuBegin()
{
	return menuBegin;
}

int getMenuEnd()
{
	return menuEnd;
}
