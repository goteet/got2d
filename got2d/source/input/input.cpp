#include <algorithm>
#include "input.h"

bool KeyEventReceiver::operator==(const KeyEventReceiver& other) const
{
	return (UserData == other.UserData && Functor == other.Functor);
}

bool MouseEventReceiver::operator==(const MouseEventReceiver& other) const
{
	return (UserData == other.UserData && Functor == other.Functor);
}

void KeyEvent::NotifyAll(g2d::KeyCode key)
{
	Traversal([&](const KeyEventReceiver&receiver)
	{
		receiver.Functor(receiver.UserData, key);
	});
}

void MouseEvent::NotifyAll(g2d::MouseButton button)
{
	Traversal([&](const MouseEventReceiver&receiver)
	{
		receiver.Functor(receiver.UserData, button);
	});
}


Keyboard::~Keyboard()
{
	for (auto& keyState : mStates)
	{
		delete keyState.second;
	}
	mStates.clear();
}

g2d::SwitchState Keyboard::GetPressState(g2d::KeyCode key) const
{
	return GetState(key).State();
}

unsigned int Keyboard::GetRepeatingCount(g2d::KeyCode key) const
{
	return GetState(key).RepeatingCount();
}

bool Keyboard::IsFree() const
{
	for (auto& key : mStates)
	{
		if (key.second->State() != g2d::SwitchState::Releasing)
			return false;
	}
	return true;
}

void Keyboard::CreateKeyState(g2d::KeyCode key)
{
	mStates.insert({ key, new KeyState(key) });
	mStates[key]->OnPress = [this](KeyState& state) { this->OnPress.NotifyAll(state.Key); };
	mStates[key]->OnPressingBegin = [this](KeyState& state) { this->OnPressingBegin.NotifyAll(state.Key); };
	mStates[key]->OnPressing = [this](KeyState& state) { this->OnPressing.NotifyAll(state.Key); };
	mStates[key]->OnPressingEnd = [this](KeyState& state) { this->OnPressingEnd.NotifyAll(state.Key); };

}

Keyboard::KeyState& Keyboard::GetState(g2d::KeyCode key) const
{
	if (mStates.count(key) == 0)
	{
		const_cast<Keyboard*>(this)->CreateKeyState(key);
	}
	return *(mStates.at(key));
}

void Keyboard::OnMessage(const g2d::Message& message, unsigned int currentTimeStamp)
{
	if (message.Source == g2d::MessageSource::Keyboard)
	{
		GetState(message.Key).OnMessage(message, currentTimeStamp);
	}
	else if (message.Event == g2d::MessageEvent::LostFocus)
	{
		for (auto& keyState : mStates)
		{
			keyState.second->ForceRelease();
		}
	}
}

void Keyboard::Update(unsigned int currentTimeStamp)
{
	auto ALTKey = g2d::KeyCode::Alt;
	auto ALTDown = AltDownWin32();
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
	for (auto& keyState : mStates)
	{
		keyState.second->Update(currentTimeStamp);
	}
}

void Keyboard::KeyState::OnMessage(const g2d::Message& message, unsigned int currentTimeStamp)
{
	if (message.Event == g2d::MessageEvent::KeyDown)
	{
		// keydown will send each frame
		// we will simulate pressing event in update
		// so that mainloop wont stuck
		if (state == g2d::SwitchState::Releasing)
		{
			state = g2d::SwitchState::JustPressed;
			pressTimeStamp = currentTimeStamp;
		}
	}
	else if (message.Event == g2d::MessageEvent::KeyUp)
	{
		if (state == g2d::SwitchState::JustPressed)
		{
			OnPress(*this);
		}
		else if (state == g2d::SwitchState::Pressing)
		{
			OnPressingEnd(*this);
			repeatCount = 0;
		}
		else
		{
			// unexpected state
		}
		state = g2d::SwitchState::Releasing;
	}
}

void Keyboard::KeyState::Update(unsigned int currentTimeStamp)
{
	if (state == g2d::SwitchState::JustPressed && (currentTimeStamp - pressTimeStamp) > PRESSING_INTERVAL)
	{
		state = g2d::SwitchState::Pressing;
		OnPressingBegin(*this);
		repeatCount = 1;
	}
	if (state == g2d::SwitchState::Pressing)
	{
		OnPressing(*this);
		repeatCount++;
	}
}

inline void Keyboard::KeyState::ForceRelease()
{
	if (state == g2d::SwitchState::JustPressed)
	{
		OnPress(*this);
	}
	else if (state == g2d::SwitchState::Pressing)
	{
		OnPressingEnd(*this);
		repeatCount = 0;
	}
	state = g2d::SwitchState::Releasing;
}

Mouse::Mouse()
	: mButtons{ g2d::MouseButton::Left, g2d::MouseButton::Middle, g2d::MouseButton::Right, }
{
	for (auto& button : mButtons)
	{
		button.OnPress = [this](ButtonState& state) { this->OnPress.NotifyAll(state.Button); };
		button.OnPressingBegin = [this](ButtonState& state) { this->OnPressingBegin.NotifyAll(state.Button); };
		button.OnPressing = [this](ButtonState& state) { this->OnPressing.NotifyAll(state.Button); };
		button.OnPressingEnd = [this](ButtonState& state) { this->OnPressingEnd.NotifyAll(state.Button); };
	}
}

