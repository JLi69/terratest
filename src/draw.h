#ifndef DRAW_H
void clear(void);

void setRectPos(float x, float y);
void setRectSize(float x, float y);
//Input based on 0 - 255
void setRectColor(float r, float g, float b, float a);
void drawRect(void);
void turnOnTexture(void);
void turnOffTexture(void);
void setTexFrac(float fracX, float fracY);
void setTexOffset(float x, float y);
void setTexSize(float texWidth, float texHeight);
void turnOnFlip(void);
void turnOffFlip(void);
void flip(int flipped);
void setTransparency(float a);
void setOutline(float width);
void setLevel(float level);
void setBrightness(float brightness);
//For sun/moon shader
void setRayCount(float rays);
void setPhase(float phase);
//Draws an integer onto the screen, assumes icons.png is loaded already
//Returns the x coordinate of the final digit
float drawInteger(int value, float x, float y, float digitSz);
float drawUnsignedInteger(unsigned int value, float x, float y, float digitSz);
//Draws text, returns x coordinate of the end character,
//assumes icons.png is loaded already
//Draws all letters as Capitals
float drawString(const char *str, float x, float y, float charSz);
void setRotationRad(float rotation); //In radians

#endif
#define DRAW_H
