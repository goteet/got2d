#pragma once
#include "g2dconfig.h"

namespace g2d
{
	enum class G2DAPI MessageEvent
	{
		Unknown,					// We dont care.
		LostFocus,					// Switch to other window.
		MouseMove,					// Cursor Position changes.
		MouseButtonDown,
		MouseButtonUp,
		MouseButtonDoubleClick,
		KeyDown,
		KeyUp,
	};

	// Input device types.
	enum class G2DAPI MessageSource
	{
		Mouse, 
		Keyboard, 
		None
	};

	// Mouse Button ID.
	enum class G2DAPI MouseButton : int
	{
		Left = 0,
		Middle = 1,
		Right = 2,
		None = 3
	};

	constexpr int NUMPAD_OFFSET = 0x1000;

	enum class G2DAPI KeyCode : int
	{
		Invalid = 0,

		// Function keys.
		Escape,
		Enter = '\n',
		Space = ' ',
		Tab = '\t',
		Control,
		Shift,
		Alt,
		Capital,
		Backspace,
		Pause,
		PageUp, PageDown, Home, End, Insert, Delete,
		ArrowLeft, ArrowUp, ArrowRight, ArrowDown,

		// F area.
		F1 = 0x1101, F2 = 0x1102, F3 = 0x1103,
		F4 = 0x1104, F5 = 0x1105, F6 = 0x1106,
		F7 = 0x1107, F8 = 0x1108, F9 = 0x1109,
		F10 = 0x110A, F11 = 0x110B, F12 = 0x110C,

		// Numbers, equals to '0' to '9'.
		Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
		Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',

		// Numpad area, equals to 0x100+'0' to 0x100+'9'.
		Numpad0 = '0' + NUMPAD_OFFSET, Numpad1 = '1' + NUMPAD_OFFSET, Numpad2 = '2' + NUMPAD_OFFSET,
		Numpad3 = '3' + NUMPAD_OFFSET, Numpad4 = '4' + NUMPAD_OFFSET, Numpad5 = '5' + NUMPAD_OFFSET,
		Numpad6 = '6' + NUMPAD_OFFSET, Numpad7 = '7' + NUMPAD_OFFSET, Numpad8 = '8' + NUMPAD_OFFSET,
		Numpad9 = '9' + NUMPAD_OFFSET, NumpadLock,
		NumpadDecimal = '.' + NUMPAD_OFFSET, NumpadEnter = '\n' + NUMPAD_OFFSET,
		NumpadAdd = '+' + NUMPAD_OFFSET, NumpadSub = '-' + NUMPAD_OFFSET,
		NumpadMul = '*' + NUMPAD_OFFSET, NumpadDiv = '/' + NUMPAD_OFFSET,

		// Characters, equals to 'A' to 'Z'.
		KeyA = 'A', KeyB = 'B', KeyC = 'C', KeyD = 'D', KeyE = 'E', KeyF = 'F', KeyG = 'G',
		KeyH = 'H', KeyI = 'I', KeyJ = 'J', KeyK = 'K', KeyL = 'L', KeyM = 'M', KeyN = 'N',
		KeyO = 'O', KeyP = 'P', KeyQ = 'Q', KeyR = 'R', KeyS = 'S', KeyT = 'T',
		KeyU = 'U', KeyV = 'V', KeyW = 'W', KeyX = 'X', KeyY = 'Y', KeyZ = 'Z',

		// Symbols.
		OMETilde = '`', OEMSub = '-', OEMAdd = '+', OEMLBracket = '[', OEMRBracket = ']', OEMRSlash = '\\',
		OEMColon = ';', OEMQuote = '\'', OEMComma = ',', OEMPeriod = '.', OEMSlash = '/',
	};

	// Message used in Engine.
	// Generally, only the Engine will generates virtual messages.
	struct G2DAPI Message
	{
		// Indicate what the message is.
		const MessageEvent Event = MessageEvent::Unknown;

		// Indicate where the message comes from.
		const MessageSource Source = MessageSource::None;

		// Cursor Infos.
		const MouseButton MouseButton = MouseButton::None;

		const int CursorPositionX = 0;

		const int CursorPositionY = 0;

		// Key Infos.
		const KeyCode Key = KeyCode::Invalid;

		Message() = default;

		Message(const Message& other) = default;

		// Build message of mouse event.
		Message(MessageEvent ev, g2d::MouseButton btn, unsigned int x, unsigned int y);

		// Build message of keyboard event.
		Message(MessageEvent ev, KeyCode key);

		// Build message of lost focus event.
		Message(MessageEvent ev, MessageSource src);

	public:// internal use
		// Build mouse of mouse event by another incomplete message.
		Message(MessageEvent ev, g2d::MouseButton btn);

		Message(const Message& m, int x, int y);

		// Build mouse of keyboard event by another incomplete message.
		Message(const Message& m, KeyCode key);
	};

	// Use this function to translate system messages to engine-used Messages.
	Message G2DAPI TranslateMessageWin32(unsigned int message, unsigned int wparam, unsigned int lparam);
}