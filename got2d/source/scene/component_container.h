#pragma once
#include <vector>
#include "g2dscene.h"

class SceneNode;
class Scene;
class Engine;


class ComponentContainer
{
public:
	struct ComponentRecord
	{
		ComponentRecord(g2d::Component* c, bool ar)
			: ComponentPtr(c)
			, AutoRelease(ar)
		{ }

		g2d::Component* ComponentPtr = nullptr;
		bool AutoRelease = false;
	};

public:
	ComponentContainer() = default;

	~ComponentContainer();

	bool Add(g2d::SceneNode* parent, g2d::Component* component, bool autoRelease);

	bool Remove(g2d::Component* component, bool forceNotReleased);

	bool IsAutoRelease(g2d::Component* component) const;

	g2d::Component* At(unsigned int index) const;

	bool Exist(g2d::Component*) const;

	unsigned int GetCount() const;

	template<typename TVisitor> void Traversal(TVisitor f)
	{
		for (ComponentRecord& c : mComponents)
		{
			f(c.ComponentPtr);
		}

	}

public:
	void OnPositionChanging(const cxx::point2d<float>& p);

	void OnPositionChanged(const cxx::point2d<float>& p);

	void OnPivotChanging(const cxx::float2& p);

	void OnPivotChanged( const cxx::float2& p);

	void OnRotateChanging(cxx::radian<float> r);

	void OnRotateChanged(cxx::radian<float> r);

	void OnScaleChanging(const cxx::float2& s);

	void OnScaleChanged(const cxx::float2& s);

	void OnPostUpdateTransformChanged();

	void OnMessage(const g2d::Message& message);

	void OnUpdate(unsigned int deltaTime);

	void OnCursorEnterFrom(::SceneNode* adjacency);

	void OnCursorHovering();

	void OnCursorLeaveTo(::SceneNode* adjacency);

	void OnLClick();

	void OnRClick();

	void OnMClick();

	void OnLDoubleClick();

	void OnRDoubleClick();

	void OnMDoubleClick();

	void OnLDragBegin();

	void OnRDragBegin();

	void OnMDragBegin();

	void OnLDragging();

	void OnRDragging();

	void OnMDragging();

	void OnLDragEnd();

	void OnRDragEnd();

	void OnMDragEnd();

	void OnLDropping(::SceneNode* dropped);

	void OnRDropping(::SceneNode* dropped);

	void OnMDropping(::SceneNode* dropped);

	void OnLDropTo(::SceneNode* dropped);

	void OnRDropTo(::SceneNode* dropped);

	void OnMDropTo(::SceneNode* dropped);

	void OnKeyPress(g2d::KeyCode key);

	void OnKeyPressingBegin(g2d::KeyCode key);

	void OnKeyPressing(g2d::KeyCode key);

	void OnKeyPressingEnd(g2d::KeyCode key);

private:
	void CacheComponentsForTraversal();

	void DestroyRemovedComponents();

	std::vector<ComponentRecord> mComponents;
	std::vector<ComponentRecord> mRemovedComponents;
	std::vector<g2d::Component*> mCacheComponentsForTraversal;
	bool mCachedComponentsChanged = true;
};