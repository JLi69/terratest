#ifndef ENEMY_H
#include "sprite.h"
#include "block.h"
#include "player.h"

#define ENEMY_DAMAGE_COOLDOWN 0.5f

#define SPAWN_AT_NIGHT 1024
#define SPAWN_IN_CAVE 256
#define SPAWN_CHICKEN 81920
#define SPAWN_WAVE 128

enum EnemyType
{
	//1 x 1 enemies
	DELETED = -1,
	CHICKEN,
	GREEN_SLIME,
	BLUE_SLIME,	
	RED_SLIME,
	PINK_SLIME,
	//1 x 2 enemies
	ZOMBIE = 64
};

enum AttackMode
{
	NEUTRAL,
	WANDER,
	CHASE,
	RUN_AWAY,
	PASSIVE,
	KNOCKBACK
};

struct Enemy
{
	struct Sprite spr;
	int health, maxHealth;
	enum AttackMode attackmode;
	float walkDistance;
	float timer, damageCooldown;
	float despawnTimer;
};

int maxHealthEnemy(enum EnemyType type);
void drawEnemy1x1(struct Enemy enemy, struct Vector2D camPos);
void drawEnemy1x2(struct Enemy enemy, struct Vector2D camPos);
struct Enemy createEnemy(enum EnemyType type, float x, float y);
void updateEnemy(struct Enemy *enemy, 
				 float timePassed, 
				 struct Block *blocks, 
				 struct BoundingRect boundRect,
				 int maxBlockInd,
				 struct Player *player);
int damageEnemy(struct Enemy *enemy, int amt);
enum Item droppedLoot(enum EnemyType enemy);

#endif
#define ENEMY_H
