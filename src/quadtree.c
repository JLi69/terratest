#include "quadtree.h"
#include <stdlib.h>

#include <stdio.h>

struct Node createNode(union Point pt1, union Point pt2)
{
	struct Node node;
	node.botLeftCorner = pt1;
	node.topRightCorner = pt2;
	node.ptIndices = (int*)malloc(sizeof(int) * CAPACITY);
	node.totalPts = 0;
	node.botLeftInd = node.botRightInd = node.topLeftInd = node.topRightInd = NIL_NODE;
	return node;
}

struct IntVec createVec()
{
	struct IntVec vec;
	vec.maxSz = 32;
	vec.sz = 0;
	vec.values = (int*)malloc(sizeof(int) * vec.maxSz);
	return vec;
}

void vecPush(struct IntVec *vec, int value)
{
	if(vec->sz < vec->maxSz)
		vec->values[vec->sz++] = value;
	//Vector is full, expand it
	else
	{
		int* temp = (int*)malloc(sizeof(int) * vec->sz);
		for(int i = 0; i < vec->sz; i++)
			temp[i] = vec->values[i];
		vec->maxSz *= 2;
		free(vec->values);
		vec->values = (int*)malloc(sizeof(int) * vec->maxSz);
		for(int i = 0; i < vec->sz; i++)
			vec->values[i] = temp[i];
		vec->values[vec->sz++] = value;
		free(temp);
	}
}

void searchInRect(struct QuadTree *qtree, union Point botleft, union Point topright,
				 struct IntVec *indices, int nodeid)
{
	if(nodeid == NIL_NODE)
		return;
	float width = topright.x - botleft.x,
		  height = topright.y - botleft.y;
	//Check if node's bounding rectangle is colliding with range
	//if it isn't quit the search
	if(qtree->nodes[nodeid].botLeftCorner.x > topright.x ||
		qtree->nodes[nodeid].botLeftCorner.y > topright.y ||
		qtree->nodes[nodeid].topRightCorner.x < botleft.x ||
		qtree->nodes[nodeid].topRightCorner.y < botleft.y)
		return;
	//Otherwise continue and check if it is a leaf node
	if(qtree->nodes[nodeid].ptIndices != NULL)
	{
		for(int i = 0; i < qtree->nodes[nodeid].totalPts; i++)
		{
			if(qtree->enemyArr[qtree->nodes[nodeid].ptIndices[i]].spr.type == DELETED)
				continue;
			union Point pt = qtree->enemyArr[qtree->nodes[nodeid].ptIndices[i]].spr.hitbox.position;
			if(pt.x >= botleft.x &&
			   pt.x <= topright.x &&
			   pt.y >= botleft.y &&
			   pt.y <= topright.y)
			{
				//Add point to the index vector
				vecPush(indices, qtree->nodes[nodeid].ptIndices[i]);
				//found[(*totalFound)++] = qtree->nodes[nodeid].ptIndices[i];	
			}
		}
		return;
	}

	//If not a leaf node, continue searching
	searchInRect(qtree, botleft, topright, indices, 
				 qtree->nodes[nodeid].botLeftInd);
	searchInRect(qtree, botleft, topright, indices, 
				 qtree->nodes[nodeid].topLeftInd);
	searchInRect(qtree, botleft, topright, indices, 
				 qtree->nodes[nodeid].botRightInd);
	searchInRect(qtree, botleft, topright, indices, 
				 qtree->nodes[nodeid].topRightInd);
}

