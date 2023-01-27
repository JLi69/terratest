#ifndef QUADTREE_H
#include "sprite.h"
#include "enemy.h"

#define CAPACITY 16
#define DEFAULT_SZ 2048 
#define ROOT 0
#define NIL_NODE -1
#define MAX_ENEMIES 32000

struct IntVec
{
	int *values;
	int sz, maxSz;
};

struct IntVec createVec();
void vecPush(struct IntVec *vec, int value);

struct Node
{
	int topLeftInd,
		topRightInd,
		botLeftInd,
		botRightInd;
	int *ptIndices;
	union Point botLeftCorner, topRightCorner;
	int totalPts;
};

struct QuadTree
{
	struct Enemy *enemyArr;
	struct Node *nodes;
	int nodeCount, maxNodeCount;
	int pointCount, maxPointCount;
	union Point botLeftCorner, topRightCorner;
};

//Creates a node
struct Node createNode(union Point pt1, union Point pt2);
//Creates quadtree
struct QuadTree* createQuadTree(union Point botleft, union Point topright);
void destroyQuadtree(struct QuadTree *tree);
//Add empty node and returns the index
int addNode(struct QuadTree *qtree, struct Node n);
void splitNode(struct QuadTree *qtree, int nodeid);
//Node operations
void insertIntoNode(struct QuadTree *qtree, int nodeid, int ind);
void insertEnemy(struct QuadTree *qtree, struct Enemy enemy);
//Returns number of elements found
void searchInRect(struct QuadTree *qtree, union Point botleft, union Point topright,
				 struct IntVec *indices, int nodeid);
void searchInRectAndGetNodes(struct QuadTree *qtree, union Point botleft, union Point topright,
				 struct IntVec *indices, struct IntVec *nodes, int nodeid);
//Takes the index of a point and then removes the point from the quadtree
//and then reinserts it to keep the quadtree accurate when we have dynamic
//objects in it
void updatePoint(struct QuadTree *qtree, int ind, int nodeid);

//Removes a point out of the quadtree
void deletePtOutOfNode(struct QuadTree *qtree, int ind, int nodeid);
//Deletes the object at an index and deletes it from the quadtree
void deletePoint(struct QuadTree *qtree, int ind);

//Returns the index of the first object found that the enemy is colliding with
//If it can't find any collision, return -1
int getCollision(struct QuadTree *qtree, struct Enemy enemy, int nodeid);

struct QuadTree* rebuildQTree(struct QuadTree *qtree);

#endif
#define QUADTREE_H