void Mouse::OnMessage(const g2d::Message& message, unsigned int currentTimeStamp)
{
	if (message.Source == g2d::MessageSource::Mouse)
	{
		if (message.Event == g2d::MessageEvent::MouseMove)
		{
			mCursorPosition = cxx::int2(message.CursorPositionX, message.CursorPositionY);
			for (auto& button : mButtons)
			{
				button.BeginDrag();
			}
			this->OnMoving.NotifyAll(g2d::MouseButton::None);
		}
		else
		{
			if (mCursorPosition.x != message.CursorPositionX ||
				mCursorPosition.y != message.CursorPositionY)
			{
				mCursorPosition = cxx::int2(message.CursorPositionX, message.CursorPositionY);
				this->OnMoving.NotifyAll(g2d::MouseButton::None);
			}
			if (message.Event == g2d::MessageEvent::MouseButtonDoubleClick)
			{
				OnDoubleClick.NotifyAll(message.MouseButton);
			}
			else
			{
				for (auto& button : mButtons)
				{
					if (message.MouseButton == button.Button)
						button.OnMessage(message, currentTimeStamp);
				}
			}
		}
	}
	else if (message.Event == g2d::MessageEvent::LostFocus)
	{
		for (auto& button : mButtons)
		{
			button.ForceRelease();
		}
	}
}

void Mouse::Update(unsigned int currentTimeStamp)
{
	for (auto& button : mButtons)
	{
		button.Update(currentTimeStamp);
	}
}

const cxx::int2& Mouse::GetCursorPosition() const
{
	return mCursorPosition;
}

const cxx::int2& Mouse::GetCursorPressPosition(g2d::MouseButton button) const
{
	if (button == g2d::MouseButton::None)
		return GetCursorPosition();
	else
		return GetButton(button).CursorPressPos();
}


g2d::SwitchState Mouse::GetPressState(g2d::MouseButton button) const
{
	if (button == g2d::MouseButton::None)
		return g2d::SwitchState::Releasing;
	else
		return GetButton(button).State();
}

unsigned int Mouse::GetRepeatingCount(g2d::MouseButton button) const
{
	if (button == g2d::MouseButton::None)
		return 0;
	else
		return GetButton(button).RepeatingCount();
}

bool Mouse::IsFree() const
{
	for (auto& button : mButtons)
	{
		if (button.State() != g2d::SwitchState::Releasing)
			return false;
	}
	return true;
}


const Mouse::ButtonState& Mouse::GetButton(g2d::MouseButton& button) const
{
	return mButtons[(int)button];
}

Mouse::ButtonState& Mouse::GetButton(g2d::MouseButton& button)
{
	return mButtons[(int)button];
}

void Mouse::ButtonState::BeginDrag()
{
	if (state == g2d::SwitchState::JustPressed)
	{
		state = g2d::SwitchState::Pressing;
		OnPressingBegin(*this);
		repeatCount = 1;
		repeated = true;
	}
}

void Mouse::ButtonState::OnMessage(const g2d::Message& message, unsigned int currentTimeStamp)
{
	if (message.Event == g2d::MessageEvent::MouseButtonDown)
	{
		if (state != g2d::SwitchState::JustPressed)
		{
			// unexpected state
		}
		else if (state != g2d::SwitchState::Pressing)
		{
			// unexpected state
			OnPressingEnd(*this);
			repeated = true;
			repeatCount = 0;
		}

		state = g2d::SwitchState::JustPressed;
		pressTimeStamp = currentTimeStamp;
	}
	else if (message.Event == g2d::MessageEvent::MouseButtonUp)
	{
		if (state == g2d::SwitchState::JustPressed)
		{
			OnPress(*this);
		}
		else if (state == g2d::SwitchState::Pressing)
		{
			OnPressingEnd(*this);
			repeated = true;
			repeatCount = 0;
		}
		else
		{
			// unexpected state
		}
		state = g2d::SwitchState::Releasing;
	}
}

void Mouse::ButtonState::Update(unsigned int currentTimeStamp)
{
	if (state == g2d::SwitchState::JustPressed && (currentTimeStamp - pressTimeStamp) > PRESSING_INTERVAL)
	{
		state = g2d::SwitchState::Pressing;
		OnPressingBegin(*this);
		repeatCount = 1;
		repeated = true;
	}
	if (state == g2d::SwitchState::Pressing && !repeated)
	{
		OnPressing(*this);
		repeatCount++;
		repeated = true;
	}
	repeated = false;
}

void Mouse::ButtonState::ForceRelease()
{
	if (state == g2d::SwitchState::JustPressed)
	{
		OnPress(*this);
	}
	else if (state == g2d::SwitchState::Pressing)
	{
		OnPressingEnd(*this);
		repeatCount = 0;
	}
	state = g2d::SwitchState::Releasing;
}
