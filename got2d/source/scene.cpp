#include "scene.h"

g2d::Entity::~Entity() {}

void g2d::Entity::OnInitial() {}

void g2d::Entity::OnUpdate() {}

void g2d::Entity::OnRender() {}

void g2d::Entity::SetSceneNode(g2d::SceneNode* node)
{
	m_sceneNode = node;
}

g2d::SceneNode* g2d::Entity::GetSceneNode() { return m_sceneNode; }


g2d::SceneNode::~SceneNode() { }

g2d::Scene::~Scene() { }

SceneNode::SceneNode(SceneNode* parent, g2d::Entity* entity, bool autoRelease)
	: m_parent(parent)
	, m_entity(entity)
	, m_autoRelease(autoRelease)
	, m_matrixLocal(gml::mat32::I())
	, m_position(gml::vec2::zero())
	, m_pivot(gml::vec2::zero())
	, m_scale(gml::vec2::one())
	, m_rotationRadian(0)
{
	if (m_entity)
	{
		m_entity->SetSceneNode(this);
		m_entity->OnInitial();
	}
}

SceneNode::~SceneNode()
{
	for (auto& child : m_children)
	{
		delete child;
	}
	m_children.clear();
	if (m_autoRelease && m_entity)
	{
		m_entity->Release();
	}
}

const gml::mat32& SceneNode::GetLocalMatrix()
{
	if (m_matrixLocalDirty)
	{
		m_matrixLocal = gml::mat32::trsp(m_position, m_rotationRadian, m_scale, m_pivot);
		m_matrixLocalDirty = false;
	}
	return m_matrixLocal;
}

const gml::mat32& SceneNode::GetWorldMatrix()
{
	if (m_parent == nullptr)//get rid of transform of ROOT.
	{
		return GetLocalMatrix();
	}

	if (m_matrixWorldDirty)
	{
		auto matParent = m_parent->GetWorldMatrix();
		m_matrixWorld = matParent * GetLocalMatrix();
		m_matrixWorldDirty = false;
	}
	return m_matrixWorld;

}

void SceneNode::SetLocalMatrixDirty()
{
	m_matrixLocalDirty = true;
	for (auto& child : m_children)
	{
		m_matrixWorldDirty = true;
		child->m_matrixWorldDirty = true;
	}
}

void SceneNode::Update(unsigned int elpasedTime)
{
	if (m_entity)m_entity->OnUpdate();
	for (auto& child : m_children)
	{
		child->Update(elpasedTime);
	}
}

void SceneNode::Render()
{
	if (m_entity)m_entity->OnRender();
	for (auto& child : m_children)
	{
		child->Render();
	}
}

g2d::SceneNode* SceneNode::CreateSceneNode(g2d::Entity* e, bool autoRelease)
{
	if (e == nullptr)
	{
		return nullptr;
	}

	auto rst = new ::SceneNode(this, e, autoRelease);
	m_children.push_back(rst);
	return rst;
}

g2d::SceneNode* SceneNode::SetPivot(const gml::vec2& pivot)
{
	SetLocalMatrixDirty();
	m_pivot = pivot;
	return this;
}
g2d::SceneNode* SceneNode::SetScale(const gml::vec2& scale)
{
	SetLocalMatrixDirty();
	m_scale = scale;
	return this;
}
g2d::SceneNode* SceneNode::SetPosition(const gml::vec2& position)
{
	SetLocalMatrixDirty();
	m_position = position;
	return this;
}
g2d::SceneNode* SceneNode::SetRotation(float radian)
{
	SetLocalMatrixDirty();
	m_rotationRadian = radian;
	return this;
}

#include <g2dengine.h>
QuadEntity::QuadEntity()
{
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	m_mesh = g2d::GetEngine()->GetRenderSystem()->CreateMesh(4, 6);
	auto idx = m_mesh->GetRawIndices();

	for (int i = 0; i < 6; i++)
	{
		idx[i] = indices[i];
	}

	g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
	vertices[0].position.set(-0.5f, -0.5f);
	vertices[3].position.set(-0.5f, +0.5f);
	vertices[2].position.set(+0.5f, +0.5f);
	vertices[1].position.set(+0.5f, -0.5f);

	vertices[0].texcoord.set(0, 1);
	vertices[3].texcoord.set(0, 0);
	vertices[2].texcoord.set(1, 0);
	vertices[1].texcoord.set(1, 1);

	vertices[0].vtxcolor = gml::color4::random();
	vertices[1].vtxcolor = gml::color4::random();
	vertices[2].vtxcolor = gml::color4::random();
	vertices[3].vtxcolor = gml::color4::random();

	switch ((rand() % 3))
	{
	case 0:
		m_material = g2d::GetEngine()->GetRenderSystem()->CreateSimpleColorMaterial();
		break;
	case 1:
		m_material = g2d::GetEngine()->GetRenderSystem()->CreateSimpleTextureMaterial();
		m_material->GetPass(0)->SetTexture(0, g2d::GetEngine()->LoadTexture((rand() % 2) ? "test_alpha.bmp" : "test_alpha.png"), true);
		break;
	case 2:
		m_material = g2d::GetEngine()->GetRenderSystem()->CreateColorTextureMaterial();
		m_material->GetPass(0)->SetTexture(0, g2d::GetEngine()->LoadTexture((rand() % 2) ? "test_alpha.bmp" : "test_alpha.png"), true);
		break;
	}
}
void QuadEntity::OnInitial()
{
	GetSceneNode()->SetPivot(gml::vec2(-0.5f, -0.5f));
}
QuadEntity::~QuadEntity()
{
	m_mesh->Release();
	m_material->Release();
}
void QuadEntity::OnRender()
{
	g2d::GetEngine()->GetRenderSystem()->RenderMesh(g2d::RenderOrder::RORDER_DEFAULT, m_mesh, m_material, GetSceneNode()->GetWorldMatrix());
}
g2d::Entity* QuadEntity::SetSize(const gml::vec2& size)
{
	g2d::GeometryVertex* vertices = m_mesh->GetRawVertices();
	vertices[0].position.set(-0.5f, -0.5f) *= size;
	vertices[3].position.set(-0.5f, +0.5f) *= size;
	vertices[2].position.set(+0.5f, +0.5f) *= size;
	vertices[1].position.set(+0.5f, -0.5f) *= size;
	return this;
}

Scene::Scene()
{
	m_root = new ::SceneNode(nullptr, nullptr, false);
}
Scene::~Scene()
{
	delete m_root;
}