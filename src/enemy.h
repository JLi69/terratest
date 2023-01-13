#ifndef ENEMY_H
#include "sprite.h"

enum EnemyType
{
	CHICKEN,
	GREEN_SLIME,
	BLUE_SLIME,
	PINK_SLIME,
	RED_SLIME,
	ZOMBIE
};

enum AttackMode
{
	WANDER,
	CHASE,
	RUN_AWAY
};

struct Enemy
{
	struct Sprite spr;
	int health, maxHealth;
	enum AttackMode attackmode;
};

int maxHealthEnemy(enum EnemyType type);
void drawEnemy1x1(struct Enemy enemy, struct Vector2D camPos);
struct Enemy createEnemy(enum EnemyType type, float x, float y);
void updateEnemy(struct Enemy *enemy, float timePassed);

#endif
#define ENEMY_H
