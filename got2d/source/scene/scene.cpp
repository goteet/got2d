#include <algorithm>
#include "../system_blackboard.h"
#include "../render/render_system.h"
#include "scene.h"
#include "scene_node.h"
#include "camera.h"

bool RenderingOrderSorter(g2d::Component* a, g2d::Component* b);

//*************************************************************
// overrides
//*************************************************************
g2d::SceneNode* Scene::GetRootNode()
{
	return mRootNode.get();
}

g2d::Camera* Scene::CreateAdditionalCameraNode()
{
	Camera* pCamera = new ::Camera(this, static_cast<unsigned int>(mCameraList.size()));
	mCameraList.push_back(pCamera);
	mCameraOrderDirty = true;

	mRootNode->CreateChild()->AddComponent(pCamera, true);
	return pCamera;
}

g2d::Camera * Scene::GetDefaultCamera()
{
	return GetCameraByIndex(0);
}

g2d::Camera* Scene::GetCameraByIndex(unsigned int index)
{
	ENSURE(index < GetCameraCount());
	return mCameraList.at(index);
}

unsigned int Scene::GetCameraCount() const
{
	return static_cast<unsigned int>(mCameraList.size());
}

void Scene::Render()
{
	GetRenderSystem().FlushRequests();
	ResortCameraOrder();
	ResetRenderingOrder();
	for (auto camera : mCameraOrder)
	{
		if (!camera->IsActivity())
			continue;

		GetRenderSystem().SetViewMatrix(camera->GetViewMatrix());

		camera->mVisibleComponents.clear();
		mSpatial.RecursiveFindVisible(camera);

		//sort visibleEntities by render order
		std::sort(
			camera->mVisibleComponents.begin(),
			camera->mVisibleComponents.end(),
			RenderingOrderSorter);

		for (auto& component : camera->mVisibleComponents)
		{
			component->OnRender();
		}
		GetRenderSystem().FlushRequests();
	}
}

void Scene::Release()
{
	delete this;
}


//*************************************************************
// functions
//*************************************************************

Scene::Scene(float boundSize)
	: mRootNode(new ::RootSceneNode(this))
	, mSpatial(boundSize)
	, mMouseButtonState{ 0, 1, 2 }
{
	//for main pCamera
	CreateAdditionalCameraNode();

	RegisterKeyEventReceiver();
	RegisterMouseEventReceiver();
}

Scene::~Scene()
{
	UnRegisterKeyEventReceiver();
	UnRegisterMouseEventReceiver();
}

void Scene::RegisterKeyEventReceiver()
{
	mKeyPressReceiver.UserData
		= mKeyPressingBeginReceiver.UserData
		= mKeyPressingReceiver.UserData
		= mKeyPressingEndReceiver.UserData
		= this;

	mKeyPressReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPress(key);
	};

	mKeyPressingBeginReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPressingBegin(key);
	};

	mKeyPressingReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPressing(key);
	};

	mKeyPressingEndReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPressingEnd(key);
	};

	GetKeyboard().OnPress += mKeyPressReceiver;
	GetKeyboard().OnPressingBegin += mKeyPressingBeginReceiver;
	GetKeyboard().OnPressing += mKeyPressingReceiver;
	GetKeyboard().OnPressingEnd += mKeyPressingEndReceiver;
}

void Scene::RegisterMouseEventReceiver()
{
	mMousePressReceiver.UserData
		= mMousePressingBeginReceiver.UserData
		= mMousePressingReceiver.UserData
		= mMousePressingEndReceiver.UserData
		= mMouseMovingReceiver.UserData
		= mMouseDoubleClickReceiver.UserData
		= this;

	mMousePressReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePress(button);
	};

	mMousePressingBeginReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePressingBegin(button);
	};

	mMousePressingReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePressing(button);
	};

	mMousePressingEndReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePressingEnd(button);
	};

	mMouseMovingReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMouseMoving();
	};

	mMouseDoubleClickReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		auto scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMouseDoubleClick(button);
	};

	GetMouse().OnPress += mMousePressReceiver;
	GetMouse().OnPressingBegin += mMousePressingBeginReceiver;
	GetMouse().OnPressing += mMousePressingReceiver;
	GetMouse().OnPressingEnd += mMousePressingEndReceiver;
	GetMouse().OnMoving += mMouseMovingReceiver;
	GetMouse().OnDoubleClick += mMouseDoubleClickReceiver;
}

