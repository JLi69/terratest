#ifndef GAME_H
#include "world.h"
#include "sprite.h"

#define MAX_SHADERS 8
#define MAX_BUFFERS 4

#define PLAYER_SPEED 256.0f
#define JUMP_SPEED 320.0f

//Initializes shaders and buffers
void initGL(void);
//Initializes game objects
void initGame(struct World *world, struct Sprite *player);

void display(struct World world, struct Sprite player);
void updateGameobjects(struct World *world, struct Sprite *player, float secondsPerFrame);
void loop(void);

void cleanup(void);
#endif
#define GAME_H
