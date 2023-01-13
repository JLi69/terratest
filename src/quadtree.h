#ifndef QUADTREE_H
#include "sprite.h"
#include "enemy.h"

#define CAPACITY 32
#define DEFAULT_SZ 2048 
#define ROOT 0
#define NIL_NODE -1

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
#endif
#define QUADTREE_H
