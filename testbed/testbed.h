#pragma once
#include <string>
#include <windows.h>

extern HINSTANCE hInst;
extern HWND hWnd;

namespace g2d
{
	class Mesh;
	class Scene;
}

class Testbed
{
public:
	const std::wstring& GetWindowClassName();
	const std::wstring& GetWindowTitle();

	bool InitApp();
	void DestroyApp();
	void FirstTick();
	void OnResize(long width, long height);
	bool MainLoop();//return false��ʾ�رմ����˳���

protected:
	void QuitApp();
	unsigned long GetFrameCount() const;
	unsigned long GetElapsedTime() const;


	virtual void Start();
	virtual void End();
	virtual bool Update(unsigned long);
	
private:
	unsigned long m_frameCount = 0;
	unsigned long m_elapsedTime = 0;
	unsigned long m_lastTimeStamp;
	unsigned long m_tickInterval;
	bool m_running = true;

	g2d::Scene* mainScene = nullptr;
};