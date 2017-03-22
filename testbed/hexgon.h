#pragma once
#include <g2dengine.h>
#include <g2dscene.h>
#include <g2drender.h>
#include <vector>

class Hexgon : public g2d::Entity
{
	RTTI_IMPL;
public://implement
	virtual void Release() override { delete this; }
	virtual const gml::aabb2d& GetLocalAABB() const override { return m_aabb; };

public:
	Hexgon()
	{
		colors.resize(7);
		//mesh
		{
			m_mesh = g2d::Mesh::Create(7, 6 * 3);

			//indices;
			{
				uint32_t indices[] = {
					0, 1, 2,
					0, 2, 3,
					0, 3, 4,
					0, 4, 5,
					0, 5, 6,
					0, 6, 1 };
				auto idx = m_mesh->GetRawIndices();
				for (uint32_t i = 0; i < m_mesh->GetIndexCount(); i++)
				{
					idx[i] = indices[i];
				}
			}
			//vertices
			{
				g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();

				vertices[0].position.set(0.0f, 0.0f);
				vertices[0].texcoord.set(0.5f, 0.5f);
				vertices[0].vtxcolor = gml::color4::random();

				for (int i = 1; i < 7; i++)
				{
					gml::vec2 v(0, 1);
					float d = (i - 1) * 360.0f / 6;
					v = gml::mat22::rotate((gml::radian)gml::degree(d)) *  v;
					vertices[i].position = v * 50;
					vertices[i].texcoord.set(0, 0);

					vertices[i].vtxcolor = gml::color4::random();
					m_aabb.expand(vertices[i].position);
				}
			}
		}

		//material
		{
			m_material = g2d::Material::CreateSimpleColor();
		}
	}

	~Hexgon()
	{
		m_mesh->Release();
		m_material->Release();
	}

	virtual void OnRender() override
	{
		g2d::GetEngine()->GetRenderSystem()->RenderMesh(
			g2d::RenderLayer::Default,
			m_mesh, m_material,
			GetSceneNode()->GetWorldMatrix());
	}

	virtual void OnLClick(const gml::coord& cursorPos, g2d::Keyboard& keyboard) override
	{
		g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
		for (int i = 0; i < 7; i++)
		{
			colors[i] = gml::color4::random();
		}
	}

	virtual void OnMouseEnterFrom(g2d::SceneNode* adjacency, const gml::coord& cursorPos, g2d::Keyboard& keyboard) override
	{
		g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
		for (int i = 0; i < 7; i++)
		{
			colors[i] = vertices[i].vtxcolor;
			vertices[i].vtxcolor = gml::color4::red();
		}
	}

	virtual void OnMouseLeaveTo(g2d::SceneNode* adjacency, const gml::coord& cursorPos, g2d::Keyboard& keyboard) override
	{
		g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
		for (int i = 0; i < 7; i++)
		{
			vertices[i].vtxcolor = colors[i];
		}
	}

	virtual void OnLDoubleClick(const gml::coord& cursorPos, g2d::Keyboard& keyboard) override
	{
		g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
		for (int i = 0; i < 7; i++)
		{
			colors[i] = gml::color4::green();
		}
	}

	virtual void OnLDragBegin(const gml::coord& cursorPos, g2d::Keyboard& keyboard) override
	{
		g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
		for (int i = 0; i < 7; i++)
		{
			vertices[i].vtxcolor = gml::color4::yellow();
		}
	}

	virtual void OnLDragging(const gml::coord& cursorPos, g2d::Keyboard& keyboard) override
	{
		auto worldP = GetSceneNode()->GetScene()->GetMainCamera()->ScreenToWorld(cursorPos);
		auto parentP = GetSceneNode()->WorldToParent(worldP);
		GetSceneNode()->SetPosition(parentP);
	}

	virtual void OnLDragEnd(const gml::coord& cursorPos, g2d::Keyboard& keyboard) override
	{
		g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
		for (int i = 0; i < 7; i++)
		{
			vertices[i].vtxcolor = colors[i];
		}
	}

	virtual void OnKeyPress(g2d::KeyCode key, g2d::Keyboard& keyboard) override
	{
		if ((int)key == 'C')
		{
			g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
			for (int i = 0; i < 7; i++)
			{
				vertices[i].vtxcolor = colors[i] = gml::color4::random();
			}
		}
	}

private:
	gml::aabb2d m_aabb;
	g2d::Mesh* m_mesh;
	g2d::Material* m_material;
	std::vector<gml::color4> colors;
};