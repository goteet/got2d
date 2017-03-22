#include "spatial_graph.h"
#include "../include/g2dscene.h"
#include "entity.h"
#include <algorithm>

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
			dirNode = nullptr;
		}
	}
}

inline bool Contains(const gml::vec2& center, float gridSize, const gml::aabb2d& nodeAABB)
{
	gml::aabb2d bounding(
		gml::vec2(center.x - gridSize, center.y - gridSize),
		gml::vec2(center.x + gridSize, center.y + gridSize)
	);

	return bounding.is_intersect(nodeAABB) == gml::it_mode::contain;
}

QuadTreeNode* QuadTreeNode::AddRecursive(const gml::aabb2d& entityBound, g2d::Entity& entity)
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

		if (bounding.is_intersect(entityBound) == gml::it_mode::contain)
		{
			if (m_directionNodes[Direction::LeftTop] == nullptr)
			{
				m_directionNodes[Direction::LeftTop] = new QuadTreeNode(this, center, halfExtend);
			}
			return m_directionNodes[Direction::LeftTop]->AddRecursive(entityBound, entity);
		}

		//x-neg, y-neg
		bounding.move({ 0, -extend });
		if (bounding.is_intersect(entityBound) == gml::it_mode::contain)
		{
			if (m_directionNodes[Direction::LeftDown] == nullptr)
			{
				m_directionNodes[Direction::LeftDown] = new QuadTreeNode(this, center, halfExtend);
			}
			return  m_directionNodes[Direction::LeftDown]->AddRecursive(entityBound, entity);
		}

		//x-pos,y-pos
		bounding.move({ extend, extend });
		if (bounding.is_intersect(entityBound) == gml::it_mode::contain)
		{
			if (m_directionNodes[Direction::RightTop] == nullptr)
			{
				m_directionNodes[Direction::RightTop] = new QuadTreeNode(this, center, halfExtend);
			}
			return m_directionNodes[Direction::RightTop]->AddRecursive(entityBound, entity);
		}

		//x-pos, y-neg
		bounding.move({ 0, -extend });
		if (bounding.is_intersect(entityBound) == gml::it_mode::contain)
		{
			if (m_directionNodes[Direction::RightDown] == nullptr)
			{
				m_directionNodes[Direction::RightDown] = new QuadTreeNode(this, center, halfExtend);
			}
			return m_directionNodes[Direction::RightDown]->AddRecursive(entityBound, entity);
		}

		return AddToDynamicList(entity);
	}
	else
	{
		return AddToDynamicList(entity);
	}
}

QuadTreeNode* QuadTreeNode::AddToDynamicList(g2d::Entity& entity)
{
	m_isEmpty = false;
	m_dynamicEntities.push_back(&entity);
	return this;
}

void QuadTreeNode::TryMarkEmpty()
{
	if (m_dynamicEntities.size() > 0)
	{
		return;
	}

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
void QuadTreeNode::Remove(g2d::Entity& entity)
{
	auto newEnd = std::remove(
		std::begin(m_dynamicEntities),
		std::end(m_dynamicEntities),
		&entity);

	m_dynamicEntities.erase(newEnd, std::end(m_dynamicEntities));

	TryMarkEmpty();
}

void QuadTreeNode::FindVisible(Camera& camera)
{
	if (IsEmpty())
		return;

	for (auto& entity : m_dynamicEntities)
	{
		if (entity->GetSceneNode()->IsVisible() && camera.TestVisible(*entity))
		{
			camera.visibleEntities.push_back(entity);
		}
	}

	for (auto& child : m_directionNodes)
	{
		if (child != nullptr && camera.TestVisible(child->GetBounding()))
		{
			child->FindVisible(camera);
		}
	}
}

SpatialGraph::SpatialGraph(float boundSize)
{
	m_root = new QuadTreeNode(nullptr, gml::vec2::zero(), boundSize);
}

void SpatialGraph::Add(g2d::Entity& entity)
{
	Remove(entity);

	QuadTreeNode* node = nullptr;
	if (entity.GetSceneNode()->IsStatic())
	{
		gml::aabb2d nodeAABB = entity.GetWorldAABB();
		node = m_root->AddRecursive(nodeAABB, entity);
	}
	else
	{
		//m_root->m_isEmpty = false;
		node = m_root->AddToDynamicList(entity);
	}
	m_linkRef[&entity] = node;
}

void SpatialGraph::Remove(g2d::Entity& entity)
{
	if (m_linkRef.count(&entity))
	{
		auto& node = m_linkRef[&entity];
		node->Remove(entity);
		m_linkRef.erase(&entity);
	}
}

void SpatialGraph::FindVisible(Camera& camera)
{
	m_root->FindVisible(camera);
}