#include <string>
#include "render_system.h"
#include "shader.h"

Texture* RenderSystem::CreateTextureFromFile(const char* resPath)
{
	return new Texture(resPath);
}

bool RenderSystem::CreateBlendModes()
{
	mBlendModes[g2d::BlendMode::None] = nullptr;


	mBlendModes[g2d::BlendMode::Normal] = mDevice->CreateBlendState(true,
		rhi::BlendFactor::SourceAlpha,
		rhi::BlendFactor::InverseSourceAlpha,
		rhi::BlendOperator::Add);

	if (mBlendModes[g2d::BlendMode::Normal] == nullptr)
		return false;

	mBlendModes[g2d::BlendMode::Additve] = mDevice->CreateBlendState(true,
		rhi::BlendFactor::One,
		rhi::BlendFactor::One,
		rhi::BlendOperator::Add);

	if (mBlendModes[g2d::BlendMode::Additve] == nullptr)
		return false;

	return true;
}

//===================================================================
//	overrides
//===================================================================

void RenderSystem::BeginRender()
{
	Clear();
}

void RenderSystem::EndRender()
{
	FlushRequests();
	Present();
}

void RenderSystem::RenderMesh(unsigned int layer, g2d::Mesh* mesh, g2d::Material* material, const cxx::float2x3& worldMatrix)
{
	if (mRenderRequests.count(layer) == 0)
	{
		mRenderRequests[layer] = new RenderRequestList();
	}
	RenderRequestList* list = mRenderRequests[layer];
	list->push_back({ *mesh, *material, worldMatrix });
}

unsigned int RenderSystem::GetWindowWidth() const
{
	return mSwapChain->GetWidth();
}

unsigned int RenderSystem::GetWindowHeight() const
{
	return mSwapChain->GetHeight();
}

cxx::point2d<float> RenderSystem::ScreenToView(const cxx::point2d<int>& screen) const
{
	float windowWidth = static_cast<float>(GetWindowWidth());
	float windowHeight = static_cast<float>(GetWindowHeight());
	float x = screen.x - windowWidth * 0.5f;
	float y = windowHeight * 0.5f - screen.y;
	return { x, y };
}

cxx::point2d<int> RenderSystem::ViewToScreen(const cxx::point2d<float> & view) const
{
	float windowWidth = static_cast<float>(GetWindowWidth());
	float windowHeight = static_cast<float>(GetWindowHeight());
	int x = static_cast<int>(view.x + windowWidth * 0.5f);
	int y = static_cast<int>(windowHeight * 0.5f - view.y);
	return { x, y };
}


//===================================================================
//	functions
//===================================================================
bool RenderSystem::Create(void* nativeWindow)
{
	mViewport.LTPosition = cxx::float2::zero();
	mViewport.MinMaxZ = cxx::float2(0.0f, 1.0f);

	auto failGuard = cxx::make_scope_guard([&] {
		Destroy();
	});

	rhi::RHICreationResult rhiResult = rhi::CreateRHI();
	if (!rhiResult.Success)
	{
		return false;
	}

	mDevice = rhiResult.DevicePtr;
	mContext = rhiResult.ContextPtr;

	mSwapChain = mDevice->CreateSwapChain(nativeWindow, false, 0, 0);
	if (mSwapChain == nullptr)
	{
		return false;
	}

	mSceneConstBuffer = mDevice->CreateBuffer(rhi::BufferBinding::Constant,
		rhi::ResourceUsage::Dynamic,
		sizeof(cxx::float4) * 6);

	if (mSceneConstBuffer == nullptr)
	{
		return false;
	}

	if (!OnResize(mSwapChain->GetWidth(), mSwapChain->GetHeight()))
	{
		return false;
	}

	if (!CreateBlendModes())
	{
		return false;
	}

	SetBlendMode(g2d::BlendMode::None);

	//all creation using RenderSystem should be start here.
	if (!mTexPool.CreateDefaultTexture())
	{
		return false;
	}

	mShaderlib = new ShaderLib();
	failGuard.dismiss();

	return true;
}

