#ifndef GAME_H
#include "world.h"
#include "sprite.h"
#include "inventory.h"

#define MAX_SHADERS 8
#define MAX_BUFFERS 4
#define MAX_TEXTURES 80

#define PLAYER_SPEED 256.0f
#define JUMP_SPEED 320.0f

#define CURSOR_SIZE 16.0f

#define START_HEALTH 8
#define BREATH 16.0f
//Number of bubbles to draw to represent breath
#define BUBBLE_COUNT 8.0f
#define ICON_SIZE 20.0f

enum AnimationStates
{
	IDLE,
	WALKING,
	FALLING,
};

enum GameState
{
	ON_MAIN_MENU,
	ON_SAVE_FILE_LIST,	
	ON_DELETE_WORLD_PROMPT,
	ON_CREATE_WORLD_PROMPT,
	PLAYING,
};

//Initializes shaders and buffers
void initGL(void);
//Initializes game objects
void initGame(struct World *world, struct Player *player, int seed);

float getBlockBreakTimer();
void toggleCraftingMenu();
int craftingMenuShown();
int getMenuSelection();
void setMenuSelection(int selection);
enum BlockType getSelectedBlock();
int getMenuBegin();
int getMenuEnd();

void displayFPS(int fps); //Frame Counter
void display(struct World world, struct Player player);
void updateGameobjects(struct World *world, struct Player *player, float secondsPerFrame);
void loop(void);
void animateSprites(struct World *world, struct Sprite *player, float secondsPerFrame);

void displayMainMenu(float secondsPerFrame);
void displaySaveMenu(const char **savepaths, int savecount, int perColumn, int selected);
void displayDeletePrompt();
void displayCreatePrompt(unsigned int seed);
void typeSeed(unsigned int *seed);
//returns id of button clicked
//returns -1 if no button clicked
//returns -2 if cancelled is clicked
//returns -3 if delete is clicked
int interactWithSaveMenu(const char **savepaths, int savecount, int perColumn, int selected);

void cleanup(void);
#endif
#define GAME_H
