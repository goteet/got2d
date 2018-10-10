#include "render_system.h"

g2d::Material* g2d::Material::CreateColorTexture()
{
	auto mat = new ::Material(1);
	mat->SetPass(0, new ::Pass("default", "color.texture"));
	mat->GetPassByIndex(0)->SetTexture(0, &::Texture::Default(), false);
	return mat;
}

g2d::Material* g2d::Material::CreateSimpleTexture()
{
	auto mat = new ::Material(1);
	mat->SetPass(0, new ::Pass("default", "simple.texture"));
	mat->GetPassByIndex(0)->SetTexture(0, &::Texture::Default(), false);
	return mat;
}

g2d::Material* g2d::Material::CreateSimpleColor()
{
	auto mat = new ::Material(1);
	mat->SetPass(0, new ::Pass("default", "simple.color"));
	return mat;
}

bool Shader::Create(const std::string& vsCode, unsigned int vcbLength, const std::string& psCode, unsigned int pcbLength)
{
	rhi::Semantic layouts[3] =
	{
		{ "POSITION", 0, 0, 0xFFFFFFFF, rhi::InputFormat::Float2, false, 0 },
		{ "TEXCOORD", 0, 0, 0xFFFFFFFF, rhi::InputFormat::Float2, false, 0 },
		{ "COLOR",    0, 0, 0xFFFFFFFF, rhi::InputFormat::Float4, false, 0 },
	};

	autor<rhi::VertexShader> vertexShader = GetRenderSystem().GetDevice()->CreateVertexShader(vsCode.c_str(), "VSMain", layouts, 3);
	autor<rhi::PixelShader> pixelShader = GetRenderSystem().GetDevice()->CreatePixelShader(psCode.c_str(), "PSMain");

	if (vertexShader.is_null() || pixelShader.is_null())
		return false;

	autor<rhi::ShaderProgram> shaderProgram = nullptr;
	autor<rhi::Buffer> vertexConstBuffer = nullptr;
	autor<rhi::Buffer> pixelConstBuffer = nullptr;

	shaderProgram = GetRenderSystem().GetDevice()->LinkShader(vertexShader, pixelShader);
	if (shaderProgram.is_null())
		return false;

	if (vcbLength > 0)
	{
		vertexConstBuffer = GetRenderSystem().GetDevice()->CreateBuffer(
			rhi::BufferBinding::Constant,
			rhi::ResourceUsage::Dynamic,
			vcbLength
		);
		if (vertexConstBuffer.is_null())
			return false;
	}

	if (pcbLength > 0)
	{
		pixelConstBuffer = GetRenderSystem().GetDevice()->CreateBuffer(
			rhi::BufferBinding::Constant,
			rhi::ResourceUsage::Dynamic,
			pcbLength
		);
		if (pixelConstBuffer.is_null())
			return false;
	}

	mShaderProgram = std::move(shaderProgram);
	mVertexConstBuffer = std::move(vertexConstBuffer);
	mPixelConstBuffer = std::move(pixelConstBuffer);
	return true;
}

void Shader::Destroy()
{
	mShaderProgram.release();
	mVertexConstBuffer.release();
	mPixelConstBuffer.release();
}

class DefaultVSData : public VSData
{
public:
	virtual const char* GetName() override { return "default"; }
	virtual const char* GetCode() override
	{
		return R"(
			cbuffer scene
			{
				float4x2 matrixView;
				float4x4 matrixProj;
			}
			struct GeometryVertex
			{
				float2 position : POSITION;
				float2 texcoord : TEXCOORD0;
				float4 vtxcolor : COLOR;
			};
			struct VertexOutput
			{
				float4 position : SV_POSITION;
				float2 texcoord : TEXCOORD0;
				float4 vtxcolor : COLOR;
			};
			VertexOutput VSMain(GeometryVertex input)
			{
				VertexOutput output;
				float3 position = float3(input.position,1);
				float2 viewPos = float2(
					dot(position, float3(matrixView[0][0],matrixView[1][0],matrixView[2][0])),
					dot(position, float3(matrixView[0][1],matrixView[1][1],matrixView[2][1])));
				output.position = mul(float4(viewPos, 0, 1), matrixProj);
				output.texcoord = input.texcoord;
				output.vtxcolor = input.vtxcolor;
				return output;
			}
		)";
	}
	virtual unsigned int GetConstBufferLength() override { return 0; }
};

