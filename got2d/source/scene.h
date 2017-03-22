#pragma once
#include "../include/g2dscene.h"
#include "entity.h"
#include "spatial_graph.h"
#include "input.h"
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

	void _Update(uint32_t deltaTime);

	::SceneNode* GetChildByIndex(uint32_t index) const;

	uint32_t GetChildCount() const { return static_cast<uint32_t>(m_children.size()); }

	void MoveChild(uint32_t from, uint32_t to);

	void Remove(::SceneNode* child);

	void EmptyChildren();

	template<typename FUNC>
	void TraversalChildrenByIndex(uint32_t startIndex, FUNC func)
	{
		for (uint32_t size = static_cast<uint32_t>(m_children.size()); startIndex < size; startIndex++)
			func(startIndex, m_children[startIndex]);
	}

	template<typename FUNC>
	void TraversalChildren(FUNC func)
	{
		for (auto& child : m_children) func(child);
	}

	template<typename FUNC>
	void InverseTraversalChildren(FUNC func)
	{
		auto it = std::rbegin(m_children);
		auto end = std::rend(m_children);
		for (; it != end; it++) func(*child);
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

	void Update(uint32_t deltaTime);

	void SetChildIndex(uint32_t index) { m_childID = index; }

	void SetRenderingOrder(uint32_t& index);

	void OnMessage(const g2d::Message& message);

	void OnMouseEnterFrom(::SceneNode* adjacency, const gml::coord& cursorPos);

	void OnMouseLeaveTo(::SceneNode* adjacency, const gml::coord& cursorPos);

	void OnClick(g2d::MouseButton button, const gml::coord& cursorPos);

	void OnDoubleClick(g2d::MouseButton button, const gml::coord& cursorPos);

	void OnDragBegin(g2d::MouseButton button, const gml::coord& cursorPos);

	void OnDragging(g2d::MouseButton button, const gml::coord& cursorPos);

	void OnDragEnd(g2d::MouseButton button, const gml::coord& cursorPos);

	void OnDropping(::SceneNode* dropped, g2d::MouseButton button, const gml::coord& cursorPos);

	void OnDropTo(::SceneNode* dropped, g2d::MouseButton button, const gml::coord& cursorPos);

	void OnKeyPressing(g2d::KeyCode key, g2d::Keyboard& keyboard);

	void OnKeyPress(g2d::KeyCode key, g2d::Keyboard& keyboard);

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

	virtual gml::vec2 GetWorldPosition() override;

	virtual g2d::Entity* GetEntity() const override { return m_entity; }

	virtual bool IsVisible() const override { return _IsVisible(); }

	virtual bool IsStatic() const override { return m_isStatic; }

	virtual uint32_t GetVisibleMask() const override { return _GetVisibleMask(); }

	virtual gml::vec2 WorldToLocal(const gml::vec2& pos) override;

	virtual gml::vec2 WorldToParent(const gml::vec2& pos) override;

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

	void Update(uint32_t elapsedTime, uint32_t deltaTime);

	void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);

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

	virtual gml::vec2 GetWorldPosition() override { return _GetPosition(); };

	virtual g2d::Entity* GetEntity() const override { return nullptr; }

	virtual bool IsVisible() const override { return _IsVisible(); }

	virtual bool IsStatic() const override { return true; }

	virtual uint32_t GetVisibleMask() const override { return _GetVisibleMask(); }

	virtual gml::vec2 WorldToLocal(const gml::vec2& pos) override { return pos; }

	virtual gml::vec2 WorldToParent(const gml::vec2& pos) override { return pos; }

public:	//g2d::Scene
	virtual void Release() override;

	virtual g2d::Camera* CreateCameraNode() override;

	virtual g2d::Camera* GetMainCamera() const override { return GetCameraByIndex(0); }

	virtual g2d::Camera* GetCameraByIndex(uint32_t) const override;

	virtual void Render() override;

private:	//BaseNode
	virtual ::BaseNode* _GetParent() override { return nullptr; }

	virtual void AdjustRenderingOrder() override;

private:
	void ResortCameraOrder();

	::SceneNode* FindInteractiveObject(const g2d::Message& message);

	void RegisterKeyboardListener();

	void UnRegisterKeyboardListener();

	void OnKeyPress(g2d::KeyCode key);

	void OnKeyPressing(g2d::KeyCode key);

	KeyEventReceiver m_pressReceiver;
	KeyEventReceiver m_pressingReceiver;

	SpatialGraph m_spatial;
	std::vector<::Camera*> m_cameras;
	std::vector<::Camera*> m_cameraOrder;
	bool m_cameraOrderDirty = true;

	class MouseButtonState
	{
	public:
		const g2d::MouseButton button;

		MouseButtonState(g2d::MouseButton btn) : button(btn) { }
		void Update(uint32_t currentStamp);
		bool OnMessage(const g2d::Message& message, uint32_t currentStamp, ::SceneNode* hitNode);
		void ForceRelease();
	private:
		void OnDoubleClick(const g2d::Message& message);
		void OnMouseDown(const g2d::Message& message, uint32_t currentStamp);
		bool OnMouseUp(const g2d::Message& message);
		bool OnMouseMove(const g2d::Message& message);

		bool isDragging = false;
		bool isPressing = false;
		uint32_t pressTimeStamp;
		gml::coord pressCursorPos;
		::SceneNode* nodeDragging = nullptr;
		::SceneNode* nodeHovering = nullptr;
	} m_mouseButtonState[3];
	::SceneNode* m_hoverNode = nullptr;
};
