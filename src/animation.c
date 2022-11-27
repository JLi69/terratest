#include "game.h"

const int idleAnimation[] = { 0, 1 };
const int fallingAnimation[] = { 2, 3 };
const int walkingAnimation[] = { 4, 5, 4, 6 };

void animateSprites(struct World *world, struct Sprite *player, float secondsPerFrame)
{
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
}
