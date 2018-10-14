#include "inner_RHI.h"
#include "../../source/scope_utility.h"
#include "cxx_scope.h"
rhi::RHICreationResult rhi::CreateRHI()
{
	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;

	auto fb = cxx::make_scope_guard([&]
	{
		SR(d3dDevice);
		SR(d3dContext);
	});

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
	D3D11_CREATE_DEVICE_FLAG deviceFlag = D3D11_CREATE_DEVICE_SINGLETHREADED;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = ::D3D11CreateDevice(
		NULL,		//adapter
		driverType,	//hardware device
		NULL,		//no software renderer
		deviceFlag,	//single-thread
		&featureLevel, 1,	//desire feature level setting
		D3D11_SDK_VERSION,
		&d3dDevice,
		NULL,		//dont care feature level, since we have only one feature.
		&d3dContext);

	RHICreationResult result;
	if (S_OK == hr)
	{
		result.Success = true;
		result.DevicePtr = new ::Device(*d3dDevice);
		result.ContextPtr = new ::Context(*d3dContext);
		fb.dismiss();
	}
	else
	{
		result.Success = false;
	}
	return result;
}

Buffer::Buffer(ID3D11Buffer & buffer, rhi::BufferBinding binding, rhi::ResourceUsage usage, unsigned int length)
	: m_buffer(buffer)
	, m_bufferBinding(binding)
	, m_bufferUsage(usage)
	, m_bufferLength(length)
{
}

Buffer::~Buffer()
{
	m_buffer.Release();
}

TextureSampler::TextureSampler(ID3D11SamplerState& samplerState)
	: m_samplerState(samplerState)
{

}

TextureSampler::~TextureSampler()
{
	m_samplerState.Release();
}


BlendState::BlendState(ID3D11BlendState& blendState, bool enable, rhi::BlendFactor srcFactor, rhi::BlendFactor dstFactor, rhi::BlendOperator blendOp)
	: m_blendState(blendState)
	, m_srcFactor(srcFactor)
	, m_dstFactor(dstFactor)
	, m_blendOp(blendOp)
{

}

BlendState::~BlendState()
{
	m_blendState.Release();
}

Texture2D::Texture2D(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srView, rhi::TextureFormat format, unsigned int width, unsigned int height)
	: m_texture(texture)
	, m_srView(srView)
	, m_rtView(nullptr)
	, m_dsView(nullptr)
	, m_width(width)
	, m_height(height)
	, m_format(format)
{
}

Texture2D::Texture2D(ID3D11Texture2D* texture, ID3D11RenderTargetView* view, ID3D11ShaderResourceView* srView, rhi::TextureFormat format, unsigned int width, unsigned int height)
	: m_texture(texture)
	, m_srView(srView)
	, m_rtView(view)
	, m_dsView(nullptr)
	, m_width(width)
	, m_height(height)
	, m_format(format)
{
}

Texture2D::Texture2D(ID3D11Texture2D* texture, ID3D11DepthStencilView* view, ID3D11ShaderResourceView* srView, rhi::TextureFormat format, unsigned int width, unsigned int height)
	: m_texture(texture)
	, m_srView(srView)
	, m_rtView(nullptr)
	, m_dsView(view)
	, m_width(width)
	, m_height(height)
	, m_format(format)
{
}

Texture2D::~Texture2D()
{
	cxx::safe_release(m_srView);
	cxx::safe_release(m_rtView);
	cxx::safe_release(m_dsView);
	cxx::safe_release(m_texture);
}