void Scene::ResortCameraOrder()
{
	if (mCameraOrderDirty)
	{
		mCameraOrderDirty = false;
		mCameraOrder = mCameraList;

		std::sort(mCameraOrder.begin(), mCameraOrder.end(),
			[](g2d::Camera* a, g2d::Camera* b)->bool {

			auto aOrder = a->GetRenderingOrder();
			auto bOrder = b->GetRenderingOrder();
			if (aOrder == bOrder)
			{
				return a->GetIndex() < b->GetIndex();
			}
			else
			{
				return aOrder < bOrder;
			}
		});
	}
}

void Scene::ResetRenderingOrder()
{
	if (mRenderingOrderDirtyNode != nullptr)
	{
		mRenderingOrderDirtyNode->AdjustRenderingOrder();
		mRenderingOrderDirtyNode = nullptr;
	}
}

void Scene::UnRegisterKeyEventReceiver()
{
	GetKeyboard().OnPress -= mKeyPressReceiver;
	GetKeyboard().OnPressingBegin -= mKeyPressingBeginReceiver;
	GetKeyboard().OnPressing -= mKeyPressingReceiver;
	GetKeyboard().OnPressingEnd -= mKeyPressingEndReceiver;
}

void Scene::UnRegisterMouseEventReceiver()
{
	GetMouse().OnPress -= mMousePressReceiver;
	GetMouse().OnPressingBegin -= mMousePressingBeginReceiver;
	GetMouse().OnPressing -= mMousePressingReceiver;
	GetMouse().OnPressingEnd -= mMousePressingEndReceiver;
	GetMouse().OnMoving -= mMouseMovingReceiver;
	GetMouse().OnDoubleClick -= mMouseDoubleClickReceiver;
}

void Scene::Update(unsigned int elapsedTime, unsigned int deltaTime)
{
	if (mHoverNode != nullptr && mCanTickHovering && GetMouse().IsFree())
	{
		mHoverNode->OnCursorHovering();
		mCanTickHovering = false;
	}

	mRootNode->OnUpdate(deltaTime);

	//TODO: checking whether mHovering is deleted
	mCanTickHovering = true;
}

void Scene::OnMessage(const g2d::Message& message, unsigned int currentTimeStamp)
{
	mRootNode->OnMessage(message);
}

void Scene::SetRenderingOrderDirty(::SceneNode* parent)
{
	if (mRenderingOrderDirtyNode == nullptr)
	{
		mRenderingOrderDirtyNode = parent;
	}
	else if (mRenderingOrderDirtyNode->GetRenderingOrder() > parent->GetRenderingOrder())
	{
		mRenderingOrderDirtyNode = parent;
	}
}

void Scene::OnResize()
{
	for (auto& camera : mCameraList)
	{
		camera->OnPostUpdateTransformChanged();
	}
}

void Scene::AdjustRenderingOrder()
{
	//mRenderingOrderEnd = 1;
	//mChildrenNodes.Traversal([&](::SceneNode* child)
	//{
	//	child->SetRenderingOrder(mRenderingOrderEnd);
	//});
}

void Scene::OnRemoveSceneNode(::SceneNode* node)
{
	if (node == mHoverNode)
	{
		mHoverNode = nullptr;
	}

	for (auto& button : mMouseButtonState)
	{
		button.OnRemoveSceneNode(node);
	}

	for (auto& camera : mCameraList)
	{
		camera->OnRemoveSceneNode(node);
	}
}

void Scene::OnKeyPress(g2d::KeyCode key)
{
	mRootNode->OnKeyPress(key);
}

void Scene::OnKeyPressingBegin(g2d::KeyCode key)
{
	mRootNode->OnKeyPressingBegin(key);
}

void Scene::OnKeyPressing(g2d::KeyCode key)
{
	mRootNode->OnKeyPressing(key);
}

void Scene::OnKeyPressingEnd(g2d::KeyCode key)
{
	mRootNode->OnKeyPressingEnd(key);
}