void searchInRectAndGetNodes(struct QuadTree *qtree, union Point botleft, union Point topright,
				 struct IntVec *indices, struct IntVec *nodes, int nodeid)
{
	if(nodeid == NIL_NODE)
		return;
	float width = topright.x - botleft.x,
		  height = topright.y - botleft.y;
	//Check if node's bounding rectangle is colliding with range
	//if it isn't quit the search
	if(qtree->nodes[nodeid].botLeftCorner.x > topright.x ||
		qtree->nodes[nodeid].botLeftCorner.y > topright.y ||
		qtree->nodes[nodeid].topRightCorner.x < botleft.x ||
		qtree->nodes[nodeid].topRightCorner.y < botleft.y)
		return;
	//Otherwise continue and check if it is a leaf node
	if(qtree->nodes[nodeid].ptIndices != NULL)
	{
		for(int i = 0; i < qtree->nodes[nodeid].totalPts; i++)
		{
			if(qtree->enemyArr[qtree->nodes[nodeid].ptIndices[i]].spr.type == DELETED)
				continue;
			union Point pt = qtree->enemyArr[qtree->nodes[nodeid].ptIndices[i]].spr.hitbox.position;
			if(pt.x >= botleft.x &&
			   pt.x <= topright.x &&
			   pt.y >= botleft.y &&
			   pt.y <= topright.y)
			{
				//Add point to the index vector
				vecPush(indices, qtree->nodes[nodeid].ptIndices[i]);
				vecPush(nodes, nodeid);
			}
		}
		return;
	}

	//If not a leaf node, continue searching
	searchInRectAndGetNodes(qtree, botleft, topright, indices, nodes,
				 qtree->nodes[nodeid].botLeftInd);
	searchInRectAndGetNodes(qtree, botleft, topright, indices, nodes,
				 qtree->nodes[nodeid].topLeftInd);
	searchInRectAndGetNodes(qtree, botleft, topright, indices, nodes,
				 qtree->nodes[nodeid].botRightInd);
	searchInRectAndGetNodes(qtree, botleft, topright, indices, nodes, 
				 qtree->nodes[nodeid].topRightInd);
}

void destroyQuadtree(struct QuadTree *tree)
{
	free(tree->enemyArr);
	for(int i = 0; i < tree->nodeCount; i++)
	{
		free(tree->nodes[i].ptIndices);
	}
	free(tree->nodes);
	free(tree);
}

struct QuadTree* createQuadTree(union Point botleft, union Point topright)
{
	struct QuadTree* qtree = (struct QuadTree*)malloc(sizeof(struct QuadTree));
	qtree->enemyArr = (struct Enemy*)malloc(sizeof(struct Enemy) * DEFAULT_SZ * CAPACITY);
	qtree->nodes = (struct Node*)malloc(sizeof(struct Node) * DEFAULT_SZ);
	qtree->nodeCount = 1;
	qtree->maxNodeCount = DEFAULT_SZ;
	qtree->nodes[ROOT] = createNode(botleft, topright);
	qtree->pointCount = 0;
	qtree->maxPointCount = DEFAULT_SZ * CAPACITY;
	return qtree;
}

void splitNode(struct QuadTree *qtree, int nodeid)
{
	union Point mid = midpoint(qtree->nodes[nodeid].botLeftCorner,
								qtree->nodes[nodeid].topRightCorner);
	//Split node into 4 quadrants and insert those subnodes into the quadtree
	int id;

	id = addNode(qtree, createNode(qtree->nodes[nodeid].botLeftCorner, mid));
	qtree->nodes[nodeid].botLeftInd = id;

	id = addNode(qtree, 
			createNode(
					newpt(mid.x, qtree->nodes[nodeid].botLeftCorner.y),
					newpt(qtree->nodes[nodeid].topRightCorner.x, mid.y)
				));
	qtree->nodes[nodeid].botRightInd = id;

	id = addNode(qtree, 
			createNode(
					newpt(qtree->nodes[nodeid].botLeftCorner.x, mid.y),
					newpt(mid.x, qtree->nodes[nodeid].topRightCorner.y)
				));
	qtree->nodes[nodeid].topLeftInd = id; 
	
	id = addNode(qtree, createNode(mid, qtree->nodes[nodeid].topRightCorner));
	qtree->nodes[nodeid].topRightInd = id;
}

