#ifndef MENU_H
#define MAX_TEXT_LEN 128
#define MAX_BUTTONS 128
#define MAX_TEXT 128
#define MAX_MENU 16

struct MenuObj 
{
	float x, y, charSz;
	char text[MAX_TEXT_LEN];
};

struct Menu
{
	struct MenuObj menuButtons[MAX_BUTTONS];
	struct MenuObj menuText[MAX_TEXT];
	int buttonCount, textCount;
};

enum MenuIds
{
	NO_MENU = -1,
	RESPAWN,
	PAUSED,
	MAIN
};

//Button darkens when you hover over it
int buttonHover(struct MenuObj button);
int buttonClicked(enum MenuIds menu, int buttonInd, int mouseButton);
void drawMenu(enum MenuIds menu);

struct Menu emptyMenu();
struct MenuObj createMenuObj(const char *str, float x, float y, float chSz);
void addTextToMenu(struct Menu *menu, const char *text, float x, float y, float sz);
void addButtonToMenu(struct Menu *menu, const char *text, float x, float y, float sz);
void initMenus();

#endif

#define MENU_H
