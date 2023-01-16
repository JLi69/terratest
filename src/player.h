#ifndef PLAYER_H
#include "inventory.h"
#include "sprite.h"

#define DAMAGE_COOLDOWN 1.0f

struct Player
{
	struct Sprite playerSpr;
	struct Inventory inventory;
	int health, maxHealth;
	float breath, maxBreath;
	
	float damageCooldown;
	int damageTaken;
};

void damagePlayer(struct Player *player, int amt);

#endif
