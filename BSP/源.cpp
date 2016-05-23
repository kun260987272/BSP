//----------------BSP算法-----------------
//
//1.多边形集如果是凸多边形集，就停止分割
//2.多边形集如果不是凸多边形集，则选择“分割多边形":
//	2.1剔除掉凸包多边形
//	2.2计算非凸包多边形的正面多边形个数、负面多边形个数和被分割的多边形个数
//	2.3选择“分割多边形”
//3.根据“分割多边形”将多边形集，分割成正负两个子集
//4.对两个子集分别重复1、2、3
//  

#define COINCIDING 0
#define INFRONT 1
#define BEHIND -1
#define SPANNING 2
#define MINIMUMRELATION 0.8
#define MINIMUMSCALE 2
#define INFINITY 1000
#include"BSPTree.h"
#include<iostream>
using namespace std;


int classify_point(BSPTreePolygon polygon, vec3 point)
{
	float sidevalue = dot(polygon.normal, point);
	if (sidevalue == polygon.distance)
	{
		return COINCIDING;
	}
	else if (sidevalue > polygon.distance)
	{
		return INFRONT;
	}
	else
	{
		return BEHIND;
	}
}

int calculate_side(BSPTreePolygon polygon1, BSPTreePolygon polygon2)
{
	int NumPositive = 0;
	int NumNegative = 0;
	if (classify_point(polygon1, polygon2.point1) == INFRONT)
	{
		++NumPositive;
	}
	if (classify_point(polygon1, polygon2.point1) == BEHIND)
	{
		++NumNegative;
	}

	if (classify_point(polygon1, polygon2.point2) == INFRONT)
	{
		++NumPositive;
	}
	if (classify_point(polygon1, polygon2.point2) == BEHIND)
	{
		++NumNegative;
	}

	if (classify_point(polygon1, polygon2.point3) == INFRONT)
	{
		++NumPositive;
	}
	if (classify_point(polygon1, polygon2.point3) == BEHIND)
	{
		++NumNegative;
	}

	if (NumPositive > 0 && NumNegative == 0)
	{
		return INFRONT;
	}
	else if (NumPositive == 0 && NumNegative > 0)
	{
		return BEHIND;
	}
	else if (NumPositive == 0 && NumNegative == 0)
	{
		return COINCIDING;
	}
	else if (NumPositive > 0 && NumNegative > 0)
	{
		return SPANNING;
	}
}

bool is_convex_set(BSPTreeNode node)
{
	for (int i = 0;i < node.NumPolygon;++i)
	{
		for (int j = 0;j < node.NumPolygon;++j)
		{
			if (i != j && (calculate_side(*node.PolygonSet[i], *node.PolygonSet[j]) == BEHIND || calculate_side(*node.PolygonSet[i], *node.PolygonSet[j]) == SPANNING))
			{
				return false;
			}
		}
	}

	return true;
}

void tag_convex_hull(BSPTreeNode node)
{

	for (int i = 0;i < node.NumPolygon;++i)
	{
		bool flag = 0;
		for (int j = 0;j < node.NumPolygon;++j)
		{
			if (i != j && !(calculate_side(*node.PolygonSet[i], *node.PolygonSet[j]) == INFRONT || calculate_side(*node.PolygonSet[i], *node.PolygonSet[j]) == COINCIDING))
			{
				flag = 1;
			}
		}
		if (!flag)
		{
			node.PolygonSet[i]->hull = 1;
		}
	}
}


