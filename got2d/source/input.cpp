#include "input.h"
#include <algorithm>

void KeyEventDelegate::operator+=(const KeyEventReceiver& listener)
{
	auto itFind = std::find_if(std::begin(m_listeners), std::end(m_listeners), [&](const KeyEventReceiver& l) {
		return (listener.UserData == l.UserData && listener.Functor == l.Functor);
	});

	if (itFind == std::end(m_listeners))
	{
		m_listeners.push_back(listener);
	}
}

void KeyEventDelegate::operator-=(const KeyEventReceiver& listener)
{
	auto itFind = std::find_if(std::begin(m_listeners), std::end(m_listeners), [&](const KeyEventReceiver& l) {
		return (listener.UserData == l.UserData && listener.Functor == l.Functor);
	});

	if (itFind != std::end(m_listeners))
	{
		m_listeners.erase(itFind);
	}
}

void KeyEventDelegate::SetPressing(g2d::KeyCode key)
{
	for (auto& listener : m_listeners)
	{
		listener.Functor(listener.UserData, key);
	}
}

void KeyEventDelegate::SetPress(g2d::KeyCode key)
{
	for (auto& listener : m_listeners)
	{
		listener.Functor(listener.UserData, key);
	}
}

Keyboard Keyboard::Instance;

Keyboard::~Keyboard()
{
	for (auto& keyState : m_states)
	{
		delete keyState.second;
	}
	m_states.clear();
}

bool Keyboard::IsPressing(g2d::KeyCode key)
{
	return GetState(key).IsPressing();
}

void Keyboard::OnKeyPressing(KeyState & state)
{
	OnPressing.SetPressing(state.Key);
}

void Keyboard::OnKeyPress(KeyState & state)
{
	OnPress.SetPress(state.Key);
}

Keyboard::KeyState& Keyboard::GetState(g2d::KeyCode key)
{
	if (m_states.count(key) == 0)
	{
		m_states.insert({ key, new KeyState(key) });

		using namespace std::placeholders;
		m_states[key]->OnPressing = [&](KeyState& state) { OnKeyPressing(state); };
		m_states[key]->OnPress = [&](KeyState& state) { OnKeyPress(state); };
	}
	return *(m_states[key]);
}

void Keyboard::OnMessage(const g2d::Message& message, uint32_t currentTimeStamp)
{
	if (message.Source == g2d::MessageSource::Keyboard)
	{
		GetState(message.Key).OnMessage(message, currentTimeStamp);
	}
	else if (message.Event == g2d::MessageEvent::LostFocus)
	{
		for (auto& keyState : m_states)
		{
			keyState.second->ForceRelease();
		}
	}
}

#include <windows.h>
void Keyboard::Update(uint32_t currentTimeStamp)
{
	auto ALTKey = VK_MENU;
	auto ALTDown = VirtualKeyDown(ALTKey);
	auto& ALTState = GetState(ALTKey);
	if (ALTDown && !ALTState.IsPressing())
	{
		ALTState.OnMessage(g2d::Message(g2d::MessageEvent::KeyDown, ALTKey), currentTimeStamp);
	}
	else if (!ALTDown && ALTState.IsPressing())
	{
		ALTState.OnMessage(g2d::Message(g2d::MessageEvent::KeyUp, ALTKey), currentTimeStamp);
	}
	for (auto& keyState : m_states)
	{
		keyState.second->Update(currentTimeStamp);
	}
}

void Keyboard::KeyState::OnMessage(const g2d::Message& message, uint32_t currentTimeStamp)
{
	if (message.Event == g2d::MessageEvent::KeyDown)
	{
		if (pressState != State::Releasing)
		{
			//�쳣״̬
		}

		pressState = State::Pressed;
		pressTimeStamp = currentTimeStamp;
	}
	else if (message.Event == g2d::MessageEvent::KeyUp)
	{
		if (pressState == State::Pressed)
		{
			OnPress(*this);
		}
		else if (pressState == State::Releasing)
		{
			//�쳣״̬
		}
		pressState = State::Releasing;
	}
}

void Keyboard::KeyState::Update(uint32_t currentTimeStamp)
{
	if (pressState == State::Pressed && (currentTimeStamp - pressTimeStamp) > PRESSING_INTERVAL)
	{
		pressState = State::Pressing;
	}
	if (pressState == State::Pressing)
	{
		OnPressing(*this);
	}
}

inline void Keyboard::KeyState::ForceRelease()
{
	if (pressState == State::Pressed)
	{
		OnPress(*this);
	}
	pressState = State::Releasing;
}

bool Keyboard::VirtualKeyDown(uint32_t virtualKey)
{
	return (HIBYTE(GetKeyState(virtualKey)) & 0x80) != 0;
}
