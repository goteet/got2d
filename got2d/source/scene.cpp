#include "scene.h"
#include "spatial_graph.h"
#include <algorithm>
#include <gmlconversion.h>

g2d::SceneNode::~SceneNode() { }

g2d::Scene::~Scene() { }

SceneNode::SceneNode(::Scene* scene, SceneNode* parent, int childID, g2d::Entity* entity, bool autoRelease)
	: m_scene(scene)
	, m_parent(parent)
	, m_entity(entity)
	, m_childID(childID)
	, m_baseRenderingOrder(0)
	, m_autoRelease(autoRelease)
	, m_matrixLocal(gml::mat32::identity())
	, m_position(gml::vec2::zero())
	, m_pivot(gml::vec2::zero())
	, m_scale(gml::vec2::one())
	, m_rotation(0)
	, m_isVisible(true)
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
		m_matrixLocal = gml::mat32::trsp(m_position, m_rotation, m_scale, m_pivot);
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

void SceneNode::SetVisibleMask(unsigned int mask, bool recurssive)
{
	m_visibleMask = mask;
	if (recurssive)
	{
		for (auto& child : m_children)
		{
			child->SetVisibleMask(mask, true);
		}
	}
}

void SceneNode::SetWorldMatrixDirty()
{
	m_matrixWorldDirty = true;
	for (auto& child : m_children)
	{
		child->SetWorldMatrixDirty();
	}
	m_matrixDirtyUpdate = true;
}

void SceneNode::SetLocalMatrixDirty()
{
	m_matrixLocalDirty = true;
	SetWorldMatrixDirty();
}

void SceneNode::AdjustSpatial()
{
	m_scene->GetSpatialGraph()->Add(m_entity);
}

void SceneNode::Update(unsigned int elpasedTime)
{
	if (m_entity)
	{
		m_entity->OnUpdate(elpasedTime);
		if (m_matrixDirtyUpdate)
		{
			//现在阶段只需要在test visible之前处理好就行.
			//也就是 Scene::Render之前
			if (IsStatic())
			{
				AdjustSpatial();
			}
			m_entity->OnUpdateMatrixChanged();
			m_matrixDirtyUpdate = false;
		}
	}
	for (auto& child : m_children)
	{
		child->Update(elpasedTime);
	}
}

void SceneNode::Render(g2d::Camera* camera)
{
	RenderSingle(camera);
	for (auto& child : m_children)
	{
		child->Render(camera);
	}
}

void SceneNode::RenderSingle(g2d::Camera* camera)
{
	if (IsVisible())
	{
		if (m_entity && camera->TestVisible(m_entity))
		{
			m_entity->OnRender();
		}
	}
}

void SceneNode::SetRenderingOrder(int& index)
{
	//for mulity-entity backup.
	m_baseRenderingOrder = index++;
	if (m_entity)
	{
		m_entity->SetRenderingOrder(index);
		index++;
	}

	for (auto& child : m_children)
	{
		child->SetRenderingOrder(index);
	}
}

::SceneNode* SceneNode::GetPrevSibling()
{
	if (m_parent == nullptr || m_childID == 0)
	{
		return nullptr;
	}
	return m_parent->m_children[m_childID - 1];
}

::SceneNode* SceneNode::GetNextSibling()
{
	if (m_parent == nullptr || m_childID == m_parent->m_children.size() - 1)
	{
		return nullptr;
	}
	return m_parent->m_children[m_childID + 1];
}

void SceneNode::AdjustRenderingOrder()
{
	int curIndex = m_baseRenderingOrder + 1;
	if (m_entity)
	{
		m_entity->SetRenderingOrder(curIndex);
		curIndex++;
	}

	for (auto& child : m_children)
	{
		child->SetRenderingOrder(curIndex);
	}

	auto parent = m_parent;
	auto current = this;
	while (parent != nullptr)
	{
		auto& siblings = parent->m_children;
		for (int i = m_childID + 1; i < siblings.size(); i++)
		{
			auto sibling = siblings[i];
			sibling->SetRenderingOrder(curIndex);
		}

		current = parent;
		parent = current->m_parent;
	}
}

g2d::Scene* SceneNode::GetScene() const
{
	return m_scene;
}

g2d::SceneNode* SceneNode::GetParentNode()
{
	return m_parent;
}

g2d::SceneNode* SceneNode::GetPrevSiblingNode()
{
	return GetPrevSibling();
}

