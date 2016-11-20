#include "spatial_graph.h"
#include "../include/g2dscene.h"
#include <assert.h>

QuadTreeNode::QuadTreeNode(QuadTreeNode* parent, const gml::vec2& center, float gridSize)
	: m_bounding(
		gml::vec2(center.x - gridSize, center.y - gridSize),
		gml::vec2(center.x + gridSize, center.y + gridSize))
	, m_kCanBranch(gridSize > MIN_SIZE)
	, m_parent(parent)
{
	for (auto& dirNode : m_directionNodes)
	{
		dirNode = nullptr;
	}
}
QuadTreeNode::~QuadTreeNode()
{
	for (auto& dirNode : m_directionNodes)
	{
		if (dirNode)
		{
			delete dirNode;
		}
	}
}

inline bool Contains(const gml::vec2& center, float gridSize, const gml::aabb2d& nodeAABB)
{
	gml::aabb2d bounding(
		gml::vec2(center.x - gridSize, center.y - gridSize),
		gml::vec2(center.x + gridSize, center.y + gridSize)
		);

	return bounding.is_intersect(nodeAABB) == gml::it_contain;
}

QuadTreeNode* QuadTreeNode::AddRecursive(const gml::aabb2d& entityBound, g2d::Entity* entity)
{
	m_isEmpty = false;
	if (m_kCanBranch)
	{
		//try to push child node
		//push to self if failed.

		//WE HERE NEED aabb::move() !
		float extend = m_bounding.extend().x;
		float halfExtend = extend * 0.5f;
		auto& center = m_bounding.center();
		//x-neg, y-pos
		gml::aabb2d bounding(
			gml::vec2(center.x - extend, center.y + extend),
			m_bounding.center());

		if (bounding.is_intersect(entityBound) == gml::it_contain)
		{
			if (m_directionNodes[DIR_LT] == nullptr)
			{
				m_directionNodes[DIR_LT] = new QuadTreeNode(this, center, halfExtend);
			}
			return m_directionNodes[DIR_LT]->AddRecursive(entityBound, entity);
		}

		//x-neg, y-neg
		bounding.move({ 0, -extend });
		if (bounding.is_intersect(entityBound) == gml::it_contain)
		{
			if (m_directionNodes[DIR_LD] == nullptr)
			{
				m_directionNodes[DIR_LD] = new QuadTreeNode(this, center, halfExtend);
			}
			return  m_directionNodes[DIR_LD]->AddRecursive(entityBound, entity);
		}

		//x-pos,y-pos
		bounding.move({ extend, extend });
		if (bounding.is_intersect(entityBound) == gml::it_contain)
		{
			if (m_directionNodes[DIR_RT] == nullptr)
			{
				m_directionNodes[DIR_RT] = new QuadTreeNode(this, center, halfExtend);
			}
			return m_directionNodes[DIR_RT]->AddRecursive(entityBound, entity);
		}

		//x-pos, y-neg
		bounding.move({ 0, -extend });
		if (bounding.is_intersect(entityBound) == gml::it_contain)
		{
			if (m_directionNodes[DIR_RD] == nullptr)
			{
				m_directionNodes[DIR_RD] = new QuadTreeNode(this, center, halfExtend);
			}
			return m_directionNodes[DIR_RD]->AddRecursive(entityBound, entity);
		}

		return AddToDynamicList(entity);
	}
	else
	{
		return AddToDynamicList(entity);
	}
}

QuadTreeNode* QuadTreeNode::AddToDynamicList(g2d::Entity* entity)
{
	m_dynamicEntities.push_back(entity);
	return this;
}
void QuadTreeNode::TryMarkEmpty()
{
	if (m_dynamicEntities.size() > 0)
		return;

	bool hasEntities = false;
	for (auto child : m_directionNodes)
	{
		if (child && !child->IsEmpty())
		{
			hasEntities = true;
			break;
		}
	}

	if (!hasEntities)
	{
		m_isEmpty = true;
		if (m_parent)
		{
			m_parent->TryMarkEmpty();
		}
	}
}
void QuadTreeNode::Remove(g2d::Entity* entity)
{
	for (auto it = m_dynamicEntities.begin(), end = m_dynamicEntities.end(); it != end; it++)
	{
		if (*it == entity)
		{
			*it = m_dynamicEntities.back();
			m_dynamicEntities.pop_back();
			break;
		}
	}

	TryMarkEmpty();
}

void QuadTreeNode::FindVisible(g2d::Camera* camera, std::vector<g2d::Entity*>& outVisibleList)
{
	if (IsEmpty())
		return;

	for (auto& entity : m_dynamicEntities)
	{
		if (entity->GetSceneNode()->IsVisible() && camera->TestVisible(entity))
		{
			outVisibleList.push_back(entity);
		}
	}

	for (auto& child : m_directionNodes)
	{
		if (child != nullptr && camera->TestVisible(child->GetBounding()))
		{
			child->FindVisible(camera, outVisibleList);
		}
	}
}

SpatialGraph::SpatialGraph(float boundSize)
{
	m_root = new QuadTreeNode(nullptr, gml::vec2::zero(), boundSize);
}

SpatialGraph::~SpatialGraph()
{
	delete m_root;
}

void SpatialGraph::Add(g2d::Entity* entity)
{
	if (entity == nullptr)
		return;

	Remove(entity);

	QuadTreeNode* node = nullptr;
	if (entity->GetSceneNode()->IsStatic())
	{
		gml::aabb2d nodeAABB = entity->GetWorldAABB();
		node = m_root->AddRecursive(nodeAABB, entity);
	}
	else
	{
		node = m_root->AddToDynamicList(entity);
	}
	m_linkRef[entity] = node;
}

void SpatialGraph::Remove(g2d::Entity* entity)
{
	if (entity && m_linkRef.count(entity))
	{
		auto& node = m_linkRef[entity];
		node->Remove(entity);
		m_linkRef.erase(entity);
	}
}

void SpatialGraph::FindVisible(g2d::Camera* camera, std::vector<g2d::Entity*>& outVisibleList)
{
	m_root->FindVisible(camera, outVisibleList);
}