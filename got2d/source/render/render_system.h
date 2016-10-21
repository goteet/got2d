#pragma once
#include <map>
#include <vector>
#include "g2drender.h"
#include "../inner_utility.h"
#include "../scope_utility.h"
#include "../RHI/RHI.h"

#include "cxx_scope.h"
#include "../system_blackboard.h"
#include "mesh.h"
#include "texture.h"

class Pass;
class ShaderLib;

class RenderSystem : public g2d::RenderSystem
{
	RTTI_IMPL;
public:
	virtual void BeginRender() override;

	virtual void EndRender() override;

	virtual void RenderMesh(unsigned int layer, g2d::Mesh*, g2d::Material*, const cxx::float2x3&) override;

	virtual unsigned int GetWindowWidth() const override;

	virtual unsigned int GetWindowHeight() const override;

	virtual cxx::point2d<float> ScreenToView(const cxx::point2d<int>& screen) const override;

	virtual cxx::point2d<int> ViewToScreen(const cxx::point2d<float> & view) const override;

public:
	bool Create(void* nativeWindow);

	void Destroy();

	void Clear();

	void FlushRequests();

	void Present();

	void SetBlendMode(g2d::BlendMode blendMode);

	void SetViewMatrix(const cxx::float2x3& viewMatrix);

	const cxx::float4x4& GetProjectionMatrix();

	rhi::Device* GetDevice() { return mDevice; }

	rhi::Context* GetContext() { return mContext; }

	bool OnResize(unsigned int width, unsigned int height);

public:
	Texture* CreateTextureFromFile(const char* resPath);

private:
	bool CreateBlendModes();

	void FlushBatch(Mesh& mesh, g2d::Material&);

	void UpdateConstBuffer(rhi::Buffer* cbuffer, const void* data, unsigned int length);

	void UpdateSceneConstBuffer();

	rhi::Device*		mDevice = nullptr;
	rhi::Context*		mContext = nullptr;
	rhi::SwapChain*		mSwapChain = nullptr;
	rhi::RenderTarget*	mRenderTarget = nullptr;
	rhi::Buffer*		mSceneConstBuffer = nullptr;
	rhi::RenderTarget*	mBackBufferRT = nullptr;

	ShaderLib*	mShaderlib = nullptr;

	rhi::Viewport mViewport;
	std::map<g2d::BlendMode, rhi::BlendState*> mBlendModes;
	std::vector<rhi::TextureSampler*> mTextureSamplers;
	std::vector<rhi::Texture2D*> mTextures;

	cxx::color4f mBkColor = cxx::color4f::blue();

	//render request
	struct RenderRequest {
		RenderRequest(g2d::Mesh& inMesh, g2d::Material& inMaterial, const cxx::float2x3& inWorldMatrix)
			: mesh(inMesh), material(inMaterial), worldMatrix(inWorldMatrix)
		{	}
		g2d::Mesh& mesh;
		g2d::Material& material;
		cxx::float2x3 worldMatrix = cxx::float2x3::identity();
	};

	typedef std::vector<RenderRequest> RenderRequestList;
	std::map<unsigned int, RenderRequestList*> mRenderRequests;

	Geometry mGeometry;
	TexturePool mTexPool;
	
	cxx::float2x3 mMatrixView = cxx::float2x3::identity();
	cxx::float4x4 mMatProj = cxx::float4x4::identity();
	bool mMatrixConstBufferDirty = true;
	bool mMatrixProjDirty = true;
};