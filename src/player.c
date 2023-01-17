#include "player.h"

void damagePlayer(struct Player *player, int amt)
{
	if(player->damageCooldown > 0.0f)
		return;
	player->damageCooldown = DAMAGE_COOLDOWN;
	player->damageTaken = amt;
	player->health -= amt;
}

void activateUseAnimation(struct Player *player)
{	
	if(player->useItemTimer > 0.0f)
		return;
	player->useItemTimer = USE_ANIMATION_LENGTH;
}
