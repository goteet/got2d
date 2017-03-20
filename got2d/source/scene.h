#pragma once
#include "../include/g2dscene.h"
#include "entity.h"
#include "spatial_graph.h"
#include <gmlmatrix.h>
#include <vector>

class SpatialGraph;
class SceneNode;
class Scene;

class BaseNode
{
	friend class SceneNode;
	friend class Scene;
protected:
	BaseNode();

	~BaseNode();

	g2d::SceneNode* _CreateSceneNodeChild(::Scene& scene, ::SceneNode& parent, g2d::Entity& e, bool autoRelease);

	g2d::SceneNode* _CreateSceneNodeChild(::Scene& scene, g2d::Entity& e, bool autoRelease);

	const gml::mat32& _GetLocalMatrix();

	void _SetVisibleMask(uint32_t mask, bool recursive);

	const gml::vec2& _GetPosition() const { return m_position; }

	const gml::vec2& _GetPivot() const { return m_pivot; }

	const gml::vec2& _GetScale() const { return m_scale; }

	gml::radian _GetRotation() const { return m_rotation; }

	uint32_t _GetVisibleMask() const { return m_visibleMask; }

	bool _IsVisible() const { return m_isVisible; }

	void _SetPosition(const gml::vec2& position);

	void _SetPivot(const gml::vec2& pivot);

	void _SetScale(const gml::vec2& scale);

	void _SetRotation(gml::radian r);

	void _SetVisible(bool visible) { m_isVisible = visible; }

	void _Update(uint32_t elpasedTime);

	::SceneNode* GetChildByIndex(uint32_t index) const;

	uint32_t GetChildCount() const { return static_cast<uint32_t>(m_children.size()); }

	void MoveChild(uint32_t from, uint32_t to);

	void Remove(::SceneNode* child);

	void EmptyChildren();

	template<typename FUNC>
	void TraversalChildrenByIndex(uint32_t startIndex, FUNC func)
	{
		for(uint32_t size = static_cast<uint32_t>(m_children.size()); startIndex < size; startIndex++)
			func(startIndex, m_children[startIndex]);
	}

	template<typename FUNC>
	void TraversalChildren(FUNC func)
	{
		for (auto& child : m_children) func(child);
	}

	virtual BaseNode* _GetParent() = 0;

private:
	void OnCreateChild(::Scene&, ::SceneNode&);

	void SetLocalMatrixDirty();

	void RemoveReleasedChildren();

	virtual void AdjustRenderingOrder() = 0;

	gml::vec2 m_position;
	gml::vec2 m_pivot;
	gml::vec2 m_scale;
	gml::radian m_rotation;
	gml::mat32 m_matrixLocal;
	bool m_matrixLocalDirty = true;
	bool m_isVisible = true;
	uint32_t m_visibleMask = g2d::DEFAULT_VISIBLE_MASK;

	std::vector<::SceneNode*> m_children;
	std::vector<::SceneNode*> m_pendingReleased;
};

class SceneNode : public g2d::SceneNode, public BaseNode
{
	RTTI_IMPL;
public:
	SceneNode(::Scene& scene, ::SceneNode& parent, uint32_t childID, g2d::Entity* entity, bool autoRelease);

	SceneNode(::Scene& parent, uint32_t childID, g2d::Entity* entity, bool autoRelease);

	~SceneNode();

	void Update(uint32_t elpasedTime);

	void SetChildIndex(uint32_t index) { m_childID = index; }

	void SetRenderingOrder(uint32_t& index);
	
public:	//g2d::SceneNode
	virtual g2d::Scene* GetScene() const override;

	virtual g2d::SceneNode* GetParentNode() const override { return &m_iparent; }

	virtual g2d::SceneNode* GetPrevSiblingNode() const override { return GetPrevSibling(); }

	virtual g2d::SceneNode* GetNextSiblingNode() const override { return GetNextSibling(); }

	virtual g2d::SceneNode* CreateSceneNodeChild(g2d::Entity* entity, bool autoRelease) override { return _CreateSceneNodeChild(m_scene, *this, *entity, autoRelease); }

	virtual void RemoveFromParent() override { m_bparent.Remove(this); }

	virtual void MoveToFront() override;

	virtual void MoveToBack() override;

	virtual void MovePrev() override;

	virtual void MoveNext() override;

	virtual const gml::mat32& GetLocalMatrix() override { return _GetLocalMatrix(); }

	virtual const gml::mat32& GetWorldMatrix() override;

	virtual g2d::SceneNode* SetPosition(const gml::vec2& position) override;

	virtual g2d::SceneNode* SetPivot(const gml::vec2& pivot) override;

	virtual g2d::SceneNode* SetScale(const gml::vec2& scale) override;

	virtual g2d::SceneNode* SetRotation(gml::radian r) override;

	virtual void SetVisible(bool visible) override { _SetVisible(visible); }

