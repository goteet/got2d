#pragma once
#include <g2dconfig.h>
#include <g2dinput.h>
#include <gmlvector.h>
#include <gmlmatrix.h>
#include <gmlaabb.h>
#include <gmlrect.h>
namespace g2d
{
	constexpr uint32_t DEFAULT_VISIBLE_MASK = 0xFFFFFFFF;

	class Camera;
	class SceneNode;
	class Scene;

	// 以下为消息响应接口
	// 用户自定义实体重载这些虚函数以获得事件响应
	class G2DAPI EventReceiver
	{
	public:
		// 节点被成功构造的事件
		// 一般初始化代码写这个事件里
		virtual void OnInitial() { }

		// 节点进行更新的事件
		// 一般每帧逻辑更新写这个事件里
		virtual void OnUpdate(uint32_t deltaTime) { }

		// 节点进行渲染的事件
		// 渲染节点的时候需要用户把render request加入到渲染队列中
		virtual void OnRender() { }

		// 节点局部旋转更新事件
		virtual void OnRotate(gml::radian r) { }

		// 节点局部缩放更新事件
		virtual void OnScale(const gml::vec2& newScaler) { }

		// 节点局部位置更新事件
		virtual void OnMove(const gml::vec2& newPos) { }

		// 用户输入消息事件
		// 单纯的获取原始消息，消息会被转化成以下特殊事件
		virtual void OnMessage(const g2d::Message& message) { }

		// 节点局部坐标变化之后，第一次更新的事件
		// 一般AABB的变换在这里计算。
		// 顺序在 OnUpdate之后
		virtual void OnUpdateMatrixChanged() { }

