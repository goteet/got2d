#include "scene.h"
#include <algorithm>

BaseNode::BaseNode()
	: m_position(gml::vec2::zero())
	, m_pivot(gml::vec2::zero())
	, m_scale(gml::vec2::one())
	, m_rotation(0)
	, m_matrixLocal(gml::mat32::identity())
{
}

BaseNode::~BaseNode()
{
	for (auto& c : m_components)
	{
		if (c.AutoRelease)
		{
			c.ComponentPtr->Release();
		}
	}
	m_components.clear();
	DelayRemoveComponents();
	EmptyChildren();
}

void BaseNode::OnUpdateChildren(uint32_t deltaTime)
{
	auto onUpdate = [&](::SceneNode* child)
	{
		child->OnUpdate(deltaTime);
	};
	DispatchChildren(onUpdate);
}

void BaseNode::OnMessageComponentsAndChildren(const g2d::Message& message)
{
	auto cf = [&](g2d::Component* component)
	{
		component->OnMessage(message);
	};
	auto nf = [&](::SceneNode* child)
	{
		child->OnMessage(message);
	};
	DispatchRecursive(cf, nf);
}

void BaseNode::OnKeyPressComponentsAndChildren(g2d::KeyCode key)
{
	auto cf = [&](g2d::Component* component)
	{
		component->OnKeyPress(key, GetMouse(), GetKeyboard());
	};
	auto nf = [&](::SceneNode* child)
	{
		child->OnKeyPress(key);
	};
	DispatchRecursive(cf, nf);
}

void BaseNode::OnKeyPressingBeginComponentsAndChildren(g2d::KeyCode key)
{
	auto cf = [&](g2d::Component* component)
	{
		component->OnKeyPressingBegin(key, GetMouse(), GetKeyboard());
	};
	auto nf = [&](::SceneNode* child)
	{
		child->OnKeyPressingBegin(key);
	};
	DispatchRecursive(cf, nf);
}

void BaseNode::OnKeyPressingComponentsAndChildren(g2d::KeyCode key)
{
	auto cf = [&](g2d::Component* component)
	{
		component->OnKeyPressing(key, GetMouse(), GetKeyboard());
	};
	auto nf = [&](::SceneNode* child)
	{
		child->OnKeyPressing(key);
	};
	DispatchRecursive(cf, nf);
}

void BaseNode::OnKeyPressingEndComponentsAndChildren(g2d::KeyCode key)
{
	auto cf = [&](g2d::Component* component)
	{
		component->OnKeyPressingEnd(key, GetMouse(), GetKeyboard());
	};
	auto nf = [&](::SceneNode* child)
	{
		child->OnKeyPressingEnd(key);
	};
	DispatchRecursive(cf, nf);
}

void BaseNode::EmptyChildren()
{
	for (auto& child : m_children)
	{
		delete child;
	}
	m_children.clear();
	DelayRemoveChildren();
}

bool BaseNode::_AddComponent(g2d::Component* component, bool autoRelease, g2d::SceneNode* node)
{
	ENSURE(component != nullptr);
	if (m_components.empty())
	{
		m_components.push_back({ component , autoRelease });
		component->SetSceneNode(node);
		component->OnInitial();
		m_componentsChanged = true;
		return true;
	}
	else
	{
		auto itEndReleased = std::end(m_releasedComponents);
		auto itFoundReleased = std::find_if(std::begin(m_releasedComponents), itEndReleased,
			[component](const NodeComponent& c) { return component == c.ComponentPtr; });
		if (itFoundReleased != itEndReleased)
		{
			m_releasedComponents.erase(itFoundReleased);
			return true;
		}

		auto itEnd = std::end(m_components);
		auto itFound = std::find_if(std::begin(m_components), itEnd,
			[component](const NodeComponent& c) { return component == c.ComponentPtr; });
		if (itFound != itEnd)
		{
			return false;
		}

		int cOrder = component->GetExecuteOrder();
		int lastOrder = m_components.back().ComponentPtr->GetExecuteOrder();
		if (cOrder < lastOrder)
		{
			m_components.push_back({ component, autoRelease });
			component->SetSceneNode(node);
			component->OnInitial();
		}
		else //try insert
		{
			auto itCur = std::begin(m_components);
			auto itEnd = std::end(m_components);
			for (; itCur != itEnd; itCur++)
			{
				if (itCur->ComponentPtr->GetExecuteOrder() > cOrder)
					break;
			}
			m_components.insert(itCur, { component, autoRelease });
			component->SetSceneNode(node);
			component->OnInitial();
		}
		m_componentsChanged = true;
		return true;
	}
}

