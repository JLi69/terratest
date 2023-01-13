#ifndef SPRITE_H

#define GRAVITY 640.0f

#define newpt createPoint

#define PI 3.1415926535f

union Point
{
	struct
	{
		float x, y;
	};

	struct
	{
		float w, h;
	};
};

struct Vector2D
{
	float x, y;
};

struct Rectangle
{
	union Point position;
	union Point dimensions;
};

struct Sprite
{
	struct Rectangle hitbox;
	struct Vector2D vel;
	short int falling, canMove, flipped, animating, canJump;
	int animationFrame;
	float timeSinceLastUpdate;

	unsigned int type, animationState;
	float timeExisted;
};

union Point midpoint(union Point pt1, union Point pt2);
union Point createPoint(float x, float y);
struct Rectangle createRectFromCorner(union Point botLeft, union Point topRight);
struct Rectangle createRect(float x, float y, float w, float h);
int colliding(struct Rectangle r1, struct Rectangle r2);

struct Sprite createSprite(struct Rectangle hitbox);
struct Sprite createSpriteWithType(struct Rectangle hitbox, int type);
void updateAnimation(
		struct Sprite *spr, 
		const int *frames, 
		const int numOfFrames, 
		const float timePassed, 
		const float frameLen);

void updateSpriteX(struct Sprite *spr, float secondsPerFrame);
void updateSpriteY(struct Sprite *spr, float secondsPerFrame);
void updateSprite(struct Sprite *spr, float secondsPerFrame);

//Vector operations
struct Vector2D createVector(float x, float y);
struct Vector2D generateUnit(float angle); //Angle is in radians
float dotProduct(struct Vector2D v1, struct Vector2D v2);

#endif
#define SPRITE_H
