#ifndef BOSS_H
#include "sprite.h"
#include "player.h"

#define BOSS_HEALTH 2000

enum BossAttackMode
{
	FOLLOW,
	CHARGE_VERTICAL,
	CHARGE_HORIZONTAL
};

struct Boss
{
	struct Sprite spr;	
	// -1 = not summoned
	// 0 = intro text
	// 1 = phase 1
	// 2 = phase 3
	int phase;
	float timer;
	int health, maxHealth;
	float spawnX, spawnY;
	enum BossAttackMode attackmode;
	float damageCooldown;
};

//Returns 1 if successful, 0 otherwise
//Will not spawn a boss if another one exists
int summonBoss(struct Boss *boss, float x, float y);

struct Boss initBoss(void);

void drawBoss(struct Boss boss, struct Vector2D camPos);
void updateBoss(struct Boss *boss, struct Player *player, float timePassed);

int damageBoss(struct Boss *boss, int amt);

#endif
#define BOSS_H