bool BaseNode::_RemoveComponent(g2d::Component* component, bool forceNotReleased)
{
	auto itEndReleased = std::end(m_releasedComponents);
	if (itEndReleased != std::find_if(std::begin(m_releasedComponents), itEndReleased,
		[component](const NodeComponent& c) { return component == c.ComponentPtr; }))
	{
		return false;
	}

	auto itEnd = std::end(m_components);
	auto itFound = std::find_if(std::begin(m_components), itEnd,
		[component](NodeComponent& c) {return c.ComponentPtr == component; });

	if (itFound != itEnd)
	{
		m_releasedComponents.push_back({ component, !forceNotReleased && itFound->AutoRelease });
		m_components.erase(itFound);
		m_componentsChanged = true;
		return true;
	}
	else
	{
		return false;
	}
}

bool BaseNode::_IsComponentAutoRelease(g2d::Component* component) const
{
	bool forcedNotReleased = false;
	auto itEndReleased = std::end(m_releasedComponents);
	auto itFoundReleased = std::find_if(std::begin(m_releasedComponents), itEndReleased,
		[component](const NodeComponent& c) { return component == c.ComponentPtr; });
	if (itFoundReleased != itEndReleased)
	{
		forcedNotReleased = itFoundReleased->AutoRelease;
	}

	auto itEnd = std::end(m_components);
	auto itFound = std::find_if(std::begin(m_components), itEnd,
		[component](const NodeComponent& c) { return component == c.ComponentPtr; });
	if (itFound == itEnd)
	{
		return false;
	}
	return !forcedNotReleased && itFound->AutoRelease;
}

g2d::Component* BaseNode::_GetComponentByIndex(uint32_t index) const
{
	ENSURE(index < _GetComponentCount());
	return m_components.at(index).ComponentPtr;
}

std::vector<NodeComponent>& BaseNode::GetComponentCollection()
{
	if (m_componentsChanged)
	{
		DelayRemoveComponents();
		RecollectComponents();
		m_componentsChanged = false;
	}
	return m_collectionComponents;
}

std::vector<::SceneNode*>& BaseNode::GetChildrenCollection()
{
	if (m_childrenChanged)
	{
		DelayRemoveChildren();
		RecollectChildren();
		m_childrenChanged = false;
	}
	return m_collectionChildren;
}

void BaseNode::OnCreateChild(::Scene& scene, ::SceneNode& child)
{
	AdjustRenderingOrder();
	scene.GetSpatialGraph()->Add(*child.GetEntity());
	m_childrenChanged = true;
}

g2d::SceneNode* BaseNode::_CreateSceneNodeChild(::Scene& scene, ::SceneNode& parent, g2d::Entity& e, bool autoRelease)
{
	uint32_t childID = static_cast<uint32_t>(m_children.size());
	auto rst = new ::SceneNode(scene, parent, childID, &e, autoRelease);
	m_children.push_back(rst);
	OnCreateChild(scene, *rst);
	return rst;
}

g2d::SceneNode* BaseNode::_CreateSceneNodeChild(::Scene& scene, g2d::Entity& e, bool autoRelease)
{
	uint32_t childID = static_cast<uint32_t>(m_children.size());
	auto rst = new ::SceneNode(scene, childID, &e, autoRelease);
	m_children.push_back(rst);
	OnCreateChild(scene, *rst);
	return rst;
}

