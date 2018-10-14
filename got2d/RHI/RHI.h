#pragma once
#include <cinttypes>
#include "cxx_math/cxx_color.h"
#include "cxx_math/cxx_vector.h"
#include "../source/scope_utility.h"

namespace rhi
{
	enum class BufferBinding : int
	{
		Vertex = 0,
		Index = 1,
		Constant = 2,
	};

	enum class ResourceUsage : int
	{
		Default = 0,
		Dynamic = 1,
	};

	enum class IndexFormat : int
	{
		Int16 = 0,
		Int32 = 1,
	};

	enum class Primitive : int
	{
		TriangleList = 0,
		TriangleStrip = 1,
	};

	enum class TextureFormat : int
	{
		Unknown = 0,
		RGBA = 1, BGRA = 2,
		DXT1 = 3, DXT3 = 4, DXT5 = 5,
		D24S8 = 6, Float32 = 7,
	};

	enum class InputFormat : int
	{
		Float2, Float3, Float4,
	};

	enum class BlendFactor : int
	{
		Zero = 0,
		One = 1,
		SourceColor = 2,
		SourceAlpha = 3,
		DestinationColor = 4,
		DestinationAlpha = 5,
		InverseSourceColor = 6,
		InverseSourceAlpha = 7,
		InverseDestinationColor = 8,
		InverseDestinationAlpha = 9,
	};

	enum class BlendOperator : int
	{
		Add = 0, Sub = 1,
	};

	class TextureBinding
	{
	public:
		constexpr static int ShaderResource = 1 << 0;
		constexpr static int RenderTarget = 1 << 1;
		constexpr static int DepthStencil = 1 << 2;
		constexpr static int StreamOutput = 1 << 3;
		constexpr static int Unordered = 1 << 4;
		constexpr static int Count = 5;
	};

	enum class SamplerFilter : int
	{
		MinMagMipPoint,
		MinMagMipLinear,
	};

	enum class TextureAddress : int
	{
		Repeat = 0,
		Clamp = 1,
		Mirror = 2,
	};

	class RHIObject
	{
	public:
		RHIObject() = default;
		virtual ~RHIObject() { }
		virtual void Release() = 0;
	private:
		RHIObject(const RHIObject&) = delete;
		RHIObject(const RHIObject&&) = delete;
		RHIObject& operator=(const RHIObject&) = delete;
	};

	template<typename T, T TMin, T TMax> struct CountRange
	{
		const T Count;
		constexpr static T Min = TMin;
		constexpr static T Max = TMax;
		CountRange(T c) : Count(c) { ENSURE(Count >= TMin && Count <= TMax); }
		operator T() const { return Count; }
		CountRange operator+(T offset) const { return Count + offset; }
		CountRange operator-(T offset) const { return Count - offset; }
	};

	template<unsigned int TEnd> struct Index
	{
		const unsigned int Value;
		constexpr static unsigned int End = TEnd;
		constexpr static unsigned int Max = TEnd - 1;
		Index(unsigned int i) : Value(i) { ENSURE(Value < TEnd); }
		operator unsigned int() const { return Value; }
		Index operator+(unsigned int offset) const { return Value + offset; }
		Index operator-(unsigned int offset) const { return Value - offset; }
	};

	constexpr unsigned int MinRenderTarget = 1;
	constexpr unsigned int MaxRenderTarget = 4;
	typedef CountRange<unsigned int, MinRenderTarget, MaxRenderTarget> RTCount;
	typedef Index<MaxRenderTarget> RTIndex;


	constexpr unsigned int MinSemantic = 1;
	constexpr unsigned int MaxSemantic = 8;
	typedef CountRange<unsigned int, MinSemantic, MaxSemantic> SemanticCount;
	typedef Index<MaxSemantic> SemanticIndex;


	class Buffer : public RHIObject
	{
	public:
		virtual rhi::BufferBinding GetBinding() const = 0;

		virtual rhi::ResourceUsage GetUsage() const = 0;

		virtual unsigned int GetLength() const = 0;
	};

	class Texture2D : public RHIObject
	{
	public:
		virtual unsigned int GetWidth() const = 0;

		virtual unsigned int GetHeight() const = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual bool IsRenderTarget() const = 0;

		virtual bool IsShaderResource() const = 0;

		virtual bool IsDepthStencil() const = 0;
	};

	class RenderTarget : public RHIObject
	{
	public:
		virtual Texture2D* GetColorBufferByIndex(RTIndex index) const = 0;

		virtual unsigned int GetColorBufferCount() const = 0;

		// return nullptr if depth/stencil is not enabled.
		virtual Texture2D* GetDepthStencilBuffer() const = 0;

		virtual bool IsDepthStencilUsed() const = 0;

		virtual unsigned int GetWidth() const = 0;

		virtual unsigned int GetHeight() const = 0;
	};


	struct Semantic
	{
		const char* SemanticName = "";
		unsigned int SemanticIndex = 0;
		unsigned int InputSlot = 0;
		unsigned int AlignOffset = 0xFFFFFFFF;
		InputFormat Format = InputFormat::Float4;
		bool IsInstanced = false;
		unsigned int InstanceRepeatRate = 1;
	};

