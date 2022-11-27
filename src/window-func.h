#ifndef WINDOW_UTIL_H
#define DEFAULT_WIDTH 960
#define DEFAULT_HEIGHT 600

//Initialize window
void initWindow(void);
void updateActiveShaderWindowSize(void);
int canQuit(void);
void updateWindow(void);

//Helper function for checking if a key is pressed
//Returns 1 if the key is pressed, 0 if the key is not 
//pressed
int isPressed(int key);

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

#endif
#define WINDOW_UTIL_H
