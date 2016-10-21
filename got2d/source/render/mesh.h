#pragma once
#include <vector>
#include "g2drender.h"
#include "../RHI/RHI.h"

class Geometry
{
public:
	bool Create(unsigned int vertexCount, unsigned int indexCount);

	bool MakeEnoughVertexArray(unsigned int numVertices);

	bool MakeEnoughIndexArray(unsigned int numIndices);

	void UploadVertices(unsigned int offset, g2d::GeometryVertex*, unsigned int count);

	void UploadIndices(unsigned int offset, unsigned int* indices, unsigned int count);

	void Destroy();

	rhi::Buffer* mVertexBuffer = nullptr;
	rhi::Buffer* mIndexBuffer = nullptr;
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;
};

class Mesh : public g2d::Mesh
{
	RTTI_IMPL;
public:
	Mesh(unsigned int vertexCount, unsigned int indexCount);

	void Clear();

	bool Merge(const g2d::Mesh& other, const cxx::float2x3& transform);

public:
	virtual const g2d::GeometryVertex* GetRawVertices() const override;

	virtual g2d::GeometryVertex* GetRawVertices() override;

	virtual const unsigned int* GetRawIndices() const override;

	virtual unsigned int* GetRawIndices() override;

	virtual unsigned int GetVertexCount() const override;

	virtual unsigned int GetIndexCount() const override;

	virtual void ResizeVertexArray(unsigned int vertexCount) override;

	virtual void ResizeIndexArray(unsigned int indexCount) override;

	virtual bool Merge(g2d::Mesh* other, const cxx::float2x3& transform) override;

	virtual void Release() override;

private:
	std::vector<g2d::GeometryVertex> mVertices;
	std::vector<unsigned int> mIndices;
};
