#include "scene.h"
#include "input.h"

SceneNode::SceneNode(::Scene& scene, ::SceneNode* parent, uint32_t childID)
	: m_scene(scene)
	, m_parent(parent)
	, m_iparent(*parent)
	, m_bparent(*parent)
	, m_childIndex(childID)
{
}

SceneNode::SceneNode(::Scene& scene, uint32_t childID)
	: m_scene(scene)
	, m_iparent(scene)
	, m_parent(nullptr)
	, m_bparent(scene)
	, m_childIndex(childID)
{
}

SceneNode::~SceneNode()
{
	/*
	m_scene.GetSpatialGraph()->Remove(*m_entity);
	if (m_autoRelease)
	{
		m_entity->Release();
	}
	*/
}

g2d::Scene* SceneNode::GetScene() const
{
	return &m_scene;
}

const gml::mat32& SceneNode::GetWorldMatrix()
{
	if (m_matrixWorldDirty)
	{
		auto& matParent = m_iparent.GetWorldMatrix();
		m_matrixWorld = matParent * GetLocalMatrix();
		m_matrixWorldDirty = false;
	}
	return m_matrixWorld;
}

g2d::SceneNode* SceneNode::SetPivot(const gml::vec2& pivot)
{
	_SetPivot(pivot);
	SetWorldMatrixDirty();
	return this;
}

g2d::SceneNode* SceneNode::SetScale(const gml::vec2& scale)
{
	TraversalComponent([&](g2d::Component* component)
	{
		component->OnScale(scale);
	});
	_SetScale(scale);
	SetWorldMatrixDirty();
	return this;
}

g2d::SceneNode* SceneNode::SetPosition(const gml::vec2& position)
{
	TraversalComponent([&](g2d::Component* component)
	{
		component->OnMove(position);
	});
	_SetPosition(position);
	SetWorldMatrixDirty();
	return this;
}

g2d::SceneNode* SceneNode::SetRotation(gml::radian r)
{
	TraversalComponent([&](g2d::Component* component)
	{
		component->OnRotate(r);
	});
	_SetRotation(r);
	SetWorldMatrixDirty();
	return this;
}

void SceneNode::AdjustRenderingOrder()
{
	auto index = m_renderingOrder + 1;
	TraversalChildren([&](::SceneNode* child)
	{
		child->SetRenderingOrder(index);
	});

	auto current = this;
	while (current != nullptr)
	{
		auto next = current->GetNextSibling();
		while (next != nullptr)
		{
			next->SetRenderingOrder(index);
			next = next->GetNextSibling();
		}
		current = current->GetParent();
	}
}

void SceneNode::SetWorldMatrixDirty()
{
	m_matrixWorldDirty = true;
	TraversalChildren([](::SceneNode* child)
	{
		child->SetWorldMatrixDirty();
	});
	m_matrixDirtyEntityUpdate = true;
}

void SceneNode::AdjustSpatial()
{
	//m_scene.GetSpatialGraph()->Add(*m_entity);
}

void SceneNode::OnUpdate(uint32_t deltaTime)
{
	auto onUpdate = [&](g2d::Component* component)
	{
		component->OnUpdate(deltaTime);
	};
	DispatchComponent(onUpdate);
	if (m_matrixDirtyEntityUpdate)
	{
		// ����Ǿ�̬������Ҫ�������������Ĳ�����λ��
		// ���ڽ׶�ֻ��Ҫ��test visible֮ǰ����þ���.
		// Ҳ���� Scene::Render֮ǰ
		if (IsStatic())
		{
			AdjustSpatial();
		}
		auto onUpdateMatrixChanged = [&](g2d::Component* component)
		{
			component->OnUpdateMatrixChanged();
		};
		DispatchComponent(onUpdateMatrixChanged);
		m_matrixDirtyEntityUpdate = false;
	}

	OnUpdateChildren(deltaTime);
}