	virtual void SetStatic(bool s) override;

	virtual void SetVisibleMask(uint32_t mask, bool recursive) override { _SetVisibleMask(mask, recursive); }

	virtual const gml::vec2& GetPosition()  const override { return _GetPosition(); }

	virtual const gml::vec2& GetPivot() const override { return _GetPivot(); }

	virtual const gml::vec2& GetScale() const override { return _GetScale(); }

	virtual gml::radian GetRotation() const override { return _GetRotation(); }

	virtual g2d::Entity* GetEntity() const override { return m_entity; }

	virtual bool IsVisible() const override { return _IsVisible(); }

	virtual bool IsStatic() const override { return m_isStatic; }

	virtual uint32_t GetVisibleMask() const override { return _GetVisibleMask(); }

private:
	void SetWorldMatrixDirty();

	void AdjustSpatial();

	::SceneNode* GetPrevSibling() const;

	::SceneNode* GetNextSibling() const;

	virtual void AdjustRenderingOrder() override;

	virtual ::BaseNode* _GetParent() override { return &m_bparent; }

	::Scene& m_scene;
	::BaseNode& m_bparent;
	g2d::SceneNode& m_iparent;
	g2d::Entity* m_entity = nullptr;
	bool m_autoRelease = false;
	uint32_t m_childID = 0;
	uint32_t m_baseRenderingOrder = 0;
	bool m_isStatic = false;
	bool m_matrixDirtyUpdate = true;
	bool m_matrixWorldDirty = true;
	gml::mat32 m_matrixWorld;
};

class Scene : public g2d::Scene, public BaseNode
{
	RTTI_IMPL;
public:
	Scene(float boundSize);

	void SetCameraOrderDirty() { m_cameraOrderDirty = true; }

	SpatialGraph* GetSpatialGraph() { return &m_spatial; }

public: //g2d::SceneNode
	virtual g2d::Scene* GetScene() const override { return const_cast<::Scene*>(this); }

	virtual SceneNode* GetParentNode() const override { return nullptr; }

	virtual SceneNode* GetPrevSiblingNode() const override { return nullptr; }

	virtual SceneNode* GetNextSiblingNode() const override { return nullptr; }

	virtual g2d::SceneNode* CreateSceneNodeChild(g2d::Entity* entity, bool autoRelease) override { return _CreateSceneNodeChild(*this, *entity, autoRelease); }

	virtual void RemoveFromParent() override { }

	virtual void MoveToFront() override { }

	virtual void MoveToBack() override { }

	virtual void MovePrev() override { }

	virtual void MoveNext() override { }

	virtual const gml::mat32& GetLocalMatrix() override { return _GetLocalMatrix(); }

	virtual const gml::mat32& GetWorldMatrix() override { return _GetLocalMatrix(); }

	virtual void SetVisibleMask(uint32_t mask, bool recursive) override { _SetVisibleMask(mask, recursive); }

	virtual g2d::SceneNode* SetPosition(const gml::vec2& position) override { _SetPosition(position); return this; }

	virtual g2d::SceneNode* SetPivot(const gml::vec2& pivot) override { _SetPivot(pivot); return this; }

	virtual g2d::SceneNode* SetScale(const gml::vec2& scale) override { _SetScale(scale); return this; }

	virtual g2d::SceneNode* SetRotation(gml::radian r) override { _SetRotation(r); return this; }

	virtual void SetVisible(bool visible) override { _SetVisible(visible); }

	virtual void SetStatic(bool visible) override { }

	virtual const gml::vec2& GetPosition()  const override { return _GetPosition(); }

	virtual const gml::vec2& GetPivot() const override { return _GetPivot(); }

	virtual const gml::vec2& GetScale() const override { return _GetScale(); }

	virtual gml::radian GetRotation() const override { return _GetRotation(); }

	virtual g2d::Entity* GetEntity() const override { return nullptr; }

	virtual bool IsVisible() const override { return _IsVisible(); }

	virtual bool IsStatic() const override { return true; }

	virtual uint32_t GetVisibleMask() const override { return _GetVisibleMask(); }

public:	//g2d::Scene
	virtual void Release() override;

	virtual g2d::Camera* CreateCameraNode() override;

	virtual g2d::Camera* GetMainCamera() const override { return GetCameraByIndex(0); }

	virtual g2d::Camera* GetCameraByIndex(uint32_t) const override;

	virtual void Render() override;

	virtual void Update(uint32_t elpasedTime) override;

	virtual void OnMessage(const g2d::Message& message) override;

private:
	void ResortCameraOrder();

	virtual ::BaseNode* _GetParent() override { return nullptr; }

	virtual void AdjustRenderingOrder() override;

	SpatialGraph m_spatial;
	std::vector<::Camera*> m_cameras;
	std::vector<::Camera*> m_cameraOrder;
	bool m_cameraOrderDirty;
};