BSPTreePolygon* choose_dividing_polygon(BSPTreeNode node)
{
	if (is_convex_set(node))
	{
		return NULL;
	}
	float MinRelation = MINIMUMRELATION;
	BSPTreePolygon *BestPolygon = NULL;
	int LeastSplits = INFINITY;
	float BestRelation = 0;

	while (BestPolygon == NULL)
	{
		for (int i = 0;i < node.NumPolygon;++i)
		{
			if (node.PolygonSet[i]->divider == 0 && node.PolygonSet[i]->hull == 0)
			{
				int NumPositive = 1;
				int  NumNegative = 0;
				int NumSpanning = 0;
				int Relation;
				for (int j = 0;j < node.NumPolygon;++j)
				{
					if (i != j)
					{
						int value = calculate_side(*node.PolygonSet[i], *node.PolygonSet[j]);
						if (value == INFRONT || value == COINCIDING)
						{
							++NumPositive;
						}
						else if (value == BEHIND)
						{
							++NumPositive;
						}
						else if (value == SPANNING)
						{
							++NumSpanning;
						}
					}
				}

				if (NumPositive < NumNegative)
				{
					Relation = NumPositive / NumNegative;
				}
				else
				{
					Relation = NumNegative / NumPositive;
				}

				if (Relation > MinRelation && (NumSpanning<LeastSplits || (NumSpanning == LeastSplits&&Relation>BestRelation)))
				{
					BestPolygon = node.PolygonSet[i];
					LeastSplits = NumSpanning;
					BestRelation = Relation;
				}
			}
		}

		MinRelation /= MINIMUMSCALE;	
	}
	BestPolygon->divider = 1;
	return BestPolygon;
}

void split_polygon(BSPTreePolygon P, BSPTreePolygon Divider, BSPTreePolygon &Front, BSPTreePolygon &Back)
{
	int count = 0;
	vec3 intersect[2];
	if (classify_point(Divider, P.point1) != classify_point(Divider, P.point2))
	{
		float t = (dot(Divider.normal, P.point1) + Divider.distance) / dot(Divider.normal, vec3(P.point2.x - P.point1.x, P.point2.y - P.point1.y, P.point2.z - P.point1.z));
		float x = P.point1.x + (P.point2.x - P.point1.x)*t;
		float y = P.point1.y + (P.point2.y - P.point1.y)*t;
		float z = P.point1.z + (P.point2.z - P.point1.z)*t;
		intersect[count++] = (x, y, z);
	}
	if (classify_point(Divider, P.point1) != classify_point(Divider, P.point3))
	{
		float t = (dot(Divider.normal, P.point1) + Divider.distance) / dot(Divider.normal, vec3(P.point3.x - P.point1.x, P.point3.y - P.point1.y, P.point3.z - P.point1.z));
		float x = P.point1.x + (P.point3.x - P.point1.x)*t;
		float y = P.point1.y + (P.point3.y - P.point1.y)*t;
		float z = P.point1.z + (P.point3.z - P.point1.z)*t;
		intersect[count++] = (x, y, z);
	}
	if (classify_point(Divider, P.point2) != classify_point(Divider, P.point3))
	{
		float t = (dot(Divider.normal, P.point2) + Divider.distance) / dot(Divider.normal, vec3(P.point3.x - P.point2.x, P.point3.y - P.point2.y, P.point3.z - P.point2.z));
		float x = P.point2.x + (P.point3.x - P.point2.x)*t;
		float y = P.point2.y + (P.point3.y - P.point2.y)*t;
		float z = P.point2.z + (P.point3.z - P.point2.z)*t;
		intersect[count++] = (x, y, z);
	}

	if (classify_point(Divider, P.point1)==INFRONT)
	{
		Front.point1 = P.point1;
		Front.point2 = intersect[0];
		Front.point3 = intersect[1];
	}
	else
	{
		Back.point1 = P.point1;
		Back.point2 = intersect[0];
		Back.point3 = intersect[1];
	}
	if (classify_point(Divider, P.point2) != classify_point(Divider, P.point1))
	{
		if (classify_point(Divider, P.point2) == INFRONT)
		{
			Front.point1 = P.point2;
			Front.point2 = intersect[0];
			Front.point3 = intersect[1];
		}
		else
		{
			Back.point1 = P.point2;
			Back.point2 = intersect[0];
			Back.point3 = intersect[1];
		}
	}
	else
	{

		if (classify_point(Divider, P.point3) == INFRONT)
		{
			Front.point1 = P.point3;
			Front.point2 = intersect[0];
			Front.point3 = intersect[1];
		}
		else
		{
			Back.point1 = P.point3;
			Back.point2 = intersect[0];
			Back.point3 = intersect[1];
		}
	}	

	Front.distance = P.distance;
	Front.normal = P.normal;
	Back.distance = P.distance;
	Back.normal = P.normal;
}

