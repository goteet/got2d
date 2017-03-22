#pragma once
#include <gmlrect.h>
#include <g2dinput.h>
#include <functional>
#include <map>
#include <vector>

constexpr uint32_t PRESSING_INTERVAL = 700u;

//用户填充这个收听键盘消息
struct KeyEventReceiver
{
	void* UserData = nullptr;
	void(*Functor)(void* userData, g2d::KeyCode key) = nullptr;
	bool operator==(const KeyEventReceiver& other) const;
};

class KeyEventDelegate
{
public:
	void operator+=(const KeyEventReceiver&);
	void operator-=(const KeyEventReceiver&);
	void NotifyAll(g2d::KeyCode key) const;
private:
	std::vector<KeyEventReceiver> m_receivers;
};

class Keyboard : public g2d::Keyboard
{
public:
	virtual g2d::SwitchState PressState(g2d::KeyCode key) const override;

public:
	static Keyboard Instance;

	~Keyboard();

	void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);

	void Update(uint32_t currentTimeStamp);

	KeyEventDelegate OnPressing;
	KeyEventDelegate OnPress;

private:
	class KeyState
	{
		g2d::SwitchState pressState = g2d::SwitchState::Releasing;
		uint32_t pressTimeStamp;
	public:
		const g2d::KeyCode Key;
		KeyState(g2d::KeyCode key) : Key(key) { }
		g2d::SwitchState PressState() const { return pressState; }
		void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);
		void Update(uint32_t currentTimeStamp);
		void ForceRelease();
		std::function<void(KeyState&)> OnPressing = nullptr;
		std::function<void(KeyState&)> OnPress = nullptr;
	};
	KeyState& GetState(g2d::KeyCode key) const;
	void CreateKeyState(g2d::KeyCode key);
	bool VirtualKeyDown(uint32_t virtualKey);
	std::map<g2d::KeyCode, KeyState*> m_states;
};

class Mouse : public g2d::Mouse
{
public:
	static Mouse Instance;

	void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);

public:	//g2d::Mouse
	virtual g2d::SwitchState PressState(g2d::MouseButton button) const override;

	virtual const gml::coord& CursorPosition() const override;

private:
	gml::coord m_cursorPosition;
};

inline ::Keyboard& GetKeyboard()
{
	return ::Keyboard::Instance;
}

inline ::Mouse& GetMouse()
{
	return ::Mouse::Instance;
}