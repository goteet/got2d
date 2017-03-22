#include "spatial_graph.h"
#include "scene.h"
#include "engine.h"
#include "input.h"
#include <algorithm>

Scene::Scene(float boundSize)
	: m_spatial(boundSize)
	, m_mouseButtonState{ g2d::MouseButton::Left, g2d::MouseButton::Right, g2d::MouseButton::Middle }
{
	CreateCameraNode();
	RegisterKeyboardListener();
}

void Scene::RegisterKeyboardListener()
{
	m_pressReceiver.UserData = m_pressingReceiver.UserData = this;
	m_pressReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPress(key);
	};

	m_pressingReceiver.Functor = [](void* userData, g2d::KeyCode key)
	{
		::Scene* scene = reinterpret_cast<::Scene*>(userData);
		scene->OnKeyPressing(key);
	};

	GetKeyboard().OnPress += m_pressReceiver;
	GetKeyboard().OnPressing += m_pressingReceiver;
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
	UnRegisterKeyboardListener();
	::GetEngineImpl()->RemoveScene(*this);
	EmptyChildren();
	delete this;
}



void Scene::UnRegisterKeyboardListener()
{
	GetKeyboard().OnPress -= m_pressReceiver;
	GetKeyboard().OnPressing -= m_pressingReceiver;
}

