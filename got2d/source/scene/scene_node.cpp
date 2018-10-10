#include "scene_node.h"
#include "scene.h"
#include "cxx_math/cxx_point.h"

//*************************************************************
// overrides
//*************************************************************
g2d::Scene* SceneNode::GetScene()
{
	return GetSceneImpl();
}

g2d::SceneNode * SceneNode::GetParentNode()
{
	return GetParentNodeImpl();
}

g2d::SceneNode * SceneNode::GetFirstChild()
{
	return mChildrenNodes.First();
}

g2d::SceneNode * SceneNode::GetPrevSiblingNode()
{
	return GetPrevSiblingImpl();
}

g2d::SceneNode * SceneNode::GetNextSiblingNode()
{
	return GetNextSiblingImpl();
}

g2d::SceneNode * SceneNode::GetLastChild()
{
	return mChildrenNodes.Last();
}

g2d::SceneNode * SceneNode::GetChildByIndex(unsigned int index)
{
	return mChildrenNodes.At(index);
}

unsigned int SceneNode::GetChildCount() const
{
	return mChildrenNodes.GetCount();
}


g2d::SceneNode * SceneNode::CreateChild()
{
	::SceneNode* pChild = new SceneNode(mScene, this);
	mChildrenNodes.Add(pChild);
	mScene->SetRenderingOrderDirty(this);
	return pChild;
}


void SceneNode::MoveToFront()
{
	unsigned int oldIndex = mChildIndex;
	if (mParentNode->mChildrenNodes.Move(mChildIndex, mParentNode->mChildrenNodes.GetCount() - 1))
	{
		mScene->SetRenderingOrderDirty(mParentNode->mChildrenNodes.At(oldIndex));
	}
}

void SceneNode::MoveToBack()
{
	if (mParentNode->mChildrenNodes.Move(mChildIndex, 0))
	{
		mScene->SetRenderingOrderDirty(this);
	}
}

void SceneNode::MovePrev()
{
	if (mParentNode->mChildrenNodes.Move(mChildIndex, mChildIndex - 1))
	{
		mScene->SetRenderingOrderDirty(this);
	}
}

void SceneNode::MoveNext()
{
	unsigned int oldIndex = mChildIndex;
	if (mParentNode->mChildrenNodes.Move(mChildIndex, mChildIndex + 1))
	{
		mScene->SetRenderingOrderDirty(mParentNode->mChildrenNodes.At(oldIndex));
	}
}

bool SceneNode::AddComponent(g2d::Component* component, bool autoRelease)
{
	ENSURE(component != nullptr);
	bool successed = mComponenList.Add(this, component, autoRelease);
	if (successed)
	{
		mScene->GetSpatialGraph().Add(component);
		mScene->SetRenderingOrderDirty(this);
		return true;
	}
	else
	{
		return false;
	}
}

bool SceneNode::RemoveComponent(g2d::Component * component)
{
	ENSURE(component != nullptr);
	if (mComponenList.Remove(component, false))
	{
		mScene->GetSpatialGraph().Remove(component);
		return true;
	}
	else
	{
		return false;
	}
}

bool SceneNode::RemoveComponentWithoutRelease(g2d::Component * component)
{
	ENSURE(component != nullptr);
	return mComponenList.Remove(component, true);
}

bool SceneNode::HasComponent(g2d::Component* comp) const
{
	return mComponenList.Exist(comp);
}

bool SceneNode::IsComponentAutoRelease(g2d::Component * component) const
{
	ENSURE(component != nullptr);
	return mComponenList.IsAutoRelease(component);
}

g2d::Component* SceneNode::GetComponentByIndex(unsigned int index)
{
	ENSURE(index < mComponenList.GetCount());
	return mComponenList.At(index);
}

unsigned int SceneNode::GetComponentCount() const
{
	return mComponenList.GetCount();
}

const cxx::float2x3 & SceneNode::GetLocalMatrix()
{
	return mTransform.GetMatrix();
}

const cxx::float2x3& SceneNode::GetWorldMatrix()
{
	return mTransform.GetWorldMatrix();
}

