#include "stdafx.h"
#include <cinttypes>
#include "framework.h"


namespace g2d
{
	struct Scene;
	struct SceneNode;
}

class Testbed
{
public:
	Testbed(Framework& fw) : framework(fw) { }
	void Start();
	void End();
	bool Update(uint32_t deltaTime);
	void OnMessage(const g2d::Message& message);
private:
	Framework& framework;

	g2d::Scene* mainScene = nullptr;
	g2d::SceneNode* HexagonNode = nullptr;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Framework framework(hInstance);
	Testbed testbed(framework);

	// 注册事件
	{
		framework.OnStart = [&] { testbed.Start(); };
		framework.OnFinish = [&] { testbed.End(); };
		framework.OnUpdate = [&](uint32_t deltaTime)->bool { return testbed.Update(deltaTime); };
		framework.OnMessage = [&](const g2d::Message& message) { testbed.OnMessage(message); };
	}

	// 运行程序
	if (framework.Initial(nCmdShow, "../extern/res/win32_test/"))
	{
		return framework.Start();
	}
	return 0;
}

#include "hexagon.h"
#include "g2dengine.h"
#include "g2drender.h"
#include "g2dscene.h"

g2d::SceneNode* CreateQuadNode(g2d::SceneNode* parent);

class KeyboardMoving : public g2d::Component
{
	RTTI_IMPL;
public: //implement
	virtual void Release() override { delete this; }

	virtual void OnKeyPress(g2d::KeyCode key, const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override
	{
		if (key == g2d::KeyCode::Enter && (GetSceneNode()->GetChildCount() < 5 || GetSceneNode()->GetParentNode() == nullptr))
		{
			CreateQuadNode(GetSceneNode());
		}
		else if (key == g2d::KeyCode::Delete && GetSceneNode()->GetChildIndex() == 3)
		{
			GetSceneNode()->Release();
		}
		else if (key == g2d::KeyCode::KeyR)
		{
			GetSceneNode()->SetRightDirection(cxx::float2x2::rotate(cxx::degree<float>(45)) * cxx::float2(1, 0));
		}
		else if (key == g2d::KeyCode::KeyQ)
		{
			GetSceneNode()->SetUpDirection(cxx::float2x2::rotate(cxx::degree<float>(-45)) * cxx::float2(0, 1));
		}
	}

	virtual void OnKeyPressing(g2d::KeyCode key, const g2d::Mouse& mouse, const g2d::Keyboard& keyboard) override
	{
		if (key == g2d::KeyCode::ArrowLeft)
		{
			GetSceneNode()->SetPosition(GetSceneNode()->GetPosition() + cxx::float2(-1, 0));
		}
		else if (key == g2d::KeyCode::ArrowRight)
		{
			GetSceneNode()->SetPosition(GetSceneNode()->GetPosition() + cxx::float2(1, 0));
		}
		else if (key == g2d::KeyCode::ArrowUp)
		{
			GetSceneNode()->SetPosition(GetSceneNode()->GetPosition() + cxx::float2(0, 1));
		}
		else if (key == g2d::KeyCode::ArrowDown)
		{
			GetSceneNode()->SetPosition(GetSceneNode()->GetPosition() + cxx::float2(0, -1));
		}
		else if (key == g2d::KeyCode::KeyE)
		{
			GetSceneNode()->SetRightDirection(cxx::float2x2::rotate(cxx::degree<float>(1)) * GetSceneNode()->GetRightDirection());
		}
		else if (key == g2d::KeyCode::KeyW)
		{
			GetSceneNode()->SetUpDirection(cxx::float2x2::rotate(cxx::degree<float>(-1)) * GetSceneNode()->GetUpDirection());
		}
	}
};


g2d::SceneNode* CreateQuadNode(g2d::SceneNode* parent)
{
	auto quad = g2d::Quad::Create()->SetSize(cxx::float2(100, 120));
	auto child = parent->CreateChild()->SetPosition(cxx::float2(50, 20));
	child->AddComponent(quad, true);
	child->AddComponent(new KeyboardMoving(), true);
	child->SetStatic(false);
	return child;
}


void Testbed::Start()
{
	mainScene = g2d::Engine::GetInstance()->CreateNewScene(2 << 10);

	// board
	auto boardNode = mainScene->GetRootNode()->CreateChild();
	boardNode->SetPosition({ -200.0f, 0.0f });
	boardNode->AddComponent(new HexagonBoard(), true);

	//hexgon node
	HexagonNode = mainScene->GetRootNode()->CreateChild();
	HexagonNode->SetPosition({ 0,0 });
	HexagonNode->AddComponent(new Hexagon(), true);
	HexagonNode->AddComponent(new EntityDragging(), true);

	auto quad = g2d::Quad::Create()->SetSize(cxx::float2(100, 120));
	auto node = mainScene->GetRootNode()->CreateChild()->SetPosition(cxx::float2(300, 0));
	node->AddComponent(quad, true);
	node->SetStatic(true);

	for (int i = 0; i < 5; i++)
	{
		auto quad = g2d::Quad::Create()->SetSize(cxx::float2(100, 120));
		auto child = node->CreateChild()->SetPosition(cxx::float2(50, 60));
		child->AddComponent(quad, true);
		child->AddComponent(new KeyboardMoving(), true);
		child->SetStatic(true);
		node = child;
	}

	HexagonNode->MoveToFront();
}

void Testbed::End()
{
	mainScene->Release();
	mainScene = nullptr;
}

bool Testbed::Update(uint32_t deltaTime)
{
	g2d::Engine::GetInstance()->Update(deltaTime);
	g2d::Engine::GetInstance()->GetRenderSystem()->BeginRender();
	mainScene->Render();
	g2d::Engine::GetInstance()->GetRenderSystem()->EndRender();
	return true;
}

void Testbed::OnMessage(const g2d::Message& message)
{
	g2d::Engine::GetInstance()->OnMessage(message);
}