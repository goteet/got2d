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

bool Shader::Create(const std::string& vsCode, uint32_t vcbLength, const std::string& psCode, uint32_t pcbLength)
{
	rhi::Semantic layouts[3] =
	{
		{ "POSITION", 0, 0, 0xFFFFFFFF, rhi::InputFormat::Float2, false, 0 },
		{ "TEXCOORD", 0, 0, 0xFFFFFFFF, rhi::InputFormat::Float2, false, 0 },
		{ "COLOR",    0, 0, 0xFFFFFFFF, rhi::InputFormat::Float4, false, 0 },
	};

	autor<rhi::VertexShader> vertexShader = GetRenderSystem()->GetDevice()->CreateVertexShader(vsCode.c_str(), "VSMain", layouts, 3);
	autor<rhi::PixelShader> pixelShader = GetRenderSystem()->GetDevice()->CreatePixelShader(psCode.c_str(), "PSMain");

	if (vertexShader.is_null() || pixelShader.is_null())
		return false;

	autor<rhi::ShaderProgram> shaderProgram = nullptr;
	autor<rhi::Buffer> vertexConstBuffer = nullptr;
	autor<rhi::Buffer> pixelConstBuffer = nullptr;

	shaderProgram = GetRenderSystem()->GetDevice()->LinkShader(vertexShader, pixelShader);
	if (shaderProgram.is_null())
		return false;

	if (vcbLength > 0)
	{
		vertexConstBuffer = GetRenderSystem()->GetDevice()->CreateBuffer(
			rhi::BufferBinding::Constant,
			rhi::ResourceUsage::Dynamic,
			vcbLength
		);
		if (vertexConstBuffer.is_null())
			return false;
	}

	if (pcbLength > 0)
	{
		pixelConstBuffer = GetRenderSystem()->GetDevice()->CreateBuffer(
			rhi::BufferBinding::Constant,
			rhi::ResourceUsage::Dynamic,
			pcbLength
		);
		if (pixelConstBuffer.is_null())
			return false;
	}

	m_shaderProgram = std::move(shaderProgram);
	m_vertexConstBuffer = std::move(vertexConstBuffer);
	m_pixelConstBuffer = std::move(pixelConstBuffer);
	return true;
}

void Shader::Destroy()
{
	m_shaderProgram.release();
	m_vertexConstBuffer.release();
	m_pixelConstBuffer.release();
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
	virtual uint32_t GetConstBufferLength() override { return 0; }
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
	virtual uint32_t GetConstBufferLength() override { return 0; }
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
	virtual uint32_t GetConstBufferLength() override { return 0; }
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
	virtual uint32_t GetConstBufferLength() override { return 0; }
};

ShaderLib::ShaderLib()
{
	VSData* vsd = new DefaultVSData();
	m_vsSources[vsd->GetName()] = vsd;

	PSData* psd;
	psd = new SimpleColorPSData();
	m_psSources[psd->GetName()] = psd;

	psd = new SimpleTexturePSData();
	m_psSources[psd->GetName()] = psd;

	psd = new ColorTexturePSData();
	m_psSources[psd->GetName()] = psd;
}

ShaderLib::~ShaderLib()
{
	for (auto& psd : m_psSources)
	{
		delete psd.second;
	}
	m_psSources.clear();

	for (auto& vsd : m_vsSources)
	{
		delete vsd.second;
	}
	m_vsSources.clear();

	for (auto& shader : m_shaders)
	{
		delete shader.second;
	}
	m_shaders.clear();

}

std::string ShaderLib::GetEffectName(const std::string& vsName, const std::string& psName)
{
	return std::move(vsName + psName);
}

Shader* ShaderLib::GetShaderByName(const std::string& vsName, const std::string& psName)
{
	std::string effectName = GetEffectName(vsName, psName);
	if (!m_shaders.count(effectName))
	{
		if (!BuildShader(effectName, vsName, psName))
		{
			return nullptr;
		}
	}
	return m_shaders.at(effectName);
}

bool ShaderLib::BuildShader(const std::string& effectName, const std::string& vsName, const std::string& psName)
{
	auto vsData = m_vsSources[vsName];
	auto psData = m_psSources[psName];
	if (vsData == nullptr || psData == nullptr)
		return false;

	Shader* shader = new Shader();
	if (shader->Create(
		vsData->GetCode(), vsData->GetConstBufferLength(),
		psData->GetCode(), psData->GetConstBufferLength()))
	{
		m_shaders[effectName] = shader;
		return true;
	}
	delete shader;
	return false;
}

