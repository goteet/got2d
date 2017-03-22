#include "input.h"
#include <algorithm>
#include <windows.h>

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

Keyboard Keyboard::Instance;

Keyboard::~Keyboard()
{
	for (auto& keyState : m_states)
	{
		delete keyState.second;
	}
	m_states.clear();
}

bool Keyboard::IsPressing(g2d::KeyCode key) const
{
	return GetState(key).IsPressing();
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

void Mouse::OnMessage(const g2d::Message& message, uint32_t currentTimeStamp)
{

}

bool Mouse::IsPressing(g2d::MouseButton button) const
{
	return false;
}

const gml::coord& Mouse::CursorPosition() const
{
	return m_cursorPosition;
}