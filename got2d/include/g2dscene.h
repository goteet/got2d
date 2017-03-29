#pragma once
#include <g2dconfig.h>
#include <g2dinput.h>
#include <gmlvector.h>
#include <gmlmatrix.h>
#include <gmlaabb.h>
#include <gmlrect.h>
namespace g2d
{
	constexpr uint32_t DEF_VISIBLE_MASK = 0xFFFFFFFF;
	constexpr int DEF_COMPONENT_ORDER = 0x5000;

	class Component;
	class Camera;
	class SceneNode;
	class Scene;


	// ������࣬���ڳ����ڵ��ϣ�һ��SceneNode���ԹҶ��Component
	// ���ؽӿ��ѻ���Զ����¼���Ӧ
	class G2DAPI Component : public GObject
	{
	public:
		// �û��Զ���ʵ����Ҫ������ڴ��ͷŵĽӿ�
		// �����ڲ����������ӿ��ͷ�Component��Դ
		// ֻ��������ʱ�򱻵���
		virtual void Release() = 0;

		// �ֲ�����ϵ������İ�Χ�д�С
		virtual const gml::aabb2d& GetLocalAABB() const { static gml::aabb2d b; return b; }

		// �ڵ�������ռ��а�Χ�еĴ�С
		// Ĭ��ʵ�����������任�ֲ�����ϵ�µİ�Χ��
		virtual gml::aabb2d GetWorldAABB() const;

		// �û���������������������ִ��˳��
		// ���ֵԽ��ִ��˳��Լ����
		virtual int GetExecuteOrder() const { return DEF_COMPONENT_ORDER; }

		// ������صĵĳ����ڵ�
		// ����ӿ�һ���ṩ���û��Զ����component�ڲ���ȡnode�������
		SceneNode* GetSceneNode() const { return m_sceneNode; }

		// ��ȡ���ڳ����ڵ��������ɼ�Flag
		// �����һ��ת����Ϣ�Ŀ�ݺ�����
		uint32_t GetVisibleMask() const;

	public:
		// �ڵ㱻�ɹ�������¼�
		// һ���ʼ������д����¼���
		virtual void OnInitial() { }

		// �ڵ���и��µ��¼�
		// һ��ÿ֡�߼�����д����¼���
		virtual void OnUpdate(uint32_t deltaTime) { }

		// �ڵ������Ⱦ���¼�
		// ��Ⱦ�ڵ��ʱ����Ҫ�û���render request���뵽��Ⱦ������
		virtual void OnRender() { }

		// �ڵ�ֲ���ת�����¼�
		virtual void OnRotate(gml::radian r) { }

		// �ڵ�ֲ����Ÿ����¼�
		virtual void OnScale(const gml::vec2& newScaler) { }

		// �ڵ�ֲ�λ�ø����¼�
		virtual void OnMove(const gml::vec2& newPos) { }

		// �û�������Ϣ�¼�
		// �����Ļ�ȡԭʼ��Ϣ����Ϣ�ᱻת�������������¼�
		virtual void OnMessage(const g2d::Message& message) { }

		// �ڵ�ֲ�����仯֮�󣬵�һ�θ��µ��¼�
		// һ��AABB�ı任��������㡣
		// ˳���� OnUpdate֮��
		virtual void OnUpdateMatrixChanged() { }

		// ����������ڵ����(Entity)��ʱ�򴥷�
		// ������ǰһ�������ͣ�Ķ���
		// ֱ�Ӵ�δ��ͣ״̬�´�������ʱ����Ϊ��
		virtual void OnCursorEnterFrom(SceneNode* adjacency, const g2d::Mouse&, const g2d::Keyboard&) { }

		// �������ͣ�������ϵ�ʱ���������
		virtual void OnCursorHovering(const g2d::Mouse&, const g2d::Keyboard&) { }

		// ������뿪�ڵ����(Entity)��ʱ�򴥷�
		// �����ǵ�ǰ�����ͣ�Ķ���
		// ����뿪ʵ�����û�д������󣬴˲���Ϊ��
		virtual void OnCursorLeaveTo(SceneNode* adjacency, const g2d::Mouse&, const g2d::Keyboard&) { }

