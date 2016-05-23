#pragma once
#include"vec.h"
struct BSPTree
{
	BSPTreeNode RootNode;
};

struct BSPTreeNode
{
	BSPTree Tree;
	BSPTreePolygon *Divider=NULL;

	BSPTreeNode *RightChild = NULL;
	BSPTreeNode *LeftChild=NULL;

	BSPTreePolygon *PolygonSet[1000]{NULL};
	int NumPolygon=0;

	PointSet *SamplePoints{ NULL };
};

struct BSPTreePolygon
{
	vec3 point1;
	vec3 point2;
	vec3 point3;
	vec3 normal;
	float distance;
	bool divider = 0;
	bool hull = 0;
	bool portal = 0;
	BSPTreeNode *sector[2]{ NULL };
	int NumSector = 0;
	bool isDraw = 0;
};


struct Frustum
{
	BSPTreePolygon *PolygonSet[1000]{ NULL };
	int NumPolygon = 0;
};

struct PointSet
{
	vec3 *Point[1000]{NULL};
	int NumPoint = 0;
};

struct Ray
{
	vec3 Point1;
	vec3 Point2;
};

struct Sphere
{

};