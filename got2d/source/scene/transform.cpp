#include "transform.h"
#include "scene_node.h"

Transform::Transform(Transform* parent)
	: mParent(parent)
{
}

void Transform::SetPosition(const cxx::point2d<float>& Position)
{
	SetMatrixDirty();
	mLocalPosition = Position;
}

void Transform::SetPivot(const cxx::float2& pivot)
{
	SetMatrixDirty();
	mPivotOffset = pivot;
}
void Transform::SetScale(const cxx::float2& scale)
{
	SetMatrixDirty();
	mScale = scale;
}

void Transform::SetRotation(cxx::radian<float> r)
{
	SetMatrixDirty();
	mRotation = r;
}

const cxx::float2x3& Transform::GetMatrix()
{
	if (mMatrixNeedUpdate)
	{
		mLocalMatrix = cxx::float2x3::trsp(mLocalPosition, mRotation, mScale, mPivotOffset);
		mMatrixNeedUpdate = false;
	}
	return mLocalMatrix;
}

const cxx::float2x3& Transform::GetWorldMatrix()
{
	if (mWorldMatrixNeedUpdate)
	{
		mWorldMatrixNeedUpdate = false;
		if (mParent != nullptr)
		{
			mWorldRightDirectionDirty = true;
			mWorldUpDirectionDirty = true;
			mWorldPositionDirty = true;

			mWorldMatrix = mParent->GetWorldMatrix() * GetMatrix();
			return mWorldMatrix;
		}
	}
	return GetMatrix();
}

const cxx::point2d<float>& Transform::GetPosition() const
{
	return mLocalPosition;
}

const cxx::point2d<float> & Transform::GetWorldPosition()
{
	if (mWorldPositionDirty)
	{
		mWorldPositionDirty = false;
		if (mParent != nullptr)
		{
			mWorldPosition = cxx::transform(
				mParent->GetWorldMatrix(),
				GetPosition()
			);

			return mWorldPosition;
		}
	}
	return GetPosition();
}

const cxx::float2 & Transform::GetPivot() const
{
	return mPivotOffset;
}

const cxx::float2 & Transform::GetScale() const
{
	return mScale;
}

cxx::radian<float> Transform::GetRotation() const
{
	return mRotation;
}

void Transform::SetMatrixDirty()
{
	mMatrixNeedUpdate = true;
	mWorldMatrixNeedUpdate = true;
	mWorldPositionDirty = true;
	mWorldRightDirectionDirty = true;
	mWorldUpDirectionDirty = true;
}

void Transform::NotifyChildrenTransformChanged()
{
	mWorldMatrixNeedUpdate = true;
	mWorldPositionDirty = true;
	mWorldRightDirectionDirty = true;
	mWorldUpDirectionDirty = true;
}

const cxx::nfloat2 & Transform::GetWorldUpDirection()
{
	if (mWorldUpDirectionDirty)
	{
		mWorldUpDirection = cxx::transform(GetWorldMatrix(), cxx::nfloat2::unit_y());
		mWorldUpDirectionDirty = false;
	}
	return mWorldUpDirection;
}

const cxx::nfloat2 & Transform::GetWorldRightDirection()
{
	if (mWorldRightDirectionDirty)
	{
		mWorldRightDirection = cxx::transform(GetWorldMatrix(), cxx::nfloat2::unit_y());
		mWorldRightDirectionDirty = false;
	}
	return mWorldRightDirection;
}
