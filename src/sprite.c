#include "sprite.h"
#include <math.h>

int colliding(struct Rectangle r1, struct Rectangle r2)
{
	return r1.position.x - r1.dimensions.w / 2.0f < r2.position.x + r2.dimensions.w / 2.0f &&
		   r1.position.y - r1.dimensions.h / 2.0f < r2.position.y + r2.dimensions.h / 2.0f &&
		   r1.position.x + r1.dimensions.w / 2.0f > r2.position.x - r2.dimensions.w / 2.0f &&	
		   r1.position.y + r1.dimensions.h / 2.0f > r2.position.y - r2.dimensions.h / 2.0f;
}

union Point createPoint(float x, float y)
{
	union Point p;
	p.x = x;
	p.y = y;
	return p;
}

struct Rectangle createRectFromCorner(union Point botLeft, union Point topRight)
{
	struct Rectangle r;
	r.dimensions.w = topRight.x - botLeft.x;
	r.dimensions.h = topRight.y - botLeft.y;
	r.position.x = (botLeft.x + topRight.x) / 2.0f;
	r.position.y = (botLeft.y + topRight.y) / 2.0f;
	return r;
}

struct Rectangle createRect(float x, float y, float w, float h)
{
	struct Rectangle r;
	r.dimensions.w = w;
	r.dimensions.h = h;
	r.position.x = x;
	r.position.y = y;
	return r;
}

union Point midpoint(union Point pt1, union Point pt2)
{
	union Point midpoint;
	midpoint.x = (pt1.x + pt2.x) / 2.0f;
	midpoint.y = (pt1.y + pt2.y) / 2.0f;
	return midpoint;
}

struct Sprite createSprite(struct Rectangle hitbox)
{
	struct Sprite spr;
	spr.hitbox = hitbox;
	spr.vel.x = 0.0f;
	spr.vel.y = 0.0f;
	spr.falling = 1;
	spr.canMove = 0;
	spr.flipped = 0;
	spr.animating = 0;
	spr.animationFrame = 0;
	spr.type = 0;
	spr.timeSinceLastUpdate = 0.0f;
	spr.canJump = 0;
	spr.timeExisted = 0.0f;
	return spr;
}

struct Sprite createSpriteWithType(struct Rectangle hitbox, int type)
{
	struct Sprite spr = createSprite(hitbox);
	spr.type = type;
	return spr;
}

void updateAnimation(
	struct Sprite *spr, 
	const int *frames, 
	const int numOfFrames, 
	const float timePassed, 
	const float frameLen)
{
	if(!spr->animating)
		return;
	int index = (int)(timePassed / frameLen) % numOfFrames;
	spr->animationFrame = frames[index];
}

void updateSpriteX(struct Sprite *spr, float secondsPerFrame)
{
	//Nonmoving object
	if(!spr->canMove)
		return;

	spr->hitbox.position.x += spr->vel.x * secondsPerFrame;
}

void updateSpriteY(struct Sprite *spr, float secondsPerFrame)
{
	//Nonmoving object
	if(!spr->canMove)
		return;

	spr->hitbox.position.y += spr->vel.y * secondsPerFrame;
	
	if(spr->falling)
		spr->vel.y -= GRAVITY * secondsPerFrame;
}

void updateSprite(struct Sprite *spr, float secondsPerFrame)
{
	updateSpriteX(spr, secondsPerFrame);
	updateSpriteY(spr, secondsPerFrame);
}

struct Vector2D createVector(float x, float y)
{
	struct Vector2D v;
	v.x = x;
	v.y = y;
	return v;
}

struct Vector2D generateUnit(float angle)
{
	struct Vector2D unit;
	unit.x = cosf(angle);
	unit.y = sinf(angle);
	return unit;
}

float dotProduct(struct Vector2D v1, struct Vector2D v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
