#include "stdafx.h"
#include "hexagon.h"

constexpr float kHexagonMargin = 2.0f;
constexpr float kHexagonSize = 30.0f;
constexpr float kHexagonRealSize = kHexagonSize - kHexagonMargin;
constexpr float kHeightStride = kHexagonSize * 1.5f;
const float kWidthStride = kHexagonSize * sqrt(3.0f);

g2d::Mesh * CreateHexagonMesh(float size, cxx::color4f color, cxx::aabb2d<float>* aabb)
{
	auto mesh = g2d::Mesh::Create(7, 6 * 3);

	//indices;
	{
		uint32_t indices[] = {
			0, 2, 1,
			0, 3, 2,
			0, 4, 3,
			0, 5, 4,
			0, 6, 5,
			0, 1, 6 };

		auto indexPtr = mesh->GetRawIndices();
		memcpy(indexPtr, indices, sizeof(uint32_t) * mesh->GetIndexCount());
	}

	//vertices
	{
		g2d::GeometryVertex* vertices = mesh->GetRawVertices();

		vertices[0].Position = cxx::point2d<float>::origin();
		vertices[0].Texcoord = cxx::point2d<float>(0.5f, 0.5f);
		vertices[0].VertexColor = color;

		for (int i = 1; i < 7; i++)
		{
			cxx::float2 v(0, 1);
			float d = (i - 1) * 360.0f / 6;
			v = cxx::float2x2::rotate(cxx::degree<float>(d)) *  v;
			vertices[i].Position = v * (size);
			vertices[i].Texcoord = v;
			vertices[i].VertexColor = color;

			if (aabb != nullptr)
			{
				aabb->expand(vertices[i].Position);
			}
		}
	}
	return mesh;
}

void Hexagon::OnRender()
{
	g2d::Engine::GetInstance()->GetRenderSystem()->RenderMesh(
		g2d::RenderLayer::Default,
		m_mesh, m_material,
		GetSceneNode()->GetWorldMatrix());
}


Hexagon::Hexagon()
{
	m_aabb.clear();
	cxx::aabb2d<float> HexagonAABB;
	m_lastColor = m_color = cxx::color4f::random();
	cxx::float2x3 transform = cxx::float2x3::identity();

	auto HexagonMesh = CreateHexagonMesh(kHexagonSize - kHexagonMargin, m_color, &HexagonAABB);
	m_mesh = g2d::Mesh::Create(0, 0);

	for (int line = 1; line < 3; line++)
	{
		float y = (line - 2) * kHeightStride;
		float xOffset = (line % 2 == 0) ? kWidthStride * 0.5f : 0.0f;
		for (int h = 0; h < line; h++)
		{
			float x = (h - line / 2) * kWidthStride + xOffset;
			transform = cxx::float2x3::translate(x, y);
			m_mesh->Merge(HexagonMesh, transform);
			auto hexagonWorldAABB = cxx::transform(transform, HexagonAABB);
			m_aabb.expand(hexagonWorldAABB);
		}
	}
	HexagonMesh->Release();
	m_material = g2d::Material::CreateSimpleColor();
}

Hexagon::~Hexagon()
{
	m_mesh->Release();
	m_material->Release();
}

const cxx::color4f& Hexagon::GetColor()
{
	return m_color;
}

void Hexagon::SetColor(const cxx::color4f & color)
{
	m_color = color;
	uint32_t count = m_mesh->GetVertexCount();
	g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
	for (uint32_t i = 0; i < count; i++)
	{
		vertices[i].VertexColor = color;
	}
}

void HexagonBoard::OnInitial()
{
	GetSceneNode()->SetStatic(true);
}

inline void HexagonBoard::OnRender()
{
	g2d::Engine::GetInstance()->GetRenderSystem()->RenderMesh(
		g2d::RenderLayer::Default,
		m_mesh, m_material,
		GetSceneNode()->GetWorldMatrix());
}

void Hexagon::OnLClick(const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	SetColor(cxx::color4f::random());
}

void Hexagon::OnCursorEnterFrom(g2d::SceneNode * adjacency, const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	m_lastColor = m_color;
}

void Hexagon::OnCursorLeaveTo(g2d::SceneNode * adjacency, const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	SetColor(m_lastColor);
}

void Hexagon::OnLDragBegin(const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	SetColor(cxx::color4f::yellow());
}

void Hexagon::OnLDragEnd(const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	SetColor(m_lastColor);
}