g2d::SceneNode* SceneNode::SetPosition(const cxx::point2d<float>& position)
{
	mComponenList.OnPositionChanging(position);
	mTransform.SetPosition(position);
	NotifyChildrenTransformChanged();
	mComponenList.OnPositionChanged(position);
	return this;
}

cxx::point2d<float> SceneNode::GetPosition() const
{
	return mTransform.GetPosition();
}

g2d::SceneNode * SceneNode::SetWorldPosition(const cxx::point2d<float>& position)
{
	auto localPos = WorldToParent(position);
	return SetPosition(localPos);
}

cxx::point2d<float> SceneNode::GetWorldPosition()
{
	return mTransform.GetPosition();
}

g2d::SceneNode * SceneNode::SetRightDirection(const cxx::nfloat2 & right)
{
	cxx::nfloat2 oldRight = mTransform.GetWorldRightDirection();
	auto cos = dot(right, oldRight);
	bool ccw = cross(right, oldRight) < 0;
	if (cxx::is_equal(cos, -1.0f))
	{
		cxx::radian<float> r = mTransform.GetRotation() + cxx::radian<float>(cxx::constants<float>::pi);
		SetRotation(normalized(r));
	}
	else if (!cxx::is_equal(cos, 1.0f))
	{
		float acosr = acos(cos);
		cxx::radian<float> rdiff = cxx::radian<float>(ccw ? acosr : -acosr);
		cxx::radian<float> r = mTransform.GetRotation() + rdiff;
		SetRotation(normalized(r));
	}
	return this;
}


const cxx::nfloat2 SceneNode::GetRightDirection()
{
	return mTransform.GetWorldRightDirection();
}

g2d::SceneNode * SceneNode::SetUpDirection(const cxx::nfloat2 & up)
{
	cxx::nfloat2 oldUp = mTransform.GetWorldUpDirection();
	auto cos = dot(up, oldUp);
	bool ccw = cross(up, oldUp) < 0;
	if (cxx::is_equal(cos, -1.0f))
	{
		cxx::radian<float> r = mTransform.GetRotation() + cxx::radian<float>(cxx::constants<float>::pi);
		SetRotation(normalized(r));
	}
	else if (!cxx::is_equal(cos, 1.0f))
	{
		float acosr = acos(cos);
		cxx::radian<float> rdiff = cxx::radian<float>(ccw ? acosr : -acosr);
		cxx::radian<float> r = mTransform.GetRotation() + rdiff;
		SetRotation(normalized(r));
	}
	return this;
}

const cxx::nfloat2 SceneNode::GetUpDirection()
{
	return mTransform.GetWorldUpDirection();
}

g2d::SceneNode* SceneNode::SetPivot(const cxx::float2& pivot)
{
	mComponenList.OnPivotChanging(pivot);
	mTransform.SetPivot(pivot);
	NotifyChildrenTransformChanged();
	mComponenList.OnPivotChanged(pivot);
	return this;
}

const cxx::float2 & SceneNode::GetPivot() const
{
	return mTransform.GetPivot();
}

g2d::SceneNode* SceneNode::SetScale(const cxx::float2& scale)
{
	mComponenList.OnScaleChanging(scale);
	mTransform.SetScale(scale);
	NotifyChildrenTransformChanged();
	mComponenList.OnScaleChanged(scale);
	return this;
}

const cxx::float2 & SceneNode::GetScale() const
{
	return mTransform.GetScale();
}

g2d::SceneNode* SceneNode::SetRotation(cxx::radian<float> r)
{
	mComponenList.OnRotateChanging(r);
	mTransform.SetRotation(r);
	NotifyChildrenTransformChanged();
	mComponenList.OnRotateChanged(r);
	return this;
}

cxx::radian<float> SceneNode::GetRotation() const
{
	return mTransform.GetRotation();
}

void SceneNode::SetVisible(bool visible)
{
	mIsVisible = visible;
}

bool SceneNode::IsVisible() const
{
	return mIsVisible;
}

void SceneNode::SetStatic(bool s)
{
	if (mIsStatic != s)
	{
		mIsStatic = s;
		AdjustSpatial();
	}
}

bool SceneNode::IsStatic() const
{
	return mIsStatic;
}

