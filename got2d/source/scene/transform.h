#pragma once
#include "cxx_math.h"

class SceneNode;

class Transform
{
public:
	Transform(Transform* parent);

	void NotifyChildrenTransformChanged();

	void SetPosition(const cxx::point2d<float>& position);

	void SetPivot(const cxx::float2& pivot);

	void SetScale(const cxx::float2& scale);

	void SetRotation(cxx::radian<float> r);

	const cxx::float2x3& GetMatrix();

	const cxx::float2x3& GetWorldMatrix();

	const cxx::point2d<float>& GetPosition() const;

	const cxx::point2d<float>& GetWorldPosition();

	const cxx::float2& GetPivot() const;

	const cxx::float2& GetScale() const;

	cxx::radian<float> GetRotation() const;

	const cxx::nfloat2& GetWorldUpDirection();

	const cxx::nfloat2& GetWorldRightDirection();


private:
	void SetMatrixDirty();

	Transform* mParent = nullptr;

	bool mMatrixNeedUpdate = true;
	bool mWorldMatrixNeedUpdate = true;
	bool mWorldPositionDirty = true;
	bool mWorldRightDirectionDirty = true;
	bool mWorldUpDirectionDirty = true;

	cxx::point2d<float> mLocalPosition = cxx::point2d<float>::origin();
	cxx::point2d<float> mWorldPosition = cxx::point2d<float>::origin();

	cxx::float2 mPivotOffset = cxx::float2::zero();

	cxx::float2 mScale = cxx::float2::one();

	cxx::radian<float> mRotation = cxx::radian<float>(0);

	cxx::nfloat2 mWorldRightDirection = cxx::float2::unit_x();

	cxx::nfloat2 mWorldUpDirection = cxx::float2::unit_y();

	cxx::float2x3 mLocalMatrix = cxx::float2x3::identity();
	cxx::float2x3 mWorldMatrix = cxx::float2x3::identity();
};