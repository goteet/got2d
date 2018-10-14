#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include "../RHI.h"

class Device;
class Context;

class Buffer : public rhi::Buffer
{
public:
	virtual void Release() override { delete this; }

	virtual rhi::BufferBinding GetBinding() const override { return m_bufferBinding; }

	virtual rhi::ResourceUsage GetUsage() const override { return m_bufferUsage; }

	virtual unsigned int GetLength() const override { return m_bufferLength; }

	ID3D11Buffer* GetRaw() { return &m_buffer; }

public:
	Buffer(ID3D11Buffer& buffer, rhi::BufferBinding binding, rhi::ResourceUsage usage, unsigned int length);

	~Buffer();

private:
	ID3D11Buffer& m_buffer;
	rhi::BufferBinding m_bufferBinding;
	rhi::ResourceUsage m_bufferUsage;
	const unsigned int m_bufferLength;
};

class Texture2D : public rhi::Texture2D
{
public:
	virtual void Release() override { delete this; }

	virtual unsigned int GetWidth() const override { return m_width; }

	virtual unsigned int GetHeight() const override { return m_height; }

	virtual rhi::TextureFormat GetFormat() const override { return m_format; }

	virtual bool IsRenderTarget() const override { return m_rtView != nullptr; }

	virtual bool IsShaderResource() const override { return m_srView != nullptr; }

	virtual bool IsDepthStencil() const override { return m_dsView != nullptr; }

public:
	Texture2D(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srView, rhi::TextureFormat format, unsigned int width, unsigned int height);

	Texture2D(ID3D11Texture2D* texture, ID3D11RenderTargetView* view, ID3D11ShaderResourceView* srView, rhi::TextureFormat format, unsigned int width, unsigned int height);

	Texture2D(ID3D11Texture2D* texture, ID3D11DepthStencilView* view, ID3D11ShaderResourceView* srView, rhi::TextureFormat format, unsigned int width, unsigned int height);

	~Texture2D();

	ID3D11Texture2D* GetRaw() { return m_texture; }

	ID3D11RenderTargetView* GetRTView() { return m_rtView; }

	ID3D11DepthStencilView* GetDSView() { return m_dsView; }

	ID3D11ShaderResourceView* GetSRView() { return m_srView; }

private:
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_srView = nullptr;
	ID3D11RenderTargetView* m_rtView = nullptr;
	ID3D11DepthStencilView* m_dsView = nullptr;
	const unsigned int m_width;
	const unsigned int m_height;
	rhi::TextureFormat m_format;
};

class VertexShader : public rhi::VertexShader
{
public:
	virtual void Release() override;

	virtual void AddReference() override;

	virtual rhi::Semantic GetSemanticByIndex(rhi::SemanticIndex index) const override;

	virtual rhi::SemanticIndex GetSemanticCount() const override;

public:
	VertexShader(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout, std::vector<rhi::Semantic>&& layouts);

	~VertexShader();

	ID3D11VertexShader* GetRaw() { return m_vertexShader; }

	ID3D11InputLayout* GetInputLayout() { return m_inputLayout; }

private:
	unsigned int m_refCount = 1;
	ID3D11VertexShader* m_vertexShader;
	ID3D11InputLayout* m_inputLayout;
	std::vector<rhi::Semantic> m_semantics;
};

class PixelShader : public rhi::PixelShader
{
public:
	virtual void Release() override;

	virtual void AddReference() override;

public:
	PixelShader(ID3D11PixelShader* pixelShader);

	~PixelShader();

	ID3D11PixelShader* GetRaw() { return m_pixelShader; }

private:
	unsigned int m_refCount = 1;
	ID3D11PixelShader* m_pixelShader;
};

class ShaderProgram : public rhi::ShaderProgram
{
public:
	virtual void Release() override { delete this; }

	virtual rhi::VertexShader* GetVertexShader() const override { return m_vertexShader; }

	virtual rhi::PixelShader* GetPixelShader() const override { return m_pixelShader; }

public:
	ShaderProgram(::VertexShader* vertexShader, ::PixelShader* pixelShader);

	~ShaderProgram();

	ID3D11VertexShader* GetVertexShader() { return m_vertexShader->GetRaw(); }

	ID3D11PixelShader* GetPixelShader() { return m_pixelShader->GetRaw(); }

