#include "g2dscene.h"

namespace g2d
{
	cxx::aabb2d<float> Component::GetWorldAABB() const
	{
		if (!GetLocalAABB().is_valid())
		{
			return GetLocalAABB();
		}
		else
		{
			float2x3 worldMatrix = GetSceneNode()->GetWorldMatrix();
			return transform(worldMatrix, GetLocalAABB());
		}
	}

	void Component::_SetSceneNode_Internal(g2d::SceneNode* node)
	{
		mAttachNode = node;
	}

	void Component::_SetRenderingOrder_Internal(unsigned int& order)
	{
		mRenderingOrder = order++;
	}

	point2d<float> Component::GetSceneNodePosition() const
	{
		return GetSceneNode()->GetPosition();
	}

	point2d<float> Component::GetSceneNodeWorldPosition() const
	{
		return GetSceneNode()->GetWorldPosition();
	}

	float2 Component::GetSceneNodeScale() const
	{
		return GetSceneNode()->GetScale();
	}

	radian<float> Component::GetSceneNodeRotation() const
	{
		return GetSceneNode()->GetRotation();
	}

	unsigned int Component::GetCameraVisibleMask() const
	{
		return GetSceneNode()->GetCameraVisibleMask();
	}

}