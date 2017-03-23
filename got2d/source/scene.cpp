#include "spatial_graph.h"
#include "scene.h"
#include "engine.h"
#include "input.h"
#include <algorithm>

Scene::Scene(float boundSize)
	: m_spatial(boundSize)
	, m_mouseButtonState{ 0, 1, 2 }
{
	CreateCameraNode();
	RegisterKeyEventReceiver();
	RegisterMouseEventReceiver();
}

void Scene::RegisterKeyEventReceiver()
{
	m_keyPressReceiver.UserData
		= m_keyPressingBeginReceiver.UserData
		= m_keyPressingReceiver.UserData
		= m_keyPressingEndReceiver.UserData
		= this;

	m_keyPressReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPress(key);
	};

	m_keyPressingBeginReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPressingBegin(key);
	};

	m_keyPressingReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPressing(key);
	};

	m_keyPressingEndReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPressingEnd(key);
	};

	GetKeyboard().OnPress += m_keyPressReceiver;
	GetKeyboard().OnPressingBegin += m_keyPressingBeginReceiver;
	GetKeyboard().OnPressing += m_keyPressingReceiver;
	GetKeyboard().OnPressingEnd += m_keyPressingEndReceiver;
}

void Scene::RegisterMouseEventReceiver()
{
	m_mousePressReceiver.UserData
		= m_mousePressingBeginReceiver.UserData
		= m_mousePressingReceiver.UserData
		= m_mousePressingEndReceiver.UserData
		= m_mouseMovingReceiver.UserData
		= m_mouseDoubleClickReceiver.UserData
		= this;

	m_mousePressReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePress(button);
	};

	m_mousePressingBeginReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePressingBegin(button);
	};

	m_mousePressingReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePressing(button);
	};

	m_mousePressingEndReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMousePressingEnd(button);
	};

	m_mouseMovingReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMouseMoving();
	};

	m_mouseDoubleClickReceiver.Functor = [](void* userData, g2d::MouseButton button)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnMouseDoubleClick(button);
	};

	GetMouse().OnPress += m_mousePressReceiver;
	GetMouse().OnPressingBegin += m_mousePressingBeginReceiver;
	GetMouse().OnPressing += m_mousePressingReceiver;
	GetMouse().OnPressingEnd += m_mousePressingEndReceiver;
	GetMouse().OnMoving += m_mouseMovingReceiver;
	GetMouse().OnDoubleClick += m_mouseDoubleClickReceiver;
}

void Scene::ResortCameraOrder()
{
	if (m_cameraOrderDirty)
	{
		m_cameraOrderDirty = false;
		m_cameraOrder = m_cameras;

		std::sort(m_cameraOrder.begin(), m_cameraOrder.end(),
			[](g2d::Camera* a, g2d::Camera* b)->bool {

			auto aOrder = a->GetRenderingOrder();
			auto bOrder = b->GetRenderingOrder();
			if (aOrder == bOrder)
			{
				return a->GetID() < b->GetID();
			}
			else
			{
				return aOrder < bOrder;
			}
		});
	}
}

void Scene::Release()
{
	UnRegisterKeyEventReceiver();
	UnRegisterMouseEventReceiver();
	::GetEngineImpl()->RemoveScene(*this);
	EmptyChildren();
	delete this;
}

void Scene::UnRegisterKeyEventReceiver()
{
	GetKeyboard().OnPress -= m_keyPressReceiver;
	GetKeyboard().OnPressingBegin -= m_keyPressingBeginReceiver;
	GetKeyboard().OnPressing -= m_keyPressingReceiver;
	GetKeyboard().OnPressingEnd -= m_keyPressingEndReceiver;
}

void Scene::UnRegisterMouseEventReceiver()
{
	GetMouse().OnPress -= m_mousePressReceiver;
	GetMouse().OnPressingBegin -= m_mousePressingBeginReceiver;
	GetMouse().OnPressing -= m_mousePressingReceiver;
	GetMouse().OnPressingEnd -= m_mousePressingEndReceiver;
	GetMouse().OnMoving -= m_mouseMovingReceiver;
}

void Scene::Update(uint32_t elapsedTime, uint32_t deltaTime)
{
	if (m_hoverNode != nullptr && m_canTickHovering && GetMouse().IsFree())
	{
		m_hoverNode->OnCursorHovering();
		m_canTickHovering = false;
	}

	_Update(deltaTime);
	AdjustRenderingOrder();
	m_canTickHovering = true;
}

void Scene::AdjustRenderingOrder()
{
	uint32_t curIndex = 1;

	TraversalChildren([&](::SceneNode* child)
	{
		child->SetRenderingOrder(curIndex);
	});
}

void Scene::OnMessage(const g2d::Message& message, uint32_t currentTimeStamp)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnMessage(message);
	});
}

void Scene::OnKeyPress(g2d::KeyCode key)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnKeyPress(key);
	});
}

void Scene::OnKeyPressingBegin(g2d::KeyCode key)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnKeyPressingBegin(key);
	});
}

void Scene::OnKeyPressing(g2d::KeyCode key)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnKeyPressing(key);
	});
}