void Scene::Update(uint32_t elapsedTime, uint32_t deltaTime)
{
	for (auto& state : m_mouseButtonState)
	{
		state.Update(elapsedTime);
		if (!state.IsHovering())
			m_canTickHovering = false;
	}

	if (m_canTickHovering && m_hoverNode != nullptr)
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

void Scene::MouseButtonState::Update(uint32_t currentStamp)
{
	if (isPressing && !isDragging &&
		nodeHovering != nullptr &&
		(currentStamp - pressTimeStamp > PRESSING_INTERVAL))
	{
		isDragging = true;
		// 使用上一个消息的按键
		nodeDragging = nodeHovering;
		nodeDragging->OnDragBegin(button, pressCursorPos);
	}
}

void Scene::MouseButtonState::ForceRelease()
{
	if (isDragging)
	{
		if (nodeDragging == nodeHovering)
		{
			nodeDragging->OnDragEnd(button, pressCursorPos);
		}
		else
		{
			nodeDragging->OnDropTo(nodeHovering, button, pressCursorPos);
		}

		isDragging = false;
		isPressing = false;
		nodeDragging = nullptr;
	}
	else if (isPressing)
	{
		nodeHovering->OnClick(button, pressCursorPos);
		isPressing = false;
	}
}

void Scene::MouseButtonState::OnDoubleClick(const g2d::Message& message)
{
	if (nodeHovering != nullptr)
	{
		nodeHovering->OnDoubleClick(message.MouseButton,
			gml::coord(message.CursorPositionX, message.CursorPositionY));
	}
}

void Scene::MouseButtonState::OnMouseDown(const g2d::Message& message, uint32_t currentStamp)
{
	if (nodeHovering != nullptr)
	{
		isPressing = true;
		pressTimeStamp = currentStamp;
		pressCursorPos.set(message.CursorPositionX, message.CursorPositionY);
	}
	else
	{
		isPressing = false;
	}
}

bool Scene::MouseButtonState::OnMouseUp(const g2d::Message& message)
{
	if (isDragging)
	{
		if (nodeHovering != nullptr && nodeHovering != nodeDragging)
		{
			nodeDragging->OnDropTo(nodeHovering, button,
				gml::coord(message.CursorPositionX, message.CursorPositionY));

			nodeHovering->OnCursorEnterFrom(nodeDragging,
				gml::coord(message.CursorPositionX, message.CursorPositionY));
		}
		else
		{
			nodeDragging->OnDragEnd(button,
				gml::coord(message.CursorPositionX, message.CursorPositionY));

			nodeDragging->OnCursorEnterFrom(nodeDragging,
				gml::coord(message.CursorPositionX, message.CursorPositionY));
		}
		isPressing = false;
		isDragging = false;
		nodeDragging = nullptr;
		return true;
	}
	else if (isPressing)
	{
		nodeHovering->OnClick(button,
			gml::coord(message.CursorPositionX, message.CursorPositionY));
		isPressing = false;
		return true;
	}

	return false;
}

bool Scene::MouseButtonState::OnMouseMove(const g2d::Message& message)
{
	if (!isDragging && isPressing)
	{

		if (nodeHovering != nullptr && isPressing)
		{
			isDragging = true;
			nodeDragging = nodeHovering;
			nodeDragging->OnDragBegin(button,
				gml::coord(message.CursorPositionX, message.CursorPositionY));

			nodeDragging->OnDragging(button,
				gml::coord(message.CursorPositionX, message.CursorPositionY));
			return true;
		}
		else
		{
			isPressing = false;
		}

	}
	else if (isDragging)
	{
		if (nodeHovering != nullptr && nodeHovering != nodeDragging)
		{
			nodeDragging->OnDropping(nodeHovering, button,
				gml::coord(message.CursorPositionX, message.CursorPositionY));
		}
		else
		{
			nodeDragging->OnDragging(button,
				gml::coord(message.CursorPositionX, message.CursorPositionY));
		}
		return true;
	}
	return false;
}

bool Scene::MouseButtonState::OnMessage(const g2d::Message& message, uint32_t currentStamp, ::SceneNode* hitNode)
{
	nodeHovering = hitNode;
	bool sameButton = message.MouseButton == button;

	if (sameButton)
	{
		if (message.Event == g2d::MessageEvent::MouseButtonDoubleClick)
		{
			OnDoubleClick(message);
		}
		else if (message.Event == g2d::MessageEvent::MouseButtonDown)
		{
			if (isDragging)//异常状态
			{
				// 让上次的dragging事件正常结束
				nodeDragging->OnDragEnd(button,
					gml::coord(message.CursorPositionX, message.CursorPositionY));

				isDragging = false;
				isPressing = false;
				nodeDragging = nullptr;
			}
			else if (isPressing)//异常状态
			{
				// 取消上次的点击操作
				isPressing = false;
			}
			OnMouseDown(message, currentStamp);
			return true;
		}
		else if (message.Event == g2d::MessageEvent::MouseButtonUp)
		{
			if (OnMouseUp(message))
				return true;
		}
	}
	else if (message.Event == g2d::MessageEvent::MouseMove)
	{
		if (OnMouseMove(message))
			return true;
	}
	return false;
}

void Scene::OnMessage(const g2d::Message& message, uint32_t currentTimeStamp)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnMessage(message);
	});

	::SceneNode* hitNode = FindInteractiveObject(message);
	if (message.Event == g2d::MessageEvent::LostFocus)
	{
		for (auto& state : m_mouseButtonState)
			state.ForceRelease();
	}
	else if (message.Source == g2d::MessageSource::Mouse)
	{
		bool handleMove = false;
		for (auto& state : m_mouseButtonState)
		{
			if (state.OnMessage(message, currentTimeStamp, hitNode))
			{
				handleMove = true;
				m_canTickHovering = true;
			}
		}

		if (!handleMove && message.Event == g2d::MessageEvent::MouseMove)
		{
			if (m_hoverNode != hitNode)
			{
				if (m_hoverNode != nullptr)
				{
					m_hoverNode->OnCursorLeaveTo(hitNode,
						gml::coord(message.CursorPositionX, message.CursorPositionY));
				}
				if (hitNode != nullptr)
				{
					hitNode->OnCursorEnterFrom(m_hoverNode,
						gml::coord(message.CursorPositionX, message.CursorPositionY));
				}
				m_hoverNode = hitNode;
			}
			else
			{
				if (m_hoverNode != nullptr && m_canTickHovering)
				{
					m_hoverNode->OnCursorHovering();
					m_canTickHovering = false;
				}
			}
		}
	}
}

void Scene::OnKeyPress(g2d::KeyCode key)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnKeyPress(key);
	});
}

void Scene::OnKeyPressing(g2d::KeyCode key)
{
	TraversalChildren([&](::SceneNode* child) {
		child->OnKeyPressing(key);
	});
}

::SceneNode* Scene::FindInteractiveObject(const g2d::Message& message)
{
	auto cur = std::rbegin(m_cameraOrder);
	auto end = std::rend(m_cameraOrder);
	g2d::Entity* frontEntity = nullptr;
	for (; cur != end; cur++)
	{
		::Camera* camera = *cur;

		int coordX = message.CursorPositionX;
		int coordY = message.CursorPositionY;
		gml::vec2 worldCoord = camera->ScreenToWorld({ coordX, coordY });
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