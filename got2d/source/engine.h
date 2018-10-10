#pragma once
#include <vector>
#include <string>
#include "g2dengine.h"
#include "render/render_system.h"
#include "scene/scene.h"
#include "input/input.h"

class Engine : public g2d::Engine
{
	RTTI_IMPL;
public:
	virtual g2d::RenderSystem* GetRenderSystem() override;

	virtual g2d::Scene* CreateNewScene(float sceneBoundSize) override;

	virtual void Update(unsigned int deltaTime) override;

	virtual void OnMessage(const g2d::Message& message) override;

	virtual bool OnResize(unsigned int width, unsigned int height) override;

	virtual void Release() override;

	//system getters
public:
	::RenderSystem& GetRenderSystemImpl();

	::Mouse& GetMouseImpl();

	::Keyboard& GetKeyboardImpl();

public:
	static Engine* Instance;

	Engine() = default;

	~Engine();

	bool Initialize(const CreationConfig& config);

	void SetResourceRoot(const std::string& resPath);

	const std::string& GetResourceRoot() const;

	//void RemoveScene(::Scene& scene);

private:
	bool CreateRenderSystem(void* nativeWindow);

	void* mNativeWindow = nullptr;

	unsigned int mElapsedTime = 0;

	std::string mResourceRoot;

	RenderSystem	mRenderSystem;
	Mouse			mMouse;
	Keyboard		mKeyboard;

	std::vector<::Scene*> mSceneList;
};