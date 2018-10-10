#pragma once
#include "g2dscene.h"
#include "g2drender.h"

class Quad : public g2d::Quad
{
	RTTI_IMPL;
public:
	virtual const cxx::aabb2d<float>& GetLocalAABB() const override { return mAABB; }

	virtual void OnRender() override;

	virtual void Release() override { delete this; }

public:
	virtual g2d::Quad* SetSize(const cxx::float2& size) override;

	virtual const cxx::float2& GetSize() const override { return mQuadSize; }

public:
	Quad();

	~Quad();

	g2d::Mesh*		mMesh = nullptr;
	g2d::Material*	mMaterial = nullptr;
	cxx::float2	mQuadSize;
	cxx::aabb2d<float> mAABB;
};