void RenderSystem::Destroy()
{
	for (auto& list : mRenderRequests)
	{
		delete list.second;
	}
	mRenderRequests.clear();

	for (auto& blendMode : mBlendModes)
	{
		if (blendMode.second)
		{
			blendMode.second->Release();
		}
	}
	mBlendModes.clear();

	mGeometry.Destroy();
	mTexPool.Destroy();

	mBackBufferRT = nullptr;
	cxx::safe_delete(mShaderlib);
	cxx::safe_release(mSceneConstBuffer);
	cxx::safe_release(mRenderTarget);
	cxx::safe_release(mSwapChain);
	cxx::safe_release(mDevice);
	cxx::safe_release(mContext);
}


void RenderSystem::Clear()
{
	mContext->ClearRenderTarget(mBackBufferRT, mBkColor);
}

void RenderSystem::FlushRequests()
{
	if (mRenderRequests.size() == 0)
		return;

	Mesh batchMesh(0, 0);
	g2d::Material* material = nullptr;
	for (auto& reqList : mRenderRequests)
	{
		RenderRequestList& list = *(reqList.second);
		if (list.size() == 0)
			continue;

		for (auto& request : list)
		{
			if (material == nullptr)
			{
				material = &(request.material);
			}
			else if (!request.material.IsSame(material))//material may be nullptr.
			{
				FlushBatch(batchMesh, *material);
				material = &(request.material);
			}

			if (!batchMesh.Merge(request.mesh, request.worldMatrix))
			{
				FlushBatch(batchMesh, *material);
				//de factor, no need to Merge when there is only ONE MESH each drawcall.
				batchMesh.Merge(request.mesh, request.worldMatrix);
			}
		}
		list.clear();
	}
	if (material != nullptr)
	{
		FlushBatch(batchMesh, *material);
	}
}

void RenderSystem::Present()
{
	mSwapChain->Present();
}

void RenderSystem::SetBlendMode(g2d::BlendMode blendMode)
{
	if (mBlendModes.count(blendMode) == 0)
		return;

	mContext->SetBlendState(mBlendModes[blendMode]);
}

void RenderSystem::SetViewMatrix(const cxx::float2x3& viewMatrix)
{
	if (viewMatrix != mMatrixView)
	{
		mMatrixView = viewMatrix;
		mMatrixConstBufferDirty = true;
	}
}

const cxx::float4x4& RenderSystem::GetProjectionMatrix()
{
	if (mMatrixProjDirty)
	{
		mMatrixProjDirty = false;
		float znear = 1.0f;
		mMatProj = cxx::float4x4::ortho2d_lh(
			static_cast<float>(GetWindowWidth()),
			static_cast<float>(GetWindowHeight()),
			znear, 1000.0f);
	}
	return mMatProj;
}


bool RenderSystem::OnResize(unsigned int width, unsigned int height)
{
	if (!mSwapChain->OnResize(width, height))
	{
		return false;
	}

	//though we create an individual render target
	//we do not use it for rendering, for now.
	//it will be used when building Compositor System.
	auto rtFormat = rhi::TextureFormat::BGRA;
	mRenderTarget = mDevice->CreateRenderTarget(width, height, &rtFormat, 1, false);
	if (mRenderTarget == nullptr)
	{
		return false;
	}

	mBackBufferRT = mSwapChain->GetBackBuffer();

	mMatrixProjDirty = true;
	mMatrixConstBufferDirty = true;

	mViewport.Size = cxx::float2((float)width, (float)height);
	mContext->SetRenderTarget(mBackBufferRT);
	mContext->SetViewport(mViewport);

	return true;
}

