// testbed.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "testbed.h"

HINSTANCE hInst;
HWND hWnd;
Testbed testbed;

void RegisterCallbacks();
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	RegisterCallbacks();
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance(hInstance, nCmdShow) || !testbed.InitApp())
	{
		return FALSE;
	}

	MSG msg;
	bool needExit = false;
	testbed.FirstTick();
	while (true)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (!needExit)
		{
			if (!testbed.MainLoop())
			{
				::DestroyWindow(hWnd);
				needExit = true;
			}
		}
	}

	testbed.DestroyApp();

	return (int)msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	::ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = testbed.GetWindowClassName().c_str();

	return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	hWnd = CreateWindowW(testbed.GetWindowClassName().c_str(), testbed.GetWindowTitle().c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		RECT rect;
		::GetClientRect(hWnd, &rect);
		testbed.OnResize(rect.right - rect.left, rect.bottom - rect.top);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



#include <g2dengine.h>
#include <g2drender.h>
#include <g2dscene.h>

#include "hexgon.h"

class MyApp
{
public:
	void Start();
	void End();
	bool Update(uint32_t elapsedTime);
private:
	g2d::Scene* mainScene = nullptr;
};

MyApp g_app;

void RegisterCallbacks()
{
	testbed.OnStart = std::bind(&MyApp::Start, &g_app);
	testbed.OnFinish = std::bind(&MyApp::End, &g_app);
	using namespace std::placeholders;
	testbed.OnUpdate = std::bind(&MyApp::Update, &g_app, _1);
}

void MyApp::Start()
{
	mainScene = g2d::GetEngine()->CreateNewScene(2 << 10);

	auto quad = g2d::Quad::Create()->SetSize(gml::vec2(100, 120));
	auto node = mainScene->CreateSceneNodeChild(quad, true)->SetPosition(gml::vec2(100, 100));

	//node.SetVisibleMask(3, true);
	node->SetStatic(true);
	for (int i = 0; i <= 4; i++)
	{
		auto quad = g2d::Quad::Create()->SetSize(gml::vec2(100, 120));
		auto child = node->CreateSceneNodeChild(quad, true)->SetPosition(gml::vec2(50, 50));
		child->SetVisibleMask((i % 2) ? 1 : 2, true);
		child->SetStatic(true);

		node = child;
	}

	if (false)//����spatial tree ����Ҫ
	{
		auto mainCamera = mainScene->GetMainCamera();
		mainCamera->SetActivity(true);

		auto camera = mainScene->CreateCameraNode();
		if (camera)
		{
			camera->SetPosition(gml::vec2(220, 100));
			camera->SetVisibleMask(2);
			camera->SetActivity(false);
		}

		camera = mainScene->CreateCameraNode();
		if (camera)
		{
			camera->SetPosition(gml::vec2(220, 100));
			camera->SetRenderingOrder(-1);
			camera->SetVisibleMask(1);
			camera->SetActivity(false);
		}
	}

	Hexgon* hexgonEntity = new Hexgon();
	auto hexgonNode = mainScene->CreateSceneNodeChild(hexgonEntity, true);
	hexgonNode->SetPosition({ 10,10 });
}

void MyApp::End()
{
	mainScene->Release();
	mainScene = nullptr;
}

bool MyApp::Update(uint32_t elapsedTime)
{
	mainScene->Update(elapsedTime);

	g2d::GetEngine()->GetRenderSystem()->BeginRender();
	mainScene->Render();
	g2d::GetEngine()->GetRenderSystem()->EndRender();
	return true;

}

