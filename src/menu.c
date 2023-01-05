#include "menu.h"
#include <string.h>
#include "draw.h"
#include "window-func.h"

struct Menu menus[MAX_MENU];

int buttonHover(struct MenuObj button)
{
	double cursorX, cursorY;
	getCursorPos(&cursorX, &cursorY);

	float width = strlen(button.text) * button.charSz;
	
	if(button.y + button.charSz / 2.0f >= cursorY &&
	   button.y - button.charSz / 2.0f <= cursorY &&
	   button.x - width / 2.0f <= cursorX &&
	   button.x + width / 2.0f >= cursorX)
		return 1;
	return 0;
}

int buttonClicked(enum MenuIds menu, int buttonInd, int mouseButton)
{
	double cursorX, cursorY;
	getCursorPos(&cursorX, &cursorY);

	float width = strlen(menus[menu].menuButtons[buttonInd].text) * 
				  menus[menu].menuButtons[buttonInd].charSz;
	
	if(menus[menu].menuButtons[buttonInd].y + menus[menu].menuButtons[buttonInd].charSz / 2.0f >= cursorY &&
	   menus[menu].menuButtons[buttonInd].y - menus[menu].menuButtons[buttonInd].charSz / 2.0f <= cursorY &&
	   menus[menu].menuButtons[buttonInd].x - width / 2.0f <= cursorX &&
	   menus[menu].menuButtons[buttonInd].x + width / 2.0f >= cursorX &&
	   mouseButtonHeld(mouseButton))
	{
		releaseMouseButton(mouseButton);
		return 1;
	}
	return 0;
}

void drawMenu(enum MenuIds menu)
{
	for(int i = 0; i < menus[menu].buttonCount; i++)
	{
		if(buttonHover(menus[menu].menuButtons[i])) setBrightness(0.5f);
		else setBrightness(1.0f);
		drawString(menus[menu].menuButtons[i].text,
				   menus[menu].menuButtons[i].x,
				   menus[menu].menuButtons[i].y,
				   menus[menu].menuButtons[i].charSz);
	}
	setBrightness(1.0f);

	for(int i = 0; i < menus[i].textCount; i++)
	{
		drawString(menus[menu].menuText[i].text,
				   menus[menu].menuText[i].x,
				   menus[menu].menuText[i].y,
				   menus[menu].menuText[i].charSz);
	}
}

struct Menu emptyMenu()
{	
	struct Menu menu;
	menu.buttonCount = 0;
	menu.textCount = 0;
	return menu;
}

struct MenuObj createMenuObj(const char *str, float x, float y, float chSz)
{
	struct MenuObj menuobj;
	menuobj.x = x;
	menuobj.y = y;
	menuobj.charSz = chSz;
	strncpy(menuobj.text, str, MAX_TEXT_LEN);
	return menuobj;
}

void addButtonToMenu(struct Menu *menu, const char *text, float x, float y, float sz)
{
	if(menu->buttonCount >= MAX_BUTTONS)
		return;
	menu->menuButtons[menu->buttonCount++] = createMenuObj(text, x, y, sz);
}

void addTextToMenu(struct Menu *menu, const char *text, float x, float y, float sz)
{
	if(menu->textCount >= MAX_TEXT)
		return;
	menu->menuText[menu->textCount++] = createMenuObj(text, x, y, sz);
}

struct Menu respawnMenu()
{
	struct Menu respawn = emptyMenu();
	addTextToMenu(&respawn, "YOU DIED!", 0.0f, 0.0f, 64.0f);

	addButtonToMenu(&respawn, "Respawn", 0.0f, -96.0f, 32.0f); //Button to respawn	
	addButtonToMenu(&respawn, "Return to Main Menu", 0.0f, -160.0f, 32.0f); //Button to main menu
	return respawn;
}

struct Menu pauseMenu()
{
	struct Menu pause = emptyMenu();
	addTextToMenu(&pause, "PAUSED", 0.0f, 192.0f, 64.0f); 
	addButtonToMenu(&pause, "Resume Game", 0.0f, 64.0f, 32.0f); //Resume game
	addButtonToMenu(&pause, "Save", 0.0f, 0.0f, 32.0f); //Main menu
	addButtonToMenu(&pause, "Save & Quit", 0.0f, -64.0f, 32.0f); //Main menu
	return pause;
}

struct Menu mainMenu()
{	
	struct Menu mainMenu = emptyMenu();
	addTextToMenu(&mainMenu, "TERRATEST", 0.0f, 192.0f, 64.0f);
	addButtonToMenu(&mainMenu, "Play", 0.0f, 32.0f, 48.0f); //Load saves or create new world
	addButtonToMenu(&mainMenu, "Quit", 0.0f, -128.0f, 48.0f); //Quit game
	return mainMenu;
}

void initMenus()
{
	menus[RESPAWN] = respawnMenu();
	menus[PAUSED] = pauseMenu();
	menus[MAIN] = mainMenu();
}
