#pragma once
#include <vector>
#include <map>
#include <gmlaabb.h>

namespace g2d
{
	class Entity;
	class Camera;
}
class QuadTreeNode
{
public:
	constexpr static float MIN_SIZE = 100.0f;

	QuadTreeNode(QuadTreeNode* parent, const gml::vec2& center, float gridSize);
	~QuadTreeNode();

	QuadTreeNode* AddRecursive(const gml::aabb2d& entityBound, g2d::Entity* entity);
	QuadTreeNode* AddToDynamicList(g2d::Entity* entity);
	void Remove(g2d::Entity* entity);
	inline gml::aabb2d GetBounding() { return m_bounding; }
	void FindVisible(g2d::Camera* camera, std::vector<g2d::Entity*>& visibleEntities);
	inline bool IsEmpty() { return m_isEmpty; }

private:
	void TryMarkEmpty();

	enum Direction
	{
		DIR_LT = 0,		
		DIR_LD = 1,
		DIR_RT = 2,		
		DIR_RD = 3,
		NUM_DIR = 4,
	};
	const bool m_kCanBranch = true;
	bool m_isEmpty = true;
	QuadTreeNode* m_parent;
	QuadTreeNode* m_directionNodes[NUM_DIR];
	std::vector<g2d::Entity*> m_dynamicEntities;
	gml::aabb2d m_bounding;
};

class SpatialGraph
{
public:
	SpatialGraph(float boundSize);
	~SpatialGraph();
	void Add(g2d::Entity* entity);
	void Remove(g2d::Entity* entity);
	void FindVisible(g2d::Camera* camera, std::vector<g2d::Entity*>& visibleEntities);

private:
	QuadTreeNode* m_root;
	std::map<g2d::Entity*, QuadTreeNode*> m_linkRef;
};