#pragma once
#include <g2dconfig.h>
#include <gmlvector.h>
#include <gmlmatrix.h>
#include <gmlaabb.h>
namespace g2d
{
	constexpr int DEFAULT_VISIBLE_MASK = 0xFFFFFFFF;

	class Camera;
	class SceneNode;
	class Scene;

	class G2DAPI Entity
	{
	public:
		DECL_CLASSID;
		virtual void Release() = 0;
		virtual gml::aabb2d GetLocalAABB() const = 0;
	public:
		virtual ~Entity();
		virtual void OnInitial();
		virtual void OnUpdate(unsigned int elpasedTime);
		virtual void OnRender();
		virtual void OnRotate(gml::radian r);
		virtual void OnScale(const gml::vec2 newScaler);
		virtual void OnMove(const gml::vec2 newPos);
		virtual void OnUpdateMatrixChanged();

		virtual gml::aabb2d GetWorldAABB() const;

	public:
		void SetSceneNode(g2d::SceneNode* node);
		void SetRenderingOrder(int order);
		unsigned int GetVisibleMask() const;
		SceneNode* GetSceneNode() const;
		int GetRenderingOrder() const;
	private:
		SceneNode* m_sceneNode = nullptr;
		int m_renderingOrder = 0;
	};

	class G2DAPI Quad : public Entity
	{
	public:
		static Quad* Create();

		virtual g2d::Entity* SetSize(const gml::vec2& size) = 0;
		virtual const gml::vec2& GetSize()  const = 0;
	};

	class G2DAPI Camera : public Entity
	{
	public:
		virtual unsigned int GetID() const = 0;
		virtual Camera* SetPosition(const gml::vec2& position) = 0;
		virtual Camera* SetScale(const gml::vec2& scale) = 0;
		virtual Camera* SetRotation(gml::radian r) = 0;
		virtual void SetRenderingOrder(int order) = 0;
		virtual void SetVisibleMask(unsigned int mask) = 0;
		virtual void SetActivity(bool activity) = 0;
		virtual const gml::mat32& GetViewMatrix() const = 0;
		virtual bool TestVisible(gml::aabb2d bounding) = 0;
		virtual bool TestVisible(g2d::Entity* entity) = 0;
		virtual unsigned int GetVisibleMask() const = 0;
		virtual int GetRenderingOrder() const = 0;
		virtual bool IsActivity() const = 0;
	};

	class G2DAPI SceneNode
	{
	public:
		virtual ~SceneNode();
		virtual Scene* GetScene() const = 0;
		virtual SceneNode* GetParentNode() = 0;
		virtual SceneNode* GetNextSiblingNode() = 0;
		virtual SceneNode* CreateSceneNode(Entity*, bool autoRelease) = 0;
		virtual const gml::mat32& GetLocalMatrix() = 0;
		virtual const gml::mat32& GetWorldMatrix() = 0;
		virtual SceneNode* SetPosition(const gml::vec2& position) = 0;
		virtual SceneNode* SetPivot(const gml::vec2& pivot) = 0;
		virtual SceneNode* SetScale(const gml::vec2& scale) = 0;
		virtual SceneNode* SetRotation(gml::radian r) = 0;
		virtual void SetVisible(bool) = 0;
		virtual void SetStatic(bool) = 0;
		virtual void SetVisibleMask(unsigned int mask, bool recursive) = 0;
		virtual const gml::vec2& GetPosition()  const = 0;
		virtual const gml::vec2& GetPivot() const = 0;
		virtual const gml::vec2& GetScale() const = 0;
		virtual gml::radian GetRotation() const = 0;
		virtual Entity* GetEntity() const = 0;
		virtual bool IsVisible() const = 0;
		virtual bool IsStatic() const = 0;
		virtual unsigned int GetVisibleMask() const = 0;
	};

	template<class T> inline T* GetEntity(SceneNode* node)
	{
		Entity* entity = node->GetEntity();
		return (T::GetClassID() == entity->GetClassID()) ? entity : nullptr;
	}

	class G2DAPI Scene : public SceneNode
	{
	public:
		virtual ~Scene();
		virtual Camera* CreateCameraNode() = 0;
		virtual Camera* GetMainCamera() const = 0;
		virtual Camera* GetCamera(unsigned int index) const = 0;
	};
}