int addNode(struct QuadTree *qtree, struct Node n)
{
    int index = qtree->nodeCount;
	//Insert it into the list 
	if(qtree->nodeCount < qtree->maxNodeCount)
	{
		qtree->nodes[qtree->nodeCount++] = n;
	}
	//List is full, expand it	
	else
	{
		struct Node* temp = (struct Node*)malloc(sizeof(struct Node) * qtree->maxNodeCount);
		for(int i = 0; i < qtree->maxNodeCount; i++)
			temp[i] = qtree->nodes[i];
		free(qtree->nodes);
		qtree->maxNodeCount *= 2;
		qtree->nodes = (struct Node*)malloc(sizeof(struct Node) * qtree->maxNodeCount);
		for(int i = 0; i < qtree->nodeCount; i++)
			qtree->nodes[i] = temp[i];
		qtree->nodes[qtree->nodeCount++] = n;
		free(temp);
	}
	return index;	
}

void insertIntoNode(struct QuadTree *qtree, int nodeid, int ind)
{	
	if(qtree->enemyArr[ind].spr.hitbox.position.x < qtree->nodes[nodeid].botLeftCorner.x ||
		qtree->enemyArr[ind].spr.hitbox.position.x >= qtree->nodes[nodeid].topRightCorner.x ||
		qtree->enemyArr[ind].spr.hitbox.position.y < qtree->nodes[nodeid].botLeftCorner.y ||
		qtree->enemyArr[ind].spr.hitbox.position.y >= qtree->nodes[nodeid].topRightCorner.y)
		return;

	if(qtree->nodes[nodeid].totalPts < CAPACITY && qtree->nodes[nodeid].ptIndices != NULL)
		qtree->nodes[nodeid].ptIndices[qtree->nodes[nodeid].totalPts++] = ind;
	//Full 
	else
	{
		//if leaf node, split and copy contents into appropriate child nodes
		if(qtree->nodes[nodeid].botLeftInd == NIL_NODE ||
		  qtree->nodes[nodeid].topLeftInd == NIL_NODE ||
		  qtree->nodes[nodeid].botRightInd == NIL_NODE ||
		  qtree->nodes[nodeid].topRightInd == NIL_NODE)
		{
			//First, check how many points have the same position as the point being inserted
			int samePos = 0;
			for(int i = 0; i < qtree->nodes[nodeid].totalPts; i++)
			{		
				int ptInd = qtree->nodes[nodeid].ptIndices[i];
				if(qtree->enemyArr[ptInd].spr.hitbox.position.x == qtree->enemyArr[ind].spr.hitbox.position.x &&
					qtree->enemyArr[ptInd].spr.hitbox.position.y == qtree->enemyArr[ind].spr.hitbox.position.y)
					samePos++;
			}

			if(samePos >= CAPACITY)
			{
				qtree->pointCount--;
				return;
			}

			splitNode(qtree, nodeid);
			//Insert points into the child nodes
			for(int i = 0; i < qtree->nodes[nodeid].totalPts; i++)
			{	
				int ptInd = qtree->nodes[nodeid].ptIndices[i];
				insertIntoNode(qtree, qtree->nodes[nodeid].botLeftInd, ptInd);
				insertIntoNode(qtree, qtree->nodes[nodeid].botRightInd, ptInd);
				insertIntoNode(qtree, qtree->nodes[nodeid].topLeftInd, ptInd);
				insertIntoNode(qtree, qtree->nodes[nodeid].topRightInd, ptInd);
			}
			free(qtree->nodes[nodeid].ptIndices);
			qtree->nodes[nodeid].ptIndices = NULL;
			qtree->nodes[nodeid].totalPts = 0;
		}
		union Point mid = midpoint(qtree->nodes[nodeid].botLeftCorner,
									qtree->nodes[nodeid].topRightCorner);
		if(qtree->enemyArr[ind].spr.hitbox.position.x < mid.x &&
		   qtree->enemyArr[ind].spr.hitbox.position.y < mid.y)
			insertIntoNode(qtree, qtree->nodes[nodeid].botLeftInd, ind);
		else if(qtree->enemyArr[ind].spr.hitbox.position.x >= mid.x &&
		   qtree->enemyArr[ind].spr.hitbox.position.y < mid.y)
			insertIntoNode(qtree, qtree->nodes[nodeid].botRightInd, ind);	
		else if(qtree->enemyArr[ind].spr.hitbox.position.x < mid.x &&
		   qtree->enemyArr[ind].spr.hitbox.position.y >= mid.y)
			insertIntoNode(qtree, qtree->nodes[nodeid].topLeftInd, ind);	
		else if(qtree->enemyArr[ind].spr.hitbox.position.x >= mid.x &&
		   qtree->enemyArr[ind].spr.hitbox.position.y >= mid.y)
			insertIntoNode(qtree, qtree->nodes[nodeid].topRightInd, ind);		
	}
}