void SceneNode::SetCameraVisibleMask(unsigned int mask, bool recursive)
{
	mCameraVisibleMask = mask;
	if (recursive)
	{
		mChildrenNodes.Traversal([&](::SceneNode* child)
		{
			child->SetCameraVisibleMask(mask, true);
		});
	}
}

unsigned int SceneNode::GetCameraVisibleMask() const
{
	return mCameraVisibleMask;
}

unsigned int SceneNode::GetChildIndex() const
{
	return mChildIndex;
}

bool SceneNode::IsRemoved() const {
	return mIsRemoved;
}

cxx::point2d<float> SceneNode::WorldToLocal(const cxx::point2d<float>& pos)
{
	cxx::float3x3 worldMatrixInv = inversed(to_matrix3x3(GetWorldMatrix()));
	return transform(worldMatrixInv, pos);
}

cxx::point2d<float> SceneNode::WorldToParent(const cxx::point2d<float>& pos)
{
	cxx::float3x3 worldMatrixInv = inversed(to_matrix3x3(mParentNode->GetWorldMatrix()));
	return transform(worldMatrixInv, pos);
}

void SceneNode::Release()
{
	/**
	*	deleteing node will affect continuity of rendering order, but wont change the order,
	*	so we do nothing when deleting nodes
	*/
	mIsRemoved = true;
	mParentNode->mChildrenNodes.Remove(this);
	mScene->OnRemoveSceneNode(this);
}
//*************************************************************
// functions
//*************************************************************
SceneNode::SceneNode(::Scene* scene, ::SceneNode* parent)
	: mScene(scene)
	, mParentNode(parent)
	, mTransform(parent == nullptr ? nullptr : parent->mTransform)
{

}

SceneNode::~SceneNode()
{
	mComponenList.Traversal([&](g2d::Component* component)
	{
		mScene->GetSpatialGraph().Remove(component);
	});
}

unsigned int SceneNode::GetRenderingOrder() const
{
	return mRenderingOrder;
}

::Scene* SceneNode::GetSceneImpl()
{
	return mScene;
}

::SceneNode* SceneNode::GetParentNodeImpl()
{
	return mParentNode;
}

::SceneNode* SceneNode::GetPrevSiblingImpl()
{
	if (mChildIndex == 0)
	{
		return nullptr;
	}
	return mParentNode->mChildrenNodes.At(mChildIndex - 1);
}

::SceneNode* SceneNode::GetNextSiblingImpl()
{
	if (mChildIndex == mParentNode->mChildrenNodes.GetCount() - 1)
	{
		return nullptr;
	}
	return mParentNode->mChildrenNodes.At(mChildIndex + 1);
}
void SceneNode::AdjustRenderingOrder()
{
	auto index = mRenderingOrder + 1;
	mChildrenNodes.Traversal([&](::SceneNode* child)
	{
		child->SetRenderingOrder(index);
	});

	::SceneNode* current = this;
	while (current != nullptr)
	{
		::SceneNode* next = current->GetNextSiblingImpl();
		while (next != nullptr)
		{
			next->SetRenderingOrder(index);
			next = next->GetNextSiblingImpl();
		}
		current = current->GetParentNodeImpl();
	}
}

void SceneNode::NotifyChildrenTransformChanged()
{
	mTransform.NotifyChildrenTransformChanged();
	mChildrenNodes.Traversal([](::SceneNode* child)
	{
		child->NotifyChildrenTransformChanged();
	});
	mTranformChanged = true;
}

void SceneNode::AdjustSpatial()
{
	mComponenList.Traversal([&](g2d::Component* component)
	{
		mScene->GetSpatialGraph().Add(component);
	});
}

void SceneNode::OnUpdate(unsigned int deltaTime)
{
	mComponenList.OnUpdate(deltaTime);
	if (mTranformChanged)
	{
		// static object need to adjust location in
		// quad tree, at this time, we may adjust it
		// before visibility testing process, aka 
		// rendering process
		if (IsStatic())
		{
			AdjustSpatial();
		}
		mComponenList.OnPostUpdateTransformChanged();
		mTranformChanged = false;
	}

	mChildrenNodes.OnUpdate(deltaTime);
}

