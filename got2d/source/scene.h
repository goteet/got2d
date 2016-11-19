#pragma once
#include "../include/g2dscene.h"
#include "entity.h"
#include "spatial_graph.h"
#include <gmlmatrix.h>
#include <vector>

class SpatialGraph;
class Scene;

class SceneNode : public g2d::SceneNode
{
public:
	SceneNode(::Scene* scene, SceneNode* parent, int childID, g2d::Entity* entity, bool autoRelease);
	virtual ~SceneNode();
	void Update(unsigned int elpasedTime);
	void Render(g2d::Camera* camera);
	void RenderSingle(g2d::Camera* camera);
	void AdjustRenderingOrder();

public:
	virtual g2d::Scene* GetScene() const override;
	virtual g2d::SceneNode* GetParentNode() override;
	virtual g2d::SceneNode* GetPrevSiblingNode() override;
	virtual g2d::SceneNode* GetNextSiblingNode() override;
	virtual g2d::SceneNode* CreateSceneNode(g2d::Entity* entity, bool autoRelease)override;
	virtual const gml::mat32& GetLocalMatrix() override;
	virtual const gml::mat32& GetWorldMatrix() override;
	virtual void SetVisibleMask(unsigned int mask, bool recursive) override;
	virtual g2d::SceneNode* SetPosition(const gml::vec2& position) override;
	virtual g2d::SceneNode* SetPivot(const gml::vec2& pivot) override;
	virtual g2d::SceneNode* SetScale(const gml::vec2& scale) override;
	virtual g2d::SceneNode* SetRotation(gml::radian r) override;
	virtual void MovePrevToFront() override;
	virtual void MovePrevToBack() override;
	virtual void MovePrev() override;
	virtual void MoveNext() override;
	inline virtual void SetVisible(bool visible) override { m_isVisible = visible; }
	virtual void SetStatic(bool s) override;
	inline virtual const gml::vec2& GetPosition()  const override { return m_position; }
	inline virtual const gml::vec2& GetPivot() const override { return m_pivot; }
	inline virtual const gml::vec2& GetScale() const override { return m_scale; }
	inline virtual gml::radian GetRotation() const override { return m_rotation; }
	inline virtual g2d::Entity* GetEntity() const override { return m_entity; }
	inline virtual bool IsVisible() const override { return m_isVisible; }
	inline virtual bool IsStatic() const override { return m_isStatic; }
	inline virtual unsigned int GetVisibleMask() const override { return m_visibleMask; }

private:
	void SetLocalMatrixDirty();
	void SetWorldMatrixDirty();
	void AdjustSpatial();
	void SetRenderingOrder(int& index);
	void MoveSelfTo(int to);
	::SceneNode* GetPrevSibling();
	::SceneNode* GetNextSibling();

	::Scene* m_scene = nullptr;
	::SceneNode* m_parent = nullptr;
	g2d::Entity* m_entity = nullptr;
	int m_childID = 0;
	int m_baseRenderingOrder = 0;

	unsigned int m_visibleMask = g2d::DEFAULT_VISIBLE_MASK;
	bool m_autoRelease = false;
	bool m_isVisible = true;
	bool m_isStatic = false;

	gml::vec2 m_position;
	gml::vec2 m_pivot;
	gml::vec2 m_scale;
	gml::radian m_rotation;

	bool m_matrixDirtyUpdate = false;
	bool m_matrixLocalDirty = false;
	bool m_matrixWorldDirty = true;
	gml::mat32 m_matrixLocal;
	gml::mat32 m_matrixWorld;
	std::vector<::SceneNode*> m_children;
};

class Scene : public g2d::Scene
{
public:
	Scene();
	~Scene();

	inline ::SceneNode* GetRoot() { return m_root; }
	inline void Update(unsigned int elpasedTime) { return m_root->Update(elpasedTime); }
	void Render();
	void SetCameraOrderDirty();
	SpatialGraph* GetSpatialGraph() { return &m_spatial; }

public:
	inline virtual g2d::Scene* GetScene() const override { return m_root->GetScene(); }
	inline virtual SceneNode* GetParentNode() override { return m_root->GetParentNode(); }
	inline virtual SceneNode* GetPrevSiblingNode() override { return m_root->GetPrevSiblingNode(); }
	inline virtual SceneNode* GetNextSiblingNode() override { return m_root->GetNextSiblingNode(); }
	virtual g2d::SceneNode* CreateSceneNode(g2d::Entity* e, bool autoRelease) override;
	inline virtual const gml::mat32& GetLocalMatrix() override { return m_root->GetLocalMatrix(); }
	inline virtual const gml::mat32& GetWorldMatrix() override { return m_root->GetWorldMatrix(); }
	inline virtual void SetVisibleMask(unsigned int mask, bool recursive) override { m_root->SetVisibleMask(mask, recursive); }
	inline virtual g2d::SceneNode* SetPosition(const gml::vec2& position) override { m_root->SetPosition(position); return this; }
	inline virtual g2d::SceneNode* SetPivot(const gml::vec2& pivot) override { m_root->SetPivot(pivot); return this; }
	inline virtual g2d::SceneNode* SetScale(const gml::vec2& scale) override { m_root->SetScale(scale); return this; }
	inline virtual g2d::SceneNode* SetRotation(gml::radian r) override { m_root->SetRotation(r); return this; }
	virtual void MovePrevToFront() override { m_root->MovePrevToFront(); }
	virtual void MovePrevToBack() override { m_root->MovePrevToBack(); }
	virtual void MovePrev() override { m_root->MovePrev(); }
	virtual void MoveNext() override { m_root->MoveNext(); }
	inline virtual void SetVisible(bool visible) override { m_root->SetVisible(visible); }
	inline virtual void SetStatic(bool visible) override { m_root->SetStatic(visible); }
	inline virtual const gml::vec2& GetPosition()  const override { return m_root->GetPosition(); }
	inline virtual const gml::vec2& GetPivot() const override { return m_root->GetPivot(); }
	inline virtual const gml::vec2& GetScale() const override { return m_root->GetScale(); }
	inline virtual gml::radian GetRotation() const override { return m_root->GetRotation(); }
	inline virtual g2d::Entity* GetEntity() const override { return m_root->GetEntity(); }
	inline virtual bool IsVisible() const override { return m_root->IsVisible(); }
	inline virtual bool IsStatic() const override { return m_root->IsStatic(); }
	inline virtual unsigned int GetVisibleMask() const override { return m_root->GetVisibleMask(); }
public:
	virtual g2d::Camera* CreateCameraNode() override;
	virtual g2d::Camera* GetMainCamera() const override { return GetCamera(0); }
	virtual g2d::Camera* GetCamera(unsigned int index) const override;

private:
	void ResortCameraOrder();

	::SceneNode* m_root;
	SpatialGraph m_spatial;
	std::vector<g2d::Camera*> m_cameras;
	std::vector<g2d::Camera*> m_renderingOrder;
	bool m_cameraOrderDirty;
};