	ID3D11InputLayout* GetInputLayout() { return m_vertexShader->GetInputLayout(); }

private:
	::VertexShader* m_vertexShader = nullptr;
	::PixelShader* m_pixelShader = nullptr;
};

class TextureSampler : public rhi::TextureSampler
{
public:
	virtual void Release() override { delete this; }

public:
	TextureSampler(ID3D11SamplerState& samplerState);

	~TextureSampler();

	ID3D11SamplerState* GetRaw() { return &m_samplerState; }

private:
	ID3D11SamplerState& m_samplerState;
};

class BlendState : public rhi::BlendState
{
public:
	virtual void Release() override { delete this; }

	virtual bool IsEnabled() const override { return m_enabled; }

	virtual rhi::BlendFactor GetSourceFactor() const override { return m_srcFactor; }

	virtual rhi::BlendFactor GetDestinationFactor() const override { return m_dstFactor; }

	virtual rhi::BlendOperator GetOperator() const override { return m_blendOp; }

public:
	BlendState(ID3D11BlendState& blendState, bool enable, rhi::BlendFactor srcFactor, rhi::BlendFactor dstFactor, rhi::BlendOperator blendOp);

	~BlendState();

	ID3D11BlendState* GetRaw() { return &m_blendState; }

private:
	ID3D11BlendState& m_blendState;
	bool m_enabled = false;
	rhi::BlendFactor m_srcFactor = rhi::BlendFactor::One;
	rhi::BlendFactor m_dstFactor = rhi::BlendFactor::Zero;
	rhi::BlendOperator m_blendOp = rhi::BlendOperator::Add;
};

class RenderTarget : public rhi::RenderTarget
{
public:
	virtual void Release() { delete this; }

	virtual rhi::Texture2D* GetColorBufferByIndex(rhi::RTIndex index) const override { return GetColorBufferImplByIndex(index); }

	virtual unsigned int GetColorBufferCount() const override { return static_cast<unsigned int>(m_colorBuffers.size()); }

	virtual rhi::Texture2D* GetDepthStencilBuffer() const override { return GetDepthStencilBufferImpl(); }

	virtual bool IsDepthStencilUsed() const override { return m_depthStencilBuffer != nullptr; }

	virtual unsigned int GetWidth() const override { return m_width; }

	virtual unsigned int GetHeight() const override { return m_height; }
public:
	RenderTarget(unsigned int width, unsigned int height, std::vector<::Texture2D*>&& colorBuffers, ::Texture2D* dsBuffer);

	~RenderTarget();

	::Texture2D* GetColorBufferImplByIndex(rhi::RTIndex index) const;

	::Texture2D* GetDepthStencilBufferImpl() const { return m_depthStencilBuffer; }

private:
	const unsigned int m_width;
	const unsigned int m_height;
	std::vector<::Texture2D*> m_colorBuffers;
	::Texture2D* m_depthStencilBuffer;
};

class SwapChain : public rhi::SwapChain
{
public:
	virtual void Release() override { delete this; }

	virtual rhi::RenderTarget* GetBackBuffer() const override { return m_renderTarget; }

	virtual unsigned int GetWidth() const override { return m_windowWidth; }

	virtual unsigned int GetHeight() const override { return m_windowHeight; }

	virtual bool OnResize(unsigned int width, unsigned int height) override;

	virtual void SetFullscreen(bool fullscreen) override;

	virtual bool IsFullscreen() const override { return m_fullscreen; }

	virtual void Present() override;

public:
	SwapChain(::Device& device, IDXGISwapChain& swapChain, bool useDepthStencil);

	~SwapChain();

	void UpdateWindowSize();

	IDXGISwapChain* GetRaw() { return &m_swapChain; }

private:
	bool CreateRenderTarget();

	::Device& m_device;
	IDXGISwapChain& m_swapChain;
	::RenderTarget* m_renderTarget = nullptr;
	unsigned int m_windowWidth = 0;
	unsigned int m_windowHeight = 0;
	const bool m_useDepthStencil;
	bool m_fullscreen = false;
};

class Device : public rhi::Device
{
public:
	virtual void Release() override { delete this; }

	virtual rhi::SwapChain* CreateSwapChain(void* nativeWindow, bool useDepthStencil, unsigned int windowWidth, unsigned int windowHeight) override;

	virtual rhi::Buffer* CreateBuffer(rhi::BufferBinding binding, rhi::ResourceUsage usage, unsigned int bufferLength) override;