void Scene::OnMousePress(g2d::MouseButton button)
{
	if (mHoverNode != nullptr)
	{
		mHoverNode->OnClick(button);
	}
}

void Scene::MouseButtonState::OnPressingBegin(::SceneNode* hitNode)
{
	if (hitNode != nullptr)
	{
		mDraggingNode = hitNode;
		mDraggingNode->OnDragBegin(Button);
	}
}

void Scene::OnMousePressingBegin(g2d::MouseButton button)
{
	mMouseButtonState[(int)button].OnPressingBegin(mHoverNode);
}

void Scene::MouseButtonState::OnPressing(::SceneNode* hitNode)
{
	//*  heart-beaten?
	if (mDraggingNode != nullptr)
	{
		if (hitNode != nullptr && hitNode != mDraggingNode)
		{
			mDraggingNode->OnDropping(hitNode, Button);
		}
		else
		{
			mDraggingNode->OnDragging(Button);
		}
	}
}

void Scene::OnMousePressing(g2d::MouseButton button)
{
	mMouseButtonState[(int)button].OnPressing(mHoverNode);
}

void Scene::MouseButtonState::OnPressingEnd(::SceneNode* hitNode)
{
	if (mDraggingNode != nullptr)
	{
		if (hitNode == nullptr && hitNode != mDraggingNode)
		{
			mDraggingNode->OnDropTo(hitNode, Button);
			hitNode->OnCursorEnterFrom(mDraggingNode);
		}
		else
		{
			mDraggingNode->OnDragEnd(Button);
			mDraggingNode->OnCursorEnterFrom(nullptr);
		}
		mDraggingNode = nullptr;
	}
}

void Scene::MouseButtonState::OnRemoveSceneNode(::SceneNode* node)
{
	if (node == mDraggingNode)
	{
		mDraggingNode = nullptr;
	}
}

void Scene::OnMousePressingEnd(g2d::MouseButton button)
{
	mMouseButtonState[(int)button].OnPressingEnd(mHoverNode);
}

void Scene::OnMouseDoubleClick(g2d::MouseButton button)
{
	if (mHoverNode != nullptr)
	{
		mHoverNode->OnDoubleClick(button);
	}
}

void Scene::MouseButtonState::OnMoving(::SceneNode* hitNode)
{
	if (mDraggingNode != nullptr)
	{
		if (hitNode != nullptr && hitNode != mDraggingNode)
		{
			mDraggingNode->OnDropping(hitNode, Button);
		}
		else
		{
			mDraggingNode->OnDragging(Button);
		}
	}
}

void Scene::OnMouseMoving()
{
	::SceneNode* hitNode = FindInteractiveObject(GetMouse().GetCursorPosition());
	if (GetMouse().IsFree())
	{
		if (mHoverNode != hitNode)
		{
			if (mHoverNode != nullptr)
			{
				mHoverNode->OnCursorLeaveTo(hitNode);
			}
			if (hitNode != nullptr)
			{
				hitNode->OnCursorEnterFrom(mHoverNode);
			}
			mHoverNode = hitNode;
		}
		else if (mHoverNode != nullptr && mCanTickHovering)
		{
			mHoverNode->OnCursorHovering();
			mCanTickHovering = false;
		}
	}
	else
	{
		for (auto& mouseButton : mMouseButtonState)
		{
			mouseButton.OnMoving(hitNode);
		}
	}
}

::SceneNode* Scene::FindInteractiveObject(const cxx::int2& cursorPos)
{
	auto cur = mCameraOrder.rbegin();
	auto end = mCameraOrder.rend();

	for (; cur != end; cur++)
	{
		auto& camera = *cur;
		cxx::float2 worldCoord = camera->ScreenToWorld(cursorPos);
		auto component = camera->FindNearestComponent(worldCoord);
		if (component != nullptr)
		{
			return reinterpret_cast<::SceneNode*>(component->GetSceneNode());
		}
	}
	return nullptr;
}

bool RenderingOrderSorter(g2d::Component* a, g2d::Component* b)
{
	return a->_GetRenderingOrder_Internal() < b->_GetRenderingOrder_Internal();
}