void insertEnemy(struct QuadTree *qtree, struct Enemy enemy)
{
	if(qtree->pointCount >= MAX_ENEMIES)
		return;
	int ind = qtree->pointCount;
	//Insert it into the list 
	if(qtree->pointCount < qtree->maxPointCount)
	{
		qtree->enemyArr[qtree->pointCount++] = enemy;
		//Insert the point into the nodes
		insertIntoNode(qtree, ROOT, ind);
	}
	//List is full, expand it	
	else
	{
		printf("Expand\n");
		struct Enemy* temp = (struct Enemy*)malloc(sizeof(struct Enemy) * qtree->maxPointCount);
		for(int i = 0; i < qtree->maxPointCount; i++)
			temp[i] = qtree->enemyArr[i];
		free(qtree->enemyArr);
		qtree->maxPointCount *= 2;
		qtree->enemyArr = (struct Enemy*)malloc(sizeof(struct Enemy) * qtree->maxPointCount);
		for(int i = 0; i < qtree->pointCount; i++)
			qtree->enemyArr[i] = temp[i];
		qtree->enemyArr[qtree->pointCount++] = enemy;
		free(temp);
		//Insert the point into the nodes
		insertIntoNode(qtree, ROOT, ind);
	}
}

void updatePoint(struct QuadTree *qtree, int ind, int nodeid)
{
	for(int i = 0; i < qtree->nodes[nodeid].totalPts; i++)
	{	
		int ptInd = qtree->nodes[nodeid].ptIndices[i];
		if(ptInd == ind && 
			(qtree->enemyArr[ind].spr.hitbox.position.x < qtree->nodes[nodeid].botLeftCorner.x ||
			 qtree->enemyArr[ind].spr.hitbox.position.x >= qtree->nodes[nodeid].topRightCorner.x ||
			 qtree->enemyArr[ind].spr.hitbox.position.y < qtree->nodes[nodeid].botLeftCorner.y ||
			 qtree->enemyArr[ind].spr.hitbox.position.y >= qtree->nodes[nodeid].topRightCorner.y)) //Reinsert into tree only if it is no longer inside the node
		{
			//Swap
			int temp = qtree->nodes[nodeid].ptIndices[qtree->nodes[nodeid].totalPts - 1];
			qtree->nodes[nodeid].ptIndices[qtree->nodes[nodeid].totalPts - 1] = ptInd;
			qtree->nodes[nodeid].ptIndices[i] = temp;
			qtree->nodes[nodeid].totalPts--;
			insertIntoNode(qtree, ROOT, ind);
			return;	
		}
		else if(ptInd == ind) return;
	}
}

