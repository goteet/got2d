#include "input.h"
#include <algorithm>
#include <windows.h>

Keyboard Keyboard::Instance;

Mouse Mouse::Instance;

bool KeyEventReceiver::operator==(const KeyEventReceiver& other) const
{
	return (UserData == other.UserData && Functor == other.Functor);
}

void KeyEventDelegate::operator+=(const KeyEventReceiver& receiver)
{
	auto itFind = std::find(std::begin(m_receivers), std::end(m_receivers), receiver);

	if (itFind == std::end(m_receivers))
	{
		m_receivers.push_back(receiver);
	}
}

void KeyEventDelegate::operator-=(const KeyEventReceiver& receiver)
{
	auto itFind = std::find(std::begin(m_receivers), std::end(m_receivers), receiver);
	if (itFind != std::end(m_receivers))
	{
		m_receivers.erase(itFind);
	}
}

void KeyEventDelegate::NotifyAll(g2d::KeyCode key) const
{
	for (auto& listener : m_receivers)
	{
		listener.Functor(listener.UserData, key);
	}
}

Keyboard::~Keyboard()
{
	for (auto& keyState : m_states)
	{
		delete keyState.second;
	}
	m_states.clear();
}

g2d::SwitchState Keyboard::PressState(g2d::KeyCode key) const
{
	return GetState(key).State();
}

void Keyboard::CreateKeyState(g2d::KeyCode key)
{
	m_states.insert({ key, new KeyState(key) });
	m_states[key]->OnPressing = [this](KeyState& state) { this->OnPressing.NotifyAll(state.Key); };
	m_states[key]->OnPress = [this](KeyState& state) { this->OnPress.NotifyAll(state.Key); };
}

Keyboard::KeyState& Keyboard::GetState(g2d::KeyCode key) const
{
	if (m_states.count(key) == 0)
	{
		const_cast<Keyboard*>(this)->CreateKeyState(key);
	}
	return *(m_states.at(key));
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

void Keyboard::Update(uint32_t currentTimeStamp)
{
	auto ALTKey = g2d::KeyCode::Alt;
	auto ALTDown = VirtualKeyDown(VK_MENU);
	auto& ALTState = GetState(ALTKey);
	bool ALTPressing = ALTState.State() != g2d::SwitchState::Releasing;
	if (ALTDown && !ALTPressing)
	{
		ALTState.OnMessage(g2d::Message(g2d::MessageEvent::KeyDown, ALTKey), currentTimeStamp);
	}
	else if (!ALTDown && ALTPressing)
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
		if (state != g2d::SwitchState::Releasing)
		{
			//�쳣״̬
		}

		state = g2d::SwitchState::JustPressed;
		pressTimeStamp = currentTimeStamp;
	}
	else if (message.Event == g2d::MessageEvent::KeyUp)
	{
		if (state == g2d::SwitchState::JustPressed)
		{
			OnPress(*this);
		}
		else if (state == g2d::SwitchState::Releasing)
		{
			//�쳣״̬
		}
		state = g2d::SwitchState::Releasing;
	}
}

void Keyboard::KeyState::Update(uint32_t currentTimeStamp)
{
	if (state == g2d::SwitchState::JustPressed && (currentTimeStamp - pressTimeStamp) > PRESSING_INTERVAL)
	{
		state = g2d::SwitchState::Pressing;
	}
	if (state == g2d::SwitchState::Pressing)
	{
		OnPressing(*this);
	}
}

inline void Keyboard::KeyState::ForceRelease()
{
	if (state == g2d::SwitchState::JustPressed)
	{
		OnPress(*this);
	}
	state = g2d::SwitchState::Releasing;
}

bool Keyboard::VirtualKeyDown(uint32_t virtualKey)
{
	return (HIBYTE(GetKeyState(virtualKey)) & 0x80) != 0;
}

Mouse::Mouse()
	: m_buttons{ g2d::MouseButton::Left, g2d::MouseButton::Right, g2d::MouseButton::Middle }
{

}

void Mouse::OnMessage(const g2d::Message& message, uint32_t currentTimeStamp)
{
	if (message.Source == g2d::MessageSource::Mouse)
	{
		for (auto& button : m_buttons)
		{
			button.OnMessage(message, currentTimeStamp);
		}
		m_cursorPosition.set(message.CursorPositionX, message.CursorPositionY);
	}
	else if (message.Event == g2d::MessageEvent::LostFocus)
	{
		for (auto& button : m_buttons)
		{
			//button.ForceRelease();
		}
	}
}

void Mouse::Update(uint32_t currentTimeStamp)
{
	for (auto& button : m_buttons)
	{
		button.Update(currentTimeStamp);
	}
}

g2d::SwitchState Mouse::PressState(g2d::MouseButton button) const
{
	return g2d::SwitchState::Releasing;
}

const gml::coord& Mouse::CursorPosition() const
{
	return m_cursorPosition;
}

void Mouse::ButtonState::OnMessage(const g2d::Message& message, uint32_t currentTimeStamp)
{

}

void Mouse::ButtonState::Update(uint32_t currentTimeStamp)
{

}