::SceneNode* SceneNode::GetPrevSibling() const
{
	if (m_childIndex == 0)
	{
		return nullptr;
	}
	return m_bparent._GetChildByIndex(m_childIndex - 1);
}

::SceneNode* SceneNode::GetNextSibling() const
{
	if (m_childIndex == m_bparent._GetChildCount() - 1)
	{
		return nullptr;
	}
	return m_bparent._GetChildByIndex(m_childIndex + 1);
}

g2d::SceneNode * SceneNode::CreateChild()
{
	auto child = _CreateSceneNodeChild(m_scene, this);
	m_scene.SetRenderingOrderDirty(this);
	return child;
}

void SceneNode::MoveToFront()
{
	m_bparent.MoveChild(m_childIndex, m_bparent._GetChildCount() - 1);
	m_scene.SetRenderingOrderDirty(&m_bparent);
}

void SceneNode::MoveToBack()
{
	m_bparent.MoveChild(m_childIndex, 0);
	m_scene.SetRenderingOrderDirty(&m_bparent);
}

void SceneNode::MovePrev()
{
	m_bparent.MoveChild(m_childIndex, m_childIndex - 1);
	m_scene.SetRenderingOrderDirty(&m_bparent);
}

void SceneNode::MoveNext()
{
	m_bparent.MoveChild(m_childIndex, m_childIndex + 1);
	m_scene.SetRenderingOrderDirty(&m_bparent);
}

void SceneNode::SetStatic(bool s)
{
	if (m_isStatic != s)
	{
		m_isStatic = s;
		AdjustSpatial();
	}
}

gml::vec2 SceneNode::GetWorldPosition()
{
	auto localPos = _GetPosition();
	return gml::transform_point(GetWorldMatrix(), localPos);
}

gml::vec2 SceneNode::WorldToLocal(const gml::vec2& pos)
{
	gml::mat33 worldMatrixInv = gml::mat33(GetWorldMatrix()).inversed();
	auto p = gml::transform_point(worldMatrixInv, pos);
	return p;
}

gml::vec2 SceneNode::WorldToParent(const gml::vec2& pos)
{
	gml::mat33 worldMatrixInv = gml::mat33(m_iparent.GetWorldMatrix()).inversed();
	auto p = gml::transform_point(worldMatrixInv, pos);
	return p;
}

void SceneNode::SetRenderingOrder(uint32_t & order)
{
	m_renderingOrder = order++;
	TraversalChildren([&](::SceneNode* child)
	{
		child->SetRenderingOrder(order);
	});
}

void SceneNode::OnMessage(const g2d::Message& message)
{
	auto onCursorEnterFrom = [&](g2d::Component* component)
	{
		component->OnMessage(message);
	};
	DispatchComponent(onCursorEnterFrom);
}

void SceneNode::OnCursorEnterFrom(::SceneNode* adjacency)
{
	auto onCursorEnterFrom = [&](g2d::Component* component)
	{
		component->OnCursorEnterFrom(adjacency, GetMouse(), GetKeyboard());;
	};
	DispatchComponent(onCursorEnterFrom);
}

void SceneNode::OnCursorLeaveTo(::SceneNode* adjacency)
{
	auto onCursorLeaveTo = [&](g2d::Component* component)
	{
		component->OnCursorLeaveTo(adjacency, GetMouse(), GetKeyboard());
	};
	DispatchComponent(onCursorLeaveTo);
}

void SceneNode::OnCursorHovering()
{
	auto onCursorHovering = [](g2d::Component* component)
	{
		component->OnCursorHovering(GetMouse(), GetKeyboard());
	};
	DispatchComponent(onCursorHovering);
}

