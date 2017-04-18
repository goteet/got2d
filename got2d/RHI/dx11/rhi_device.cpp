#include <d3dcompiler.h>
#include "inner_RHI.h"
#include "../source/scope_utility.h"
#include "dx11_enum.h"

Device::Device(ID3D11Device& d3dDevice)
	: m_d3dDevice(d3dDevice)
{
}

Device::~Device()
{
	m_d3dDevice.Release();
}

rhi::SwapChain* Device::CreateSwapChain(void* nativeWindow, uint32_t windowWidth, uint32_t windowHeight)
{
	autor<IDXGIDevice> dxgiDevice = nullptr;
	autor<IDXGIAdapter> adapter = nullptr;
	autor<IDXGIFactory> factory = nullptr;

	if (S_OK != m_d3dDevice.QueryInterface(__uuidof(IDXGIDevice), (void **)&(dxgiDevice.pointer)))
	{
		return nullptr;
	}

	if (S_OK != dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&(adapter.pointer)))
	{
		return nullptr;
	}

	if (S_OK != adapter->GetParent(__uuidof(IDXGIFactory), (void **)&(factory.pointer)))
	{
		return nullptr;
	}

	DXGI_SWAP_CHAIN_DESC scDesc;
	scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.BufferDesc.Width = windowWidth;
	scDesc.BufferDesc.Height = windowHeight;
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferCount = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow = reinterpret_cast<HWND>(nativeWindow);
	scDesc.Windowed = TRUE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Flags = 0;

	IDXGISwapChain* swapChain = nullptr;
	if (S_OK == factory->CreateSwapChain(&m_d3dDevice, &scDesc, &swapChain))
	{
		return new ::SwapChain(*swapChain);
	}
	else
	{
		return nullptr;
	}
}

rhi::Buffer* Device::CreateBuffer(rhi::BufferBinding binding, rhi::ResourceUsage usage, uint32_t bufferLength)
{
	D3D11_BUFFER_DESC bufferDesc =
	{
		bufferLength,					//UINT ByteWidth;
		kResourceUsage[(int)usage],		//D3D11_USAGE Usage;
		kBufferBinding[(int)binding],	//UINT BindFlags;
		D3D11_CPU_ACCESS_WRITE,			//UINT CPUAccessFlags;
		0,								//UINT MiscFlags;
		0								//UINT StructureByteStride;
	};

	ID3D11Buffer* buffer = nullptr;
	if (S_OK == m_d3dDevice.CreateBuffer(&bufferDesc, NULL, &buffer))
	{
		return new ::Buffer(*buffer, binding, usage, bufferLength);
	}
	else
	{
		return nullptr;
	}
}

rhi::Texture2D* Device::CreateTexture2D(rhi::TextureFormat format, rhi::ResourceUsage usage, uint32_t binding, uint32_t width, uint32_t height)
{
	D3D11_TEXTURE2D_DESC colorTexDesc;
	colorTexDesc.Width = width;
	colorTexDesc.Height = height;
	colorTexDesc.MipLevels = 1;
	colorTexDesc.ArraySize = 1;
	colorTexDesc.Format = kTextureFormat[(int)format];
	colorTexDesc.SampleDesc.Count = 1;
	colorTexDesc.SampleDesc.Quality = 0;
	colorTexDesc.Usage = kResourceUsage[(int)usage];
	colorTexDesc.CPUAccessFlags = usage == rhi::ResourceUsage::Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	colorTexDesc.MiscFlags = 0;
	colorTexDesc.BindFlags = GetTextureBindFlag(binding);

	ID3D11Texture2D* texture = nullptr;
	if (S_OK == m_d3dDevice.CreateTexture2D(&colorTexDesc, NULL, &texture))
	{
		return new ::Texture2D(*texture, format, width, height);
	}
	else
	{
		return nullptr;
	}
}

rhi::RenderTargetView* Device::CreateRenderTargetView(rhi::Texture2D* texture2D)
{
	::Texture2D* textureImpl = reinterpret_cast<::Texture2D*>(texture2D);
	ENSURE(textureImpl != nullptr);

	ID3D11RenderTargetView* rtView = nullptr;
	if (S_OK == m_d3dDevice.CreateRenderTargetView(textureImpl->GetRaw(), NULL, &rtView))
	{
		return new ::RenderTargetView(*rtView);
	}
	else
	{
		return nullptr;
	}

}

rhi::ShaderResourceView* Device::CreateShaderResourceView(rhi::Texture2D* texture2D)
{
	::Texture2D* textureImpl = reinterpret_cast<::Texture2D*>(texture2D);
	ENSURE(textureImpl != nullptr);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	::ZeroMemory(&viewDesc, sizeof(viewDesc));
	viewDesc.Format = kTextureFormat[(int)textureImpl->GetFormat()];
	viewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = -1;
	viewDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* srView = nullptr;
	if (S_OK == m_d3dDevice.CreateShaderResourceView(textureImpl->GetRaw(), &viewDesc, &srView))
	{
		return new ::ShaderResourceView(*srView);
	}
	else
	{
		return nullptr;
	}
}

