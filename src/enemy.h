#ifndef ENEMY_H
#include "sprite.h"
#include "block.h"
#include "player.h"

#define ENEMY_DAMAGE_COOLDOWN 0.5f

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
	NEUTRAL,
	WANDER,
	CHASE,
	RUN_AWAY,
	PASSIVE
};

struct Enemy
{
	struct Sprite spr;
	int health, maxHealth;
	enum AttackMode attackmode;
	float walkDistance;
	float timer, damageCooldown;
};

int maxHealthEnemy(enum EnemyType type);
void drawEnemy1x1(struct Enemy enemy, struct Vector2D camPos);
struct Enemy createEnemy(enum EnemyType type, float x, float y);
void updateEnemy(struct Enemy *enemy, 
				 float timePassed, 
				 struct Block *blocks, 
				 struct BoundingRect boundRect,
				 int maxBlockInd,
				 struct Player *player);
void damageEnemy(struct Enemy *enemy, int amt);

#endif
#define ENEMY_H