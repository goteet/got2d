#pragma once

#include "../include/g2drender.h"
#include <map>
#include <windows.h>
#include <d3d11.h>
#include <gmlcolor.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")



class Geometry
{
public:
	bool Create(ID3D11Device* device, unsigned int vertexCount, unsigned int indexCount);
	void UploadVertices(ID3D11DeviceContext*, g2d::GeometryVertex*);
	void UploadIndices(ID3D11DeviceContext*, unsigned int*);
	void Destroy();

	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	unsigned int m_vertexCount = 0;
	unsigned int m_indexCount = 0;
};

class ShaderSource
{
public:
	virtual const char* GetShaderName() = 0;
	virtual const char* GetVertexShaderCode() = 0;
	virtual const char* GetPixelShaderCode() = 0;
};



class Shader
{
public:
	bool Create(ID3D11Device* device, const char* vsCode, const char* psCode);
	void Destroy();

	ID3D11VertexShader* GetVertexShader();
	ID3D11PixelShader* GetPixelShader();
	ID3D11InputLayout* GetInputLayout();
private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_shaderLayout = nullptr;
};

class ShaderLib
{
public:
	ShaderLib(ID3D11Device* device);
	Shader* GetShaderByName(const char* name);

private:
	bool BuildShader(const std::string& name);
	
	std::map<std::string, ShaderSource*> m_sources;
	std::map<std::string, Shader*> m_shaders;
	ID3D11Device* m_device;
};

class RenderSystem
{
public:
	RenderSystem();

	bool Create(void* nativeWindow);
	bool OnResize(int width, int height);
	void Destroy();

	void Clear();
	void Render();
	void Present();

private:
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_d3dDevice = nullptr;
	ID3D11DeviceContext* m_d3dContext = nullptr;
	ID3D11Texture2D* m_colorTexture = nullptr;
	ID3D11RenderTargetView* m_rtView = nullptr;
	ID3D11RenderTargetView* m_bbView = nullptr;
	D3D11_VIEWPORT m_viewport;

	gml::color4 m_bkColor;


	Geometry m_geometry;

	ShaderLib* shaderlib = nullptr;
};
