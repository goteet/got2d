#include "render_system.h"

g2d::Mesh* g2d::Mesh::Create(unsigned int vertexCount, unsigned int indexCount)
{
	return new ::Mesh(vertexCount, indexCount);
}

Mesh::Mesh(unsigned int vertexCount, unsigned int indexCount)
	: mVertices(vertexCount), mIndices(indexCount)
{ }

bool Mesh::Merge(const g2d::Mesh& other, const cxx::float2x3& t)
{
	constexpr const int NUMVERTEX_LIMITED = 32768;
	auto numVertex = GetVertexCount();
	if (numVertex + other.GetVertexCount() > NUMVERTEX_LIMITED)
	{
		return false;
	}

	auto vertices = other.GetRawVertices();
	for (int i = 0, n = other.GetVertexCount(); i < n; i++)
	{
		mVertices.push_back(vertices[i]);
		auto& p = mVertices.back().position;
		p = transform(t, cxx::point2d<float>(p));
	}

	auto indices = other.GetRawIndices();
	for (int i = 0, n = other.GetIndexCount(); i < n; i++)
	{
		mIndices.push_back(indices[i] + numVertex);
	}
	return true;
}

void Mesh::Clear()
{
	mVertices.clear();
	mIndices.clear();
}

const g2d::GeometryVertex* Mesh::GetRawVertices() const
{
	return &(mVertices[0]);
}

g2d::GeometryVertex* Mesh::GetRawVertices()
{
	return &(mVertices[0]);
}

const unsigned int* Mesh::GetRawIndices() const
{
	return &(mIndices[0]);
}

unsigned int* Mesh::GetRawIndices()
{
	return &(mIndices[0]);
}

unsigned int Mesh::GetVertexCount() const
{
	return static_cast<unsigned int>(mVertices.size());
}

unsigned int Mesh::GetIndexCount() const
{
	return static_cast<unsigned int>(mIndices.size());
}

void Mesh::ResizeVertexArray(unsigned int vertexCount)
{
	mVertices.resize(vertexCount);
}

void Mesh::ResizeIndexArray(unsigned int indexCount)
{
	mIndices.resize(indexCount);
}

bool Mesh::Merge(g2d::Mesh* other, const cxx::float2x3& transform)
{
	ENSURE(other != nullptr);
	return Merge(*other, transform);
}

void Mesh::Release()
{
	delete this;
}

bool Geometry::Create(unsigned int vertexCount, unsigned int indexCount)
{
	if (vertexCount == 0 || indexCount == 0)
		return false;

	mNumVertices = vertexCount;
	mNumIndices = indexCount;

	auto fb = create_fallback([&] { Destroy(); });

	if (!MakeEnoughVertexArray(vertexCount))
	{
		return false;
	}

	if (!MakeEnoughIndexArray(indexCount))
	{
		return false;
	}
	fb.cancel();
	return true;
}

bool Geometry::MakeEnoughVertexArray(unsigned int numVertices)
{
	if (mNumVertices >= numVertices)
	{
		return true;
	}

	auto vertexBuffer = GetRenderSystem().GetDevice()->CreateBuffer(rhi::BufferBinding::Vertex, rhi::ResourceUsage::Dynamic, sizeof(g2d::GeometryVertex) * numVertices);
	if (vertexBuffer == nullptr)
	{
		return  false;
	}
	else
	{
		mNumVertices = numVertices;
		mVertexBuffer = vertexBuffer;
		return true;
	}
}

bool Geometry::MakeEnoughIndexArray(unsigned int numIndices)
{
	if (mNumIndices >= numIndices)
	{
		return true;
	}

	auto indexBuffer = GetRenderSystem().GetDevice()->CreateBuffer(rhi::BufferBinding::Index, rhi::ResourceUsage::Dynamic, sizeof(unsigned int) * numIndices);
	if (indexBuffer == nullptr)
	{
		return false;
	}
	else
	{
		mNumIndices = numIndices;
		mIndexBuffer = indexBuffer;
		return true;
	}
}

void Geometry::UploadVertices(unsigned int offset, g2d::GeometryVertex* vertices, unsigned int count)
{
	ENSURE(vertices != nullptr && mVertexBuffer != nullptr);

	auto mappedResource = GetRenderSystem().GetContext()->Map(mVertexBuffer);
	if (mappedResource.success)
	{
		count = __min(mNumVertices - offset, count);
		auto data = reinterpret_cast<g2d::GeometryVertex*>(mappedResource.data);
		memcpy(data + offset, vertices, sizeof(g2d::GeometryVertex) * count);
		GetRenderSystem().GetContext()->Unmap(mVertexBuffer);
	}
}

void Geometry::UploadIndices(unsigned int offset, unsigned int* indices, unsigned int count)
{
	ENSURE(indices != nullptr && mIndexBuffer != nullptr);

	auto mappedResource = GetRenderSystem().GetContext()->Map(mIndexBuffer);
	if (mappedResource.success)
	{
		count = __min(mNumIndices - offset, count);
		auto data = reinterpret_cast<unsigned int*>(mappedResource.data);
		memcpy(data + offset, indices, sizeof(unsigned int) * count);
		GetRenderSystem().GetContext()->Unmap(mIndexBuffer);
	}
}

void Geometry::Destroy()
{
	mVertexBuffer.release();
	mIndexBuffer.release();
	mNumVertices = 0;
	mNumIndices = 0;
}