void Hexagon::OnKeyPress(g2d::KeyCode key, const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	if ((int)key == 'C')
	{
		m_lastColor = cxx::color4f::random();
		SetColor(m_lastColor);
	}
	else if ((int)key == 'X')
	{
		GetSceneNode()->SetVisible(!GetSceneNode()->IsVisible());
	}
}

void HexagonBoard::OnCursorHovering(const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	auto cursor = mouse.GetCursorPosition();
	auto pos = GetSceneNode()->GetScene()->GetDefaultCamera()->ScreenToWorld(cursor);
	pos = GetSceneNode()->WorldToLocal(pos);
	int x, y;
	PositionToHex(pos, x, y);
	SetHexagonColor(cxx::color4f::random(), x, y);
}

inline void HexagonBoard::OnCursorLeaveTo(g2d::SceneNode* adjacency, const g2d::Mouse & mouse, const g2d::Keyboard & keyboard)
{
	m_lastIndex = -1;
}

HexagonBoard::HexagonBoard()
{
	m_aabb.clear();
	cxx::aabb2d<float> HexagonAABB;
	cxx::color4f boardColor = cxx::color4f::gray();
	cxx::float2x3 transform = cxx::float2x3::identity();


	auto HexagonMesh = CreateHexagonMesh(kHexagonSize - kHexagonMargin, boardColor, &HexagonAABB);
	m_mesh = g2d::Mesh::Create(0, 0);

	for (int line = 5; line < 10; line++)
	{
		float y = (line - 9) * kHeightStride;
		float xOffset = (line % 2 == 0) ? kWidthStride * 0.5f : 0.0f;
		for (int h = 0; h < line; h++)
		{
			float x = (h - line / 2) * kWidthStride + xOffset;
			transform = cxx::float2x3::translate(x, y);
			m_mesh->Merge(HexagonMesh, transform);
			auto hexagonWorldAABB = cxx::transform(transform, HexagonAABB);
			m_aabb.expand(hexagonWorldAABB);
		}
	}

	for (int line = 8; line >= 5; line--)
	{
		float y = (9 - line) * kHeightStride;
		float xOffset = (line % 2 == 0) ? kWidthStride * 0.5f : 0.0f;
		for (int h = 0; h < line; h++)
		{
			float x = (h - line / 2) * kWidthStride + xOffset;
			transform = cxx::float2x3::translate(x, y);
			m_mesh->Merge(HexagonMesh, transform);
			auto hexagonWorldAABB = cxx::transform(transform, HexagonAABB);
			m_aabb.expand(hexagonWorldAABB);
		}
	}

	HexagonMesh->Release();

	m_material = g2d::Material::CreateSimpleColor();
}

HexagonBoard::~HexagonBoard()
{
	m_mesh->Release();
	m_material->Release();
}

void HexagonBoard::SetHexagonColor(cxx::color4f color, int q, int r)
{
	int index = HexToIndex(q, r);
	if (index != m_lastIndex)
	{
		m_lastIndex = index;
		g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
		auto beg = index * 7;
		auto end = (index + 1) * 7;
		for (int i = beg; i < end; i++)
		{
			vertices[i].VertexColor = color;
		}
	}

}

void HexagonBoard::PositionToHex(cxx::float2 pos, int & outQ, int & outR)
{
	float x = (pos.x * sqrt(3.0f) / 3.0f - pos.y / 3.0f) / kHexagonSize;
	float z = pos.y * 2.0f / 3.0f / kHexagonSize;
	float y = -x - z;

	int ix = (int)round(x);
	int iy = (int)round(y);
	int iz = (int)round(z);

	float xdiff = abs(x - ix);
	float ydiff = abs(y - iy);
	float zdiff = abs(z - iz);

	if (xdiff > ydiff && xdiff > zdiff)
	{
		outQ = -iy - iz;
		outR = iz;
	}
	else if (ydiff > zdiff)
	{
		outQ = ix;
		outR = iz;
	}
	else
	{
		outQ = ix;
		outR = -ix - iy;
	}
}

int HexagonBoard::HexToIndex(int q, int r)
{
	int x = cxx::clamp(q, -4, 4);
	int z = cxx::clamp(r, -4, 4);
	int y = cxx::clamp(-x - z, -4, 4);
	x = -z - y;
	int absz = abs(z);

	int index = 0;
	if (z <= 0)
	{
		return  (4 - y) + (8 - absz + 5) * (4 - absz) / 2;
	}
	else
	{
		return  (x + 4) + 35 + (9 - absz + 9) * (absz - 1) / 2;
	}
}
