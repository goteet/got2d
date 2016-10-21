#pragma once
#include <map>
#include <vector>
#include "g2drender.h"
#include "../RHI/RHI.h"


class VSData
{
public:
	virtual ~VSData() {}

	virtual const char* GetName() = 0;

	virtual const char* GetCode() = 0;

	virtual unsigned int GetConstBufferLength() = 0;
};

class PSData
{
public:
	virtual ~PSData() {}

	virtual const char* GetName() = 0;

	virtual const char* GetCode() = 0;

	virtual unsigned int GetConstBufferLength() = 0;
};

class Shader
{
public:
	bool Create(const std::string& vsCode, unsigned int vcbLength, const std::string& psCode, unsigned int pcbLength);

	void Destroy();

	rhi::ShaderProgram* GetShaderProgram() { return mShaderProgram; }

	rhi::Buffer* GetVertexConstBuffer() { return mVertexConstBuffer; }

	rhi::Buffer* GetPixelConstBuffer() { return mPixelConstBuffer; }

private:
	rhi::ShaderProgram* mShaderProgram = nullptr;
	rhi::Buffer* mVertexConstBuffer = nullptr;
	rhi::Buffer* mPixelConstBuffer = nullptr;
};

class ShaderLib
{
public:
	ShaderLib();

	~ShaderLib();

	Shader* GetShaderByName(const std::string& vsName, const std::string& psName);

private:
	bool BuildShader(const std::string& effectName, const std::string& vsName, const std::string& psName);

	std::string GetEffectName(const std::string& vsName, const std::string& psName);

	std::map<std::string, VSData*> mVsSources;
	std::map<std::string, PSData*> mPsSources;
	std::map<std::string, Shader*> mShaders;
};