const gml::mat32& BaseNode::_GetLocalMatrix()
{
	if (m_matrixLocalDirty)
	{
		m_matrixLocal = gml::mat32::trsp(m_position, m_rotation, m_scale, m_pivot);
		m_matrixLocalDirty = false;
	}
	return m_matrixLocal;
}

void BaseNode::_SetVisibleMask(uint32_t mask, bool recurssive)
{
	m_visibleMask = mask;
	if (recurssive)
	{
		for (auto& child : m_children)
		{
			child->SetVisibleMask(mask, true);
		}
	}
}

void BaseNode::_SetPivot(const gml::vec2& pivot)
{
	SetLocalMatrixDirty();
	m_pivot = pivot;
}

void BaseNode::_SetScale(const gml::vec2& scale)
{
	SetLocalMatrixDirty();
	m_scale = scale;
}

void BaseNode::_SetPosition(const gml::vec2& position)
{
	SetLocalMatrixDirty();
	m_position = position;
}

void BaseNode::_SetRotation(gml::radian r)
{
	SetLocalMatrixDirty();
	m_rotation = r;
}

void BaseNode::SetLocalMatrixDirty()
{
	m_matrixLocalDirty = true;
}

::SceneNode* BaseNode::_GetChildByIndex(uint32_t index) const
{
	ENSURE(index < m_children.size());
	return m_children[index];
}

void BaseNode::MoveChild(uint32_t from, uint32_t to)
{
	ENSURE(to < m_children.size() && from < m_children.size());
	if (from == to)
		return;

	auto& siblings = m_children;
	auto fromNode = siblings[from];
	if (from > to)
	{
		for (auto itID = to; itID < from; itID++)
		{
			siblings[itID + 1] = siblings[itID];
			siblings[itID + 1]->SetChildIndex(itID + 1);
		}
	}
	else
	{
		for (auto itID = from; itID < to; itID++)
		{
			siblings[itID] = siblings[itID + 1];
			siblings[itID]->SetChildIndex(itID);
		}
	}
	siblings[to] = fromNode;
	fromNode->SetChildIndex(to);
	AdjustRenderingOrder();
}

void BaseNode::RemoveChildNode(::SceneNode* child)
{
	ENSURE(child != nullptr);

	auto itEndReleased = std::end(m_releasedChildren);
	if (itEndReleased != std::find(std::begin(m_releasedChildren), itEndReleased, child))
	{
		return;
	}

	auto itEnd = std::end(m_children);
	auto itFound = std::find(std::begin(m_children), itEnd, child);
	if (itFound != itEnd)
	{
		uint32_t index = child->GetChildIndex();
		m_releasedChildren.push_back(child);
		itFound = m_children.erase(itFound);
		itEnd = std::end(m_children);
		for (; itFound != itEnd; itFound++)
		{
			(*itFound)->SetChildIndex(index++);
		}
		m_childrenChanged = true;
	}
}

void BaseNode::DelayRemoveChildren()
{
	if (m_releasedChildren.size() != 0)
	{
		for (auto& rnode : m_releasedChildren)
		{
			rnode->EmptyChildren();
			delete rnode;
		}
		m_releasedChildren.clear();
	}
}

void BaseNode::DelayRemoveComponents()
{
	if (m_releasedComponents.size() != 0)
	{
		for (auto& rcomponent : m_releasedComponents)
		{
			if (rcomponent.AutoRelease)
			{
				rcomponent.ComponentPtr->Release();
			}
		}
		m_releasedComponents.clear();
	}
}

void BaseNode::RecollectChildren()
{
	m_collectionChildren.clear();
	for (auto& c : m_children)
	{
		m_collectionChildren.push_back(c);
	}
}

void BaseNode::RecollectComponents()
{
	m_collectionComponents.clear();
	for (auto& c : m_components)
	{
		m_collectionComponents.push_back(c);
	}
}