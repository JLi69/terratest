#ifndef WINDOW_UTIL_H
#define DEFAULT_WIDTH 960
#define DEFAULT_HEIGHT 600

//Initialize window
void initWindow(void);
void updateActiveShaderWindowSize(void);
int canQuit(void);
void updateWindow(void);

//Quit the program and give an error
void crash(const char *msg);
//Quit the program
void quit(void);

#endif
#define WINDOW_UTIL_H