void deletePtOutOfNode(struct QuadTree *qtree, int ind, int nodeid)
{
	if(nodeid < 0)
		return;
	if(qtree->enemyArr[ind].spr.hitbox.position.x < qtree->nodes[nodeid].botLeftCorner.x ||
		qtree->enemyArr[ind].spr.hitbox.position.x >= qtree->nodes[nodeid].topRightCorner.x ||
		qtree->enemyArr[ind].spr.hitbox.position.y < qtree->nodes[nodeid].botLeftCorner.y ||
		qtree->enemyArr[ind].spr.hitbox.position.y >= qtree->nodes[nodeid].topRightCorner.y)
		return;

	if((qtree->nodes[nodeid].botLeftInd == NIL_NODE ||
       qtree->nodes[nodeid].topLeftInd == NIL_NODE ||
       qtree->nodes[nodeid].botRightInd == NIL_NODE ||
       qtree->nodes[nodeid].topRightInd == NIL_NODE) &&
	   qtree->nodes[nodeid].ptIndices != NULL)
	{
		for(int i = 0; i < qtree->nodes[nodeid].totalPts; i++)
		{	
			int ptInd = qtree->nodes[nodeid].ptIndices[i];
			if(ptInd == ind)
			{
				//Swap
				int temp = qtree->nodes[nodeid].ptIndices[qtree->nodes[nodeid].totalPts - 1];
				qtree->nodes[nodeid].ptIndices[qtree->nodes[nodeid].totalPts - 1] = ptInd;
				qtree->nodes[nodeid].ptIndices[i] = temp;
				qtree->nodes[nodeid].totalPts--;
				return;	
			}
		}
	}

	deletePtOutOfNode(qtree, ind, qtree->nodes[nodeid].botLeftInd);
	deletePtOutOfNode(qtree, ind, qtree->nodes[nodeid].topLeftInd);
	deletePtOutOfNode(qtree, ind, qtree->nodes[nodeid].botRightInd);
	deletePtOutOfNode(qtree, ind, qtree->nodes[nodeid].topRightInd);	
}

void deletePoint(struct QuadTree *qtree, int ind)
{
	//Store last one in a temporary variable
	struct Enemy temp = qtree->enemyArr[qtree->pointCount - 1];
	//Delete last one out of quadtree
	deletePtOutOfNode(qtree, qtree->pointCount - 1, ROOT);
	//Delete ind out of tree
	deletePtOutOfNode(qtree, ind, ROOT);
	qtree->enemyArr[ind] = temp;	
	//Decrement size
	qtree->pointCount--;
	//Reinsert last one into quadtree
	insertIntoNode(qtree, ROOT, ind);
}

int getCollision(struct QuadTree *qtree, struct Enemy enemy, int nodeid)
{
	if(nodeid < 0)
		return -1;
	if(enemy.spr.hitbox.position.x - enemy.spr.hitbox.dimensions.w 
			< qtree->nodes[nodeid].botLeftCorner.x ||
		enemy.spr.hitbox.position.x + enemy.spr.hitbox.dimensions.w 
			>= qtree->nodes[nodeid].topRightCorner.x ||
		enemy.spr.hitbox.position.y - enemy.spr.hitbox.dimensions.h 
			< qtree->nodes[nodeid].botLeftCorner.y ||
		enemy.spr.hitbox.position.y + enemy.spr.hitbox.dimensions.h
			>= qtree->nodes[nodeid].topRightCorner.y)
		return -1;

	if((qtree->nodes[nodeid].botLeftInd == NIL_NODE ||
       qtree->nodes[nodeid].topLeftInd == NIL_NODE ||
       qtree->nodes[nodeid].botRightInd == NIL_NODE ||
       qtree->nodes[nodeid].topRightInd == NIL_NODE) &&
	   qtree->nodes[nodeid].ptIndices != NULL)
	{
		for(int i = 0; i < qtree->nodes[nodeid].totalPts; i++)
		{	
			int ptInd = qtree->nodes[nodeid].ptIndices[i];
			if(colliding(qtree->enemyArr[ptInd].spr.hitbox, enemy.spr.hitbox))
				return ptInd;	
		}
		return -1;
	}

	int indices[] = 
	{
		getCollision(qtree, enemy, qtree->nodes[nodeid].botLeftInd),
		getCollision(qtree, enemy, qtree->nodes[nodeid].topLeftInd),
		getCollision(qtree, enemy, qtree->nodes[nodeid].botRightInd),
		getCollision(qtree, enemy, qtree->nodes[nodeid].topRightInd)
	};

	for(int i = 0; i < 4; i++)
		if(indices[i] != -1)
			return indices[i];
	return -1;
}

struct QuadTree* rebuildQTree(struct QuadTree *qtree)
{
	struct QuadTree* newQtree = createQuadTree(qtree->botLeftCorner, qtree->topRightCorner);
	for(int i = 0; i < qtree->pointCount; i++)
		insertEnemy(newQtree, qtree->enemyArr[i]);
	return newQtree;
}
