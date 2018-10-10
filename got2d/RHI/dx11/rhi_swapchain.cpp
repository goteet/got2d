#include "inner_RHI.h"
#include "../../source/scope_utility.h"
#include "dx11_enum.h"

::Texture2D* RenderTarget::GetColorBufferImplByIndex(rhi::RTIndex index) const
{
	ENSURE(index < GetColorBufferCount());
	return m_colorBuffers.at(index);
}

RenderTarget::RenderTarget(unsigned int width, unsigned int height, std::vector<::Texture2D*>&& colorbuffers, ::Texture2D* dsBuffer)
	: m_width(width)
	, m_height(height)
	, m_colorBuffers(std::move(colorbuffers))
	, m_depthStencilBuffer(dsBuffer)
{
}

RenderTarget::~RenderTarget()
{
	for (auto& t : m_colorBuffers)
	{
		t->Release();
	}
	m_colorBuffers.clear();
}


SwapChain::SwapChain(::Device& device, IDXGISwapChain& swapChain, bool useDepthStencil)
	: m_device(device)
	, m_swapChain(swapChain)
	, m_useDepthStencil(useDepthStencil)
{
	if (CreateRenderTarget())
	{
		UpdateWindowSize();
	}
	else
	{
		FAIL("cannot create RenderTarget");
	}
}

SwapChain::~SwapChain()
{
	m_renderTarget.release();
	if (IsFullscreen())
	{
		SetFullscreen(false);
	}
	m_swapChain.Release();
}

bool SwapChain::OnResize(unsigned int width, unsigned int height)
{
	m_renderTarget.release();
	if (S_OK == m_swapChain.ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0))
	{
		if (CreateRenderTarget())
		{
			UpdateWindowSize();
			return true;
		}
	}
	return false;
}

void SwapChain::SetFullscreen(bool fullscreen)
{
	if (m_fullscreen != fullscreen)
	{
		m_swapChain.SetFullscreenState(fullscreen ? TRUE : FALSE, NULL);
		BOOL isFullScreen;
		if (S_OK == m_swapChain.GetFullscreenState(&isFullScreen, NULL))
		{
			m_fullscreen = (isFullScreen == TRUE);
			UpdateWindowSize();
		}
		else
		{
			FAIL("cannot retrieve fullsreen state");
		}
	};
}

void SwapChain::Present()
{
	m_swapChain.Present(0, 0);
}

void SwapChain::UpdateWindowSize()
{
	DXGI_SWAP_CHAIN_DESC scDesc;
	m_swapChain.GetDesc(&scDesc);
	m_windowWidth = scDesc.BufferDesc.Width;
	m_windowHeight = scDesc.BufferDesc.Height;
}

bool SwapChain::CreateRenderTarget()
{
	if (m_renderTarget.is_not_null())
		return false;

	ID3D11Texture2D* backBuffer = nullptr;
	if (S_OK == m_swapChain.GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))
	{
		auto fbWhenCreateRTViewFail = create_fallback([&] { backBuffer->Release(); });

		::ID3D11RenderTargetView* rtView = nullptr;
		if (S_OK != m_device.GetRaw()->CreateRenderTargetView(backBuffer, NULL, &rtView))
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC bbDesc;
		backBuffer->GetDesc(&bbDesc);
		std::vector<::Texture2D*> colorBuffers(1);
		colorBuffers[0] = new ::Texture2D(*backBuffer, *rtView, nullptr,
			GetTextureFormatDX11(bbDesc.Format),
			bbDesc.Width, bbDesc.Height);

		fbWhenCreateRTViewFail.cancel();
		if (m_useDepthStencil)
		{
			auto fbWhenCreateDSFail = create_fallback([&]
			{
				colorBuffers[0]->Release();
			});

			auto dsTexture = m_device.CreateTexture2DImpl(
				rhi::TextureFormat::D24S8,
				rhi::ResourceUsage::Default,
				D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL,
				bbDesc.Width, bbDesc.Height);

			if (dsTexture == nullptr)
				return false;

			fbWhenCreateDSFail.cancel();
			m_renderTarget = new ::RenderTarget(bbDesc.Width, bbDesc.Height, std::move(colorBuffers), dsTexture);
		}
		else
		{
			m_renderTarget = new ::RenderTarget(bbDesc.Width, bbDesc.Height, std::move(colorBuffers), nullptr);
		}
		return true;
	}
	else
	{
		return false;
	}
}
