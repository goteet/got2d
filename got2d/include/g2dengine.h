#pragma once
#include <g2dconfig.h>
#include <g2dmessage.h>

namespace g2d
{
	class RenderSystem;
	class Scene;
	class Texture;

	// ����ӿڣ����ܵ������
	class G2DAPI Engine : public GObject
	{
	public:
		//�����ʼ������
		struct Config
		{
			// ���ش�����
			void* nativeWindow;

			// ��Դ�ļ���·��
			// ���е���Դ���ض�����������·�����ص�
			const char* resourceFolderPath;
		};

		// ��ʼ������ӿڣ�ϵͳ������ʱ��ֻ�ܵ���һ��
		static bool Initialize(const Config& config);

		// ���������ʱ����Ҫ��ʾ���ýӿ��ͷ�������Դ
		static void Uninitialize();

		// �ж������Ƿ��Ѿ�����ʼ��
		static bool IsInitialized();

		// ����ȫ�ֵ�ֻ����һ��ʵ�������Ի����ڲ���¼����ָ�롣
		// ��ʼ��֮�����ʹ��
		static Engine* Instance();

		// ��Ⱦϵͳ�ӿ�
		virtual RenderSystem* GetRenderSystem() = 0;

		// �����µĳ�������
		// ��Ҫ���ó��������߽磬���ӿ��ӻ��ü�Ч��
		// �ڳ����߽�֮��Ķ���ÿ����Ⱦ֮ǰ������пɼ����ж�
		virtual Scene* CreateNewScene(float boundSize) = 0;

		// ������£���������Ч������
		// ��Ҫ�û���������
		virtual void Update(uint32_t deltaTime) = 0;

		// ���û������ʱ�������г����ڵ��ɷ���Ϣ
		// ��Ҫ�û���������
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


