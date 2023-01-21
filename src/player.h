#ifndef PLAYER_H
#include "inventory.h"
#include "sprite.h"

#define JUMP_SPEED 320.0f
#define DAMAGE_COOLDOWN 1.0f
#define USE_ANIMATION_LENGTH 0.2f

struct Player
{
	struct Sprite playerSpr;
	struct Inventory inventory;
	int health, maxHealth;
	float breath, maxBreath;
	
	float damageCooldown;
	int damageTaken;

	float useItemTimer;
};

int damagePlayer(struct Player *player, int amt);
void activateUseAnimation(struct Player *player);

#endif
