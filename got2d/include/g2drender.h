#pragma once
#include "g2dobject.h"

namespace g2d
{
	// Semi-transparent blending method.
	enum class G2DAPI BlendMode
	{
		None,		// src*1 + dst*0 = src

		Normal,		// src*src_a + dst*(1-src_a)

		Additve,	// src*1 + dst*1
	};

	// Memory layout of Mesh.
	struct GeometryVertex
	{
		cxx::float2 position;
		cxx::float2 texcoord;
		cxx::color4f vtxcolor;
	};

	// User defined model mesh, it is a render resource.
	// Mesh data save in memory, render system will upload 
	// datas to video memory when rendering, depends on which
	// material be used.
	struct G2DAPI Mesh : public Object
	{
	public:
		static Mesh* Create(unsigned int vertexCount, unsigned int indexCount);

		// Call this manually when the mesh no longer 
		// being used, to release used memory and itself.
		// Function can ONLY be called ONCE.
		virtual void Release() = 0;

		// Retrieve the memory pointer of vertices data.
		// User write datas to the adress pointer to fill 
		// mesh vertices. Make sure not to exceed the boundry 
		// of the vertex data memory, otherwise it will 
		// raise a out-of-memory exception.
		// length = sizeof(GeometryVertex) * VertexCount;
		virtual const GeometryVertex* GetRawVertices() const = 0;
		virtual GeometryVertex* GetRawVertices() = 0;

		// Retrieve the memory pointer of indices data.
		// User write datas to the adress pointer to fill 
		// mesh indices. Make sure not to exceed the boundry 
		// of the index data memory, otherwise it will 
		// raise a out-of-memory exception.
		// length = sizeof(unsigned int) * IndexCount.
		virtual const unsigned int* GetRawIndices() const = 0;
		virtual unsigned int* GetRawIndices() = 0;

		// Number of vertices.
		// length = sizeof(GeometryVertex) * VertexCount;
		virtual unsigned int GetVertexCount() const = 0;

		// Number of indices.
		// length = sizeof(unsigned int) * IndexCount;
		virtual unsigned int GetIndexCount() const = 0;

		// Resize the length of vertex data memory, old data will be keeped.
		virtual void ResizeVertexArray(unsigned int vertexCount) = 0;

		// Resize the length of index data memory, old data will be keeped.
		virtual void ResizeIndexArray(unsigned int indexCount) = 0;

		// Merge another mesh to the mesh, with a given transform.
		// The transfrom will apply to that mesh, regard it as world transform
		virtual bool Merge(Mesh* other, const cxx::float2x3& transform) = 0;
	};

	// Texture is a render resource refer to 2d image.
	// It is a shared resources, so manually calling AddRef to increasing
	// reference count, and do Release when it no longer being used.
	struct G2DAPI Texture : public Object
	{
	public:
		// Read and parse colors from a image file, it support 
		// BMP/PNG/TGA files, with or without alpha channel.
		// Return nullptr if meets an unsupport file format,
		// or an error occured when loading.
		static Texture* LoadFromFile(const char* path);
		
		// Call this manually when the texture no longer
		// being referenced, to decrease reference count.
		// Texture will be destroyed when count drop to 0.
		virtual void Release() = 0;

		// Call this manually when texture being referenced
		// to add reference count.
		virtual void AddRef() = 0;

		// Texture ID.
		// At this stage, texture can only be create by loading 
		// from files, so ID eaquals to file path of the file.
		virtual const char* Identifier() const = 0;

		// Return true if texture is loading from same file,
		// otherwise return false.
		virtual bool IsSame(Texture* other) const = 0;
	};

	// Pass is a rendering resouces, a pass refer to one drawcall.
	/// Keep it simple and stupid.
	/// there is no other more easy way to define param setting interface elegantly
	/// TODO: make it more elegant.
	struct G2DAPI Pass : public Object
	{
	public:
		// Indicate what vertex shader will be used.
		virtual const char* GetVertexShaderName() const = 0;

		// Indicate what pixel shader will be used.
		virtual const char* GetPixelShaderName() const = 0;

		// Return true if two passes using same VS/PS, and
		// the datas including in passes is same. otherwise return false.
		virtual bool IsSame(Pass* other) const = 0;

		virtual void SetBlendMode(BlendMode mode) = 0;