void RenderSystem::FlushBatch(Mesh& mesh, g2d::Material& material)
{
	if (mesh.GetIndexCount() == 0)
		return;

	mGeometry.MakeEnoughVertexArray(mesh.GetVertexCount());
	mGeometry.MakeEnoughIndexArray(mesh.GetIndexCount());
	mGeometry.UploadVertices(0, mesh.GetRawVertices(), mesh.GetVertexCount());
	mGeometry.UploadIndices(0, mesh.GetRawIndices(), mesh.GetIndexCount());

	for (unsigned int i = 0; i < material.GetPassCount(); i++)
	{
		auto pass = material.GetPassByIndex(i);
		auto shader = mShaderlib->GetShaderByName(pass->GetVertexShaderName(), pass->GetPixelShaderName());
		if (shader)
		{
			rhi::VertexBufferInfo info;
			info.stride = sizeof(g2d::GeometryVertex);
			info.offset = 0;
			info.buffer = mGeometry.mVertexBuffer;
			mContext->SetVertexBuffers(0, &info, 1);
			mContext->SetIndexBuffer(mGeometry.mIndexBuffer, 0, rhi::IndexFormat::Int32);
			mContext->SetShaderProgram(shader->GetShaderProgram());
			UpdateSceneConstBuffer();
			mContext->SetVertexShaderConstantBuffers(0, &mSceneConstBuffer, 1);
			SetBlendMode(pass->GetBlendMode());

			auto vcb = shader->GetVertexConstBuffer();
			if (vcb)
			{
				auto length = (shader->GetVertexConstBuffer()->GetLength() > pass->GetVSConstantLength())
					? pass->GetVSConstantLength()
					: shader->GetVertexConstBuffer()->GetLength();
				if (length > 0)
				{
					UpdateConstBuffer(vcb, pass->GetVSConstant(), length);
					mContext->SetVertexShaderConstantBuffers(1, &vcb, 1);
				}
			}

			auto pcb = shader->GetPixelConstBuffer();
			if (pcb)
			{
				auto length = (shader->GetPixelConstBuffer()->GetLength() > pass->GetPSConstantLength())
					? pass->GetPSConstantLength()
					: shader->GetPixelConstBuffer()->GetLength();
				if (length > 0)
				{
					UpdateConstBuffer(pcb, pass->GetPSConstant(), length);
					mContext->SetPixelShaderConstantBuffers(0, &pcb, 1);
				}
			}

			if (pass->GetTextureCount() > 0)
			{
				if (mTextures.size() < pass->GetTextureCount())
				{
					mTextures.resize(pass->GetTextureCount());
					mTextureSamplers.resize(pass->GetTextureCount());
				}
				for (unsigned int t = 0; t < pass->GetTextureCount(); t++)
				{
					auto timpl = reinterpret_cast<::Texture*>(pass->GetTextureByIndex(t));
					if (timpl != nullptr)
					{
						mTextures[t] = mTexPool.GetTexture(timpl->GetResourceName());
					}
					else
					{
						mTextures[t] = mTexPool.GetDefaultTexture();
					}
					mTextureSamplers[t] = nullptr;
				}

				mContext->SetTextures(0, &(mTextures[0]), pass->GetTextureCount());
				mContext->SetTextureSampler(0, &(mTextureSamplers[0]), pass->GetTextureCount());
			}

			mContext->DrawIndexed(rhi::Primitive::TriangleList, mesh.GetIndexCount(), 0, 0);
		}
	}

	mesh.Clear();
}

void RenderSystem::UpdateConstBuffer(rhi::Buffer* cbuffer, const void* data, unsigned int length)
{
	auto mappedData = mContext->Map(cbuffer);
	if (mappedData.success)
	{
		auto dstBuffre = reinterpret_cast<uint8_t*>(mappedData.data);
		memcpy(dstBuffre, data, length);
		mContext->Unmap(cbuffer);
	}
}

void RenderSystem::UpdateSceneConstBuffer()
{
	if (!mMatrixConstBufferDirty)
		return;

	mMatrixConstBufferDirty = false;
	auto mappedData = mContext->Map(mSceneConstBuffer);
	if (mappedData.success)
	{
		auto dstBuffer = reinterpret_cast<uint8_t*>(mappedData.data);
		memcpy(dstBuffer, &(mMatrixView.r[0]), sizeof(cxx::float3));
		memcpy(dstBuffer + sizeof(cxx::float4), &(mMatrixView.r[1]), sizeof(cxx::float3));
		memcpy(dstBuffer + sizeof(cxx::float4) * 2, GetProjectionMatrix().m, sizeof(cxx::float4x4));
		mContext->Unmap(mSceneConstBuffer);
	}
}
