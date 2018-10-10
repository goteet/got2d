#include "scene_node_container.h"
#include "scene_node.h"
#include "../scope_utility.h"

SceneNodeContainer::~SceneNodeContainer()
{
	DestroyAllChildren();
}

void SceneNodeContainer::DestroyAllChildren()
{
	for (::SceneNode* pChild : mChildrenNodes)
	{
		delete pChild;
	}
	mChildrenNodes.clear();

	DestroyRemovedNodes();
}

::SceneNode * SceneNodeContainer::First() const
{
	return mChildrenNodes.empty()
		? nullptr
		: mChildrenNodes.front();
}

::SceneNode * SceneNodeContainer::Last() const
{
	return mChildrenNodes.empty()
		? nullptr
		: mChildrenNodes.back();
}

::SceneNode* SceneNodeContainer::At(unsigned int index) const
{
	ENSURE(index < GetCount());
	return mChildrenNodes.at(index);
}

unsigned int SceneNodeContainer::GetCount() const
{
	return static_cast<unsigned int>(mChildrenNodes.size());
}

void SceneNodeContainer::Add(::SceneNode* child)
{
	ENSURE(child != nullptr);
	unsigned int childIndex = GetCount();
	child->SetChildIndex(childIndex);
	mChildrenNodes.push_back(child);
	mCacheNodesChanged = true;
}
void SceneNodeContainer::Remove(::SceneNode* child)
{
	if (mRemovedNodes.end() == std::find(mRemovedNodes.begin(), mRemovedNodes.end(), child))
	{
		auto itEnd = mChildrenNodes.end();
		auto itFound = std::find(mChildrenNodes.begin(), itEnd, child);
		if (itFound != itEnd)
		{
			//update next child indices.
			unsigned int index = child->GetChildIndex();
			itFound = mChildrenNodes.erase(itFound);
			itEnd = mChildrenNodes.end();
			for (; itFound != itEnd; itFound++)
			{
				(*itFound)->SetChildIndex(index++);
			}

			//add to delay remove lists
			mRemovedNodes.push_back(child);
			mCacheNodesChanged = true;
		}
	}
}

bool SceneNodeContainer::Move(unsigned int from, unsigned int to)
{
	ENSURE(to < GetCount() && from < GetCount());
	if (from == to)
		return false;

	auto& siblings = mChildrenNodes;
	::SceneNode* nodeFrom = siblings[from];
	::SceneNode* nodeTo = siblings[to];
	unsigned int formOrder = nodeFrom->GetRenderingOrder();
	unsigned int toOrder = nodeTo->GetRenderingOrder();

	// for actual dirty flag: RenderingOrderDirtyNode
	nodeFrom->SetRenderingOrderOnly(toOrder);
	nodeTo->SetRenderingOrderOnly(formOrder);

	if (from > to)
	{
		for (unsigned int index = to; index < from; index++)
		{
			siblings[index + 1] = siblings[index];
			siblings[index + 1]->SetChildIndex(index + 1);
		}
	}
	else
	{
		for (unsigned int index = from; index < to; index++)
		{
			siblings[index] = siblings[index + 1];
			siblings[index]->SetChildIndex(index);
		}
	}
	siblings[to] = nodeFrom;
	nodeFrom->SetChildIndex(to);
	return true;
}


void SceneNodeContainer::OnMessage(const g2d::Message & message)
{
	/**
	*	Cache must not be process before dispatching
	*	or it may cause potential recursive dispatching
	*/
	for (::SceneNode* pChild : mChildrenNodesForTraversal)
	{
		pChild->OnMessage(message);
	}
	CacheChildrenForTraversal();
}

void SceneNodeContainer::OnUpdate(unsigned int deltaTime)
{
	/**
	*	Cache must not be process before dispatching
	*	or it may cause potential recursive dispatching
	*/
	for (::SceneNode* pChild : mChildrenNodesForTraversal)
	{
		pChild->OnUpdate(deltaTime);
	}
	CacheChildrenForTraversal();
}

void SceneNodeContainer::OnKeyPress(g2d::KeyCode key)
{
	/**
	*	Cache must not be process before dispatching
	*	or it may cause potential recursive dispatching
	*/
	for (::SceneNode* pChild : mChildrenNodesForTraversal)
	{
		pChild->OnKeyPress(key);
	}
	CacheChildrenForTraversal();
}

void SceneNodeContainer::OnKeyPressingBegin(g2d::KeyCode key)
{
	/**
	*	Cache must not be process before dispatching
	*	or it may cause potential recursive dispatching
	*/
	for (::SceneNode* pChild : mChildrenNodesForTraversal)
	{
		pChild->OnKeyPressingBegin(key);
	}
	CacheChildrenForTraversal();
}

void SceneNodeContainer::OnKeyPressing(g2d::KeyCode key)
{
	/**
	*	Cache must not be process before dispatching
	*	or it may cause potential recursive dispatching
	*/
	for (::SceneNode* pChild : mChildrenNodesForTraversal)
	{
		pChild->OnKeyPressing(key);
	}
	CacheChildrenForTraversal();
}

void SceneNodeContainer::OnKeyPressingEnd(g2d::KeyCode key)
{
	/**
	*	Cache must not be process before dispatching
	*	or it may cause potential recursive dispatching
	*/
	for (::SceneNode* pChild : mChildrenNodesForTraversal)
	{
		pChild->OnKeyPressingEnd(key);
	}
	CacheChildrenForTraversal();
}


void SceneNodeContainer::DestroyRemovedNodes()
{
	for (::SceneNode* pNode : mRemovedNodes)
	{
		delete pNode;
	}
	mRemovedNodes.clear();
}


void SceneNodeContainer::CacheChildrenForTraversal()
{
	if (mCacheNodesChanged)
	{
		DestroyRemovedNodes();
		mChildrenNodesForTraversal.clear();
		for (::SceneNode* pAcitveNode : mChildrenNodes)
		{
			mChildrenNodesForTraversal.push_back(pAcitveNode);
		}
		mCacheNodesChanged = false;
	}
}