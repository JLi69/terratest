#include "game.h"
#include "window-func.h"
#include "world.h"
#include "draw.h"
#include "gl-func.h"
#include <stdio.h>
#include "menu.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "savefile.h"
#include "crafting.h"

#if defined(__linux__) || defined(__MINGW64__) || defined(__GNUC__) 
#include <sys/time.h>
#else
#error "Need to find sys/time.h to compile! If you are on Windows use MinGW"
#endif

void loop(void)
{
	struct World world;
	struct Player player;
	enum GameState gameState = ON_MAIN_MENU;

	initGL();
	initMenus();
	initRecipes();

#if defined(__linux__) || defined(__MINGW64__) || defined(__GNUC__) 
	struct timeval beginFrame, endFrame;
	float seconds = 1.0f;	
	float frameUpdateTimer = 1.0f, fps = 0.0f;
	//Main loop
	while(!canQuit())
	{
		int quitFromMenu = 0;
		while(gameState == ON_MAIN_MENU && !canQuit())
		{
			gettimeofday(&beginFrame, 0);
			displayMainMenu(seconds);
			updateWindow();

			//Go to saves
			if(buttonClicked(MAIN, 0, GLFW_MOUSE_BUTTON_LEFT))
				gameState = PLAYING;
			//Quit game
			if(buttonClicked(MAIN, 1, GLFW_MOUSE_BUTTON_LEFT))
			{
				quitFromMenu = 1;
				quit();
			}

			gettimeofday(&endFrame, 0);

			//Calculate the number of seconds a frame took
			seconds = endFrame.tv_sec - beginFrame.tv_sec +
					  1e-6 * (endFrame.tv_usec - beginFrame.tv_usec);
		}

		while(gameState == ON_SAVE_FILE_LIST && !canQuit())
		{
			updateWindow();
		}

		if(canQuit())
			quitFromMenu = 1;

		if(!canQuit())
		{
			toggleCursor();
			updateWindow();
			if(readSave(&world, &player, "world1") == 0)
				initGame(&world, &player);
		}

		while(gameState == PLAYING && !canQuit())
		{
			gettimeofday(&beginFrame, 0);
			display(world, player);
			animateSprites(&world, &player.playerSpr, seconds);
			updateGameobjects(&world, &player, seconds);
			
			//FPS counter
			frameUpdateTimer += seconds;
			if(frameUpdateTimer > 1.0f)
			{
				frameUpdateTimer = 0.0f;
				fps = 1.0f / seconds;
			}	
			int winWidth, winHeight;
			getWindowSize(&winWidth, &winHeight);
			float end = drawString("FPS:", winWidth / 2.0f - 256.0f + 16.0f, winHeight / 2.0f - 80.0f, 16.0f);
			drawInteger((int)fps, end, winHeight / 2.0f - 80.0f, 16.0f);

			if(isPaused())
			{
				if(buttonClicked(PAUSED, 1, GLFW_MOUSE_BUTTON_LEFT))
				{
					saveWorld(&world, &player, "world1");
					setPaused(0);
				}
				else if(buttonClicked(PAUSED, 2, GLFW_MOUSE_BUTTON_LEFT))
				{
					gameState = ON_MAIN_MENU;
					setPaused(0);
				}
			}
			else if(player.health <= 0 && buttonClicked(RESPAWN, 1, GLFW_MOUSE_BUTTON_LEFT))
			{
				gameState = ON_MAIN_MENU;
				setPaused(0);
			}

			updateWindow();			

			gettimeofday(&endFrame, 0);

			//Calculate the number of seconds a frame took
			seconds = endFrame.tv_sec - beginFrame.tv_sec +
					  1e-6 * (endFrame.tv_usec - beginFrame.tv_usec);	
		}

		if(!quitFromMenu)
		{	
			saveWorld(&world, &player, "world1"); 
		}
	
		if(!canQuit())
		{
			enableCursor();
			free(world.blocks);
			free(world.backgroundBlocks);	
			free(world.transparentBlocks);
			free(player.inventory.slots);
		}	
	}
#endif	

	cleanup();
}