void Scene::OnKeyPressingEnd(g2d::KeyCode key)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnKeyPressingEnd(key);
	});
}

void Scene::OnMousePress(g2d::MouseButton button)
{
	if (m_hoverNode != nullptr)
	{
		m_hoverNode->OnClick(button);
	}
}

void Scene::MouseButtonState::OnPressingBegin(::SceneNode* hitNode)
{
	if (hitNode != nullptr)
	{
		dragNode = hitNode;
		dragNode->OnDragBegin(Button);
	}
}

void Scene::OnMousePressingBegin(g2d::MouseButton button)
{
	m_mouseButtonState[(int)button].OnPressingBegin(m_hoverNode);
}


void Scene::MouseButtonState::OnPressing(::SceneNode* hitNode)
{
	//*  heart-beaten?
	if (dragNode != nullptr)
	{
		if (hitNode != nullptr && hitNode != dragNode)
		{
			dragNode->OnDropping(hitNode, Button);
		}
		else
		{
			dragNode->OnDragging(Button);
		}
	}
}

void Scene::OnMousePressing(g2d::MouseButton button)
{
	m_mouseButtonState[(int)button].OnPressing(m_hoverNode);
}

void Scene::MouseButtonState::OnPressingEnd(::SceneNode* hitNode)
{
	if (dragNode != nullptr)
	{
		if (hitNode == nullptr && hitNode != dragNode)
		{
			dragNode->OnDropTo(hitNode, Button);
			hitNode->OnCursorEnterFrom(dragNode);
		}
		else
		{
			dragNode->OnDragEnd(Button);
			dragNode->OnCursorEnterFrom(nullptr);
		}
		dragNode = nullptr;
	}
}

void Scene::OnMousePressingEnd(g2d::MouseButton button)
{
	m_mouseButtonState[(int)button].OnPressingEnd(m_hoverNode);
}

void Scene::OnMouseDoubleClick(g2d::MouseButton button)
{
	if (m_hoverNode != nullptr)
	{
		m_hoverNode->OnDoubleClick(button);
	}
}

void Scene::MouseButtonState::OnMoving(::SceneNode* hitNode)
{
	if (dragNode != nullptr)
	{
		if (hitNode != nullptr && hitNode != dragNode)
		{
			dragNode->OnDropping(hitNode, Button);
		}
		else
		{
			dragNode->OnDragging(Button);
		}
	}
}

void Scene::OnMouseMoving()
{
	::SceneNode* hitNode = FindInteractiveObject(GetMouse().GetCursorPosition());
	if (GetMouse().IsFree())
	{
		if (m_hoverNode != hitNode)
		{
			if (m_hoverNode != nullptr)
			{
				m_hoverNode->OnCursorLeaveTo(hitNode);
			}
			if (hitNode != nullptr)
			{
				hitNode->OnCursorEnterFrom(m_hoverNode);
			}
			m_hoverNode = hitNode;
		}
		else if (m_hoverNode != nullptr && m_canTickHovering)
		{
			m_hoverNode->OnCursorHovering();
			m_canTickHovering = false;
		}
	}
	else
	{
		for (auto& mouseButton : m_mouseButtonState)
		{
			mouseButton.OnMoving(hitNode);
		}
	}
}

::SceneNode* Scene::FindInteractiveObject(const gml::coord& cursorPos)
{
	auto cur = std::rbegin(m_cameraOrder);
	auto end = std::rend(m_cameraOrder);
	g2d::Entity* frontEntity = nullptr;
	for (; cur != end; cur++)
	{
		auto& camera = *cur;
		gml::vec2 worldCoord = camera->ScreenToWorld(cursorPos);
		auto entity = camera->FindIntersectionObject(worldCoord);
		if (entity != nullptr)
		{
			return reinterpret_cast<::SceneNode*>(entity->GetSceneNode());
		}
	}
	return nullptr;
}

#include "render_system.h"
void Scene::Render()
{
	GetRenderSystem()->FlushRequests();
	ResortCameraOrder();
	for (auto camera : m_cameraOrder)
	{
		if (camera->IsActivity())
		{
			camera->visibleEntities.clear();
			GetRenderSystem()->SetViewMatrix(camera->GetViewMatrix());
			m_spatial.FindVisible(*camera);

			//sort visibleEntities by render order
			std::sort(std::begin(camera->visibleEntities), std::end(camera->visibleEntities),
				[](g2d::Entity* a, g2d::Entity* b) {
				return a->GetRenderingOrder() < b->GetRenderingOrder();
			});

			for (auto& entity : camera->visibleEntities)
			{
				entity->OnRender();
			}
			GetRenderSystem()->FlushRequests();
		}
	}
}

g2d::Camera* Scene::CreateCameraNode()
{
	Camera* camera = new ::Camera();
	CreateSceneNodeChild(camera, true);
	m_cameraOrderDirty = true;
	camera->SetID(static_cast<uint32_t>(m_cameras.size()));
	m_cameras.push_back(camera);
	return camera;
}

g2d::Camera* Scene::GetCameraByIndex(uint32_t index) const
{
	ENSURE(index < m_cameras.size());
	return m_cameras[index];
}