#include "world.h"
#include <stdlib.h>
#include <stdio.h>

void updateLiquid(
	struct SpriteQuadTree *liquids,
	struct SpriteQuadTree *liquidRoot,
	struct SpriteQuadTree *solidBlocks,
	struct Vector2D camPos,
	float timePassed)
{
	if(liquids == NULL)
		return;

	struct Rectangle simBox = createRect(camPos.x, camPos.y, SIM_DIST, SIM_DIST),
					 quadBound = createRectFromCorner(liquids->botLeftCorner, liquids->topRightCorner);
	if(!colliding(simBox, quadBound))
		return;

	//On leaf node
	if(liquids->botLeft == NULL &&
	   liquids->botRight == NULL &&
	   liquids->topLeft == NULL &&
	   liquids->botRight == NULL)
	{
		struct Sprite futurePos[QUAD_CAPACITY];

		for(int i = 0; i < liquids->spriteCount; i++)
		{
			futurePos[i] = liquids->sprites[i];
			if(!colliding(simBox, liquids->sprites[i].hitbox))
				continue;

			//Can move, try updating
			if(liquids->sprites[i].canMove)
			{
				//Update all liquid blocks around it
				struct Sprite* neighbor;
				struct Sprite spr;
				float adjX[] = { BLOCK_SIZE, -BLOCK_SIZE, 0.0f, 0.0f };
				float adjY[] = { 0.0f, 0.0f, BLOCK_SIZE, -BLOCK_SIZE };
				for(int j = 0; j < 4; j++)
				{
					spr = createSprite(createRect(liquids->sprites[i].hitbox.position.x + adjX[j],
												  liquids->sprites[i].hitbox.position.y + adjY[j],
												  BLOCK_SIZE,
												  BLOCK_SIZE));
					if(collisionSearch(liquidRoot, spr, &neighbor))
						neighbor->canMove = 1;
				}

				liquids->sprites[i].timeSinceLastUpdate += timePassed;
				struct Sprite *tempCollision, *tempCollisionLiquid;
				struct Sprite newPosition;
				
				//Update water and lava
				if(liquids->sprites[i].type == WATER && liquids->sprites[i].timeSinceLastUpdate > WATER_UPDATE_TIME)
				{
					liquids->sprites[i].timeSinceLastUpdate = 0.0f;
					//Flow downwards
					newPosition = liquids->sprites[i];
					newPosition.hitbox.position.y -= BLOCK_SIZE;
					if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
					   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
					{
						futurePos[i] = newPosition;
						continue;
					}
				
					int direction = rand() % 2;
					if(direction == 0)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.y -= BLOCK_SIZE;	
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.y -= BLOCK_SIZE;	
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}
					else if(direction == 1)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}

					direction = rand() % 2;
					if(direction == 0)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}
					else if(direction == 1)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}

					liquids->sprites[i].canMove = 0;
				}
				else if(liquids->sprites[i].type == LAVA && liquids->sprites[i].timeSinceLastUpdate > LAVA_UPDATE_TIME)
				{
					liquids->sprites[i].timeSinceLastUpdate = 0.0f;
					//Flow downwards
					newPosition = liquids->sprites[i];
					newPosition.hitbox.position.y -= BLOCK_SIZE;
					if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
					   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
					{
						futurePos[i] = newPosition;
						continue;
					}
				
					int direction = rand() % 2;
					if(direction == 0)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.y -= BLOCK_SIZE;	
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.y -= BLOCK_SIZE;	
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}
					else if(direction == 1)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}	

					direction = rand() % 2;	

					if(direction == 0)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}
					else if(direction == 1)
					{
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x += BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
						newPosition = liquids->sprites[i];
						newPosition.hitbox.position.x -= BLOCK_SIZE;
						if(!collisionSearch(solidBlocks, newPosition, &tempCollision) &&
						   !collisionSearch(liquidRoot, newPosition, &tempCollisionLiquid))
						{
							futurePos[i] = newPosition;
							continue;
						}
					}	

					liquids->sprites[i].canMove = 0;
				}
			}
		}

		int count = liquids->spriteCount;
		for(int i = 0; i < count; i++)
		{
			struct Sprite *temp;
			if(collisionSearch(liquidRoot, futurePos[i], &temp))
				continue;
			liquids->sprites[i].hitbox.position.x = futurePos[i].hitbox.position.x;
			liquids->sprites[i].hitbox.position.y = futurePos[i].hitbox.position.y;
			if(!colliding(quadBound, liquids->sprites[i].hitbox))
			{
				insert(liquidRoot, futurePos[i]);
			}
		}
	}
	else
	{
		updateLiquid(liquids->botLeft, liquidRoot, solidBlocks, camPos, timePassed);
		updateLiquid(liquids->botRight, liquidRoot, solidBlocks, camPos, timePassed);
		updateLiquid(liquids->topLeft, liquidRoot, solidBlocks, camPos, timePassed);
		updateLiquid(liquids->topRight, liquidRoot, solidBlocks, camPos, timePassed);
	}
}
