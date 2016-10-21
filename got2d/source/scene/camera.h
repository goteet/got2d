#pragma once
#include <vector>
#include "g2dscene.h"
#include "g2drender.h"

class Scene;
class SceneNode;

class Camera : public g2d::Camera
{
	RTTI_IMPL;
public:	//g2d::Component
	virtual const cxx::aabb2d<float>& GetLocalAABB() const override;

	virtual void OnPostUpdateTransformChanged() override;

	virtual void Release() override;

public:	//g2d::camera
	virtual unsigned int GetIndex() const override;

	virtual g2d::Camera* SetPosition(const cxx::point2d<float>& Position) override;

	virtual g2d::Camera* SetScale(const cxx::float2& scale) override;

	virtual g2d::Camera* SetRotation(cxx::radian<float> r) override;

	virtual void SetRenderingOrder(int renderingOrder) override;

	virtual int GetRenderingOrder() const override;

	virtual void SetCameraVisibleMask(unsigned int mask) override;

	virtual unsigned int GetCameraVisibleMask() const override;

	virtual void SetActivity(bool activity) override;

	virtual const cxx::float2x3& GetViewMatrix() const override;

	virtual bool TestVisible(const cxx::aabb2d<float>& bounding) const override;

	virtual bool TestVisible(g2d::Component* component) const override;

	virtual bool IsActivity() const override;

	virtual cxx::point2d<float> ScreenToWorld(const cxx::point2d<int>& pos) const override;

	virtual cxx::point2d<int> WorldToScreen(const cxx::point2d<float> & pos) const override;

public:
	Camera(Scene* scene, unsigned int index);

	void SetID(unsigned int index);

	g2d::Component* FindNearestComponent(const cxx::float2& worldPosition);

	void OnRemoveSceneNode(::SceneNode* node);

	std::vector<Component*> mVisibleComponents;

private:
	bool IsMatchCameraVisibleMask(unsigned int mask) const;

	::Scene* mScene = nullptr;
	unsigned int mIndex = 0;
	unsigned int mCameraVisibleMask = g2d::DefaultCameraVisibkeMask;
	int mRenderingOrder = 0;
	bool mIsActive = true;
	cxx::float2x3 mMatrixView;
	cxx::float3x3 mMatrixViewInverse;
	cxx::aabb2d<float> mAABB;
};