		// Fill datas to the vertex constant buffer, user should 
		// make sure not to exceed the boundry of the buffer memory,
		// or it will raise a out-of-memory exception.
		virtual void SetVSConstant(unsigned int index, float* data, unsigned int size, unsigned int count) = 0;

		// Fill datas to the pixel constant buffer, user should 
		// make sure not to exceed the boundry of the buffer memory,
		// or it will raise a out-of-memory exception.
		virtual void SetPSConstant(unsigned int index, float* data, unsigned int size, unsigned int count) = 0;

		// Set a texture to a pass.
		// if texture index is exceed the max texture index,
		// it will automatic expand the max texture. count of Texture
		// will changes, and those index between old/new max index, 
		// will be set to nullptr.
		// Render system will keep last sampling states of each slot when 
		// there is no texture in the slot during render process.
		virtual void SetTexture(unsigned int index, Texture*, bool autoRelease) = 0;

		// Not every slot has solid data, it means some index
		// may return nullptr when they never be set.
		virtual Texture* GetTextureByIndex(unsigned int index) const = 0;

		// Get the max count of texture slot.
		virtual unsigned int GetTextureCount() const = 0;

		// Retrieve vertex constant buffer pointer.
		virtual const float* GetVSConstant() const = 0;

		// memory size of vertex constant buffer.
		virtual unsigned int GetVSConstantLength() const = 0;

		// Retrieve pixel constant buffer pointer.
		virtual const float* GetPSConstant() const = 0;

		// memory size of pixel constant buffer.
		virtual unsigned int GetPSConstantLength() const = 0;

		virtual BlendMode GetBlendMode() const = 0;
	};

	// Material is another rendering resources.
	// One material can holds one or more passes,
	// each pass will save the material info datas,
	// so do NOT use one material with different objects.
	struct G2DAPI Material : public Object
	{
	public:
		// Create a new material that rendering with 
		// combination of vertex color and texture color
		static Material* CreateColorTexture();

		// Create a new material that rendering with texture color
		static Material* CreateSimpleTexture();

		// Create a new material that rendering with vertex color 
		static Material* CreateSimpleColor();

		// Call this manually when the material no longer 
		// being used, to release passes and itself.
		// Function can ONLY be called ONCE.
		virtual void Release() = 0;

		// Usually, a material holds one pass, but some can 
		// holds multiple passes, such as filtering,
		// silhouette materials, they can invoke multiple drawcalls.
		virtual Pass* GetPassByIndex(unsigned int index) const = 0;

		virtual unsigned int GetPassCount() const = 0;

		// Check whether two material holds same passes.
		virtual bool IsSame(Material* other) const = 0;

		// Create a new material contains same passes inside.
		virtual Material* Clone() const = 0;
	};

	// Define the order of a render request.
	// In 2D rendering, adjusting rendering order is a common need.
	// e.g. we need draw shadow sprites before all character sprites
	struct G2DAPI RenderLayer
	{
	public:
		constexpr static unsigned int PreZ = 0x1000;
		constexpr static unsigned int BackGround = 0x2000;
		constexpr static unsigned int Default = BackGround + 0x2000;
		constexpr static unsigned int ForeGround = BackGround + 0x4000;
		constexpr static unsigned int Overlay = 0x8000;
	};

	struct G2DAPI RenderSystem : public Object
	{
	public:
		// Begin rendering task, do some prepare jobs 
		// like clear screen and so on, all render requests
		// need to be sent after this function be called.
		virtual void BeginRender() = 0;

		// Finished rendering task, flush all requests rendering pipe.
		// It must be called after BeginRender(), all render requests
		// need to be sent before this function be called.
		virtual void EndRender() = 0;

		// Register a rendering request.
		// It provide ability to those custom components, to tell 
		// rendersytem drawing object when OnRender event occured.
		virtual void RenderMesh(unsigned int layer, Mesh*, Material*, const cxx::float2x3&) = 0;

		// Retrieve size of rendering window.
		virtual unsigned int GetWindowWidth() const = 0;
		virtual unsigned int GetWindowHeight() const = 0;

		// Convert screen-space coordinate to camera-space coordinate.
		virtual cxx::float2 ScreenToView(const cxx::int2& screen) const = 0;

		// Convert camera-space coordinate to screen-space coordinate.
		virtual cxx::int2 ViewToScreen(const cxx::float2 & view) const = 0;
	};
}
