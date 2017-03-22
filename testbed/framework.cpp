#include "stdafx.h"
#include "framework.h"
#include <timeapi.h>
#include <time.h>
#include <g2dengine.h>
#include <g2drender.h>
#include <g2dinput.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	return 0;
	case WM_ACTIVATE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		Framework* pFramework = (Framework*)::GetWindowLongPtrW(hWnd, 0);
		pFramework->OnWindowMessage(message,
			static_cast<uint32_t>(wParam),
			static_cast<uint32_t>(lParam)
		);
	}
	break;
	case WM_SIZE:
	{
		RECT rect;
		::GetClientRect(hWnd, &rect);
		Framework* pFramework = (Framework*)::GetWindowLongPtrW(hWnd, 0);
		pFramework->OnWindowResize(rect.right - rect.left, rect.bottom - rect.top);
	}
	break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int AutoWinClassRegister::s_initialCount = 0;
const std::wstring AutoWinClassRegister::name = L"got2d_window_class";

AutoWinClassRegister::AutoWinClassRegister(HINSTANCE hInstance)
	: instance(hInstance)
{
	if (s_initialCount == 0)
	{
		WNDCLASSEXW wcex;
		::ZeroMemory(&wcex, sizeof(WNDCLASSEX));

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(Framework*);
		wcex.hInstance = instance;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszClassName = name.c_str();

		if (::RegisterClassExW(&wcex) == 0)
		{
			throw nullptr;
		}
	}
	++s_initialCount;
}

AutoWinClassRegister::~AutoWinClassRegister()
{
	--s_initialCount;
	if (s_initialCount == 0)
	{
		::UnregisterClassW(name.c_str(), instance);
	}
}

Framework::Framework(HINSTANCE instance)
	: m_autoClassRegister(instance)
{
	OnMessageInternal = [](const g2d::Message& message) {};
}

Framework::~Framework()
{
	//should not be here right now;
	if (m_initial)
	{
		if (m_hWindow != NULL)
		{
			::DestroyWindow(m_hWindow);
			m_hWindow = NULL;
		}

		if (g2d::Engine::IsInitialized())
		{
			g2d::Engine::Uninitialize();
		}
	}
}

void Framework::QuitApp()
{
	m_running = false;
}



void Framework::SetCursorPos(const gml::coord& pos)
{
	POINT p = { pos.x, pos.y };
	::ClientToScreen(m_hWindow, &p);
	::SetCursorPos(p.x, p.y);
}

gml::coord Framework::GetCursorPos()
{
	POINT p;
	::GetCursorPos(&p);
	::ScreenToClient(m_hWindow, &p);
	return { p.x, p.y };
}

const std::wstring& Framework::GetWindowTitle()
{
	static const std::wstring title = L"got2d test bed";
	return title;
}

uint32_t Framework::GetFrameCount() const
{
	return m_frameCount;
}

uint32_t Framework::GetElapsedTime() const
{
	return m_elapsedTime;
}

bool Framework::Initial(int nCmdShow, const std::string& resPath)
{
	if (m_initial)
	{
		throw nullptr;
	}

	if (!CreateRenderingWindow(nCmdShow))
		return false;

	if (!IntializeEngine(resPath))
		return false;

	m_initial = true;
	return true;
}

bool Framework::CreateRenderingWindow(int nCmdShow)
{
	m_hWindow = CreateWindowW(
		AutoWinClassRegister::name.c_str(),
		GetWindowTitle().c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		nullptr, nullptr,
		m_autoClassRegister.instance,
		nullptr);

	if (m_hWindow != NULL)
	{
		::SetWindowLongPtrW(m_hWindow, 0, (LONG_PTR)this);
		ShowWindow(m_hWindow, nCmdShow);
		UpdateWindow(m_hWindow);
		return true;
	}
	else
	{
		return false;
	}
}

std::string GetRunningDirectory()
{
	char path[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, path);
	return path;
}

bool Framework::IntializeEngine(const std::string& resPath)
{
	srand(static_cast<unsigned int>(time(0)));
	m_elapsedTime = 0;
	m_tickInterval = 1000 / 60;

	std::string path = GetRunningDirectory() + resPath;

	g2d::Engine::Config ecfg;
	ecfg.nativeWindow = m_hWindow;
	ecfg.resourceFolderPath = path.c_str();

	return g2d::Engine::Initialize(ecfg);
}

int Framework::Start()
{
	FirstTick();
	return MainLoop();
}

void Framework::FirstTick()
{
	m_lastTimeStamp = timeGetTime();
	if (OnStart != nullptr)
	{
		OnStart();
	}

	if (OnMessage != nullptr)
	{
		OnMessageInternal = [&](const g2d::Message& message)
		{
			if (message.Event != g2d::MessageEvent::Invalid)
				OnMessage(message);
		};
	}
}

int Framework::MainLoop()
{
	MSG msg;
	bool needExit = false;

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
			if (!Tick())
			{
				::DestroyWindow(m_hWindow);
				m_hWindow = NULL;
				needExit = true;
			}
		}
	}

	DestroyApp();
	return (int)msg.wParam;
}

//return false表示关闭窗口退出。
bool Framework::Tick()
{
	auto deltaTime = timeGetTime() - m_lastTimeStamp;
	if (deltaTime > m_tickInterval)
	{
		m_elapsedTime += deltaTime;
		m_lastTimeStamp = timeGetTime();
		++m_frameCount;
		if (OnUpdate != nullptr)
		{
			return OnUpdate(deltaTime);
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}

void Framework::DestroyApp()
{
	if (OnFinish != nullptr)
	{
		OnFinish();
	}

	g2d::Engine::Uninitialize();
	m_initial = false;
}

void Framework::OnWindowResize(uint32_t width, uint32_t height)
{
	//要在初始化之后再做这件事情
	if (g2d::IsEngineInitialized())
	{
		g2d::GetEngine()->GetRenderSystem()->OnResize(width, height);
	}
}

void Framework::OnWindowMessage(uint32_t m, uint32_t wp, uint32_t lp)
{
	g2d::Message message = g2d::TranslateMessageFromWin32(m, wp, lp);
	OnMessageInternal(message);
}


