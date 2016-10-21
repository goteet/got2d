#include "cxx_common.h"
#include "../system_blackboard.h"
#include "../render/render_system.h"
#include "quad.h"

Quad::Quad()
{
	mMesh = g2d::Mesh::Create(4, 6);

	unsigned int indices[] = { 0, 2, 1, 0, 3, 2 };
	unsigned int* pIndexPtr = mMesh->GetRawIndices();

	for (int i = 0; i < 6; i++)
	{
		pIndexPtr[i] = indices[i];
	}

	g2d::GeometryVertex* vertices = mMesh->GetRawVertices();

	vertices[0].Position = cxx::float2(-0.5f, -0.5f);
	vertices[3].Position = cxx::float2(-0.5f, +0.5f);
	vertices[2].Position = cxx::float2(+0.5f, +0.5f);
	vertices[1].Position = cxx::float2(+0.5f, -0.5f);

	vertices[0].Texcoord = cxx::float2(0, 1);
	vertices[3].Texcoord = cxx::float2(0, 0);
	vertices[2].Texcoord = cxx::float2(1, 0);
	vertices[1].Texcoord = cxx::float2(1, 1);

	vertices[0].VertexColor = cxx::color4f::random();
	vertices[1].VertexColor = cxx::color4f::random();
	vertices[2].VertexColor = cxx::color4f::random();
	vertices[3].VertexColor = cxx::color4f::random();


	mAABB.expand(cxx::float2(-0.5f, -0.5f));
	mAABB.expand(cxx::float2(+0.5f, +0.5f));

	switch ((rand() % 3))
	{
	case 0:
		mMaterial = g2d::Material::CreateSimpleColor();
		break;
	case 1:
		mMaterial = g2d::Material::CreateSimpleTexture();
		mMaterial->GetPassByIndex(0)->SetTexture(0, g2d::Texture::LoadFromFile((rand() % 2) ? "palette8_100x128.bmp" : "dxt1_100x128.dds"), true);
		break;
	case 2:
		mMaterial = g2d::Material::CreateColorTexture();
		mMaterial->GetPassByIndex(0)->SetTexture(0, g2d::Texture::LoadFromFile((rand() % 2) ? "dxt5a_128x128.dds" : "rgba_128x128.png"), true);
		break;
	}
}

Quad::~Quad()
{
	cxx::safe_release(mMesh);
	cxx::safe_release(mMaterial);
}


void Quad::OnRender()
{
	GetRenderSystem().RenderMesh(
		g2d::RenderLayer::Default,
			mMesh,
			mMaterial,
		GetSceneNode()->GetWorldMatrix()
	);
}

g2d::Quad* Quad::SetSize(const cxx::float2& size)
{
	g2d::GeometryVertex* vertices = mMesh->GetRawVertices();
	vertices[0].Position = cxx::float2(-0.5f, -0.5f);
	vertices[3].Position = cxx::float2(-0.5f, +0.5f);
	vertices[2].Position = cxx::float2(+0.5f, +0.5f);
	vertices[1].Position = cxx::float2(+0.5f, -0.5f);

	vertices[0].Position *= size;
	vertices[3].Position *= size;
	vertices[2].Position *= size;
	vertices[1].Position *= size;

	mAABB.expand(cxx::float2(-0.5f, -0.5f) * size);
	mAABB.expand(cxx::float2(+0.5f, +0.5f) * size);
	return this;
}