#pragma once
#include "g2dconfig.h"
#include "g2dmessage.h"

namespace g2d
{
	// Button state of Mouse/Keyboard
	enum class G2DAPI SwitchState : int
	{
		Releasing,		// not pressed
		JustPressed,	// just pressed down, without response
		Pressing,		// pressed for a while, will repeatly triggering
	};

	// Retrieve Keyboard states 
	struct G2DAPI Keyboard
	{
	public:
		virtual SwitchState GetPressState(KeyCode key) const = 0;

		// Repeating count will be increasing each frame
		virtual unsigned int GetRepeatingCount(KeyCode key) const = 0;

		// None of button is pressed/pressing
		virtual bool IsFree() const = 0;

		bool IsReleasing(KeyCode key) { return GetPressState(key) == SwitchState::Releasing; }

		bool IsPressing(KeyCode key) { return GetPressState(key) == SwitchState::Pressing; }

		bool IsJustPressed(KeyCode key) { return GetPressState(key) == SwitchState::JustPressed; }
	};

	// Retrieve Mouse states
	struct G2DAPI Mouse
	{
	public:
		// Current screen position of cursor
		virtual const cxx::int2& GetCursorPosition() const = 0;

		// Screen position of cursor when button was pressed just
		virtual const cxx::int2& GetCursorPressPosition(MouseButton button) const = 0;

		virtual SwitchState GetPressState(MouseButton button) const = 0;

		// Repeating count will be increasing each frame
		virtual unsigned int GetRepeatingCount(MouseButton button) const = 0;

		// None of button is pressed/pressing
		virtual bool IsFree() const = 0;

		bool IsReleasing(MouseButton button) { return GetPressState(button) == SwitchState::Releasing; }

		bool IsPressing(MouseButton button) { return GetPressState(button) == SwitchState::Pressing; }

		bool IsJustPressed(MouseButton button) { return GetPressState(button) == SwitchState::JustPressed; }
	};
}
