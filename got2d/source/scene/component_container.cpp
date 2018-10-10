#include "component_container.h"
#include "../system_blackboard.h"
#include "../input/input.h"
#include "scene_node.h"

ComponentContainer::~ComponentContainer()
{
	for (ComponentRecord& c : mComponents)
	{
		if (c.AutoRelease)
		{
			c.ComponentPtr->Release();
		}
	}
	mComponents.clear();

	DestroyRemovedComponents();
}

bool ComponentContainer::Add(g2d::SceneNode* parent, g2d::Component* component, bool autoRelease)
{
	if (mComponents.empty())
	{
		mComponents.push_back({ component , autoRelease });

		component->_SetSceneNode_Internal(parent);
		component->OnInitial();

		mCachedComponentsChanged = true;
		return true;
	}
	else
	{
		//check whether the component is removed.
		bool removed = false;
		{
			auto end = mRemovedComponents.end();
			auto found = std::find_if(mRemovedComponents.begin(), end, [component](const ComponentRecord& c) {
				return component == c.ComponentPtr;
			});

			if (found != end)
			{
				mRemovedComponents.erase(found);
				removed = true;
			}
		}

		//check whether the component is already attached.
		if (!removed)
		{
			auto end = mComponents.end();
			auto found = std::find_if(mComponents.begin(), end, [component](const ComponentRecord& c) {
				return component == c.ComponentPtr;
			});

			if (found != end)
			{
				return true;
			}
		}

		int cOrder = component->GetExecuteOrder();
		int lastOrder = mComponents.back().ComponentPtr->GetExecuteOrder();
		if (cOrder < lastOrder)
		{
			mComponents.push_back({ component, autoRelease });
		}
		else //try insert
		{
			auto itCur = mComponents.begin();
			auto itEnd = mComponents.end();
			while (itCur != itEnd)
			{
				if (itCur->ComponentPtr->GetExecuteOrder() > cOrder)
				{
					break;
				}
				++itCur;
			}
			mComponents.insert(itCur, { component, autoRelease });
		}
		component->_SetSceneNode_Internal(parent);
		component->OnInitial();
		mCachedComponentsChanged = true;
		return true;
	}
}