BSPTreeNode* generate_bsp_tree(BSPTreeNode &node)
{
	BSPTreeNode positive;
	BSPTreeNode negative;

	if (is_convex_set(node))
	{
		return &node;
	}

	node.Divider = choose_dividing_polygon(node);

	for (int i = 0;i < node.NumPolygon;++i)
	{
		int value = calculate_side(*node.Divider, *node.PolygonSet[i]);
		if (value == INFRONT || value == COINCIDING)
		{
			positive.PolygonSet[positive.NumPolygon++] = node.PolygonSet[i];
		}
		else if(value==BEHIND)
		{
			negative.PolygonSet[positive.NumPolygon++] = node.PolygonSet[i];
		}
		else if (value == SPANNING)
		{
			BSPTreePolygon Front;
			BSPTreePolygon Back;
			split_polygon(*node.PolygonSet[i], *node.Divider, Front, Back);
			positive.PolygonSet[positive.NumPolygon++] = &Front;
			negative.PolygonSet[positive.NumPolygon++] = &Back;
		}
	}
	node.RightChild = generate_bsp_tree(positive);
	node.LeftChild = generate_bsp_tree(negative);
}
//临时的渲染函数
void draw_polygons(BSPTreeNode node)
{
	for (int i = 0;i < node.NumPolygon;++i)
	{
		cout << node.PolygonSet[i]->point1 << " ";
		cout << node.PolygonSet[i]->point2 << " ";
		cout << node.PolygonSet[i]->point3 << endl;
	}
	cout << endl;
}

void draw_bsp_tree(BSPTreeNode node,vec3 eye)
{
	if (node.RightChild == NULL&&node.LeftChild == NULL)
	{
		draw_polygons(node);
		return;
	}

	int side = classify_point(*node.Divider, eye);
	if (side == INFRONT || side == COINCIDING)
	{
		draw_bsp_tree(*node.LeftChild, eye);
		draw_bsp_tree(*node.RightChild, eye);
	}
	else if (side == BEHIND)
	{
		draw_bsp_tree(*node.RightChild, eye);
		draw_bsp_tree(*node.LeftChild, eye);
	}
}



//portal算法
bool inside_frustum(Frustum frustum, BSPTreePolygon polygon)
{
	for (int i = 0;i < frustum.NumPolygon;++i)
	{
		if (classify_point(*frustum.PolygonSet[i], polygon.point1) == BEHIND
			|| classify_point(*frustum.PolygonSet[i], polygon.point2) == BEHIND
			|| classify_point(*frustum.PolygonSet[i], polygon.point3) == BEHIND)
		{
			return false;
		}
	}
	return true;
}

//此函数未完成
Frustum clip_frustum(Frustum frustum, BSPTreePolygon polygon)
{

	//？？？？？？？？？
	return frustum;
}


//临时的渲染函数
void draw_a_polygon(BSPTreePolygon polygon)
{
	cout << polygon.point1 << " ";
	cout << polygon.point2 << " ";
	cout << polygon.point3 << endl;
}

void render_portal(BSPTreeNode node, Frustum frustum)
{
	if (node.LeftChild == NULL&&node.RightChild == NULL)
	{
		for (int i = 0;i < node.NumPolygon;++i)
		{
			if (node.PolygonSet[i]->portal == 1 && inside_frustum(frustum, *node.PolygonSet[i]))
			{
				Frustum NewFrustum;
				NewFrustum = clip_frustum(frustum, *node.PolygonSet[i]);
				for (int j = 0;j < 2;++j)
				{
					if (node.PolygonSet[i]->sector[i] != NULL&&node.PolygonSet[i]->sector[i] != &node)
					{
						render_portal(*node.PolygonSet[i]->sector[i], NewFrustum);
					}
				}
			}
			else if (node.PolygonSet[i]->isDraw == 0)
			{
				draw_a_polygon(*node.PolygonSet[i]);
				node.PolygonSet[i]->isDraw = 1;
			}
		}
	}
}

