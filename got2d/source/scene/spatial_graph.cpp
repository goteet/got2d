#include <algorithm>
#include "g2dscene.h"
#include "camera.h"
#include "spatial_graph.h"

QuadTreeNode::QuadTreeNode(QuadTreeNode* parent, const cxx::float2& center, float gridSize)
	: mBounding(
		cxx::float2(center.x - gridSize, center.y - gridSize),
		cxx::float2(center.x + gridSize, center.y + gridSize))
	, mIsLeaf(gridSize < MinQuadTreeNodeBoundingSize)
	, mParent(parent)
{
	for (QuadTreeNode*& pChildNode : mDirectionNodes)
	{
		pChildNode = nullptr;
	}
}

QuadTreeNode::~QuadTreeNode()
{
	for (QuadTreeNode*& pChildNode : mDirectionNodes)
	{
		cxx::safe_delete(pChildNode);
	}
}

inline bool Contains(const cxx::float2& center, float gridSize, const cxx::aabb2d<float>& nodeAABB)
{
	cxx::aabb2d<float> bounding(
		cxx::float2(center.x - gridSize, center.y - gridSize),
		cxx::float2(center.x + gridSize, center.y + gridSize)
	);

	return bounding.hit_test(nodeAABB) == cxx::intersection::contain;
}

QuadTreeNode* QuadTreeNode::RecursiveAdd(const cxx::aabb2d<float>& bounds, g2d::Component* component)
{
	mIsEmpty = false;
	if (!mIsLeaf)
	{
		//try to push pChildNode pNode
		//push to self if failed.

		//WE HERE NEED aabb::move() !
		float extend = mBounding.extend().x;
		float halfExtend = extend * 0.5f;
		auto& center = mBounding.center();
		//x-neg, y-pos
		cxx::aabb2d<float> bounding(
			cxx::float2(center.x - extend, center.y + extend),
			mBounding.center());

		if (bounding.hit_test(bounds) == cxx::intersection::contain)
		{
			if (mDirectionNodes[Direction::LeftTop] == nullptr)
			{
				mDirectionNodes[Direction::LeftTop] = new QuadTreeNode(this, center, halfExtend);
			}
			return mDirectionNodes[Direction::LeftTop]->RecursiveAdd(bounds, component);
		}

		//x-neg, y-neg
		move(bounding, { 0, -extend });
		if (bounding.hit_test(bounds) == cxx::intersection::contain)
		{
			if (mDirectionNodes[Direction::LeftDown] == nullptr)
			{
				mDirectionNodes[Direction::LeftDown] = new QuadTreeNode(this, center, halfExtend);
			}
			return  mDirectionNodes[Direction::LeftDown]->RecursiveAdd(bounds, component);
		}

		//x-pos,y-pos
		move(bounding, { extend, extend });
		if (bounding.hit_test(bounds) == cxx::intersection::contain)
		{
			if (mDirectionNodes[Direction::RightTop] == nullptr)
			{
				mDirectionNodes[Direction::RightTop] = new QuadTreeNode(this, center, halfExtend);
			}
			return mDirectionNodes[Direction::RightTop]->RecursiveAdd(bounds, component);
		}

		//x-pos, y-neg
		move(bounding, { 0, -extend });
		if (bounding.hit_test(bounds) == cxx::intersection::contain)
		{
			if (mDirectionNodes[Direction::RightDown] == nullptr)
			{
				mDirectionNodes[Direction::RightDown] = new QuadTreeNode(this, center, halfExtend);
			}
			return mDirectionNodes[Direction::RightDown]->RecursiveAdd(bounds, component);
		}

		return AddToList(component);
	}
	else
	{
		return AddToList(component);
	}
}

QuadTreeNode* QuadTreeNode::AddToList(g2d::Component* component)
{
	mIsEmpty = false;
	mComponenList.push_back(component);
	return this;
}

void QuadTreeNode::UpdateEmptyMark()
{
	if (mComponenList.size() > 0)
	{
		return;
	}

	bool hasEntities = false;
	for (QuadTreeNode* pChildNode : mDirectionNodes)
	{
		if (pChildNode && !pChildNode->IsEmpty())
		{
			hasEntities = true;
			break;
		}
	}

	if (!hasEntities)
	{
		mIsEmpty = true;
		if (mParent)
		{
			mParent->UpdateEmptyMark();
		}
	}
}
void QuadTreeNode::Remove(g2d::Component* component)
{
	auto oldEnd = mComponenList.end();
	auto newEnd = std::remove(mComponenList.begin(), oldEnd, component);
	mComponenList.erase(newEnd, oldEnd);

	UpdateEmptyMark();
}

void QuadTreeNode::RecursiveFindVisible(Camera* camera)
{
	if (IsEmpty())
		return;

	for (g2d::Component*& component : mComponenList)
	{
		if (component->GetSceneNode()->IsVisible() && camera->TestVisible(component))
		{
			camera->mVisibleComponents.push_back(component);
		}
	}

	for (auto& child : mDirectionNodes)
	{
		if (child != nullptr && camera->TestVisible(child->GetBounding()))
		{
			child->RecursiveFindVisible(camera);
		}
	}
}

SpatialGraph::SpatialGraph(float boundSize)
	: mRoot(new QuadTreeNode(nullptr, cxx::float2::zero(), boundSize))
{
}

SpatialGraph::~SpatialGraph()
{
	delete mRoot;
}

void SpatialGraph::Add(g2d::Component* component)
{
	if (!g2d::Is<::Camera>(component))
	{
		Remove(component);

		QuadTreeNode* pNode = nullptr;
		if (component->GetSceneNode()->IsStatic())
		{
			cxx::aabb2d<float> nodeAABB = component->GetWorldAABB();
			pNode = mRoot->RecursiveAdd(nodeAABB, component);
		}
		else
		{
			//dynamic objects
			// FOR HINT: mRoot->mIsEmpty = false;
			pNode = mRoot->AddToList(component);
		}
		mLinkRef[component] = pNode;
	}
}

void SpatialGraph::Remove(g2d::Component* component)
{
	if (!g2d::Is<::Camera>(component))
	{
		if (mLinkRef.count(component))
		{
			QuadTreeNode* pNode = mLinkRef[component];
			pNode->Remove(component);
			mLinkRef.erase(component);
		}
	}
}

void SpatialGraph::RecursiveFindVisible(Camera* camera)
{
	mRoot->RecursiveFindVisible(camera);
}
