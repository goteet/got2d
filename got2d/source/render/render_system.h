#pragma once
#include <map>
#include <vector>
#include "cxx_math/cxx_color.h"
#include "g2drender.h"
#include "../inner_utility.h"
#include "../scope_utility.h"
#include "../RHI/RHI.h"

#include "cxx_scope.h"
#include "../system_blackboard.h"

class Geometry
{
public:
	bool Create(unsigned int vertexCount, unsigned int indexCount);

	bool MakeEnoughVertexArray(unsigned int numVertices);

	bool MakeEnoughIndexArray(unsigned int numIndices);

	void UploadVertices(unsigned int offset, g2d::GeometryVertex*, unsigned int count);

	void UploadIndices(unsigned int offset, unsigned int* indices, unsigned int count);

	void Destroy();

	cxx::unique_i<rhi::Buffer> mVertexBuffer = nullptr;
	cxx::unique_i<rhi::Buffer> mIndexBuffer = nullptr;
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;
};

class Mesh : public g2d::Mesh
{
	RTTI_IMPL;
public:
	Mesh(unsigned int vertexCount, unsigned int indexCount);

	void Clear();

	bool Merge(const g2d::Mesh& other, const cxx::float2x3& transform);

public:
	virtual const g2d::GeometryVertex* GetRawVertices() const override;

	virtual g2d::GeometryVertex* GetRawVertices() override;

	virtual const unsigned int* GetRawIndices() const override;

	virtual unsigned int* GetRawIndices() override;

	virtual unsigned int GetVertexCount() const override;

	virtual unsigned int GetIndexCount() const override;

	virtual void ResizeVertexArray(unsigned int vertexCount) override;

	virtual void ResizeIndexArray(unsigned int indexCount) override;

	virtual bool Merge(g2d::Mesh* other, const cxx::float2x3& transform) override;

	virtual void Release() override;

private:
	std::vector<g2d::GeometryVertex> mVertices;
	std::vector<unsigned int> mIndices;
};

class Texture : public g2d::Texture
{
	RTTI_IMPL;
public:
	static Texture& Default() { static Texture t(""); return t; }

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

	rhi::Texture2D* GetDefaultTexture() { return mDefaultTexture.get(); }

private:
	bool LoadTextureFromFile(std::string resourcePath);

	std::map<std::string, rhi::Texture2D*> mTextures;
	cxx::unique_i<rhi::Texture2D> mDefaultTexture = nullptr;
};

class VSData
{
public:
	virtual ~VSData() {}

	virtual const char* GetName() = 0;

	virtual const char* GetCode() = 0;

	virtual unsigned int GetConstBufferLength() = 0;
};

class PSData
{
public:
	virtual ~PSData() {}

	virtual const char* GetName() = 0;

	virtual const char* GetCode() = 0;

	virtual unsigned int GetConstBufferLength() = 0;
};

class Shader
{
	RTTI_INNER_IMPL;
public:
	bool Create(const std::string& vsCode, unsigned int vcbLength, const std::string& psCode, unsigned int pcbLength);

	void Destroy();

	rhi::ShaderProgram* GetShaderProgram() { return mShaderProgram.get(); }

	rhi::Buffer* GetVertexConstBuffer() { return mVertexConstBuffer.get(); }

	rhi::Buffer* GetPixelConstBuffer() { return mPixelConstBuffer.get(); }

private:
	cxx::unique_i<rhi::ShaderProgram> mShaderProgram = nullptr;
	cxx::unique_i<rhi::Buffer> mVertexConstBuffer = nullptr;
	cxx::unique_i<rhi::Buffer> mPixelConstBuffer = nullptr;
};

class ShaderLib
{
public:
	ShaderLib();

	~ShaderLib();

	Shader* GetShaderByName(const std::string& vsName, const std::string& psName);

private:
	bool BuildShader(const std::string& effectName, const std::string& vsName, const std::string& psName);

