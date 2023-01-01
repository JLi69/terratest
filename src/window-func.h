#ifndef WINDOW_UTIL_H
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 800

//Initialize window
void initWindow(void);
void updateActiveShaderWindowSize(void);
int canQuit(void);
int isPaused(void);
//0 = not paused, 1 = paused
void setPaused(int value);
void updateWindow(void);

//Helper function for checking if a key is pressed
//Returns 1 if the key is pressed, 0 if the key is not 
//pressed
int isPressed(int key);
int isPressedOnce(int key); //"Unpresses" a key after queried
int mouseButtonHeld(int button);
void releaseMouseButton(int button);
double getMouseScroll(); //Gets how much the mouse is scrolling in the y direction

//Quit the program and give an error
void crash(const char *msg);
//Quit the program
void quit(void);

//Wrapper functions for the glfw ones
void getWindowSize(int *w, int *h);
//Wrapper function for the glfw function,
//however, it returns the cursor position
//translated so that the origin is in the
//center of the window
void getCursorPos(double *x, double *y);

void toggleCursor();

#endif
#define WINDOW_UTIL_H
