#include "boss.h"
#include "block.h"
#include "draw.h"
#include "window-func.h"
#include <math.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>

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

		boss->attackmode = FLOAT;

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
	boss.attackmode = FLOAT;
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
	float dist = sqrtf(powf(boss->spr.hitbox.position.x - player->playerSpr.hitbox.position.x, 2.0f) +
			     powf(boss->spr.hitbox.position.y - player->playerSpr.hitbox.position.y, 2.0f));

	//0 = introduction
	if(boss->phase == 0)
	{
		boss->timer += timePassed;
		if(boss->timer > 5.0f)
		{	
			boss->phase = 1; //Start the fight
			
			if(rand() % 2 == 0) boss->spr.vel.x = -BLOCK_SIZE * 5.0f;
			else boss->spr.vel.x = BLOCK_SIZE * 5.0f;
			boss->spr.vel.y = 0.0f;	
			boss->timer = 10.0f;
		}
	}
	//1 = phase 1
	//Boss just moves around and vertically charges toward the player
	//If hit by the vertical charge attack, 2 damage is dealt but boss
	//stays on the ground for 5 seconds
	else if(boss->phase == 1)
	{
		if(dist < BLOCK_SIZE * 64.0f)
		{
			boss->spr.hitbox.position.x += boss->spr.vel.x * timePassed;
			boss->spr.hitbox.position.y += boss->spr.vel.y * timePassed;
		}	

		if(boss->attackmode == FLOAT)
		{
			if(boss->spr.hitbox.position.y > player->playerSpr.hitbox.position.y + BLOCK_SIZE * 6.0f)
				boss->spr.vel.y = -BLOCK_SIZE * 2.0f;
			if(boss->spr.hitbox.position.y < player->playerSpr.hitbox.position.y - BLOCK_SIZE * 6.0f)
				boss->spr.vel.y = BLOCK_SIZE * 2.0f;
			else
				boss->spr.vel.y = 0.0f;

			if(fabs(boss->spr.hitbox.position.x - player->playerSpr.hitbox.position.x) < BLOCK_SIZE / 4.0f)
				boss->spr.vel.x = 0.0f;
			else if(boss->spr.hitbox.position.x < player->playerSpr.hitbox.position.x)
				boss->spr.vel.x = BLOCK_SIZE * 5.0f;
			else if(boss->spr.hitbox.position.x > player->playerSpr.hitbox.position.x)
				boss->spr.vel.x = -BLOCK_SIZE * 5.0f;

			if(boss->damageCooldown >= 0.0f && boss->spr.hitbox.position.y < player->playerSpr.hitbox.position.y + BLOCK_SIZE * 6.0f)
				boss->spr.vel.y = BLOCK_SIZE * 4.0f;

			if(boss->timer <= 0.0f)
			{
				boss->attackmode = ATTACK;
				boss->spr.vel.x = 0.0f;	
			}	
		}
		else if(boss->attackmode == ATTACK)
		{
			if(fabs(boss->spr.hitbox.position.x - player->playerSpr.hitbox.position.x) < BLOCK_SIZE / 4.0f)
			{
				boss->spr.vel.x = 0.0f;	
				boss->attackmode = CHARGE;			
			}
			else if(boss->spr.hitbox.position.x < player->playerSpr.hitbox.position.x && boss->spr.vel.y == 0.0f)
				boss->spr.vel.x = BLOCK_SIZE * 12.0f;
			else if(boss->spr.hitbox.position.x > player->playerSpr.hitbox.position.x && boss->spr.vel.y == 0.0f)
				boss->spr.vel.x = -BLOCK_SIZE * 12.0f;	
		}
		else if(boss->attackmode == CHARGE)
		{
			if(boss->spr.hitbox.position.y > player->playerSpr.hitbox.position.y)
				boss->spr.vel.y = -8.0f * BLOCK_SIZE;
			else if(boss->spr.hitbox.position.y < player->playerSpr.hitbox.position.y)
				boss->spr.vel.y = 8.0f * BLOCK_SIZE;
			
			if(fabs(boss->spr.hitbox.position.y - player->playerSpr.hitbox.position.y) < BLOCK_SIZE)
			{	
				boss->attackmode = NO_MOVE;
				boss->spr.vel.y = 0.0f;
				boss->timer = 5.0f;	
			}
		}
		else if(boss->attackmode == NO_MOVE)
		{
			boss->spr.vel.x = 0.0f;
			boss->spr.vel.y = 0.0f;
			if(boss->timer < 0.0f)
			{
				boss->attackmode = FLOAT;
				boss->timer = 10.0f;
			}
		}

		boss->timer -= timePassed;

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
		boss->spr.hitbox.position.x += boss->spr.vel.x * timePassed;
		boss->spr.hitbox.position.y += boss->spr.vel.y * timePassed;

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
		if(boss->attackmode == NO_MOVE)
			boss->damageCooldown = 0.4f;	
		else
			boss->damageCooldown = 2.5f;
		boss->health -= amt;
		return amt;	
	}
	return 0;
}