	std::string GetEffectName(const std::string& vsName, const std::string& psName);

	std::map<std::string, VSData*> mVsSources;
	std::map<std::string, PSData*> mPsSources;
	std::map<std::string, Shader*> mShaders;
};

class Pass : public g2d::Pass
{
	RTTI_IMPL;
public:
	Pass(std::string vsName, std::string psName)
		: mVsName(std::move(vsName))
		, mPsName(std::move(psName))
		, mBlendMode(g2d::BlendMode::None) {}

	Pass(const Pass& other);

	~Pass();

	Pass* Clone();

	void Release() { delete this; }

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

private:
	std::string mVsName = "";
	std::string mPsName = "";
	std::vector<g2d::Texture*> mTextures;
	std::vector<cxx::float4> mVsConstants;
	std::vector<cxx::float4> mPsConstants;
	g2d::BlendMode mBlendMode = g2d::BlendMode::None;
};

class Material : public g2d::Material
{
	RTTI_IMPL;
public:
	Material(unsigned int passCount);

	Material(const Material& other);

	~Material();

	void SetPass(unsigned int index, Pass* p);

public:
	virtual g2d::Pass* GetPassByIndex(unsigned int index) const override;

	virtual unsigned int GetPassCount() const override;

	virtual bool IsSame(g2d::Material* other) const override;

	virtual g2d::Material* Clone() const override;

	virtual void Release()  override;

private:
	std::vector<::Pass*> mPasses;

};

class RenderSystem : public g2d::RenderSystem
{
	RTTI_IMPL;
public:
	static RenderSystem* Instance;

	bool Create(void* nativeWindow);

	void Destroy();

	void Clear();

	void FlushRequests();

	void Present();

	void SetBlendMode(g2d::BlendMode blendMode);

	void SetViewMatrix(const cxx::float2x3& viewMatrix);

	const cxx::float4x4& GetProjectionMatrix();

	Texture* CreateTextureFromFile(const char* resPath);

	rhi::Device* GetDevice() { return mDevice.get(); }

	rhi::Context* GetContext() { return mContext.get(); }

	bool OnResize(unsigned int width, unsigned int height);

public:
	virtual void BeginRender() override;

	virtual void EndRender() override;

	virtual void RenderMesh(unsigned int layer, g2d::Mesh*, g2d::Material*, const cxx::float2x3&) override;

	virtual unsigned int GetWindowWidth() const override { return mSwapChain->GetWidth(); }

	virtual unsigned int GetWindowHeight() const override { return mSwapChain->GetHeight(); }

	virtual cxx::float2 ScreenToView(const cxx::int2& screen) const override;

	virtual cxx::int2 ViewToScreen(const cxx::float2 & view) const override;

private:
	bool CreateBlendModes();

	void FlushBatch(Mesh& mesh, g2d::Material&);

	void UpdateConstBuffer(rhi::Buffer* cbuffer, const void* data, unsigned int length);

	void UpdateSceneConstBuffer();

	cxx::unique_i<rhi::Device> mDevice = nullptr;
	cxx::unique_i<rhi::Context> mContext = nullptr;
	cxx::unique_i<rhi::SwapChain> mSwapChain = nullptr;
	cxx::unique_i<rhi::RenderTarget> mRenderTarget = nullptr;
	cxx::unique_i<rhi::Buffer> mSceneConstBuffer = nullptr;
	rhi::RenderTarget* mBackBufferRT = nullptr;
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

	typedef std::vector<RenderRequest> ReqList;
	std::map<unsigned int, ReqList*> mRenderRequests;

	Geometry mGeometry;
	TexturePool mTexPool;
	std::unique_ptr<ShaderLib> mShaderlib = nullptr;
	cxx::float2x3 mMatrixView = cxx::float2x3::identity();
	cxx::float4x4 mMatProj = cxx::float4x4::identity();
	bool mMatrixConstBufferDirty = true;
	bool mMatrixProjDirty = true;
};