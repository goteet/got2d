#pragma once
#include <vector>
#include "g2dscene.h"
#include "../input/input.h"
#include "spatial_graph.h"
#include "cxx_scope.h"

class SceneNode;

class Scene : public g2d::Scene
{
	RTTI_IMPL;
public:
	virtual g2d::SceneNode* GetRootNode() override;

	virtual g2d::Camera* CreateAdditionalCameraNode() override;

	virtual g2d::Camera* GetDefaultCamera() override;

	virtual g2d::Camera* GetCameraByIndex(unsigned int) override;

	virtual unsigned int GetCameraCount() const override;

	virtual void Render() override;

	virtual void Release() override;

public:
	Scene(float scenBoundSize);

	~Scene();

	void SetRenderingOrderDirty(::SceneNode* parent);

	void SetCameraOrderDirty() { mCameraOrderDirty = true; }

	void Update(unsigned int elapsedTime, unsigned int deltaTime);

	void OnMessage(const g2d::Message& message, unsigned int currentTimeStamp);

	void OnResize();

	SpatialGraph& GetSpatialGraph() { return mSpatial; }

	void AdjustRenderingOrder();

	void OnRemoveSceneNode(::SceneNode* node);


private:
	void ResortCameraOrder();

	void ResetRenderingOrder();

	::SceneNode* FindInteractiveObject(const cxx::int2& cursorPos);

	void RegisterKeyEventReceiver();

	void UnRegisterKeyEventReceiver();

	void RegisterMouseEventReceiver();

	void UnRegisterMouseEventReceiver();

	void OnKeyPress(g2d::KeyCode key);

	void OnKeyPressingBegin(g2d::KeyCode key);

	void OnKeyPressing(g2d::KeyCode key);

	void OnKeyPressingEnd(g2d::KeyCode key);

	void OnMousePress(g2d::MouseButton button);

	void OnMousePressingBegin(g2d::MouseButton button);

	void OnMousePressing(g2d::MouseButton button);

	void OnMousePressingEnd(g2d::MouseButton button);

	void OnMouseDoubleClick(g2d::MouseButton button);

	void OnMouseMoving();

	KeyEventReceiver mKeyPressReceiver;
	KeyEventReceiver mKeyPressingBeginReceiver;
	KeyEventReceiver mKeyPressingReceiver;
	KeyEventReceiver mKeyPressingEndReceiver;

	MouseEventReceiver mMousePressReceiver;
	MouseEventReceiver mMousePressingBeginReceiver;
	MouseEventReceiver mMousePressingReceiver;
	MouseEventReceiver mMousePressingEndReceiver;
	MouseEventReceiver mMouseMovingReceiver;
	MouseEventReceiver mMouseDoubleClickReceiver;

	class MouseButtonState
	{
		::SceneNode* mDraggingNode = nullptr;
	public:
		const g2d::MouseButton Button;
		MouseButtonState(int index) : Button((g2d::MouseButton)index) { }
		void OnMoving(::SceneNode* hitNode);
		void OnPressingBegin(::SceneNode* hitNode);
		void OnPressing(::SceneNode* hitNode);
		void OnPressingEnd(::SceneNode* hitNode);
		void OnRemoveSceneNode(::SceneNode* node);
	} mMouseButtonState[3];

private:
	::SceneNode* mRootNode;

	SpatialGraph mSpatial;
	std::vector<::Camera*> mCameraList;
	std::vector<::Camera*> mCameraOrder;
	bool mCameraOrderDirty = true;

	::SceneNode* mHoverNode = nullptr;
	bool mCanTickHovering = false;

	::SceneNode* mRenderingOrderDirtyNode = nullptr;
	unsigned int mRenderingOrderEnd = 1;
};
