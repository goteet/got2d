#pragma once
#include <vector>
#include <map>
#include "../scope_utility.h"

class Camera;

class QuadTreeNode
{
public:
	constexpr static float MinQuadTreeNodeBoundingSize = 100.0f;

	QuadTreeNode(QuadTreeNode* parent, const cxx::float2& center, float gridSize);

	~QuadTreeNode();

	QuadTreeNode* RecursiveAdd(const cxx::aabb2d<float>& bounds, g2d::Component* component);

	QuadTreeNode* AddToList(g2d::Component* component);

	void Remove(g2d::Component* component);

	cxx::aabb2d<float> GetBounding() { return mBounding; }

	void RecursiveFindVisible(Camera* camera);

	bool IsEmpty() { return mIsEmpty; }

private:
	void UpdateEmptyMark();

	class Direction
	{
	public:
		constexpr static unsigned int LeftTop = 0;
		constexpr static unsigned int LeftDown = 1;
		constexpr static unsigned int RightTop = 2;
		constexpr static unsigned int RightDown = 3;
		constexpr static unsigned int Count = 4;
	};

	const bool mIsLeaf = false;
	bool mIsEmpty = true;
	QuadTreeNode* mParent;
	QuadTreeNode* mDirectionNodes[Direction::Count];
	std::vector<g2d::Component*> mComponenList;
	cxx::aabb2d<float> mBounding;
};

class SpatialGraph
{
public:
	SpatialGraph(float boundSize);

	~SpatialGraph();

	void Add(g2d::Component* component);

	void Remove(g2d::Component* component);

	void RecursiveFindVisible(Camera* camera);

private:
	QuadTreeNode* mRoot;
	std::map<g2d::Component*, QuadTreeNode*> mLinkRef;
};