void SceneNode::SetRenderingOrder(unsigned int & order)
{
	mRenderingOrder = order++;
	mComponenList.Traversal([&](g2d::Component* component)
	{
		component->_SetRenderingOrder_Internal(order);
	});
	mChildrenNodes.Traversal([&](::SceneNode* child)
	{
		child->SetRenderingOrder(order);
	});
}

void SceneNode::SetRenderingOrderOnly(unsigned int order)
{
	mRenderingOrder = order;
}

void SceneNode::OnMessage(const g2d::Message& message)
{
	mComponenList.OnMessage(message);
	mChildrenNodes.OnMessage(message);
}

void SceneNode::OnCursorEnterFrom(::SceneNode* adjacency)
{
	mComponenList.OnCursorEnterFrom(adjacency);
}

void SceneNode::OnCursorLeaveTo(::SceneNode* adjacency)
{
	mComponenList.OnCursorLeaveTo(adjacency);
}

void SceneNode::OnCursorHovering()
{

	mComponenList.OnCursorHovering();
}

void SceneNode::OnClick(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		mComponenList.OnLClick();
	}
	else if (button == g2d::MouseButton::Right)
	{
		mComponenList.OnRClick();
	}
	else
	{
		mComponenList.OnMClick();
	}
}

void SceneNode::OnDoubleClick(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		mComponenList.OnLDoubleClick();
	}
	else if (button == g2d::MouseButton::Right)
	{
		mComponenList.OnRDoubleClick();
	}
	else
	{
		mComponenList.OnMDoubleClick();
	}
}

void SceneNode::OnDragBegin(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		mComponenList.OnLDragBegin();
	}
	else if (button == g2d::MouseButton::Right)
	{
		mComponenList.OnRDragBegin();
	}
	else
	{
		mComponenList.OnMDragBegin();
	}
}

void SceneNode::OnDragging(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		mComponenList.OnLDragging();
	}
	else if (button == g2d::MouseButton::Right)
	{
		mComponenList.OnRDragging();
	}
	else
	{
		mComponenList.OnMDragging();
	}
}

void SceneNode::OnDragEnd(g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		mComponenList.OnLDragEnd();
	}
	else if (button == g2d::MouseButton::Right)
	{
		mComponenList.OnRDragEnd();
	}
	else
	{
		mComponenList.OnMDragEnd();
	}
}

void SceneNode::OnDropping(::SceneNode* dropped, g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		mComponenList.OnLDropping(dropped);
	}
	else if (button == g2d::MouseButton::Right)
	{
		mComponenList.OnRDropping(dropped);
	}
	else
	{
		mComponenList.OnMDropping(dropped);
	}
}

void SceneNode::OnDropTo(::SceneNode* dropped, g2d::MouseButton button)
{
	if (button == g2d::MouseButton::Left)
	{
		mComponenList.OnLDropTo(dropped);
	}
	else if (button == g2d::MouseButton::Right)
	{
		mComponenList.OnRDropTo(dropped);
	}
	else
	{
		mComponenList.OnMDropTo(dropped);
	}
}

void SceneNode::OnKeyPress(g2d::KeyCode key)
{
	mComponenList.OnKeyPress(key);
	mChildrenNodes.OnKeyPress(key);
}

void SceneNode::OnKeyPressingBegin(g2d::KeyCode key)
{
	mComponenList.OnKeyPressingBegin(key);
	mChildrenNodes.OnKeyPressingBegin(key);
}

void SceneNode::OnKeyPressing(g2d::KeyCode key)
{
	mComponenList.OnKeyPressing(key);
	mChildrenNodes.OnKeyPressing(key);
}

void SceneNode::OnKeyPressingEnd(g2d::KeyCode key)
{
	mComponenList.OnKeyPressingEnd(key);
	mChildrenNodes.OnKeyPressingEnd(key);
}

RootSceneNode::RootSceneNode(::Scene * scene)
	: SceneNode(scene, nullptr)
{
}


void RootSceneNode::AdjustRenderingOrder()
{
	GetSceneImpl()->AdjustRenderingOrder();
}