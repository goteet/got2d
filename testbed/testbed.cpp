#include "stdafx.h"
#include "testbed.h"
#include <timeapi.h>
#include <time.h>
#include <g2dengine.h>
#include <g2drender.h>
const std::wstring& Testbed::GetWindowClassName()
{
	static const std::wstring name = L"got2d_window_class";
	return name;
}

const std::wstring& Testbed::GetWindowTitle()
{
	static const std::wstring title = L"got2d test bed";
	return title;
}

bool Testbed::InitApp()
{
	srand(static_cast<unsigned int>(time(0)));
	m_elapsedTime = 0;
	m_tickInterval = 1000 / 60;

	char path[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, path);
	std::string p = path;
	p += "/../extern/res/win32_test/";
	g2d::Engine::Config ecfg;
	ecfg.nativeWindow = hWnd;
	ecfg.resourceFolderPath = p.c_str();
	if (!g2d::Engine::Initialize(ecfg))
		return false;

	return true;
}

void Testbed::DestroyApp()
{
	if (OnFinish != nullptr)
	{
		OnFinish();
	}
	g2d::Engine::Uninitialize();
}

void Testbed::FirstTick()
{
	m_lastTimeStamp = timeGetTime();
	if (OnStart != nullptr)
	{
		OnStart();
	}
}

void Testbed::OnResize(long width, long height)
{
	//Ҫ�ڳ�ʼ��֮�������������
	if (g2d::IsEngineInitialized())
	{
		g2d::GetEngine()->GetRenderSystem()->OnResize(width, height);
	}
}

//return false��ʾ�رմ����˳���
bool Testbed::MainLoop()
{
	if (OnUpdate == nullptr)
		return false;

	auto elapseTime = timeGetTime() - m_lastTimeStamp;
	if (elapseTime > m_tickInterval)
	{
		m_elapsedTime += elapseTime;
		m_lastTimeStamp = timeGetTime();
		bool running = OnUpdate(elapseTime);
		m_frameCount++;
		return running;
	}
	else
	{
		return true;
	}
}

void Testbed::QuitApp()
{
	m_running = false;
}

unsigned long Testbed::GetFrameCount() const
{
	return m_frameCount;
}

unsigned long Testbed::GetElapsedTime() const
{
	return m_elapsedTime;
}