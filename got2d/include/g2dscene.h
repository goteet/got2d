#pragma once
#include <g2dconfig.h>
#include <gmlvector.h>
#include <gmlmatrix.h>
#include <gmlaabb.h>
namespace g2d
{
	constexpr uint32_t DEFAULT_VISIBLE_MASK = 0xFFFFFFFF;

	class Camera;
	class SceneNode;
	class Scene;

	// ʵ����࣬�ڵ��߼���ʵ��ȫ��entity��
	// ���ڳ����ڵ��ϣ�һ��SceneNodeֻ�ܹ�һ��Entity
	class G2DAPI Entity : public GObject
	{
	public:
		// �û��Զ���ʵ����Ҫʵ���ڴ��ͷŵĽӿ�
		// �����ڲ����������ӿ��ͷ�entity��Դ
		// ֻ��������ʱ�򱻵���
		virtual void Release() = 0;

		// �ֲ�����ϵ�½ڵ�İ�Χ�д�С
		virtual const gml::aabb2d& GetLocalAABB() const = 0;

		// �ڵ�������ռ��а�Χ�еĴ�С
		// Ĭ��ʵ�����������任�ֲ�����ϵ�µİ�Χ��
		virtual gml::aabb2d GetWorldAABB() const;

		// ����Ϊ��Ϣ��Ӧ�ӿ�
		// �û��Զ���ʵ��������Щ�麯���Ի���¼���Ӧ

		// �ڵ㱻�ɹ�������¼�
		// һ���ʼ������д����¼���
		virtual void OnInitial() { }

		// �ڵ���и��µ��¼�
		// һ��ÿ֡�߼�����д����¼���
		virtual void OnUpdate(uint32_t elpasedTime) { }

		// �ڵ������Ⱦ���¼�
		// ��Ⱦ�ڵ��ʱ����Ҫ�û���render request���뵽��Ⱦ������
		virtual void OnRender() { }

		// �ڵ�ֲ���ת�����¼�
		virtual void OnRotate(gml::radian r) { }

		// �ڵ�ֲ����Ÿ����¼�
		virtual void OnScale(const gml::vec2& newScaler) { }

		// �ڵ�ֲ�λ�ø����¼�
		virtual void OnMove(const gml::vec2& newPos) { }

		// �ڵ�ֲ�����仯֮�󣬵�һ�θ��µ��¼�
		// һ��AABB�ı任��������㡣
		// ˳���� OnUpdate֮��
		virtual void OnUpdateMatrixChanged() { }

		// entity���Ź����ĳ����ڵ�
		// ����ӿ�һ���ṩ���û��Զ���entity�ڲ���ȡnode�������
		SceneNode* GetSceneNode() const { return m_sceneNode; }

		// ��ȡ���ڳ����ڵ��������ɼ�Flag
		// �����һ��ת����Ϣ�Ŀ�ݺ�����
		uint32_t GetVisibleMask() const;

	public://�ڲ�ʹ��
		// ��ʼ�������ڵ��ʱ��
		// ���ù���
		void SetSceneNode(g2d::SceneNode* node);

		// ���ݳ����ڵ��ϵ������Ⱦ˳��
		// ������Ⱦ�����ڴ�ֻ����¼ʹ��
		void SetRenderingOrder(uint32_t order);

		uint32_t GetRenderingOrder() const { return m_renderingOrder; }

	private:
		SceneNode* m_sceneNode = nullptr;
		uint32_t m_renderingOrder = 0;
	};

	class G2DAPI Quad : public Entity
	{
	public:
		static Quad* Create();

		// ���� Quad Mesh ��С
		virtual g2d::Quad* SetSize(const gml::vec2& size) = 0;

		// ��ȡ Quad Mesh ��С
		virtual const gml::vec2& GetSize() const = 0;
	};

	//���ڳ������ҿɼ����壬������Ⱦ�������
	class G2DAPI Camera : public Entity
	{
	public:
		// ������ڳ����еı��
		// Ĭ��������ı��Ϊ0
		virtual uint32_t GetID() const = 0;

		// �����λ��
		// ������������ʹ����ʽ����
		virtual Camera* SetPosition(const gml::vec2& p) = 0;

		// �������ͷ����
		// ������������ʹ����ʽ����
		virtual Camera* SetScale(const gml::vec2& s) = 0;

		// ����� Roll ��ת
		// ������������ʹ����ʽ����
		virtual Camera* SetRotation(gml::radian r) = 0;

		// ������Ⱦ˳�򣬶�����������������Ⱦ˳���������
		// ˳��ԽСԽ������Ⱦ
		virtual void SetRenderingOrder(int order) = 0;

		// ���ҿɼ�����Flag
		virtual void SetVisibleMask(uint32_t mask) = 0;

		// �Ƿ��������������������ã����������������������Ⱦ
		virtual void SetActivity(bool activity) = 0;

		// ��Ⱦϵͳ��Ҫ���Ӿ���
		virtual const gml::mat32& GetViewMatrix() const = 0;

		// �ж�һ��aabb�Ƿ�ᱻ���������
		virtual bool TestVisible(const gml::aabb2d& bounding) const = 0;

		// �ж�һ��entity�Ƿ�ᱻ���������
		// aabbΪһ���㣬mask��ƥ����Ϊ���ɼ�
		virtual bool TestVisible(g2d::Entity& entity) const = 0;

