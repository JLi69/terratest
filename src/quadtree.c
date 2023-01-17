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
	qtree->deletedPoints = 0;
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
	
}

void deletePoint(struct QuadTree *qtree, int ind)
{
	qtree->deletedPoints++;
	qtree->enemyArr[ind].spr.type = DELETED;
}