Pass::Pass(const Pass& other)
	: m_vsName(other.m_vsName)
	, m_psName(other.m_psName)
	, m_textures(other.m_textures.size())
	, m_vsConstants(other.m_vsConstants.size())
	, m_psConstants(other.m_psConstants.size())
	, m_blendMode(other.m_blendMode)
{
	for (size_t i = 0, n = m_textures.size(); i < n; i++)
	{
		m_textures[i] = other.m_textures[i];
		m_textures[i]->AddRef();
	}

	if (other.GetVSConstantLength() > 0)
	{
		memcpy(&(m_vsConstants[0]), &(other.m_vsConstants[0]), other.GetVSConstantLength());
	}
	if (other.GetPSConstantLength() > 0)
	{
		memcpy(&(m_psConstants[0]), &(other.m_psConstants[0]), other.GetPSConstantLength());
	}
}

Pass::~Pass()
{
	for (auto& t : m_textures)
	{
		t->Release();
	}
	m_textures.clear();
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

	if (m_blendMode != p->m_blendMode ||
		m_vsName != p->m_vsName ||
		m_psName != p->m_psName)
		return false;

	if (m_textures.size() != p->m_textures.size() ||
		m_vsConstants.size() != p->m_vsConstants.size() ||
		m_psConstants.size() != p->m_psConstants.size())
	{
		return false;
	}

	for (size_t i = 0, n = m_textures.size(); i < n; i++)
	{
		if (!m_textures[i]->IsSame(p->m_textures[i]))
		{
			return false;
		}
	}

	//we have no idea how to deal with floats.
	if (m_vsConstants.size() > 0 &&
		0 != memcmp(&(m_vsConstants[0]), &(p->m_vsConstants[0]), m_vsConstants.size() * sizeof(float)))
	{
		return  false;
	}

	if (m_psConstants.size() > 0 &&
		0 != memcmp(&(m_psConstants[0]), &(p->m_psConstants[0]), m_psConstants.size() * sizeof(float)))
	{
		return false;
	}
	return true;
}

void Pass::SetTexture(uint32_t index, g2d::Texture* tex, bool autoRelease)
{
	size_t size = m_textures.size();
	if (index >= size)
	{
		m_textures.resize(index + 1);
		for (size_t i = size; i < index; i++)
		{
			m_textures[i] = nullptr;
		}
	}

	if (m_textures[index])
	{
		m_textures[index]->Release();
	}
	m_textures[index] = tex;
	if (!autoRelease)
	{
		m_textures[index]->AddRef();
	}
}

void Pass::SetVSConstant(uint32_t index, float* data, uint32_t size, uint32_t count)
{
	if (count == 0)
		return;

	if (index + count > m_vsConstants.size())
	{
		m_vsConstants.resize(index + count);
	}

	for (uint32_t i = 0; i < count; i++)
	{
		memcpy(&(m_vsConstants[index + i]), data + i*size, size);
	}
}

void Pass::SetPSConstant(uint32_t index, float* data, uint32_t size, uint32_t count)
{
	if (count == 0)
		return;

	if (index + count > m_psConstants.size())
	{
		m_psConstants.resize(index + count);
	}

	for (uint32_t i = 0; i < count; i++)
	{
		memcpy(&(m_psConstants[index + i]), data + i*size, size);
	}
}

Material::Material(uint32_t passCount)
	: m_passes(passCount)
{

}

Material::Material(const Material& other)
	: m_passes(other.m_passes.size())
{
	for (size_t i = 0, n = m_passes.size(); i < n; i++)
	{
		m_passes[i] = other.m_passes[i]->Clone();
	}
}

void Material::SetPass(uint32_t index, Pass* p)
{
	ENSURE(index < GetPassCount());
	m_passes[index] = p;
}

Material::~Material()
{
	for (auto& p : m_passes)
	{
		p->Release();
	}
	m_passes.clear();
}

g2d::Pass* Material::GetPassByIndex(uint32_t index) const
{
	ENSURE(index < GetPassCount());
	return m_passes.at(index);
}

uint32_t Material::GetPassCount() const
{
	return static_cast<uint32_t>(m_passes.size());
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

	for (uint32_t i = 0; i < GetPassCount(); i++)
	{
		if (!m_passes[i]->IsSame(other->GetPassByIndex(i)))
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
