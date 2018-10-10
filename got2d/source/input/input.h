#pragma once
#include <functional>
#include <vector>
#include <map>
#include "cxx_math/cxx_aabb.h"
#include "g2dinput.h"

constexpr unsigned int PRESSING_INTERVAL = 500u;

// fill this structure, to listen keyboard event
struct KeyEventReceiver
{
	void* UserData = nullptr;
	void(*Functor)(void* userData, g2d::KeyCode key) = nullptr;
	bool operator==(const KeyEventReceiver& other) const;
};

// fill this structure, to listen mouse event
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
		auto itEnd = receivers.end();
		if (itEnd == std::find(receivers.begin(), itEnd, receiver))
			receivers.push_back(receiver);
	}

	void operator-=(const RECEIVER& receiver)
	{
		auto itEnd = receivers.end();
		auto itFound = std::find(receivers.begin(), itEnd, receiver);
		if (itEnd != itFound) receivers.erase(itFound);
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

	virtual unsigned int GetRepeatingCount(g2d::KeyCode key) const override;

	virtual bool IsFree() const override;

public:
	static Keyboard Instance;

	~Keyboard();

	void OnMessage(const g2d::Message& message, unsigned int currentTimeStamp);

	void Update(unsigned int currentTimeStamp);

	KeyEvent OnPress;
	KeyEvent OnPressingBegin;
	KeyEvent OnPressing;
	KeyEvent OnPressingEnd;

private:
	class KeyState
	{
		unsigned int repeatCount = 0;
		unsigned int pressTimeStamp;
		g2d::SwitchState state = g2d::SwitchState::Releasing;
	public:
		const g2d::KeyCode Key;

		g2d::SwitchState State() const { return state; }

		unsigned int RepeatingCount() const { return repeatCount; }

		KeyState(g2d::KeyCode key) : Key(key) { }
		void OnMessage(const g2d::Message& message, unsigned int currentTimeStamp);
		void Update(unsigned int currentTimeStamp);
		void ForceRelease();
		std::function<void(KeyState&)> OnPress = nullptr;
		std::function<void(KeyState&)> OnPressingBegin = nullptr;
		std::function<void(KeyState&)> OnPressing = nullptr;
		std::function<void(KeyState&)> OnPressingEnd = nullptr;

	};
	KeyState& GetState(g2d::KeyCode key) const;
	void CreateKeyState(g2d::KeyCode key);
	std::map<g2d::KeyCode, KeyState*> mStates;
};

class Mouse : public g2d::Mouse
{
public:
	static Mouse Instance;

	Mouse();

	void OnMessage(const g2d::Message& message, unsigned int currentTimeStamp);

	void Update(unsigned int currentTimeStamp);

public:	//g2d::Mouse
	virtual const cxx::int2& GetCursorPosition() const override;

	virtual const cxx::int2& GetCursorPressPosition(g2d::MouseButton button) const override;

	virtual g2d::SwitchState GetPressState(g2d::MouseButton button) const override;

	virtual unsigned int GetRepeatingCount(g2d::MouseButton button) const override;

	virtual bool IsFree() const override;

	MouseEvent OnPress;
	MouseEvent OnPressingBegin;
	MouseEvent OnPressing;
	MouseEvent OnPressingEnd;
	MouseEvent OnMoving;
	MouseEvent OnDoubleClick;

private:
	class ButtonState
	{
		bool repeated = false;
		unsigned int repeatCount = 0;
		unsigned int pressTimeStamp;
		cxx::int2 pressCursorPos;
		g2d::SwitchState state = g2d::SwitchState::Releasing;
	public:
		const g2d::MouseButton Button;

		g2d::SwitchState State() const { return state; }

		const cxx::int2& CursorPressPos() const { return pressCursorPos; }

		unsigned int RepeatingCount() const { return repeatCount; }

		ButtonState(g2d::MouseButton btn) : Button(btn) { }
		void OnMessage(const g2d::Message& message, unsigned int currentTimeStamp);
		void BeginDrag();
		void Update(unsigned int currentTimeStamp);
		void ForceRelease();
		std::function<void(ButtonState&)> OnPress = nullptr;
		std::function<void(ButtonState&)> OnPressingBegin = nullptr;
		std::function<void(ButtonState&)> OnPressing = nullptr;
		std::function<void(ButtonState&)> OnPressingEnd = nullptr;
	} mButtons[3];

	ButtonState& GetButton(g2d::MouseButton& button);

	const ButtonState& GetButton(g2d::MouseButton& button) const;

	cxx::int2 mCursorPosition;
};

bool AltDownWin32();