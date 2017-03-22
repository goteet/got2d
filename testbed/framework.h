#pragma once
#include <gmlrect.h>
#include <g2dinput.h>
#include <functional>
#include <string>
#include <cinttypes>
#include <windows.h>

namespace g2d
{
	class Mesh;
	class Scene;
}

class AutoWinClassRegister
{
	static int s_initialCount;
public:
	static const std::wstring name;
	const HINSTANCE instance;
	AutoWinClassRegister(HINSTANCE hInstance);
	~AutoWinClassRegister();
};

class Framework
{
public:
	std::function<void()> OnStart = nullptr;
	std::function<void()> OnFinish = nullptr;
	std::function<bool(uint32_t)> OnUpdate = nullptr;
	std::function<void(const g2d::Message&)> OnMessage = nullptr;

public:
	Framework(HINSTANCE inst);
	~Framework();
	bool Initial(int nCmdShow, const std::string& resPath);
	int Start();
	void QuitApp();
	void SetCursorPos(const gml::coord&);
	gml::coord GetCursorPos();
	const std::wstring& GetWindowTitle();
	uint32_t GetFrameCount() const;
	uint32_t GetElapsedTime() const;

private:
	bool CreateRenderingWindow(int nCmdShow);
	bool IntializeEngine(const std::string& resPath);
	void FirstTick();
	int MainLoop();
	bool Tick(); //return false表示关闭窗口退出。
	void DestroyApp();

	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnWindowResize(uint32_t width, uint32_t height);
	void OnWindowMessage(uint32_t m, uint32_t wp, uint32_t lp);

	AutoWinClassRegister m_autoClassRegister;
	HWND m_hWindow = NULL;
	uint32_t m_frameCount = 0;
	uint32_t m_elapsedTime = 0;
	uint32_t m_lastTimeStamp;
	uint32_t m_tickInterval;
	bool m_running = true;
	bool m_initial = false;
	std::function<void(const g2d::Message& message)> OnMessageInternal;
};