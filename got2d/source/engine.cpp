//#include <algorithm>
#include "engine.h"

Engine* Engine::Instance = nullptr;

//*************************************************************
// overrides
//*************************************************************
g2d::RenderSystem* Engine::GetRenderSystem()
{
	return &GetRenderSystemImpl();
}

g2d::Scene* Engine::CreateNewScene(float sceneBoundSize)
{
	::Scene* pScene = new Scene(sceneBoundSize);
	mSceneList.push_back(pScene);
	return pScene;
}

void Engine::Update(unsigned int deltaTime)
{
	mElapsedTime += deltaTime;

	GetKeyboardImpl().Update(mElapsedTime);
	GetMouseImpl().Update(mElapsedTime);

	for (::Scene* pScene : mSceneList)
	{
		pScene->Update(mElapsedTime, deltaTime);
	}
}

void Engine::OnMessage(const g2d::Message& message)
{
	GetKeyboardImpl().OnMessage(message, mElapsedTime);
	GetMouseImpl().OnMessage(message, mElapsedTime);

	for (::Scene* pScene : mSceneList)
	{
		pScene->OnMessage(message, mElapsedTime);
	}
}

bool Engine::OnResize(unsigned int width, unsigned int height)
{
	if (mRenderSystem.OnResize(width, height))
	{
		for (::Scene* pScene : mSceneList)
		{
			pScene->OnResize();
		}
		return true;
	}
	else
	{
		return false;
	}
}

void Engine::Release()
{
	delete this;
}


//*************************************************************
// systems
//*************************************************************
::RenderSystem& Engine::GetRenderSystemImpl()
{
	return mRenderSystem;
}

::Mouse& Engine::GetMouseImpl()
{
	return mMouse;
}

::Keyboard& Engine::GetKeyboardImpl()
{
	return mKeyboard;
}


//*************************************************************
// functions
//*************************************************************
Engine::~Engine()
{
	mRenderSystem.Destroy();
}

bool Engine::Initialize(const CreationConfig& config)
{
	if (!CreateRenderSystem(config.NativeWindow))
	{
		return false;
	}

	SetResourceRoot(config.ResourceFolderPath);
	return true;
}

void Engine::SetResourceRoot(const std::string& resPath)
{
	if (!resPath.empty())
	{
		mResourceRoot = resPath;
		std::replace(mResourceRoot.begin(), mResourceRoot.end(), '/', '\\');
		if (mResourceRoot.back() != '\\')
		{
			mResourceRoot.push_back('\\');
		}
	}
}

const std::string & Engine::GetResourceRoot() const
{
	return mResourceRoot;
}

//void Engine::RemoveScene(::Scene& scene)
//{
//	auto oldEnd = mSceneList.end();
//	auto newEnd = std::remove(mSceneList.begin(), oldEnd, &scene);
//	mSceneList.erase(newEnd, oldEnd);
//}

bool Engine::CreateRenderSystem(void* nativeWindow)
{
	mNativeWindow = nativeWindow;
	if (!mRenderSystem.Create(nativeWindow))
	{
		return false;
	}
	return true;
}
