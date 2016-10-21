#pragma once
#include <vector>
#include "g2dmessage.h"

class Scene;
class SceneNode;

class SceneNodeContainer
{
public:
	SceneNodeContainer() = default;

	~SceneNodeContainer();

	void DestroyAllChildren();

	SceneNode* First() const;

	SceneNode* Last() const;

	SceneNode* At(unsigned int index) const;

	unsigned int GetCount() const;

	void Add(::SceneNode* child);

	void Remove(::SceneNode* child);

	bool Move(unsigned int from, unsigned int to);

	template<typename TVisitor> void Traversal(TVisitor func)
	{
		for (auto& child : mChildrenNodes)
		{
			func(child);
		}
	}

	template<typename TVisitor> void InverseTraversal(TVisitor func)
	{
		auto cur = mChildrenNodes.rbegin();
		auto end = mChildrenNodes.rend();
		while (cur != end)
		{
			func(*it);
			++cur;
		}
	}

public:
	void OnMessage(const g2d::Message& message);

	void OnUpdate(unsigned int deltaTime);

	void OnKeyPress(g2d::KeyCode key);

	void OnKeyPressingBegin(g2d::KeyCode key);

	void OnKeyPressing(g2d::KeyCode key);

	void OnKeyPressingEnd(g2d::KeyCode key);

private:
	void CacheChildrenForTraversal();

	void DestroyRemovedNodes();

	std::vector<::SceneNode*> mChildrenNodes;
	std::vector<::SceneNode*> mRemovedNodes;
	std::vector<::SceneNode*> mChildrenNodesForTraversal;
	bool mCacheNodesChanged = true;
};