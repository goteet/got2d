#include "shader.h"
#include "../system_blackboard.h"
#include "render_system.h"

bool Shader::Create(const std::string& vsCode, unsigned int vcbLength, const std::string& psCode, unsigned int pcbLength)
{
	rhi::Semantic layouts[3] =
	{
		{ "POSITION", 0, 0, 0xFFFFFFFF, rhi::InputFormat::Float2, false, 0 },
		{ "TEXCOORD", 0, 0, 0xFFFFFFFF, rhi::InputFormat::Float2, false, 0 },
		{ "COLOR",    0, 0, 0xFFFFFFFF, rhi::InputFormat::Float4, false, 0 },
	};

	rhi::VertexShader* vertexShader = GetRenderSystem().GetDevice()->CreateVertexShader(vsCode.c_str(), "VSMain", layouts, 3);
	rhi::PixelShader* pixelShader = GetRenderSystem().GetDevice()->CreatePixelShader(psCode.c_str(), "PSMain");

	if (vertexShader == nullptr || pixelShader == nullptr)
		return false;

	rhi::ShaderProgram* shaderProgram = nullptr;
	rhi::Buffer* vertexConstBuffer = nullptr;
	rhi::Buffer* pixelConstBuffer = nullptr;

	shaderProgram = GetRenderSystem().GetDevice()->LinkShader(vertexShader, pixelShader);
	if (shaderProgram == nullptr)
		return false;

	if (vcbLength > 0)
	{
		vertexConstBuffer = GetRenderSystem().GetDevice()->CreateBuffer(
			rhi::BufferBinding::Constant,
			rhi::ResourceUsage::Dynamic,
			vcbLength
		);
		if (vertexConstBuffer == nullptr)
			return false;
	}

	if (pcbLength > 0)
	{
		pixelConstBuffer = GetRenderSystem().GetDevice()->CreateBuffer(
			rhi::BufferBinding::Constant,
			rhi::ResourceUsage::Dynamic,
			pcbLength
		);
		if (pixelConstBuffer == nullptr)
			return false;
	}

	mShaderProgram = std::move(shaderProgram);
	mVertexConstBuffer = std::move(vertexConstBuffer);
	mPixelConstBuffer = std::move(pixelConstBuffer);
	return true;
}

void Shader::Destroy()
{
	cxx::safe_release(mShaderProgram);
	cxx::safe_release(mVertexConstBuffer);
	cxx::safe_release(mPixelConstBuffer);
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