		// ��������¼�
		virtual void OnLClick(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnRClick(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnMClick(const g2d::Mouse&, const g2d::Keyboard&) { }

		// ˫�������¼�
		virtual void OnLDoubleClick(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnRDoubleClick(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnMDoubleClick(const g2d::Mouse&, const g2d::Keyboard&) { }

		// �����ק��ʼ
		virtual void OnLDragBegin(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnRDragBegin(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnMDragBegin(const g2d::Mouse&, const g2d::Keyboard&) { }

		// û�д�������������ʱ�򣬹����ק��
		virtual void OnLDragging(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnRDragging(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnMDragging(const g2d::Mouse&, const g2d::Keyboard&) { }

		// û�д�������������ʱ�������ק��ʱ
		virtual void OnLDragEnd(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnRDragEnd(const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnMDragEnd(const g2d::Mouse&, const g2d::Keyboard&) { }

		// ��괥������������ʱ�������ק��
		// ���dropped �ǿյĻ��������Ϣת����OnDragging
		virtual void OnLDropping(SceneNode* dropped, const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnRDropping(SceneNode* dropped, const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnMDropping(SceneNode* dropped, const g2d::Mouse&, const g2d::Keyboard&) { }

		// ��괥������������ʱ�������ק����
		virtual void OnLDropTo(SceneNode* dropped, const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnRDropTo(SceneNode* dropped, const g2d::Mouse&, const g2d::Keyboard&) { }
		virtual void OnMDropTo(SceneNode* dropped, const g2d::Mouse&, const g2d::Keyboard&) { }

		// ��λ����������������µ��������
		virtual void OnKeyPress(KeyCode key, const g2d::Mouse&, const g2d::Keyboard& keyboard) { }

		// ��λ���������µĵ�һ��
		virtual void OnKeyPressingBegin(KeyCode key, const g2d::Mouse&, const g2d::Keyboard& keyboard) { }

		// ��λ����������
		virtual void OnKeyPressing(KeyCode key, const g2d::Mouse&, const g2d::Keyboard& keyboard) { }

		// ��λ���������µ����һ��
		virtual void OnKeyPressingEnd(KeyCode key, const g2d::Mouse&, const g2d::Keyboard& keyboard) { }

	public://�ڲ�ʹ��

		// ��ʼ�������ڵ��ʱ�����ù���
		void SetSceneNode(g2d::SceneNode* node);

		// ���ڵ�˳������ʱ���������ô˽ӿ�������Ⱦ˳��
		void SetRenderingOrder(uint32_t& order);

		// ��Ⱦ��ʱ�����˳���������
		uint32_t GetRenderingOrder() { return m_renderingOrder; }

	private:
		SceneNode* m_sceneNode = nullptr;
		uint32_t m_renderingOrder = 0xFFFFFFFF;
	};

	// һ��ͼƬ
	class G2DAPI Quad : public Component
	{
	public:
		static Quad* Create();

		// ���� Quad Mesh ��С
		virtual g2d::Quad* SetSize(const gml::vec2& size) = 0;

		// ��ȡ Quad Mesh ��С
		virtual const gml::vec2& GetSize() const = 0;
	};

	//���ڳ������ҿɼ����壬������Ⱦ�������
	class G2DAPI Camera : public Component
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

		// �ж�һ��Component�Ƿ�ᱻ���������
		// aabbΪһ���㣬mask��ƥ����Ϊ���ɼ�
		virtual bool TestVisible(Component& component) const = 0;

		// ���ҿɼ�����Flag
		virtual uint32_t GetVisibleMask() const = 0;

		// ��Ⱦ˳��
		virtual int GetRenderingOrder() const = 0;

		// ������Ƿ�����
		virtual bool IsActivity() const = 0;

		virtual gml::vec2 ScreenToWorld(const gml::coord& pos) const = 0;

		virtual gml::coord WorldToScreen(const gml::vec2& pos) const = 0;
	};

	// �����ڵ㣬�ڳ�������һ����������ʽ���
	class G2DAPI SceneNode : public GObject
	{
	public:
		// �����ڵ㣬�ѵ�ǰ�ڵ������ɾ����
		// ��ͬʱ���������ȫ��ɾ����
		virtual void Release() = 0;

		// �ڵ����ڳ���
		// Scene��Ϊ���ڵ�᷵������
		virtual Scene* GetScene() const = 0;

		// ��ȡ�ڵ�ĸ��ڵ�
		// Scene��Ϊ���ڵ�᷵��nullptr
		virtual SceneNode* GetParentNode() const = 0;

		// ��ȡͬ���ڵ����һ���ڵ�
		// ͬ�����һ���ڵ㷵��nullptr
		virtual SceneNode* GetPrevSiblingNode() const = 0;

		// ��ȡͬ���ڵ����һ���ڵ�
		// ͬ����һ���ڵ㷵��nullptr
		virtual SceneNode* GetNextSiblingNode() const = 0;

		// ��ȡ��һ������
		// ���û�к����򷵻�nullptr
		virtual g2d::SceneNode* FirstChild() const = 0;

		// ��ȡ���һ������
		// ���û�к����򷵻�nullptr
		virtual g2d::SceneNode* LastChild() const = 0;

		// ʹ����������ӽڵ�
		virtual g2d::SceneNode* GetChildByIndex(uint32_t index) const = 0;

		// ��ȡ�ӽڵ����Ŀ
		virtual uint32_t GetChildCount() const = 0;

		// �����ӽڵ�
		virtual SceneNode* CreateChild() = 0;

		// �ѵ�ǰ�ڵ��ƶ���ͬ�����һ��
		// �Ա�֤��һ����Ⱦ��
		virtual void MoveToFront() = 0;

		// �ѵ�ǰ�ڵ��ƶ���ͬ����һ��
		// �Ա�֤�����һ����Ⱦ��
		virtual void MoveToBack() = 0;

		// ��ͬ��ǰһ���ڵ㽻��λ�ã��Ա�֤��Ⱦ˳��
		// ���ڵ���ͬ����һ����ʱ��ʲôҲ����
		virtual void MovePrev() = 0;

		// ��ͬ����һ���ڵ㽻��λ�ã��Ա�֤��Ⱦ˳��
		// ���ڵ���ͬ�����һ����ʱ��ʲôҲ����
		virtual void MoveNext() = 0;

		// �������ӿ�
		virtual bool AddComponent(Component*, bool autoRelease) = 0;

		// �Ƴ�ȷ�����
		virtual bool RemoveComponent(Component*) = 0;

		// �Ƴ�ȷ�����������ǿ�Ʋ�����Release�ӿ�
		virtual bool RemoveComponentWithoutRelease(Component*) = 0;

		// ��ȡĳ��������ͷ������������������ڣ����ؼ١�
		virtual bool IsComponentAutoRelease(Component*) const = 0;

		// �����±�������ȡ�����
		// ע�⣬ ��ͬ�����������ȼ��仯������������ǻᷢ���ı��
		virtual Component* GetComponentByIndex(uint32_t index) const = 0;

		// ����ĸ���
		virtual uint32_t GetComponentCount() const = 0;

		// ��ǰ�ڵ�ľֲ�����
		virtual const gml::mat32& GetLocalMatrix() = 0;

		// �۳������и��ڵ�֮����������
		virtual const gml::mat32& GetWorldMatrix() = 0;

		// ���ýڵ��λ��
		// ������������ʹ����ʽ����
		virtual SceneNode* SetPosition(const gml::vec2& position) = 0;

		// ���ýڵ������λ�ã����API��ı�ֲ�����
		// ������������ʹ����ʽ����
		virtual g2d::SceneNode* SetWorldPosition(const gml::vec2& position) = 0;

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

		// ��ýڵ����������λ��
		virtual gml::vec2 GetWorldPosition() = 0;

		// ��ýڵ�����ƫ��
		virtual const gml::vec2& GetPivot() const = 0;

		// ��ýڵ�����
		virtual const gml::vec2& GetScale() const = 0;

		// ��ýڵ�Roll��ת
		virtual gml::radian GetRotation() const = 0;

		// ��ȡ��ǰ�ڵ��Ǹ��׵ĵڼ����ڵ�
		virtual uint32_t GetChildIndex() const = 0;

		// �ڵ��Ƿ�ɼ�����
		virtual bool IsVisible() const = 0;

		// �ڵ��Ƿ��ǹ̶�������
		virtual bool IsStatic() const = 0;

		// �ڵ�ɼ���Mask������
		virtual uint32_t GetVisibleMask() const = 0;

		// ������ת���ڵ�ֲ��ռ���
		virtual gml::vec2 WorldToLocal(const gml::vec2& pos) = 0;

		// ������ת���ڵ�ͬ���ľֲ��ռ�
		virtual gml::vec2 WorldToParent(const gml::vec2& pos) = 0;
	};

	// �������ͻ�ȡ
	template<typename T> T* FindComponent(SceneNode* node);

	class G2DAPI Scene : public GObject
	{
	public:
		// �����ĳ�����Ҫ�û��ֶ��ͷ���Դ
		// ֻ�ܵ���һ��
		virtual void Release() = 0;

		// ��ȡ��һ������
		// ���û�к����򷵻�nullptr
		virtual g2d::SceneNode* FirstChild() const = 0;

		// ��ȡ���һ������
		// ���û�к����򷵻�nullptr
		virtual g2d::SceneNode* LastChild() const = 0;

		// ʹ����������ӽڵ�
		virtual g2d::SceneNode* GetChildByIndex(uint32_t index) const = 0;

		// ��ȡ�ӽڵ����Ŀ
		virtual uint32_t GetChildCount() const = 0;

		// �����ӽڵ�
		virtual SceneNode* CreateChild() = 0;

		// Ϊ��������һ�������
		virtual Camera* CreateCameraNode() = 0;

		// ��ȡĬ�ϵ������
		virtual Camera* GetMainCamera() const = 0;

		// ����ID��ȡ�����
		// Ĭ����������Ϊ0
		virtual Camera* GetCameraByIndex(uint32_t index) const = 0;

		// ��ȡ���������Ŀ
		virtual uint32_t GetCameraCount() const = 0;

		// �ѳ����е����������Ⱦ����
		// ��Ҫ�û���������
		virtual void Render() = 0;
	};

	template<typename T> T* FindComponent(SceneNode* node)
	{
		auto count = node->GetComponentCount();
		for (uint32_t i = 0; i < count; i++)
		{
			auto component = node->GetComponentByIndex(i);
			if (Is<T>(component)) return reinterpret_cast<T*>(component);
		}
		return nullptr;
	}
}
