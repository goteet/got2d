#pragma once
#include <g2dinput.h>
#include <gmlrect.h>
#include <functional>
#include <map>
#include <vector>

constexpr uint32_t PRESSING_INTERVAL = 700u;

//�û�����������������Ϣ
struct KeyEventReceiver
{
	void* UserData = nullptr;
	void(*Functor)(void* userData, g2d::KeyCode Key) = nullptr;
};

class KeyEventDelegate
{
public:
	void operator+=(const KeyEventReceiver&);
	void operator-=(const KeyEventReceiver&);
	void SetPressing(g2d::KeyCode key);
	void SetPress(g2d::KeyCode key);

private:
	std::vector<KeyEventReceiver> m_listeners;
};

class Keyboard : public g2d::Keyboard
{
public:
	virtual bool IsPressing(g2d::KeyCode key) override;

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
		enum class State :int
		{
			Releasing, Pressed, Pressing
		};
		State pressState = State::Releasing;
		uint32_t pressTimeStamp;
	public:
		const g2d::KeyCode Key;
		KeyState(g2d::KeyCode key) : Key(key) { }
		bool IsPressing() { return pressState != State::Releasing; }
		void OnMessage(const g2d::Message& message, uint32_t currentTimeStamp);
		void Update(uint32_t currentTimeStamp);
		void ForceRelease();
		std::function<void(KeyState&)> OnPressing = nullptr;
		std::function<void(KeyState&)> OnPress = nullptr;
	};
	void OnKeyPressing(KeyState& state);
	void OnKeyPress(KeyState& state);
	KeyState& GetState(g2d::KeyCode key);
	bool VirtualKeyDown(uint32_t virtualKey);
	std::map<g2d::KeyCode, KeyState*> m_states;
};