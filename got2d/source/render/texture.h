#pragma once
#include <map>
#include <vector>
#include "g2drender.h"
#include "../RHI/RHI.h"

class Texture : public g2d::Texture
{
	RTTI_IMPL;
public:
	static Texture* Default() { static Texture t(""); return &t; }

	Texture(std::string resPath);

	const std::string& GetResourceName() const { return mResPath; }

public: // g2d::Texture
	virtual void Release() override;

	virtual const char* Identifier() const override { return mResPath.c_str(); }

	virtual bool IsSame(g2d::Texture* other) const override;

	virtual void AddRef() override;

private:
	int mRefCount = 1;
	std::string mResPath;
};

class TexturePool
{
public:
	bool CreateDefaultTexture();

	void Destroy();

	rhi::Texture2D* GetTexture(const std::string& resource);

	rhi::Texture2D* GetDefaultTexture() { return mDefaultTexture; }

private:
	bool LoadTextureFromFile(std::string resourcePath);

	std::map<std::string, rhi::Texture2D*> mTextures;
	rhi::Texture2D* mDefaultTexture = nullptr;
};