	class VertexShader : public RHIObject
	{
	public:
		virtual void AddReference() = 0;

		virtual Semantic GetSemanticByIndex(SemanticIndex index) const = 0;

		virtual SemanticIndex GetSemanticCount() const = 0;

		//virtual unsigned int GetConstantBufferLength() const = 0;
	};

	class PixelShader : public RHIObject
	{
	public:
		virtual void AddReference() = 0;

		//virtual unsigned int GetConstantBufferLength() const = 0;

		//virtual unsigned int GetTextureSlotByIndex() const = 0;

		//virtual unsigned int GetUsedTextureSlotCount() const = 0;
	};

	class ShaderProgram : public RHIObject
	{
	public:
		virtual VertexShader* GetVertexShader() const = 0;

		virtual PixelShader* GetPixelShader() const = 0;
	};

	class BlendState : public RHIObject
	{
	public:
		virtual bool IsEnabled() const = 0;

		virtual BlendFactor GetSourceFactor() const = 0;

		virtual BlendFactor GetDestinationFactor() const = 0;

		virtual BlendOperator GetOperator() const = 0;
	};

	class TextureSampler : public RHIObject { };

	class SwapChain : public RHIObject
	{
	public:
		virtual RenderTarget* GetBackBuffer() const = 0;

		virtual unsigned int GetWidth() const = 0;

		virtual unsigned int GetHeight() const = 0;

		virtual bool OnResize(unsigned int width, unsigned int height) = 0;

		virtual void SetFullscreen(bool fullscreen) = 0;

		virtual bool IsFullscreen() const = 0;

		virtual void Present() = 0;
	};

	class Device : public RHIObject
	{
	public:
		// if width / height is specified as zero, default size of native window will be used.
		virtual SwapChain* CreateSwapChain(void* nativeWindow, bool useDepthStencil, unsigned int windowWidth, unsigned int windowHeight) = 0;

		virtual Buffer* CreateBuffer(BufferBinding binding, ResourceUsage usage, unsigned int bufferLength) = 0;

		virtual Texture2D* CreateTexture2D(TextureFormat format, ResourceUsage usage, unsigned int binding, unsigned int width, unsigned int height) = 0;

		virtual VertexShader* CreateVertexShader(const char* source, const char* entry, Semantic* layouts, SemanticCount layoutCount) = 0;

		virtual PixelShader* CreatePixelShader(const char* source, const char* entry) = 0;

		virtual ShaderProgram* LinkShader(VertexShader*, PixelShader*) = 0;

		virtual BlendState* CreateBlendState(bool enabled, BlendFactor source, BlendFactor dest, BlendOperator op) = 0;

		virtual TextureSampler* CreateTextureSampler(SamplerFilter filter, TextureAddress addressU, TextureAddress addressV) = 0;

		virtual RenderTarget* CreateRenderTarget(unsigned int width, unsigned int height, TextureFormat* rtFormats, RTCount rtCount, bool useDpethStencil) = 0;
	};

	struct Viewport
	{
		cxx::float2 LTPosition;
		cxx::float2 Size;
		cxx::float2 MinMaxZ;
	};

	struct VertexBufferInfo
	{
		Buffer* buffer = nullptr;
		unsigned int stride = 0;
		unsigned int offset = 0;
	};

	struct MappedResource
	{
		bool success = false;
		void* data = nullptr;
		unsigned int linePitch = 0;
	};

	class Context :public RHIObject
	{
	public:
		virtual void ClearRenderTarget(RenderTarget* renderTarget, cxx::color4f clearColor) = 0;

		virtual void SetViewport(const Viewport& viewport) = 0;

		virtual void SetRenderTarget(RenderTarget* renderTarget) = 0;

		virtual void SetVertexBuffers(unsigned int startSlot, VertexBufferInfo* buffers, unsigned int bufferCount) = 0;

		virtual void SetIndexBuffer(Buffer* buffer, unsigned int offset, IndexFormat format) = 0;

		virtual void SetShaderProgram(ShaderProgram* program) = 0;

		virtual void SetVertexShaderConstantBuffers(unsigned int startSlot, Buffer** buffers, unsigned int bufferCount) = 0;

		virtual void SetPixelShaderConstantBuffers(unsigned int startSlot, Buffer** buffers, unsigned int bufferCount) = 0;

		virtual void SetTextures(unsigned int startSlot, Texture2D** textures, unsigned int resCount) = 0;

		virtual void SetBlendState(BlendState* state) = 0;

		virtual void SetTextureSampler(unsigned int startSlot, TextureSampler** samplers, unsigned int count) = 0;

		virtual void DrawIndexed(Primitive primitive, unsigned int indexCount, unsigned int startIndex, unsigned int baseVertex) = 0;

		virtual MappedResource Map(Buffer* buffer) = 0;

		virtual MappedResource Map(Texture2D* buffer) = 0;

		virtual void Unmap(Buffer* buffer) = 0;

		virtual void Unmap(Texture2D* buffer) = 0;

		virtual void GenerateMipmaps(Texture2D* texture) = 0;
	};

	struct RHICreationResult
	{
		bool Success = false;
		Device* DevicePtr = nullptr;
		Context* ContextPtr = nullptr;
	};

	RHICreationResult CreateRHI();
}