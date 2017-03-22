#pragma once
#include <g2dconfig.h>
#include <g2dmessage.h>

namespace g2d
{
	class RenderSystem;
	class Scene;
	class Texture;

	// 引擎接口，功能的总入口
	class G2DAPI Engine : public GObject
	{
	public:
		//引擎初始化参数
		struct Config
		{
			// 本地窗体句柄
			void* nativeWindow;

			// 资源文件夹路径
			// 所有的资源加载都是相对于这个路径加载的
			const char* resourceFolderPath;
		};

		// 初始化引擎接口，系统启动的时候只能调用一次
		static bool Initialize(const Config& config);

		// 程序结束的时候，需要显示调用接口释放引擎资源
		static void Uninitialize();

		// 判断引擎是否已经被初始化
		static bool IsInitialized();

		// 引擎全局的只能有一个实例，所以会在内部记录引擎指针。
		// 初始化之后才能使用
		static Engine* Instance();

		// 渲染系统接口
		virtual RenderSystem* GetRenderSystem() = 0;

		// 创建新的场景对象
		// 需要设置场景的最大边界，增加可视化裁剪效率
		// 在场景边界之外的对象，每次渲染之前都会进行可见性判断
		virtual Scene* CreateNewScene(float boundSize) = 0;

		// 引擎更新，场景，特效等物体
		// 需要用户主动调用
		virtual void Update(uint32_t deltaTime) = 0;

		// 当用户输入的时候，往所有场景节点派发消息
		// 需要用户主动调用
		virtual void OnMessage(const Message& message) = 0;
	};

	inline bool IsEngineInitialized()
	{
		return g2d::Engine::IsInitialized();
	}

	inline Engine* GetEngine()
	{
		return g2d::Engine::Instance();
	}
}


