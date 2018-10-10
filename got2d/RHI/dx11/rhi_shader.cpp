#include "inner_RHI.h"
#include "../../source/scope_utility.h"

VertexShader::VertexShader(ID3D11VertexShader& vertexShader, ID3D11InputLayout& inputLayout, std::vector<rhi::Semantic>&& layouts)
	: m_vertexShader(vertexShader)
	, m_inputLayout(inputLayout)
	, m_semantics(std::move(layouts))
{

}

VertexShader::~VertexShader()
{
	m_vertexShader.Release();
	m_inputLayout.Release();
}

void VertexShader::Release()
{
	if (--m_refCount == 0)
	{
		delete this;
	}
}

void VertexShader::AddReference()
{
	m_refCount++;
}

rhi::Semantic VertexShader::GetSemanticByIndex(rhi::SemanticIndex index) const
{
	ENSURE(index < GetSemanticCount());
	return m_semantics.at(index);
}

rhi::SemanticIndex VertexShader::GetSemanticCount() const
{
	return static_cast<unsigned int>(m_semantics.size());
}

PixelShader::PixelShader(ID3D11PixelShader& pixelShader)
	: m_pixelShader(pixelShader)
{

}

PixelShader::~PixelShader()
{
	m_pixelShader.Release();
}

void PixelShader::Release()
{
	if (--m_refCount == 0)
	{
		delete this;
	}
}

void PixelShader::AddReference()
{
	m_refCount++;
}

ShaderProgram::ShaderProgram(::VertexShader& vertexShader, ::PixelShader& pixelShader)
	: m_vertexShader(&vertexShader)
	, m_pixelShader(&pixelShader)
{
	m_vertexShader->AddReference();
	m_pixelShader->AddReference();
}