rhi::DepthStencilView* Device::CreateDepthStencilView(rhi::Texture2D* texture2D)
{
	::Texture2D* textureImpl = reinterpret_cast<::Texture2D*>(texture2D);
	ENSURE(textureImpl != nullptr);

	ID3D11DepthStencilView* dsView = nullptr;
	if (S_OK == m_d3dDevice.CreateDepthStencilView(textureImpl->GetRaw(), NULL, &dsView))
	{
		return new ::DepthStencilView(*dsView);
	}
	else
	{
		return nullptr;
	}
}

ID3DBlob* CompileShaderSource(std::string sourceCodes, std::string entryPoint, std::string profileTarget)
{
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	if (S_OK == ::D3DCompile(
		sourceCodes.c_str(), sourceCodes.length(),
		NULL,	//Name
		NULL,	//Defines
		NULL,	//Inlcudes
		entryPoint.c_str(),
		profileTarget.c_str(),
		0,		//Flag1
		0,		//Flag2
		&shaderBlob, &errorBlob))
	{
		return shaderBlob;
	}
	else
	{
		const char* reason = (const char*)errorBlob->GetBufferPointer();
		errorBlob->Release();
		FAIL("compile shader failed.")(reason);
		return nullptr;
	}
}

rhi::ShaderProgram* Device::CreateShaderProgram(
	const char * vsSource, const char * vsEntry,
	const char * psSource, const char * psEntry,
	rhi::InputLayout * layouts, uint32_t layoutCount)
{
	autor<ID3DBlob> vsBlob = CompileShaderSource(vsSource, vsEntry, "vs_5_0");
	autor<ID3DBlob> psBlob = CompileShaderSource(psSource, psEntry, "ps_5_0");

	if (vsBlob.is_null() || psBlob.is_null())
		return nullptr;

	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;

	auto fb = create_fallback([&]
	{
		SR(vertexShader);
		SR(pixelShader);
		SR(inputLayout);
	});

	if (S_OK != m_d3dDevice.CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader))
	{
		return nullptr;
	}

	if (S_OK != m_d3dDevice.CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader))
	{
		return nullptr;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputLayouts;
	for (uint32_t i = 0; i < layoutCount; i++)
	{
		rhi::InputLayout& layout = layouts[i];
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = layout.SemanticName;
		elementDesc.SemanticIndex = layout.SemanticIndex;
		elementDesc.Format = kInputFormat[(int)layout.Format];
		elementDesc.InputSlot = layout.InputSlot;
		elementDesc.AlignedByteOffset = (layout.AlignOffset == 0xFFFFFFFF) ? D3D11_APPEND_ALIGNED_ELEMENT : layout.AlignOffset;
		elementDesc.InputSlotClass = layout.IsInstanced ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = layout.InstanceRepeatRate;
		m_inputLayouts.push_back(elementDesc);
	}

	if (S_OK != m_d3dDevice.CreateInputLayout(
		&(m_inputLayouts[0]), m_inputLayouts.size(),
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
		&inputLayout))
	{
		return nullptr;
	}

	fb.cancel();
	return new ::ShaderProgram(*vertexShader, *pixelShader, *inputLayout);
}

rhi::BlendState* Device::CreateBlendState(bool enabled, rhi::BlendFactor source, rhi::BlendFactor dest, rhi::BlendOperator op)
{
	D3D11_BLEND srcBlend = kBlendOperand[(int)source];
	D3D11_BLEND dstBlend = kBlendOperand[(int)dest];
	D3D11_BLEND_OP blendOp = kBlendOperator[(int)op];
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		blendDesc.RenderTarget[i].BlendEnable = enabled ? TRUE : FALSE;

		blendDesc.RenderTarget[i].SrcBlend = srcBlend;
		blendDesc.RenderTarget[i].DestBlend = dstBlend;
		blendDesc.RenderTarget[i].BlendOp = blendOp;
		// default setting
		blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	ID3D11BlendState* blendState = nullptr;
	if (S_OK == m_d3dDevice.CreateBlendState(&blendDesc, &blendState))
	{
		return new ::BlendState(*blendState, enabled, source, dest, op);
	}
	else
	{
		return nullptr;
	}
}

rhi::TextureSampler* Device::CreateTextureSampler(rhi::SamplerFilter filter, rhi::TextureAddress addressU, rhi::TextureAddress addressV)
{
	D3D11_SAMPLER_DESC samlerDesc;

	samlerDesc.Filter = kSamplerFilters[(int)filter];
	samlerDesc.AddressU = kTextureAddress[(int)addressU];
	samlerDesc.AddressV = kTextureAddress[(int)addressV];
	samlerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samlerDesc.MinLOD = -FLT_MAX;
	samlerDesc.MaxLOD = FLT_MAX;
	samlerDesc.MipLODBias = 0.0f;
	samlerDesc.MaxAnisotropy = 1;
	samlerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	ID3D11SamplerState* sampler = nullptr;
	if (S_OK == m_d3dDevice.CreateSamplerState(&samlerDesc, &sampler))
	{
		return new ::TextureSampler(*sampler);
	}
	else
	{
		return nullptr;
	}
}
