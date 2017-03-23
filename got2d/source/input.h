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

struct MouseEventReceiver
{
	void* UserData = nullptr;
	void(*Functor)(void* userData, g2d::MouseButton button) = nullptr;
	bool operator==(const MouseEventReceiver& other) const;
};

template<typename RECEIVER> class EventDelegate
{
	std::vector<RECEIVER> receivers;
public:
	void operator+=(const RECEIVER& receiver)
	{
		auto itFind = std::find(std::begin(receivers), std::end(receivers), receiver);
		if (itFind == std::end(receivers)) receivers.push_back(receiver);
	}

	void operator-=(const RECEIVER& receiver)
	{
		auto itFind = std::find(std::begin(receivers), std::end(receivers), receiver);
		if (itFind != std::end(receivers)) receivers.erase(itFind);
	}

	template<typename TFUNC> void Traversal(TFUNC func)
	{
		for (auto& r : receivers) func(r);
	}
};

struct KeyEvent : public EventDelegate<KeyEventReceiver>
{
	void NotifyAll(g2d::KeyCode key);
};

struct MouseEvent : public EventDelegate<MouseEventReceiver>
{
	void NotifyAll(g2d::MouseButton button);
};

class Keyboard : public g2d::Keyboard
{
public:
	virtual g2d::SwitchState GetPressState(g2d::KeyCode key) const override;

	virtual uint32_t GetRepeatingCount(g2d::KeyCode key) const override;

public:
	static Keyboard Instance;

	~Keyboard();

	void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);

	void Update(uint32_t currentTimeStamp);

	KeyEvent OnPress;
	KeyEvent OnPressingBegin;
	KeyEvent OnPressing;
	KeyEvent OnPressingEnd;

private:
	class KeyState
	{
		uint32_t repeatCount = 0;
		uint32_t pressTimeStamp;
		g2d::SwitchState state = g2d::SwitchState::Releasing;
	public:
		const g2d::KeyCode Key;

		g2d::SwitchState State() const { return state; }

		uint32_t RepeatingCount() const { return repeatCount; }

		KeyState(g2d::KeyCode key) : Key(key) { }
		void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);
		void Update(uint32_t currentTimeStamp);
		void ForceRelease();
		std::function<void(KeyState&)> OnPress = nullptr;
		std::function<void(KeyState&)> OnPressingBegin = nullptr;
		std::function<void(KeyState&)> OnPressing = nullptr;
		std::function<void(KeyState&)> OnPressingEnd = nullptr;

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

	Mouse();

	void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);

	void Update(uint32_t currentTimeStamp);

public:	//g2d::Mouse
	virtual const gml::coord& GetCursorPosition() const override;

	virtual const gml::coord& GetCursorPressPosition(g2d::MouseButton button) const override;

	virtual g2d::SwitchState GetPressState(g2d::MouseButton button) const override;

	uint32_t GetRepeatingCount(g2d::MouseButton button) const override;

	MouseEvent OnPress;
	MouseEvent OnPressingBegin;
	MouseEvent OnPressing;
	MouseEvent OnPressingEnd;
	MouseEvent OnMoving;
private:
	class ButtonState
	{
		bool repeated = false;
		uint32_t repeatCount = 0;
		uint32_t pressTimeStamp;
		gml::coord pressCursorPos;
		g2d::SwitchState state = g2d::SwitchState::Releasing;
	public:
		const g2d::MouseButton Button;

		g2d::SwitchState State() const { return state; }

		const gml::coord& CursorPressPos() const { return pressCursorPos; }

		uint32_t RepeatingCount() const { return repeatCount; }

		ButtonState(g2d::MouseButton btn) : Button(btn) { }
		void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);
		void Update(uint32_t currentTimeStamp);
		void ForceRelease();
		std::function<void(ButtonState&)> OnPress = nullptr;
		std::function<void(ButtonState&)> OnPressingBegin = nullptr;
		std::function<void(ButtonState&)> OnPressing = nullptr;
		std::function<void(ButtonState&)> OnPressingEnd = nullptr;
	} m_buttons[3];

	ButtonState& GetButton(g2d::MouseButton& button);

	const ButtonState& GetButton(g2d::MouseButton& button) const;

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