		// ���ҿɼ�����Flag
		virtual uint32_t GetVisibleMask() const = 0;

		// ��Ⱦ˳��
		virtual int GetRenderingOrder() const = 0;

		// ������Ƿ�����
		virtual bool IsActivity() const = 0;
	};

	// �����ڵ㣬�ڳ�������һ����������ʽ���
	class G2DAPI SceneNode : public GObject
	{
	public:
		// �ڵ����ڳ���
		// Scene��Ϊ���ڵ�᷵������
		virtual Scene* GetScene() const = 0;

		// ��ȡ�ڵ�ĸ��ڵ�
		// Scene��Ϊ���ڵ�᷵��nullptr
		virtual SceneNode* GetParentNode() const = 0;

		// ��ȡͬ���ڵ����һ���ڵ�
		// ͬ�����һ���ڵ㷵��nullptr
		virtual SceneNode* GetPrevSiblingNode() const  = 0;

		// ��ȡͬ���ڵ����һ���ڵ�
		// ͬ����һ���ڵ㷵��nullptr
		virtual SceneNode* GetNextSiblingNode() const  = 0;

		// �����ӽڵ㣬���봫��Entity����
		virtual SceneNode* CreateSceneNodeChild(Entity*, bool autoRelease) = 0;

		// �Ӹ��׽ڵ��Ƴ�������ƺ������汣���ӿ�
		virtual void RemoveFromParent() = 0;

		// �ѵ�ǰ�ڵ��ƶ���ͬ����һ�����Ա�֤��Ⱦ˳��
		virtual void MoveToFront() = 0;

		// �ѵ�ǰ�ڵ��ƶ���ͬ�����һ�����Ա�֤��Ⱦ˳��
		virtual void MoveToBack() = 0;

		// ��ͬ��ǰһ���ڵ㽻��λ�ã��Ա�֤��Ⱦ˳��
		// ���ڵ���ͬ����һ����ʱ��ʲôҲ����
		virtual void MovePrev() = 0;

		// ��ͬ����һ���ڵ㽻��λ�ã��Ա�֤��Ⱦ˳��
		// ���ڵ���ͬ�����һ����ʱ��ʲôҲ����
		virtual void MoveNext() = 0;

		// ��ǰ�ڵ�ľֲ�����
		virtual const gml::mat32& GetLocalMatrix() = 0;

		// �۳������и��ڵ�֮����������
		virtual const gml::mat32& GetWorldMatrix() = 0;

		// ���ýڵ��λ��
		// ������������ʹ����ʽ����
		virtual SceneNode* SetPosition(const gml::vec2& position) = 0;

		// ���ýڵ������ƫ�ƣ���Ӱ������
		// ������������ʹ����ʽ����
		virtual SceneNode* SetPivot(const gml::vec2& pivot) = 0;

		// ���ýڵ������
		// ������������ʹ����ʽ����
		virtual SceneNode* SetScale(const gml::vec2& scale) = 0;

		// ���ýڵ�� Roll ��ת
		// ������������ʹ����ʽ����
		virtual SceneNode* SetRotation(gml::radian r) = 0;

		// �ֶ������Ƿ�ɼ������ɼ��Ľڵ㲻�������Ⱦ
		virtual void SetVisible(bool) = 0;

		// �Ƿ��ǹ̶��ڵ�
		// �̶��ڵ�ᱻ���뵽�����Ĳ�����
		// �Ż��ɼ��ж��ٶ�
		virtual void SetStatic(bool) = 0;

		// ����������ɼ��� Mask
		virtual void SetVisibleMask(uint32_t mask, bool recursive) = 0;

		// ��ýڵ�λ��
		virtual const gml::vec2& GetPosition() const = 0;

		// ��ýڵ�����ƫ��
		virtual const gml::vec2& GetPivot() const = 0;

		// ��ýڵ�����
		virtual const gml::vec2& GetScale() const = 0;

		// ��ýڵ�Roll��ת
		virtual gml::radian GetRotation() const = 0;

		// ��ȡ�ڵ�󶨵�Entity����
		virtual Entity* GetEntity() const = 0;

		// �ڵ��Ƿ�ɼ�����
		virtual bool IsVisible() const = 0;

		// �ڵ��Ƿ��ǹ̶�������
		virtual bool IsStatic() const = 0;

		// �ڵ�ɼ���Mask������
		virtual uint32_t GetVisibleMask() const = 0;
	};

	class G2DAPI Scene : public SceneNode
	{
	public:
		// �����ĳ�����Ҫ�û��ֶ��ͷ���Դ
		// ֻ�ܵ���һ��
		virtual void Release() = 0;

		// Ϊ��������һ�������
		virtual Camera* CreateCameraNode() = 0;

		// ��ȡĬ�ϵ������
		virtual Camera* GetMainCamera() const = 0;

		// ����ID��ȡ�����
		// Ĭ����������Ϊ0
		virtual Camera* GetCameraByIndex(uint32_t index) const = 0;

		// �ѳ����е����������Ⱦ����
		// �����û���������
		virtual void Render() = 0;

		// ��ϸ����
		// �����û���������
		virtual void Update(uint32_t elapsedTime) = 0;
	};
}
