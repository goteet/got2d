#pragma once
#include "g2dscene.h"
#include "scene_node_container.h"
#include "component_container.h"
#include "transform.h"

class Scene;

class SceneNode : public g2d::SceneNode
{
	RTTI_IMPL;
public:

public:
	virtual g2d::Scene* GetScene() override;

	virtual g2d::SceneNode* GetParentNode() override;

	virtual g2d::SceneNode* GetFirstChild() override;

	virtual g2d::SceneNode* GetPrevSiblingNode() override;

	virtual g2d::SceneNode* GetNextSiblingNode() override;

	virtual g2d::SceneNode* GetLastChild() override;

	virtual g2d::SceneNode* GetChildByIndex(unsigned int index) override;

	virtual unsigned int GetChildCount() const override;

	virtual g2d::SceneNode* CreateChild() override;

	virtual void MoveToFront() override;

	virtual void MoveToBack() override;

	virtual void MovePrev() override;

	virtual void MoveNext() override;

	virtual bool AddComponent(g2d::Component* component, bool autoRelease) override;

	virtual bool RemoveComponent(g2d::Component* component) override;

	virtual bool RemoveComponentWithoutRelease(g2d::Component* component) override;

	virtual bool HasComponent(g2d::Component*) const override;

	virtual bool IsComponentAutoRelease(g2d::Component* component) const override;

	virtual g2d::Component* GetComponentByIndex(unsigned int index) override;

	virtual unsigned int GetComponentCount() const override;

	virtual const cxx::float2x3& GetLocalMatrix() override;

	virtual const cxx::float2x3& GetWorldMatrix() override;

	virtual g2d::SceneNode* SetPosition(const cxx::point2d<float>& Position) override;

	virtual cxx::point2d<float> GetPosition() const override;

	virtual g2d::SceneNode* SetWorldPosition(const cxx::point2d<float>& Position) override;

	virtual cxx::point2d<float> GetWorldPosition() override;

	virtual g2d::SceneNode* SetRightDirection(const cxx::nfloat2& right) override;

	virtual const cxx::nfloat2 GetRightDirection() override;

	virtual g2d::SceneNode* SetUpDirection(const cxx::nfloat2& up) override;

	virtual const cxx::nfloat2 GetUpDirection() override;

	virtual g2d::SceneNode* SetPivot(const cxx::float2& pivot) override;

	virtual const cxx::float2& GetPivot() const override;

	virtual g2d::SceneNode* SetScale(const cxx::float2& scale) override;

	virtual const cxx::float2& GetScale() const override;

	virtual g2d::SceneNode* SetRotation(cxx::radian<float> r) override;

	virtual cxx::radian<float> GetRotation() const override;

	virtual void SetVisible(bool visible) override;

	virtual bool IsVisible() const override;

	virtual void SetStatic(bool s) override;

	virtual bool IsStatic() const override;

	virtual void SetCameraVisibleMask(unsigned int mask, bool recursive) override;

	virtual unsigned int GetCameraVisibleMask() const override;

	virtual unsigned int GetChildIndex() const override;

	virtual bool IsRemoved() const override;

	virtual cxx::point2d<float> WorldToLocal(const cxx::point2d<float>& pos) override;

	virtual cxx::point2d<float> WorldToParent(const cxx::point2d<float>& pos) override;

	virtual void Release() override;

public:
	SceneNode(::Scene* scene, ::SceneNode* parent);

	~SceneNode();

	unsigned int GetRenderingOrder() const;

	::Scene* GetSceneImpl();

	::SceneNode* GetParentNodeImpl();

	::SceneNode* GetPrevSiblingImpl();

	::SceneNode* GetNextSiblingImpl();

	virtual void AdjustRenderingOrder();

	void OnUpdate(unsigned int deltaTime);

	void SetChildIndex(unsigned int index) { mChildIndex = index; }

	void SetRenderingOrder(unsigned int& order);

	// provided to Move function for 
	// temporary setting rendering order
	void SetRenderingOrderOnly(unsigned int order);

	//bool ParentIsRoot() const { return mParentNode == mScene->GetRootNode(); }

	void OnMessage(const g2d::Message& message);

	void OnCursorEnterFrom(::SceneNode* adjacency);

	void OnCursorHovering();

	void OnCursorLeaveTo(::SceneNode* adjacency);

	void OnClick(g2d::MouseButton button);

	void OnDoubleClick(g2d::MouseButton button);

	void OnDragBegin(g2d::MouseButton button);

	void OnDragging(g2d::MouseButton button);

	void OnDragEnd(g2d::MouseButton button);

	void OnDropping(::SceneNode* dropped, g2d::MouseButton button);

	void OnDropTo(::SceneNode* dropped, g2d::MouseButton button);

	void OnKeyPress(g2d::KeyCode key);

	void OnKeyPressingBegin(g2d::KeyCode key);

	void OnKeyPressing(g2d::KeyCode key);

	void OnKeyPressingEnd(g2d::KeyCode key);

private:

	void NotifyChildrenTransformChanged();

	void AdjustSpatial();

private:
	::Scene* mScene;

	::SceneNode* mParentNode = nullptr;

	bool mIsVisible = true;

	bool mIsStatic = false;

	bool mTranformChanged = true;

	bool mIsRemoved = false;

	unsigned int mChildIndex = 0;

	unsigned int mRenderingOrder = 0xFFFFFFFF;	// make sure the order maxinum(error) at the beginning

	unsigned int mCameraVisibleMask = g2d::DefaultCameraVisibkeMask;

	SceneNodeContainer mChildrenNodes;
	ComponentContainer mComponenList;
	Transform mTransform;
};

class RootSceneNode : public SceneNode
{
	virtual g2d::SceneNode* GetParentNode() override { return nullptr; }

	virtual g2d::SceneNode* GetPrevSiblingNode() override { return nullptr; }

	virtual g2d::SceneNode* GetNextSiblingNode() override { return nullptr; }

	virtual void MoveToFront() override { }

	virtual void MoveToBack() override { }

	virtual void MovePrev() override { }

	virtual void MoveNext() override { }

	cxx::point2d<float> WorldToParent(const cxx::point2d<float>& pos) { return pos; }

	virtual void Release() override { }

public:
	RootSceneNode(::Scene* scene);

	void AdjustRenderingOrder() override;
};