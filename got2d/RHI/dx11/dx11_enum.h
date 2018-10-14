#pragma once
#include <d3d11.h>
#include "../RHI.h"


constexpr UINT kBufferBinding[] =
{
	D3D11_BIND_VERTEX_BUFFER,	// Vertex = 0,
	D3D11_BIND_INDEX_BUFFER,	// Index = 1,
	D3D11_BIND_CONSTANT_BUFFER,	// Constant = 2,
};

constexpr D3D11_USAGE kResourceUsage[] =
{
	D3D11_USAGE_DEFAULT,	// Default = 0
	D3D11_USAGE_DYNAMIC,	// Dynamic = 1,
};

constexpr DXGI_FORMAT kIndexFormat[] =
{
	DXGI_FORMAT_R16_UINT,//		Int16 = 0,
	DXGI_FORMAT_R32_UINT,//		Int32 = 1,
};

constexpr D3D_PRIMITIVE_TOPOLOGY kPrimitive[] =
{
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,	//TriangleList = 0,
	D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,	//TriangleFan = 1,
};

constexpr DXGI_FORMAT kTextureFormat[] =
{
	DXGI_FORMAT_UNKNOWN,			// Unknown
	DXGI_FORMAT_R8G8B8A8_UNORM,		// RGBA
	DXGI_FORMAT_B8G8R8A8_UNORM,		// BGRA
	DXGI_FORMAT_BC1_UNORM,			// DXT1
	DXGI_FORMAT_BC2_UNORM,			// DXT3
	DXGI_FORMAT_BC3_UNORM,			// DXT5
	DXGI_FORMAT_D24_UNORM_S8_UINT,	// D24S8
	DXGI_FORMAT_R32_FLOAT,			// Float32
};

constexpr DXGI_FORMAT kInputFormat[] =
{
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
};

constexpr D3D11_BLEND kBlendOperand[] =
{
	D3D11_BLEND_ZERO,			// Zero = 0,
	D3D11_BLEND_ONE,			// One = 1,
	D3D11_BLEND_SRC_COLOR,		// SourceColor = 2,
	D3D11_BLEND_SRC_ALPHA,		// SourceAlpha = 3,
	D3D11_BLEND_DEST_COLOR,		// DestinationColor = 4,
	D3D11_BLEND_DEST_ALPHA,		// DestinationAlpha = 5,
	D3D11_BLEND_INV_SRC_COLOR,	// InverseSourceColor = 6,
	D3D11_BLEND_INV_SRC_ALPHA,	// InverseSourceAlpha = 7,
	D3D11_BLEND_INV_DEST_COLOR,	// InverseDestinationColor = 8,
	D3D11_BLEND_INV_DEST_ALPHA,	// InverseDestinationAlpha = 9,
};

constexpr D3D11_BLEND_OP kBlendOperator[] =
{
	D3D11_BLEND_OP_ADD,			// Add,
	D3D11_BLEND_OP_SUBTRACT,	// Sub
};

constexpr D3D11_BIND_FLAG kTextureBinding[] =
{
	D3D11_BIND_SHADER_RESOURCE,
	D3D11_BIND_RENDER_TARGET,
	D3D11_BIND_DEPTH_STENCIL,
	D3D11_BIND_STREAM_OUTPUT,
	D3D11_BIND_UNORDERED_ACCESS
};

constexpr D3D11_FILTER kSamplerFilters[] =
{
	D3D11_FILTER_MIN_MAG_MIP_POINT, // MinMagMipPoint
	D3D11_FILTER_MIN_MAG_MIP_LINEAR, // MinMagMipLinear
};

constexpr D3D11_TEXTURE_ADDRESS_MODE kTextureAddress[] =
{
	D3D11_TEXTURE_ADDRESS_WRAP,
	D3D11_TEXTURE_ADDRESS_CLAMP,
	D3D11_TEXTURE_ADDRESS_MIRROR
};

inline unsigned int GetTextureBindFlag(unsigned int binds)
{
	unsigned int bindFlag = 0;
	for (int i = 0, n = (int)rhi::TextureBinding::Count; i < n; i++)
	{
		unsigned int eachBinds = 1 << i;
		if ((binds & eachBinds) != 0)
		{
			bindFlag |= kTextureBinding[i];
		}
	}
	return bindFlag;
}

inline rhi::TextureFormat GetTextureFormatDX11(DXGI_FORMAT format)
{
	switch (format)
	{
	default:
	case DXGI_FORMAT_UNKNOWN: return rhi::TextureFormat::Unknown;
	case DXGI_FORMAT_R8G8B8A8_UNORM: return rhi::TextureFormat::RGBA;
	case DXGI_FORMAT_B8G8R8A8_UNORM: return rhi::TextureFormat::BGRA;
	case DXGI_FORMAT_BC1_UNORM: return rhi::TextureFormat::DXT1;
	case DXGI_FORMAT_BC2_UNORM: return rhi::TextureFormat::DXT3;
	case DXGI_FORMAT_BC3_UNORM: return rhi::TextureFormat::DXT5;
	case DXGI_FORMAT_D24_UNORM_S8_UINT: return rhi::TextureFormat::D24S8;
	case DXGI_FORMAT_R32_FLOAT: return rhi::TextureFormat::Float32;
	}
}