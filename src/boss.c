#include "boss.h"
#include "block.h"
#include "draw.h"
#include "window-func.h"
#include <math.h>
#include <GLFW/glfw3.h>

int summonBoss(struct Boss *boss, float x, float y)
{
	if(boss->phase == -1)
	{	
		boss->phase = 0;
		boss->spawnX = x;
		boss->spawnY = y;
		boss->spr = createSprite(createRect(x, y, BLOCK_SIZE * 4.0f, BLOCK_SIZE * 4.0f));
		
		boss->timer = 0.0f;
		boss->health = BOSS_HEALTH;
		boss->maxHealth = BOSS_HEALTH;

		boss->spr.canMove = 1;
		boss->spr.animating = 1;

		boss->damageCooldown = 5.0f;

		return 1;	
	}
	return 0;
}

struct Boss initBoss(void)
{
	struct Boss boss;
	boss.phase = -1;
	boss.timer = 0.0f;
	boss.health = 0;
	boss.maxHealth = BOSS_HEALTH;
	boss.spawnX = 0.0f;
	boss.spawnY = 0.0f;
	return boss;
}

void drawBoss(struct Boss boss, struct Vector2D camPos)
{
	setRectSize(boss.spr.hitbox.dimensions.w, boss.spr.hitbox.dimensions.h);
	setRectPos(boss.spr.hitbox.position.x - camPos.x, boss.spr.hitbox.position.y - camPos.y);
	setTexOffset(1.0f / 8.0f * boss.spr.animationFrame, 0.0f);
	if(boss.phase == 3)
		setRotationRad(powf(2.0f, boss.timer) * 3.14159f);
	drawRect();
	setRotationRad(0.0f);
}

void updateBoss(struct Boss *boss, struct Player *player, float timePassed)
{
	//0 = introduction
	if(boss->phase == 0)
	{
		boss->timer += timePassed;
		if(boss->timer > 5.0f)
			boss->phase = 1; //Start the fight
	}
	//1 = phase 1
	//Boss just moves around and vertically charges toward the player
	//If hit by the vertical charge attack, 2 damage is dealt but boss
	//stays on the ground for 5 seconds
	else if(boss->phase == 1)
	{
		if(boss->health * 2 < boss->maxHealth)
		{
			boss->phase = 2;
			boss->timer = 0.0f;
		}
	}
	//2 = phase 2
	//Boss is past half health and will horizontally charge at the player
	//and also summon enemies
	else if(boss->phase == 2)
	{
		if(boss->health <= 0)
		{
			boss->timer = 0.0f;
			boss->health = 0;
			boss->phase = 3;
		}
	}
	//3 = death
	//death animation plays
	else if(boss->phase == 3)
	{
		boss->timer += timePassed;
		if(boss->timer > 5.0f)
			boss->phase = -1; //Boss is dead!
	}

	float dist = sqrtf(powf(boss->spr.hitbox.position.x - player->playerSpr.hitbox.position.x, 2.0f) +
			     powf(boss->spr.hitbox.position.y - player->playerSpr.hitbox.position.y, 2.0f));
	if(boss->phase > 0)
	{
		if(mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT) &&
			dist < BLOCK_SIZE * 4.0f &&
			((boss->spr.hitbox.position.x > player->playerSpr.hitbox.position.x &&
			!player->playerSpr.flipped) ||
			(boss->spr.hitbox.position.x < player->playerSpr.hitbox.position.x &&
			player->playerSpr.flipped)) &&
			damageBoss(boss, damageAmount(player->inventory.slots[player->inventory.selected].item)) > 0)
		{
			activateUseAnimation(player);
			use(player->inventory.selected, &player->inventory);
			releaseMouseButton(GLFW_MOUSE_BUTTON_LEFT);	
		}
		
		if(dist < 3.0f * BLOCK_SIZE)
		{	
			
			//Phase 1, damage player
			if(boss->phase == 1)
				damagePlayer(player, 2);
		}
	}
	
	boss->damageCooldown -= timePassed;
}

int damageBoss(struct Boss *boss, int amt)
{
	if(boss->damageCooldown <= 0.0f)
	{
		boss->damageCooldown = 0.5f;
		boss->health -= amt;
		return amt;	
	}
	return 0;
}