g2d::SceneNode* SceneNode::GetNextSiblingNode()
{
	return GetNextSibling();
}

g2d::SceneNode* SceneNode::CreateSceneNode(g2d::Entity* e, bool autoRelease)
{
	if (e == nullptr)
	{
		return nullptr;
	}
	int childID = static_cast<int>(m_children.size());
	auto rst = new ::SceneNode(m_scene, this, childID, e, autoRelease);
	m_children.push_back(rst);
	AdjustRenderingOrder();

	auto scene = dynamic_cast<::Scene*>(GetScene());
	assert(scene != nullptr);
	scene->GetSpatialGraph()->Add(e);
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
	if (m_entity)
	{
		m_entity->OnScale(scale);
	}
	SetLocalMatrixDirty();
	m_scale = scale;
	return this;
}

g2d::SceneNode* SceneNode::SetPosition(const gml::vec2& position)
{
	if (m_entity)
	{
		m_entity->OnMove(position);
	}
	SetLocalMatrixDirty();
	m_position = position;

	return this;
}

g2d::SceneNode* SceneNode::SetRotation(gml::radian r)
{
	if (m_entity)
	{
		m_entity->OnRotate(r);
	}
	SetLocalMatrixDirty();
	m_rotation = r;
	return this;
}

void SceneNode::SetStatic(bool s)
{
	if (m_isStatic != s)
	{
		m_isStatic = s;
		AdjustSpatial();
	}
}

template<class T>
constexpr T exp(T n, unsigned int iexp)
{
	return iexp == 0 ? T(1) : (iexp == 1 ? n : exp(n, iexp - 1) * n);
}

constexpr const float SCENE_SIZE = QuadTreeNode::MIN_SIZE * exp(2.0f, 8);
Scene::Scene()
	: m_spatial(SCENE_SIZE)
{
	m_root = new ::SceneNode(this, nullptr, 0, nullptr, false);
	CreateCameraNode();
}

Scene::~Scene()
{
	delete m_root;
}

void Scene::SetCameraOrderDirty()
{
	m_cameraOrderDirty = true;
}

void Scene::ResortCameraOrder()
{
	if (m_cameraOrderDirty)
	{
		m_cameraOrderDirty = false;
		m_renderingOrder = m_cameras;

		std::sort(m_renderingOrder.begin(), m_renderingOrder.end(),
			[](g2d::Camera* a, g2d::Camera* b)->bool {

			auto aOrder = a->GetRenderingOrder();
			auto bOrder = b->GetRenderingOrder();
			if (aOrder == bOrder)
			{
				return a->GetID() < b->GetID();
			}
			return aOrder < bOrder;
		});
	}
}

#include "render_system.h"
void Scene::Render()
{
	GetRenderSystem()->FlushRequests();

	ResortCameraOrder();
	for (size_t i = 0, n = m_renderingOrder.size(); i < n; i++)
	{
		auto& camera = m_renderingOrder[i];
		if (!camera->IsActivity())
			continue;

		GetRenderSystem()->SetViewMatrix(camera->GetViewMatrix());

		std::vector<g2d::Entity*> visibleEntities;
		m_spatial.FindVisible(camera, visibleEntities);

		//sort visibleEntities by render order
		std::sort(visibleEntities.begin(), visibleEntities.end(),
			[](g2d::Entity* a, g2d::Entity* b) {
			return a->GetRenderingOrder() < b->GetRenderingOrder();
		});

		for (auto& entity : visibleEntities)
		{
			entity->OnRender();
		}
		GetRenderSystem()->FlushRequests();
	}
}

g2d::Camera* Scene::CreateCameraNode()
{
	Camera* camera = new ::Camera();
	if (CreateSceneNode(camera, true) != nullptr)
	{
		m_cameraOrderDirty = true;
		camera->SetID(static_cast<unsigned int>(m_cameras.size()));
		m_cameras.push_back(camera);
		return camera;
	}
	else
	{
		camera->Release();
		return nullptr;
	}

}

g2d::Camera* Scene::GetCamera(unsigned int index) const
{
	if (index >= m_cameras.size())
		return nullptr;
	return m_cameras[index];
}

g2d::SceneNode* Scene::CreateSceneNode(g2d::Entity* e, bool autoRelease)
{
	return  m_root->CreateSceneNode(e, autoRelease);
}