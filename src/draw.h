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

#endif
#define DRAW_H
