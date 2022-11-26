#include "game.h"
#include "window-func.h"
#include "world.h"
#include "draw.h"
#include "gl-func.h"
#include <stdio.h>

#if defined(__linux__) || defined(__MINGW64__) || defined(__GNUC__) 
#include <sys/time.h>
#else
#error "Need to find sys/time.h to compile! If you are on Windows use MinGW"
#endif

void loop(void)
{
	struct World world;
	struct Sprite player;

	initGL();
	initGame(&world, &player);

#if defined(__linux__) || defined(__MINGW64__) || defined(__GNUC__) 
	struct timeval beginFrame, endFrame;
	float seconds = 1.0f;	
	//Main loop
	while(!canQuit())
	{
		gettimeofday(&beginFrame, 0);
		display(world, player);
		animateSprites(&world, &player, seconds);
		updateGameobjects(&world, &player, seconds);
		updateWindow();		
		gettimeofday(&endFrame, 0);
	
		//Calculate the number of seconds a frame took
		seconds = endFrame.tv_sec - beginFrame.tv_sec +
				  1e-6 * (endFrame.tv_usec - beginFrame.tv_usec);
		//If on dev version, output frames per second
#ifdef DEV_VERSION
		//Mark any times when the frames per second is below 50
		if(1.0f / seconds < 50.0f) fprintf(stderr, "Below 50 FPS >>>>>> ");
		fprintf(stderr, "FPS: %f\n", 1.0f / seconds);
#endif
	}
#endif	

	cleanup();
}