class SimpleColorPSData : public PSData
{
	virtual const char* GetName() override { return "simple.color"; }
	virtual const char* GetCode() override
	{
		return R"(
			struct VertexInput
			{
				float4 position : SV_POSITION;
				float2 texcoord : TEXCOORD0;
				float4 vtxcolor : COLOR;
			};
			float4 PSMain(VertexInput input):SV_TARGET
			{
				return input.vtxcolor;
			}
		)";
	}
	virtual unsigned int GetConstBufferLength() override { return 0; }
};

class SimpleTexturePSData : public PSData
{
	virtual const char* GetName() override { return "simple.texture"; }
	virtual const char* GetCode() override
	{
		return R"(
			Texture2D Tex;
			SamplerState State;
			struct VertexInput
			{
				float4 position : SV_POSITION;
				float2 texcoord : TEXCOORD0;
				float4 vtxcolor : COLOR;
			};
			float4 PSMain(VertexInput input):SV_TARGET
			{
				return Tex.Sample(State, input.texcoord);
			}
		)";
	}
	virtual unsigned int GetConstBufferLength() override { return 0; }
};

class ColorTexturePSData : public PSData
{
	virtual const char* GetName() override { return "color.texture"; }
	virtual const char* GetCode() override
	{
		return R"(
			Texture2D Tex;
			SamplerState State;
			struct VertexInput
			{
				float4 position : SV_POSITION;
				float2 texcoord : TEXCOORD0;
				float4 vtxcolor : COLOR;
			};
			float4 PSMain(VertexInput input):SV_TARGET
			{
				return input.vtxcolor * Tex.Sample(State, input.texcoord);
			}
		)";
	}
	virtual unsigned int GetConstBufferLength() override { return 0; }
};

ShaderLib::ShaderLib()
{
	VSData* vsd = new DefaultVSData();
	mVsSources[vsd->GetName()] = vsd;

	PSData* psd;
	psd = new SimpleColorPSData();
	mPsSources[psd->GetName()] = psd;

	psd = new SimpleTexturePSData();
	mPsSources[psd->GetName()] = psd;

	psd = new ColorTexturePSData();
	mPsSources[psd->GetName()] = psd;
}

ShaderLib::~ShaderLib()
{
	for (auto& psd : mPsSources)
	{
		delete psd.second;
	}
	mPsSources.clear();

	for (auto& vsd : mVsSources)
	{
		delete vsd.second;
	}
	mVsSources.clear();

	for (auto& shader : mShaders)
	{
		delete shader.second;
	}
	mShaders.clear();

}

std::string ShaderLib::GetEffectName(const std::string& vsName, const std::string& psName)
{
	return std::move(vsName + psName);
}

Shader* ShaderLib::GetShaderByName(const std::string& vsName, const std::string& psName)
{
	std::string effectName = GetEffectName(vsName, psName);
	if (!mShaders.count(effectName))
	{
		if (!BuildShader(effectName, vsName, psName))
		{
			return nullptr;
		}
	}
	return mShaders.at(effectName);
}

bool ShaderLib::BuildShader(const std::string& effectName, const std::string& vsName, const std::string& psName)
{
	auto vsData = mVsSources[vsName];
	auto psData = mPsSources[psName];
	if (vsData == nullptr || psData == nullptr)
		return false;

	Shader* shader = new Shader();
	if (shader->Create(
		vsData->GetCode(), vsData->GetConstBufferLength(),
		psData->GetCode(), psData->GetConstBufferLength()))
	{
		mShaders[effectName] = shader;
		return true;
	}
	delete shader;
	return false;
}

Pass::Pass(const Pass& other)
	: mVsName(other.mVsName)
	, mPsName(other.mPsName)
	, mTextures(other.mTextures.size())
	, mVsConstants(other.mVsConstants.size())
	, mPsConstants(other.mPsConstants.size())
	, mBlendMode(other.mBlendMode)
{
	for (size_t i = 0, n = mTextures.size(); i < n; i++)
	{
		mTextures[i] = other.mTextures[i];
		mTextures[i]->AddRef();
	}

	if (other.GetVSConstantLength() > 0)
	{
		memcpy(&(mVsConstants[0]), &(other.mVsConstants[0]), other.GetVSConstantLength());
	}
	if (other.GetPSConstantLength() > 0)
	{
		memcpy(&(mPsConstants[0]), &(other.mPsConstants[0]), other.GetPSConstantLength());
	}
}

