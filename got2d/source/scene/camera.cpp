#include "camera.h"
#include "../system_blackboard.h"
#include "../render/render_system.h"
#include "scene.h"
#include "scene_node.h"


//g2d::Component

const cxx::aabb2d<float>& Camera::GetLocalAABB() const
{
	return mAABB;
}

void Camera::OnPostUpdateTransformChanged()
{
	cxx::point2d<float> pos = GetSceneNodePosition();
	cxx::float2 scale = GetSceneNodeScale();
	cxx::radian<float> rotation = GetSceneNodeRotation();

	mMatrixView = cxx::float2x3::trs_inversed(pos, rotation, scale);
	mMatrixViewInverse = cxx::inversed(cxx::to_matrix3x3(mMatrixView));

	cxx::float2 halfWindowSize(
		GetRenderSystem().GetWindowWidth() * 0.5f,
		GetRenderSystem().GetWindowHeight()* 0.5f);

	halfWindowSize.x *= scale.x;
	halfWindowSize.y *= scale.y;
	cxx::float2 p[4] =
	{
		 -halfWindowSize,
		 +halfWindowSize,
		{+halfWindowSize.x, -halfWindowSize.y },
		{-halfWindowSize.x, +halfWindowSize.y }
	};

	cxx::float2x2 rotateMatrix = cxx::float2x2::rotate(rotation);
	mAABB.clear();
	for (cxx::float2& point : p)
	{
		point = rotateMatrix * point;
		mAABB.expand(pos + point);
	}
}

void Camera::Release()
{
	delete this;
}

unsigned int Camera::GetIndex() const
{
	return mIndex;
}

g2d::Camera* Camera::SetPosition(const cxx::point2d<float>& position)
{
	GetSceneNode()->SetPosition(position);
	return this;
}

g2d::Camera * Camera::SetScale(const cxx::float2 & scale)
{
	GetSceneNode()->SetScale(scale);
	return this;
}

g2d::Camera * Camera::SetRotation(cxx::radian<float> r)
{
	GetSceneNode()->SetRotation(r);
	return this;
}

void Camera::SetRenderingOrder(int renderingOrder)
{
	mRenderingOrder = renderingOrder;
	mScene->SetCameraOrderDirty();
}

int Camera::GetRenderingOrder() const
{
	return mRenderingOrder;
}

void Camera::SetCameraVisibleMask(unsigned int mask)
{
	mCameraVisibleMask = mask;
}

unsigned int Camera::GetCameraVisibleMask() const
{
	return mCameraVisibleMask;
}

void Camera::SetActivity(bool activity)
{
	mIsActive = activity;
}

const cxx::float2x3 & Camera::GetViewMatrix() const
{
	return mMatrixView;
}

bool Camera::TestVisible(const cxx::aabb2d<float>& bounding) const
{
	return (mAABB.hit_test(bounding) != cxx::intersection::none);
}

bool Camera::TestVisible(g2d::Component* component) const
{
	if (g2d::Is<::Camera>(component) ||
		cxx::is_single_point(component->GetLocalAABB()) ||
		!IsMatchCameraVisibleMask(component->GetCameraVisibleMask()))
	{
		return false;
	}

	return TestVisible(component->GetWorldAABB());
	return true;
}

bool Camera::IsActivity() const
{
	return mIsActive;
}

cxx::point2d<float> Camera::ScreenToWorld(const cxx::point2d<int>& pos) const
{
	cxx::point2d<float> viewPos = GetRenderSystem().ScreenToView(pos);
	return transform(mMatrixViewInverse, viewPos);
}

cxx::point2d<int> Camera::WorldToScreen(const cxx::point2d<float>& pos) const
{
	cxx::point2d<float> viewPos = transform(mMatrixView, pos);
	return GetRenderSystem().ViewToScreen(viewPos);
}

Camera::Camera(::Scene* scene, unsigned int index)
	: mScene(scene)
	, mIndex(index)
{
}

void Camera::SetID(unsigned int index)
{
	mIndex = index;
}

g2d::Component* Camera::FindNearestComponent(const cxx::float2& worldPosition)
{
	auto itCur = mVisibleComponents.rbegin();
	auto itEnd = mVisibleComponents.rend();
	for (; itCur != itEnd; itCur++)
	{
		Component* component = *itCur;
		if (!component->GetSceneNode()->IsRemoved())
		{
			cxx::point2d<float> localPos = component->GetSceneNode()->WorldToLocal(worldPosition);
			if (component->GetLocalAABB().contains(localPos))
			{
				return component;
			}
		}
	}
	return nullptr;
}

void Camera::OnRemoveSceneNode(::SceneNode* pNode)
{
	auto insideNode = [&](g2d::Component* component) ->bool {
		return component->GetSceneNode() == pNode;
	};

	auto oldEnd = mVisibleComponents.end();
	auto newEnd = std::remove_if(mVisibleComponents.begin(), oldEnd, insideNode);
	mVisibleComponents.erase(newEnd, oldEnd);
}

bool Camera::IsMatchCameraVisibleMask(unsigned int mask) const
{
	return (mCameraVisibleMask & mask) != 0;
}