bool ComponentContainer::Remove(g2d::Component* component, bool forceNotReleased)
{
	{
		auto end = mRemovedComponents.end();
		if (end != std::find_if(mRemovedComponents.begin(), end,
			[component](const ComponentRecord& c) {
			return component == c.ComponentPtr;
		}))
		{
			return false;
		}
	}

	{
		auto end = mComponents.end();
		auto found = std::find_if(mComponents.begin(), end,
			[component](ComponentRecord& c) {
			return component == c.ComponentPtr;
		});

		if (found != end)
		{
			mRemovedComponents.push_back({ component, !forceNotReleased && found->AutoRelease });
			mComponents.erase(found);
			mCachedComponentsChanged = true;
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool ComponentContainer::IsAutoRelease(g2d::Component* component) const
{
	{
		auto end = mRemovedComponents.end();
		auto found = std::find_if(mRemovedComponents.begin(), end,
			[component](const ComponentRecord& c) {
			return component == c.ComponentPtr;
		});

		if (found != end)
		{
			return found->AutoRelease;
		}
	}


	{
		auto end = mComponents.end();
		auto found = std::find_if(mComponents.begin(), end,
			[component](const ComponentRecord& c) {
			return component == c.ComponentPtr;
		});

		if (found != end)
		{
			return found->AutoRelease;
		}
	}

	return false;
}

g2d::Component* ComponentContainer::At(unsigned int index) const
{
	return mComponents.at(index).ComponentPtr;
}

bool ComponentContainer::Exist(g2d::Component* component) const
{
	return mComponents.end() != std::find_if(
		mComponents.begin(), mComponents.end(),
		[component](const ComponentRecord& c) {
		return component == c.ComponentPtr;
	});
}

unsigned int ComponentContainer::GetCount() const
{
	return static_cast<unsigned int>(mComponents.size());
}

//========================================================================
//	Event Dispatcher 
//========================================================================
void ComponentContainer::OnPositionChanging(const cxx::point2d<float> & p)
{
	for (ComponentRecord& c : mComponents)
	{
		c.ComponentPtr->OnPositionChanging(p);
	}
}

void ComponentContainer::OnRotateChanging(cxx::radian<float> r)
{
	for (ComponentRecord& c : mComponents)
	{
		c.ComponentPtr->OnRotateChanging(r);
	}
}

void ComponentContainer::OnRotateChanged(cxx::radian<float> r)
{
	for (ComponentRecord& c : mComponents)
	{
		c.ComponentPtr->OnRotateChanged(r);
	}
}

void ComponentContainer::OnScaleChanging(const cxx::float2 & s)
{
	for (ComponentRecord& c : mComponents)
	{
		c.ComponentPtr->OnScaleChanging(s);
	}
}

void ComponentContainer::OnScaleChanged(const cxx::float2 & s)
{
	for (ComponentRecord& c : mComponents)
	{
		c.ComponentPtr->OnScaleChanged(s);
	}
}

void ComponentContainer::OnPostUpdateTransformChanged()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnPostUpdateTransformChanged();
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMessage(const g2d::Message & message)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMessage(message);
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnUpdate(unsigned int deltaTime)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnUpdate(deltaTime);
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnCursorEnterFrom(::SceneNode * adjacency)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnCursorEnterFrom(adjacency, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnCursorHovering()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnCursorHovering(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnCursorLeaveTo(::SceneNode * adjacency)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnCursorLeaveTo(adjacency, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnLClick()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnLClick(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnRClick()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnRClick(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMClick()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMClick(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnLDoubleClick()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnLDoubleClick(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnRDoubleClick()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnRDoubleClick(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMDoubleClick()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMDoubleClick(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnLDragBegin()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnLDragBegin(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnRDragBegin()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnRDragBegin(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMDragBegin()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMDragBegin(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnLDragging()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnLDragging(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnRDragging()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnRDragging(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMDragging()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMDragging(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnLDragEnd()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnLDragEnd(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnRDragEnd()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnRDragEnd(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMDragEnd()
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMDragEnd(GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnLDropping(::SceneNode * dropped)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnLDropping(dropped, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnRDropping(::SceneNode * dropped)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnRDropping(dropped, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMDropping(::SceneNode * dropped)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMDropping(dropped, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnLDropTo(::SceneNode * dropped)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnLDropTo(dropped, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnRDropTo(::SceneNode * dropped)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnRDropTo(dropped, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnMDropTo(::SceneNode * dropped)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnMDropTo(dropped, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnKeyPress(g2d::KeyCode key)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnKeyPress(key, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnKeyPressingBegin(g2d::KeyCode key)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnKeyPressingBegin(key, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnKeyPressing(g2d::KeyCode key)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnKeyPressing(key, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}

void ComponentContainer::OnKeyPressingEnd(g2d::KeyCode key)
{
	CacheComponentsForTraversal();
	for (g2d::Component* c : mCacheComponentsForTraversal)
	{
		c->OnKeyPressingEnd(key, GetMouse(), GetKeyboard());
	}
	DestroyRemovedComponents();
}


void ComponentContainer::CacheComponentsForTraversal()
{
	if (mCachedComponentsChanged)
	{
		DestroyRemovedComponents();

		mCacheComponentsForTraversal.clear();
		for (ComponentRecord& c : mComponents)
		{
			mCacheComponentsForTraversal.push_back(c.ComponentPtr);
		}

		mCachedComponentsChanged = false;
	}
}

void ComponentContainer::DestroyRemovedComponents()
{
	if (mRemovedComponents.size() != 0)
	{
		for (ComponentRecord& c : mRemovedComponents)
		{
			if (c.AutoRelease)
			{
				c.ComponentPtr->Release();
			}
		}
		mRemovedComponents.clear();
	}
}