#include "game.h"
#include "window-func.h"
#include <stdlib.h>

const int idleAnimation[] = { 0, 1 };
const int fallingAnimation[] = { 2, 3 };
const int walkingAnimation[] = { 4, 5, 4, 6 };

const int enemyIdle[] = { 0, 1 };
const int enemyWalk[] = { 2, 0, 3, 0  };

const int bossPhase1[] = { 0, 1 };
const int attackPhase1[] = { 2, 3 };
const int bossPhase2[] = { 4, 5 };
const int attackPhase2[] = { 6, 7 };
const int bossDeath[] = { 4, 5, 6, 7 };

void animateSprites(struct World *world, struct Sprite *player, float secondsPerFrame)
{
	if(isPaused())
		return;

	static float timePassed = 0.0f;

	switch(player->animationState)
	{
	case IDLE:	
		updateAnimation(player, &idleAnimation[0], 2, timePassed, 0.5f);
		break;	
	case FALLING:
		updateAnimation(player, &fallingAnimation[0], 2, timePassed, 0.5f);
		break;
	case WALKING:
		updateAnimation(player, &walkingAnimation[0], 4, timePassed, 0.2f);
		break;
	default: 	
		updateAnimation(player, &idleAnimation[0], 2, timePassed, 0.5f);
		break;
	}

	timePassed += secondsPerFrame;

	//Animate enemies
	struct IntVec indices = createVec();
	searchInRect(world->enemies, 
				 newpt(player->hitbox.position.x - 32.0f * BLOCK_SIZE, player->hitbox.position.y - 20.0f * BLOCK_SIZE),
				 newpt(player->hitbox.position.x + 32.0f * BLOCK_SIZE, player->hitbox.position.y + 20.0f * BLOCK_SIZE), &indices, ROOT);
	for(int i = 0; i < indices.sz; i++)
	{
		int ind = indices.values[i];
		//Animate
		switch(world->enemies->enemyArr[ind].attackmode)
		{
		case PASSIVE:
			updateAnimation(&world->enemies->enemyArr[ind].spr, &enemyIdle[0], 2, timePassed, 0.5f);
			break;
		case WANDER:
			updateAnimation(&world->enemies->enemyArr[ind].spr, &enemyWalk[0], 4, timePassed, 0.3f);
			break;
		case RUN_AWAY: //Fall through
		case CHASE:
			if(world->enemies->enemyArr[ind].spr.type != ZOMBIE)
				updateAnimation(&world->enemies->enemyArr[ind].spr, &enemyWalk[0], 4, timePassed, 0.1f);
			else if(world->enemies->enemyArr[ind].spr.type == ZOMBIE)
				updateAnimation(&world->enemies->enemyArr[ind].spr, &enemyWalk[0], 4, timePassed, 0.4f);
			break;
		default: break;
		}
	}
	free(indices.values);

	//Animate the boss
	switch(world->boss.phase)
	{
	case 0: updateAnimation(&world->boss.spr, bossPhase1, 2, timePassed, 0.5f); break;
	case 1:
		updateAnimation(&world->boss.spr, bossPhase1, 2, timePassed, 0.5f);
		break;
	case 2:
		updateAnimation(&world->boss.spr, bossPhase2, 2, timePassed, 0.5f);
		break;
	case 3:
		updateAnimation(&world->boss.spr, bossDeath, 4, timePassed, 0.1f);
		break;
	default: break;
	}
}
