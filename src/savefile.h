#ifndef SAVEFILE_H
#include "world.h"

void saveWorld(struct World *world, struct Player *player, const char *path);
//Returns 1 if sucessful, 0 if not
int readSave(struct World *world, struct Player *player, const char *path);

#endif

#define SAVEFILE_H
