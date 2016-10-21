#pragma once
#include <vector>
#include "g2drender.h"

class Pass : public g2d::Pass
{
	RTTI_IMPL;

public:
	virtual const char* GetVertexShaderName() const override { return mVsName.c_str(); }

	virtual const char* GetPixelShaderName() const override { return mPsName.c_str(); }

	virtual bool IsSame(g2d::Pass* other) const override;

	virtual void SetTexture(unsigned int index, g2d::Texture*, bool autoRelease) override;

	virtual void SetVSConstant(unsigned int index, float* data, unsigned int size, unsigned int count) override;

	virtual void SetPSConstant(unsigned int index, float* data, unsigned int size, unsigned int count) override;

	virtual void SetBlendMode(g2d::BlendMode blendMode) override { mBlendMode = blendMode; }

	virtual g2d::Texture* GetTextureByIndex(unsigned int index) const override { return mTextures.at(index); }

	virtual unsigned int GetTextureCount() const override { return static_cast<unsigned int>(mTextures.size()); }

	virtual const float* GetVSConstant() const override { return reinterpret_cast<const float*>(&(mVsConstants[0])); }

	virtual unsigned int GetVSConstantLength() const override { return static_cast<unsigned int>(mVsConstants.size()) * 4 * sizeof(float); }

	virtual const float* GetPSConstant() const override { return reinterpret_cast<const float*>(&(mPsConstants[0])); }

	virtual unsigned int GetPSConstantLength() const override { return static_cast<unsigned int>(mPsConstants.size()) * 4 * sizeof(float); }

	virtual g2d::BlendMode GetBlendMode() const override { return mBlendMode; }

public:
	Pass(const std::string& vsName, const std::string& psName)
		: mVsName(vsName)
		, mPsName(psName)
		, mBlendMode(g2d::BlendMode::None) {}

	Pass(const Pass& other);

	~Pass();

	Pass* Clone();

	void Release() { delete this; }

private:
	g2d::BlendMode mBlendMode = g2d::BlendMode::None;

	std::string mVsName = "";
	std::string mPsName = "";

	std::vector<g2d::Texture*>	mTextures;
	std::vector<cxx::float4>	mVsConstants;
	std::vector<cxx::float4>	mPsConstants;
};