		// 当光标碰到节点对象(Entity)的时候触发
		// 参数是前一个光标悬停的对象
		// 直接从未悬停状态下触碰对象时参数为空
		virtual void OnCursorEnterFrom(SceneNode* adjacency, const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 当光标悬停在物体上的时候持续触发
		virtual void OnCursorHovering(const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 当光标离开节点对象(Entity)的时候触发
		// 参数是当前光标悬停的对象
		// 如果离开实体后光标没有触碰对象，此参数为空
		virtual void OnCursorLeaveTo(SceneNode* adjacency, const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 单击鼠标事件
		virtual void OnLClick(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnRClick(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnMClick(const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 双击鼠标的事件
		virtual void OnLDoubleClick(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnRDoubleClick(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnMDoubleClick(const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 光标拖拽开始
		virtual void OnLDragBegin(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnRDragBegin(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnMDragBegin(const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 没有触碰到别的物体的时候，光标拖拽中
		virtual void OnLDragging(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnRDragging(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnMDragging(const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 没有触碰到别的物体的时候，鼠标拖拽结时
		virtual void OnLDragEnd(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnRDragEnd(const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnMDragEnd(const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 光标触碰到别的物体的时候，鼠标拖拽中
		// 如果dropped 是空的话，会把消息转发到OnDragging
		virtual void OnLDropping(SceneNode* dropped, const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnRDropping(SceneNode* dropped, const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnMDropping(SceneNode* dropped, const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 光标触碰到别的物体的时候，鼠标拖拽结束
		virtual void OnLDropTo(SceneNode* dropped, const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnRDropTo(SceneNode* dropped, const gml::coord& cursorPos, const g2d::Keyboard&) { }
		virtual void OnMDropTo(SceneNode* dropped, const gml::coord& cursorPos, const g2d::Keyboard&) { }

		// 键位被持续按下
		virtual void OnKeyPressing(KeyCode key, const g2d::Keyboard& keyboard) { }

		// 键位被触发，与持续按下的情况互斥
		virtual void OnKeyPress(KeyCode key, const g2d::Keyboard& keyboard) { }
	};

	// 实体基类，节点逻辑的实现全在entity内
	// 挂在场景节点上，一个SceneNode只能挂一个Entity
	// 重载 EventReceiver的接口已获得自定义事件响应
	class G2DAPI Entity : public GObject, public EventReceiver
	{
	public:
		// 用户自定义实体需要实现内存释放的接口
		// 引擎内部会调用这个接口释放entity资源
		// 只会在析构时候被调用
		virtual void Release() = 0;

		// 局部坐标系下节点的包围盒大小
		virtual const gml::aabb2d& GetLocalAABB() const = 0;

		// 节点在世界空间中包围盒的大小
		// 默认实现用世界矩阵变换局部坐标系下的包围盒
		virtual gml::aabb2d GetWorldAABB() const;

		// entity附着关联的场景节点
		// 这个接口一般提供给用户自定义entity内部获取node相关属性
		SceneNode* GetSceneNode() const { return m_sceneNode; }

		// 获取所在场景节点的摄像机可见Flag
		// 这个是一个转发消息的快捷函数。
		uint32_t GetVisibleMask() const;

	public://内部使用
		// 初始化场景节点的时候
		// 设置关联
		void SetSceneNode(g2d::SceneNode* node);

		// 根据场景节点关系调整渲染顺序
		// 用于渲染排序，在此只做记录使用
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

		// 设置 Quad Mesh 大小
		virtual g2d::Quad* SetSize(const gml::vec2& size) = 0;

		// 获取 Quad Mesh 大小
		virtual const gml::vec2& GetSize() const = 0;
	};

	//用于场景查找可见物体，进行渲染的摄像机
	class G2DAPI Camera : public Entity
	{
	public:
		// 摄像机在场景中的编号
		// 默认摄像机的编号为0
		virtual uint32_t GetID() const = 0;

		// 摄像机位置
		// 返回自身，可以使用链式设置
		virtual Camera* SetPosition(const gml::vec2& p) = 0;

		// 摄像机镜头缩放
		// 返回自身，可以使用链式设置
		virtual Camera* SetScale(const gml::vec2& s) = 0;

		// 摄像机 Roll 旋转
		// 返回自身，可以使用链式设置
		virtual Camera* SetRotation(gml::radian r) = 0;

		// 设置渲染顺序，多个摄像机会根据这个渲染顺序进行排列
		// 顺序越小越优先渲染
		virtual void SetRenderingOrder(int order) = 0;

		// 查找可见物体Flag
		virtual void SetVisibleMask(uint32_t mask) = 0;

		// 是否启用摄像机，如果不启用，摄像机不会查找物体进行渲染
		virtual void SetActivity(bool activity) = 0;

		// 渲染系统需要的视矩阵
		virtual const gml::mat32& GetViewMatrix() const = 0;

		// 判断一个aabb是否会被摄像机看见
		virtual bool TestVisible(const gml::aabb2d& bounding) const = 0;

		// 判断一个entity是否会被摄像机看见
		// aabb为一个点，mask不匹配视为不可见
		virtual bool TestVisible(g2d::Entity& entity) const = 0;

		// 查找可见物体Flag
		virtual uint32_t GetVisibleMask() const = 0;

		// 渲染顺序
		virtual int GetRenderingOrder() const = 0;

		// 摄像机是否被启用
		virtual bool IsActivity() const = 0;

		virtual gml::vec2 ScreenToWorld(const gml::coord& pos) const = 0;

		virtual gml::coord WorldToScreen(const gml::vec2& pos) const = 0;
	};

	// 场景节点，在场景中是一个以树形形式组成
	class G2DAPI SceneNode : public GObject
	{
	public:
		// 节点所在场景
		// Scene作为根节点会返回自身
		virtual Scene* GetScene() const = 0;

		// 获取节点的父节点
		// Scene作为根节点会返回nullptr
		virtual SceneNode* GetParentNode() const = 0;

		// 获取同级节点的下一个节点
		// 同级最后一个节点返回nullptr
		virtual SceneNode* GetPrevSiblingNode() const = 0;

		// 获取同级节点的上一个节点
		// 同级第一个节点返回nullptr
		virtual SceneNode* GetNextSiblingNode() const = 0;

		// 创建子节点，必须传入Entity对象
		virtual SceneNode* CreateSceneNodeChild(Entity*, bool autoRelease) = 0;

		// 从父亲节点移除，这个似乎是引擎保留接口
		virtual void RemoveFromParent() = 0;

		// 把当前节点移动到同级第一个，以保证渲染顺序
		virtual void MoveToFront() = 0;

		// 把当前节点移动到同级最后一个，以保证渲染顺序
		virtual void MoveToBack() = 0;

		// 跟同级前一个节点交换位置，以保证渲染顺序
		// 当节点是同级第一个的时候什么也不做
		virtual void MovePrev() = 0;

		// 跟同级后一个节点交换位置，以保证渲染顺序
		// 当节点是同级最后一个的时候什么也不做
		virtual void MoveNext() = 0;

		// 当前节点的局部矩阵
		virtual const gml::mat32& GetLocalMatrix() = 0;

		// 累乘上所有父节点之后的世界矩阵
		virtual const gml::mat32& GetWorldMatrix() = 0;

		// 设置节点的位置
		// 返回自身，可以使用链式设置
		virtual SceneNode* SetPosition(const gml::vec2& position) = 0;

		// 设置节点的中心偏移，会影响缩放
		// 返回自身，可以使用链式设置
		virtual SceneNode* SetPivot(const gml::vec2& pivot) = 0;

		// 设置节点的缩放
		// 返回自身，可以使用链式设置
		virtual SceneNode* SetScale(const gml::vec2& scale) = 0;

		// 设置节点的 Roll 旋转
		// 返回自身，可以使用链式设置
		virtual SceneNode* SetRotation(gml::radian r) = 0;

		// 手都设置是否可见，不可见的节点不会进行渲染
		virtual void SetVisible(bool) = 0;

		// 是否是固定节点
		// 固定节点会被加入到场景四叉树中
		// 优化可见判断速度
		virtual void SetStatic(bool) = 0;

		// 设置摄像机可见的 Mask
		virtual void SetVisibleMask(uint32_t mask, bool recursive) = 0;

		// 获得节点位置
		virtual const gml::vec2& GetPosition() const = 0;

		// 获得节点中心偏移
		virtual const gml::vec2& GetPivot() const = 0;

		// 获得节点缩放
		virtual const gml::vec2& GetScale() const = 0;

		// 获得节点Roll旋转
		virtual gml::radian GetRotation() const = 0;

		// 获得节点世界坐标的位置
		virtual gml::vec2 GetWorldPosition() = 0;

		// 获取节点绑定的Entity对象
		virtual Entity* GetEntity() const = 0;

		// 节点是否可见设置
		virtual bool IsVisible() const = 0;

		// 节点是否是固定的设置
		virtual bool IsStatic() const = 0;

		// 节点可见性Mask的设置
		virtual uint32_t GetVisibleMask() const = 0;

		// 把坐标转换节点局部空间内
		virtual gml::vec2 WorldToLocal(const gml::vec2& pos) = 0;

		// 把坐标转换节点同级的局部空间
		virtual gml::vec2 WorldToParent(const gml::vec2& pos) = 0;
	};

	class G2DAPI Scene : public SceneNode
	{
	public:
		// 创建的场景需要用户手动释放资源
		// 只能调用一次
		virtual void Release() = 0;

		// 为场景创建一个摄像机
		virtual Camera* CreateCameraNode() = 0;

		// 获取默认的摄像机
		virtual Camera* GetMainCamera() const = 0;

		// 根据ID获取摄像机
		// 默认摄像机编号为0
		virtual Camera* GetCameraByIndex(uint32_t index) const = 0;

		// 把场景中的物体加入渲染队列
		// 需要用户主动调用
		virtual void Render() = 0;
	};
}
