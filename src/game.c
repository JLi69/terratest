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
#include <time.h>

#if defined(__linux__) || defined(__MINGW64__) || defined(__GNUC__) 
#include <sys/time.h>
#else
#error "Need to find sys/time.h to compile! If you are on Windows use MinGW"
#endif

#if defined(__linux__) || defined(__MINGW64__)
#include <unistd.h>
#elif defined(WIN32)
#include <io.h>
#define F_OK 0
#define access _access
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
				gameState = ON_SAVE_FILE_LIST;
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

		//User can have up to 8 worlds	
		int pathind = -1, selected = -1, clicks = 0;
		static const char *savepaths[] = 
		{
			"world1", "world2", "world3", "world4",
			"world5", "world6", "world7", "world8"
		};
		while(gameState == ON_SAVE_FILE_LIST && !canQuit() &&
			  pathind == -1)
		{
			
			displaySaveMenu(savepaths, 8, 4, selected);
			updateWindow();
			int buttonClicked = interactWithSaveMenu(savepaths, 8, 4, selected);
	
			int prevSelected = selected;
			if(buttonClicked != -1)
			{
				selected = buttonClicked;
				clicks++;

				if(prevSelected != selected)
					clicks = 0;
			}

			if(clicks >= 1)
				pathind = selected;

			if(selected == -2)
				gameState = ON_MAIN_MENU;
			else if(buttonClicked == -3 && prevSelected >= 0 &&
					access(savepaths[prevSelected], F_OK) == 0) //Deleted world
			{
				pathind = prevSelected;
				gameState = ON_DELETE_WORLD_PROMPT;
			}	
			else if(pathind >= 0)
			{
				if(access(savepaths[pathind], F_OK) == 0)
					gameState = PLAYING;
				else
					gameState = ON_CREATE_WORLD_PROMPT;
			}
		}

		while(gameState == ON_DELETE_WORLD_PROMPT && !canQuit())
		{
			displayDeletePrompt();
			updateWindow();

			if(buttonClicked(DELETE_WORLD_PROMPT, 1, GLFW_MOUSE_BUTTON_LEFT))
			{
				pathind = -1;
				gameState = ON_SAVE_FILE_LIST;
			}
			else if(buttonClicked(DELETE_WORLD_PROMPT, 0, GLFW_MOUSE_BUTTON_LEFT))
			{
				remove(savepaths[pathind]);
				pathind = -1;
				gameState = ON_SAVE_FILE_LIST;
			}
		}

		unsigned int seed = 0;
		while(gameState == ON_CREATE_WORLD_PROMPT && !canQuit())
		{
			displayCreatePrompt(seed);
			typeSeed(&seed);
			updateWindow();

			if(buttonClicked(CREATE_WORLD_PROMPT, 0, GLFW_MOUSE_BUTTON_LEFT))
				gameState = PLAYING;
			else if(buttonClicked(CREATE_WORLD_PROMPT, 1, GLFW_MOUSE_BUTTON_LEFT))
			{
				gameState = ON_SAVE_FILE_LIST;
				pathind = -1;	
			}
		}

		if(canQuit())
			quitFromMenu = 1;

		if(!canQuit() && pathind >= 0)
		{
			toggleCursor();
			updateWindow();
			if(readSave(&world, &player, savepaths[pathind]) == 0)
			{
				if(seed == 0)
					initGame(&world, &player, time(0));
				else
					initGame(&world, &player, seed);
			}
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

			updateWindow();			

			gettimeofday(&endFrame, 0);

			if(isPaused())
			{
				if(buttonClicked(PAUSED, 1, GLFW_MOUSE_BUTTON_LEFT))
				{
					saveWorld(&world, &player, "world1");	
					setPaused(0);
					toggleCursor();
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

			//Calculate the number of seconds a frame took
			seconds = endFrame.tv_sec - beginFrame.tv_sec +
					  1e-6 * (endFrame.tv_usec - beginFrame.tv_usec);	
		}

		if(!quitFromMenu && pathind >= 0)
		{	
			saveWorld(&world, &player, savepaths[pathind]); 
		}
	
		if(!canQuit() && pathind >= 0)
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