	virtual rhi::Texture2D* CreateTexture2D(rhi::TextureFormat format, rhi::ResourceUsage usage, unsigned int binding, unsigned int width, unsigned int height) override;

	virtual rhi::VertexShader* CreateVertexShader(const char* source, const char* entry, rhi::Semantic* layouts, rhi::SemanticCount layoutCount) override;

	virtual rhi::PixelShader* CreatePixelShader(const char* source, const char* entry) override;

	virtual rhi::ShaderProgram* LinkShader(rhi::VertexShader* vertexShader, rhi::PixelShader* pixelShader)  override;

	virtual rhi::BlendState* CreateBlendState(bool enabled, rhi::BlendFactor source, rhi::BlendFactor dest, rhi::BlendOperator op) override;

	virtual rhi::TextureSampler* CreateTextureSampler(rhi::SamplerFilter filter, rhi::TextureAddress addressU, rhi::TextureAddress addressV) override;

	virtual rhi::RenderTarget* CreateRenderTarget(unsigned int width, unsigned int height, rhi::TextureFormat* rtFormats, rhi::RTCount rtCount, bool useDpethStencil) override;

public:
	Device(ID3D11Device& d3dDevice);

	~Device();

	ID3D11Device* GetRaw() { return &m_d3dDevice; }

	::Texture2D* CreateTexture2DImpl(rhi::TextureFormat format, rhi::ResourceUsage usage, unsigned int binding, unsigned int width, unsigned int height);

private:
	ID3D11Device& m_d3dDevice;
};

class Context : public rhi::Context
{
public:
	virtual void Release() override { delete this; }

	virtual void ClearRenderTarget(rhi::RenderTarget* renderTarget, cxx::color4f clearColor) override;

	virtual void SetViewport(const rhi::Viewport& viewport) override;

	virtual void SetRenderTarget(rhi::RenderTarget* renderTargets) override;

	virtual void SetVertexBuffers(unsigned int startSlot, rhi::VertexBufferInfo* buffers, unsigned int bufferCount) override;

	virtual void SetIndexBuffer(rhi::Buffer* buffer, unsigned int offset, rhi::IndexFormat format) override;

	virtual void SetShaderProgram(rhi::ShaderProgram* program) override;

	virtual void SetVertexShaderConstantBuffers(unsigned int startSlot, rhi::Buffer** buffers, unsigned int bufferCount) override;

	virtual void SetPixelShaderConstantBuffers(unsigned int startSlot, rhi::Buffer** buffers, unsigned int bufferCount) override;

	virtual void SetTextures(unsigned int startSlot, rhi::Texture2D** textures, unsigned int resCount) override;

	virtual void SetBlendState(rhi::BlendState* state) override;

	virtual void SetTextureSampler(unsigned int startSlot, rhi::TextureSampler** samplers, unsigned int count) override;

	virtual void DrawIndexed(rhi::Primitive primitive, unsigned int startIndex, unsigned int indexOffset, unsigned int baseVertex) override;

	virtual rhi::MappedResource Map(rhi::Buffer* buffer) override;

	virtual rhi::MappedResource Map(rhi::Texture2D* buffer) override;

	virtual void Unmap(rhi::Buffer* buffer) override;

	virtual void Unmap(rhi::Texture2D* buffer) override;

	virtual void GenerateMipmaps(rhi::Texture2D* textures) override;

public:
	Context(ID3D11DeviceContext& d3dContext);

	~Context();

	ID3D11DeviceContext* GetRaw() { return &m_d3dContext; }

private:
	rhi::MappedResource Map(ID3D11Resource * resource, UINT subResource, D3D11_MAP mappingType, UINT flag);

	void Unmap(ID3D11Resource* resource, UINT subResource);

	ID3D11DeviceContext& m_d3dContext;
	std::vector<ID3D11Buffer*> m_vertexbuffers;
	std::vector<ID3D11Buffer*> m_vsConstantBuffers;
	std::vector<ID3D11Buffer*> m_psConstantBuffers;
	std::vector<ID3D11RenderTargetView*> m_rtViews;
	std::vector<ID3D11ShaderResourceView*> m_srViews;
	std::vector<ID3D11SamplerState*> m_samplerStates;
	std::vector<UINT> m_vertexBufferStrides;
	std::vector<UINT> m_vertexBufferOffsets;
};