void place_portal(BSPTreePolygon PortalPolygon,BSPTreeNode node)
{
	if (node.LeftChild == NULL&&node.RightChild == NULL)
	{
		bool IsClipped;
		for (int i = 0;i < node.NumPolygon;++i)
		{
			IsClipped = 0;
			if (calculate_side(*node.PolygonSet[i], PortalPolygon) == SPANNING)
			{
				IsClipped = 1;
				BSPTreePolygon Right;
				BSPTreePolygon Left;
				split_polygon(PortalPolygon, *node.PolygonSet[i], Right, Left);
				place_portal(Right, node.Tree.RootNode);
				place_portal(Left, node.Tree.RootNode);
			}
		}
		if (!IsClipped)
		{
			//去掉portal与其他多边形重合的部分
			PortalPolygon.sector[PortalPolygon.NumSector++] = &node;
		}
	}
	else
	{
		//if(该节点的分割多边形没有向下传递)
		{
			//创建比分割多边形大得多（比包围盒大）的portal多边形
			BSPTreePolygon p;
			p.portal = 1;

			place_portal(p, *node.LeftChild);
			place_portal(p, *node.RightChild);
		}
		int side = calculate_side(*node.Divider, PortalPolygon);
		if (side == SPANNING)//??????
		{
			BSPTreePolygon Right;
			BSPTreePolygon Left;
			split_polygon(PortalPolygon, *node.Divider, Right, Left);
			place_portal(Right, node.Tree.RootNode);
			place_portal(Left, node.Tree.RootNode);
		}
		if (side == INFRONT || side == COINCIDING)
		{
			place_portal(PortalPolygon, *node.RightChild);
		}
		if (side == BEHIND || side == COINCIDING)
		{
			place_portal(PortalPolygon, *node.LeftChild);
		}
	}
}




//PSV算法

void cleanup_points(BSPTreeNode node, PointSet pointset)
{
	//未完成
//Removes points from the point set
//that are either coinciding with a polygon in the node or outside the bounding box
//of the node.Complexity: O(np), where n is the number of polygons in the node
//and p is the number of points in the set.
}

PointSet* distribute_points(BSPTreeNode node)
{
	//未完成
	/*This function distributes points with a certain
	interval along the splitting plane of the incoming node, within the boundaries of
	the bounding box of the node.It returns a set of points.Complexity: O(xy),
	where x is the width of the dividing plane in the bounding box and y is the
	height.*/

	PointSet pointset;
	return &pointset;
}

void distribute_sample_point(BSPTreeNode node, PointSet pointset)
{
	cleanup_points(node, pointset);
	if (node.LeftChild == NULL&&node.RightChild == NULL)
	{
		node.SamplePoints = &pointset;
	}
	else
	{

		PointSet RightPart=*distribute_points(node);
		PointSet LeftPart=*distribute_points(node);
		for (int i = 0;i < pointset.NumPoint;++i)
		{
			int side = classify_point(*node.Divider, *pointset.Point[i]);
			if (side == COINCIDING)
			{
				RightPart.Point[RightPart.NumPoint++] = pointset.Point[i];
				LeftPart.Point[RightPart.NumPoint++] = pointset.Point[i];
			}
			else if (side == INFRONT)
			{
				RightPart.Point[RightPart.NumPoint++] = pointset.Point[i];
			}
			else if (side == BEHIND)
			{
				LeftPart.Point[RightPart.NumPoint++] = pointset.Point[i];
			}
		}

		distribute_sample_point(*node.LeftChild, LeftPart);
		distribute_sample_point(*node.RightChild, RightPart);
	}
}

bool polygon_is_hit(BSPTreePolygon polygon, Ray ray)
{
	//returns whether the ray intersects the polygon or not
}

bool ray_intersects_something_in_tree(BSPTreeNode node, Ray ray)
{
	for (int i = 0;i < node.NumPolygon;++i)
	{

	}
}

bool intersects_sphere(Sphere sphere, Ray ray)
{
	//returns whether the ray intersects the sphere or not
}

Ray* create_ray(vec3 Point1, vec3 Point2)
{
	Ray ray;
	ray.Point1 = Point1;
	ray.Point2 = Point2;
	return &ray;
}