Pass::~Pass()
{
	for (auto& t : mTextures)
	{
		t->Release();
	}
	mTextures.clear();
}

Pass* Pass::Clone()
{
	Pass* p = new Pass(*this);
	return p;
}

bool Pass::IsSame(g2d::Pass* other) const
{
	ENSURE(other != nullptr);
	if (!IsSameType(other))
		return false;

	auto p = reinterpret_cast<Pass*>(&other);

	if (this == p)
		return true;

	if (mBlendMode != p->mBlendMode ||
		mVsName != p->mVsName ||
		mPsName != p->mPsName)
		return false;

	if (mTextures.size() != p->mTextures.size() ||
		mVsConstants.size() != p->mVsConstants.size() ||
		mPsConstants.size() != p->mPsConstants.size())
	{
		return false;
	}

	for (size_t i = 0, n = mTextures.size(); i < n; i++)
	{
		if (!mTextures[i]->IsSame(p->mTextures[i]))
		{
			return false;
		}
	}

	//we have no idea how to deal with floats.
	if (mVsConstants.size() > 0 &&
		0 != memcmp(&(mVsConstants[0]), &(p->mVsConstants[0]), mVsConstants.size() * sizeof(float)))
	{
		return  false;
	}

	if (mPsConstants.size() > 0 &&
		0 != memcmp(&(mPsConstants[0]), &(p->mPsConstants[0]), mPsConstants.size() * sizeof(float)))
	{
		return false;
	}
	return true;
}

void Pass::SetTexture(unsigned int index, g2d::Texture* tex, bool autoRelease)
{
	size_t size = mTextures.size();
	if (index >= size)
	{
		mTextures.resize(index + 1);
		for (size_t i = size; i < index; i++)
		{
			mTextures[i] = nullptr;
		}
	}

	if (mTextures[index])
	{
		mTextures[index]->Release();
	}
	mTextures[index] = tex;
	if (!autoRelease)
	{
		mTextures[index]->AddRef();
	}
}

void Pass::SetVSConstant(unsigned int index, float* data, unsigned int size, unsigned int count)
{
	if (count == 0)
		return;

	if (index + count > mVsConstants.size())
	{
		mVsConstants.resize(index + count);
	}

	for (unsigned int i = 0; i < count; i++)
	{
		memcpy(&(mVsConstants[index + i]), data + i*size, size);
	}
}

void Pass::SetPSConstant(unsigned int index, float* data, unsigned int size, unsigned int count)
{
	if (count == 0)
		return;

	if (index + count > mPsConstants.size())
	{
		mPsConstants.resize(index + count);
	}

	for (unsigned int i = 0; i < count; i++)
	{
		memcpy(&(mPsConstants[index + i]), data + i*size, size);
	}
}

Material::Material(unsigned int passCount)
	: mPasses(passCount)
{

}

Material::Material(const Material& other)
	: mPasses(other.mPasses.size())
{
	for (size_t i = 0, n = mPasses.size(); i < n; i++)
	{
		mPasses[i] = other.mPasses[i]->Clone();
	}
}

void Material::SetPass(unsigned int index, Pass* p)
{
	ENSURE(index < GetPassCount());
	mPasses[index] = p;
}

Material::~Material()
{
	for (auto& p : mPasses)
	{
		p->Release();
	}
	mPasses.clear();
}

g2d::Pass* Material::GetPassByIndex(unsigned int index) const
{
	ENSURE(index < GetPassCount());
	return mPasses.at(index);
}

unsigned int Material::GetPassCount() const
{
	return static_cast<unsigned int>(mPasses.size());
}

bool Material::IsSame(g2d::Material* other) const
{
	ENSURE(other != nullptr);

	if (this == other)
		return true;

	if (!IsSameType(other))
		return false;

	if (other->GetPassCount() != GetPassCount())
		return false;

	for (unsigned int i = 0; i < GetPassCount(); i++)
	{
		if (!mPasses[i]->IsSame(other->GetPassByIndex(i)))
		{
			return false;
		}
	}
	return true;
}

g2d::Material* Material::Clone() const
{
	Material* newMat = new Material(*this);
	return newMat;
}

void Material::Release()
{
	delete this;
}
