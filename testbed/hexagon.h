#pragma once
#include <vector>
#include <got2d/include/g2dengine.h>
#include <got2d/include/g2dscene.h>
#include <got2d/include/g2drender.h>


g2d::Mesh* CreateHexagonMesh(float size, gml::color4 color, gml::aabb2d* aabb);

class Hexagon : public g2d::Component
{
	RTTI_IMPL;
public://implement
	virtual void Release() override { delete this; }

	virtual const gml::aabb2d& GetLocalAABB() const override { return m_aabb; };

	virtual void OnRender() override;

public:
	Hexagon();

	~Hexagon();

	const gml::color4& GetColor();

	void SetColor(const gml::color4& color);

	virtual void OnLClick(const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

	virtual void OnCursorEnterFrom(g2d::SceneNode* adjacency, const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

	virtual void OnCursorLeaveTo(g2d::SceneNode* adjacency, const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

	virtual void OnLDragBegin(const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

	virtual void OnLDragEnd(const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

	virtual void OnKeyPress(g2d::KeyCode key, const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

private:
	gml::color4 m_lastColor;
	gml::color4 m_color;
	gml::aabb2d m_aabb;
	g2d::Mesh* m_mesh;
	g2d::Material* m_material;
};

class HexagonBoard : public g2d::Component
{
	RTTI_IMPL;
public:
	virtual void Release() override { delete this; }

	virtual const gml::aabb2d& GetLocalAABB() const override { return m_aabb; };

	virtual void OnInitial() override;

	virtual void OnRender() override;

	virtual void OnCursorHovering(const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

	virtual void OnCursorLeaveTo(g2d::SceneNode* adjacency, const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override;

public:
	HexagonBoard();

	~HexagonBoard();

	void SetHexagonColor(gml::color4 color, int q, int r);

private:
	void PositionToHex(gml::vec2, int& outQ, int& outR);

	int HexToIndex(int q, int r);

	int m_lastIndex = -1;
	gml::aabb2d m_aabb;
	g2d::Mesh* m_mesh;
	g2d::Material* m_material;
};


class EntityDragging : public g2d::Component
{
	RTTI_IMPL;
public: //implement
	virtual void Release() override { delete this; }

	virtual void OnLDragBegin(const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override
	{
		auto worldP = GetSceneNode()->GetScene()->GetMainCamera()->ScreenToWorld(mouse.GetCursorPosition());
		m_dragOffset = GetSceneNode()->WorldToLocal(worldP);
	}

	virtual void OnLDragging(const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override
	{
		auto worldP = GetSceneNode()->GetScene()->GetMainCamera()->ScreenToWorld(mouse.GetCursorPosition());
		GetSceneNode()->SetWorldPosition(worldP - m_dragOffset);
	}

	virtual void OnKeyPress(g2d::KeyCode key, const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override
	{
		if (key == g2d::KeyCode::KeyZ)
		{
			GetSceneNode()->Release();
		}
	}

	gml::vec2 m_dragOffset;
};