void SceneNode::OnClick(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		auto onClick = [](g2d::Component* component)
		{
			component->OnLClick(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onClick);
	}
	else if (button == g2d::MouseButton::Right)
	{
		auto onClick = [](g2d::Component* component)
		{
			component->OnRClick(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onClick);
	}
	else
	{
		auto onClick = [](g2d::Component* component)
		{
			component->OnMClick(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onClick);
	}
}

void SceneNode::OnDoubleClick(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		auto onDoubleClick = [](g2d::Component* component)
		{
			component->OnLDoubleClick(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDoubleClick);
	}
	else if (button == g2d::MouseButton::Right)
	{
		auto onDoubleClick = [](g2d::Component* component)
		{
			component->OnRDoubleClick(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDoubleClick);
	}
	else
	{
		auto onDoubleClick = [](g2d::Component* component)
		{
			component->OnMDoubleClick(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDoubleClick);
	}
}

void SceneNode::OnDragBegin(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		auto onDragBegin = [](g2d::Component* component)
		{
			component->OnLDragBegin(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragBegin);
	}
	else if (button == g2d::MouseButton::Right)
	{
		auto onDragBegin = [](g2d::Component* component)
		{
			component->OnRDragBegin(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragBegin);
	}
	else
	{
		auto onDragBegin = [](g2d::Component* component)
		{
			component->OnMDragBegin(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragBegin);
	}
}

void SceneNode::OnDragging(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		auto onDragging = [](g2d::Component* component)
		{
			component->OnLDragging(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragging);
	}
	else if (button == g2d::MouseButton::Right)
	{
		auto onDragging = [](g2d::Component* component)
		{
			component->OnRDragging(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragging);
	}
	else
	{
		auto onDragging = [](g2d::Component* component)
		{
			component->OnMDragging(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragging);
	}
}

void SceneNode::OnDragEnd(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		auto onDragEnd = [](g2d::Component* component)
		{
			component->OnLDragEnd(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragEnd);
	}
	else if (button == g2d::MouseButton::Right)
	{
		auto onDragEnd = [](g2d::Component* component)
		{
			component->OnRDragEnd(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragEnd);
	}
	else
	{
		auto onDragEnd = [](g2d::Component* component)
		{
			component->OnMDragEnd(GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDragEnd);
	}
}

void SceneNode::OnDropping(::SceneNode* dropped, g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		auto onDropping = [&](g2d::Component* component)
		{
			component->OnLDropping(dropped, GetMouse(), ::GetKeyboard());
		};
		DispatchComponent(onDropping);
	}
	else if (button == g2d::MouseButton::Right)
	{
		auto onDropping = [&](g2d::Component* component)
		{
			component->OnRDropping(dropped, GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDropping);
	}
	else
	{
		auto onDropping = [&](g2d::Component* component)
		{
			component->OnMDropping(dropped, GetMouse(), GetKeyboard());
		};
		DispatchComponent(onDropping);
	}
}

void SceneNode::OnDropTo(::SceneNode* dropped, g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		auto dropTo = [&](g2d::Component* component)
		{
			component->OnLDropTo(dropped, GetMouse(), GetKeyboard());
		};
		DispatchComponent(dropTo);
	}
	else if (button == g2d::MouseButton::Right)
	{
		auto dropTo = [&](g2d::Component* component)
		{
			component->OnRDropTo(dropped, GetMouse(), GetKeyboard());
		};
		DispatchComponent(dropTo);
	}
	else
	{
		auto dropTo = [&](g2d::Component* component)
		{
			component->OnMDropTo(dropped, GetMouse(), GetKeyboard());
		};
		DispatchComponent(dropTo);
	}
}

void SceneNode::OnKeyPress(g2d::KeyCode key)
{
	OnKeyPressComponentsAndChildren(key);
}

void SceneNode::OnKeyPressingBegin(g2d::KeyCode key)
{
	OnKeyPressingBeginComponentsAndChildren(key);
}

void SceneNode::OnKeyPressing(g2d::KeyCode key)
{
	OnKeyPressingComponentsAndChildren(key);
}

void SceneNode::OnKeyPressingEnd(g2d::KeyCode key)
{
	OnKeyPressingEndComponentsAndChildren(key);
}


