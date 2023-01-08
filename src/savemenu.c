#include "game.h"
#include <math.h>
#include "menu.h"
#include "window-func.h"
#include <GLFW/glfw3.h>

#if defined(__linux__) || defined(__MINGW64__)
#include <unistd.h>
#elif defined(WIN32)
#include <io.h>
#define F_OK 0
#define access _access
#endif

int interactWithSaveMenu(const char **savepaths, int savecount, int perColumn, int selected)
{
	int columns = (int)ceilf((float)savecount / (float)perColumn);

	for(int i = 0; i < columns; i++)
	{
		for(int j = 0; j < perColumn; j++)
		{
			int ind = i * perColumn + j;
			if(ind >= savecount)
				continue;

			int saveExists = access(savepaths[ind], F_OK) == 0;

			struct MenuObj tempbutton;

			if(saveExists)
			{
				tempbutton = createMenuObj(savepaths[ind],
										   -(float)columns / 2.0f * 480.0f + i * 480.0f + 240.0f,
											(float)perColumn / 2.0f * 96.0f - j * 96.0f - 48.0f,
											48.0f);
			}
			else
			{
				tempbutton = createMenuObj("[EMPTY]",
										   -(float)columns / 2.0f * 480.0f + i * 480.0f + 240.0f,
											(float)perColumn / 2.0f * 96.0f - j * 96.0f - 48.0f,
											48.0f);
			}

			if(buttonHover(tempbutton) && mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
			{
				releaseMouseButton(GLFW_MOUSE_BUTTON_LEFT);
				return ind;
			}
		}
	}
	
	struct MenuObj cancel = createMenuObj("Cancel",
							   256.0f,
							   (float)perColumn / 2.0f * 96.0f - perColumn * 96.0f - 48.0f - 96.0f,
							   32.0f);
	if(buttonHover(cancel) && mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
	{
		releaseMouseButton(GLFW_MOUSE_BUTTON_LEFT);
		return -2;
	}

	struct MenuObj play = createMenuObj("Play",
							   0.0f,
							   (float)perColumn / 2.0f * 96.0f - perColumn * 96.0f - 48.0f - 96.0f,
							   32.0f);
	if(buttonHover(play) && mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
	{
		releaseMouseButton(GLFW_MOUSE_BUTTON_LEFT);
		return selected;
	}

	struct MenuObj deleteButton = createMenuObj("Delete",
							   -256.0f,
							   (float)perColumn / 2.0f * 96.0f - perColumn * 96.0f - 48.0f - 96.0f,
							   32.0f);
	if(buttonHover(deleteButton) && mouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
	{
		releaseMouseButton(GLFW_MOUSE_BUTTON_LEFT);
		return -3;	
	}

	return -1;
}

void typeSeed(unsigned int *seed)
{
	if(isPressedOnce(GLFW_KEY_BACKSPACE))
		*seed /= 10;

	int keys[] = 
	{
		GLFW_KEY_0,
		GLFW_KEY_1,
		GLFW_KEY_2,
		GLFW_KEY_3,
		GLFW_KEY_4,
		GLFW_KEY_5,
		GLFW_KEY_6,
		GLFW_KEY_7,
		GLFW_KEY_8,
		GLFW_KEY_9,
	};

	for(int i = 0; i < 10; i++)
	{
		if(isPressedOnce(keys[i]) && *seed < UINT32_MAX / 10)
		{
			*seed *= 10;
			*seed += i;	
		}
	}
}
