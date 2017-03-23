#pragma once
#include <g2dconfig.h>

namespace g2d
{
	enum class G2DAPI MessageEvent
	{
		Invalid,
		LostFocus,
		MouseMove,
		MouseButtonDown,
		MouseButtonUp,
		MouseButtonDoubleClick,
		KeyDown,
		KeyUp,
	};

	// �����豸���¼���Դ
	enum class MessageSource
	{
		None, Mouse, Keyboard
	};

	enum class MouseButton
	{
		Left, Middle, Right, None
	};

	constexpr int NumpadOffset = 0x1000;
	enum class KeyCode : int
	{
		Invalid = 0,

		// ���ܰ���
		Backspace, Control, Shift, Alt, Pause, Capital, Escape,
		PageUp, PageDown, Home, End, Insert, Delete,
		ArrowLeft, ArrowUp, ArrowRight, ArrowDown,
		Tab = '\t', Enter = '\n', Space = ' ',

		// F��
		F1 = 0x1101, F2 = 0x1102, F3 = 0x1103,
		F4 = 0x1104, F5 = 0x1105, F6 = 0x1106,
		F7 = 0x1107, F8 = 0x1108, F9 = 0x1109,
		F10 = 0x110A, F11 = 0x110B, F12 = 0x110C,

		// ���� equals to '0' to '9'
		Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
		Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',

		// С���� equals to 0x100+'0' to 0x100+'9'
		Numpad0 = '0' + NumpadOffset, Numpad1 = '1' + NumpadOffset, Numpad2 = '2' + NumpadOffset,
		Numpad3 = '3' + NumpadOffset, Numpad4 = '4' + NumpadOffset, Numpad5 = '5' + NumpadOffset,
		Numpad6 = '6' + NumpadOffset, Numpad7 = '7' + NumpadOffset, Numpad8 = '8' + NumpadOffset,
		Numpad9 = '9' + NumpadOffset, NumpadLock, 
		NumpadDecimal = '.' + NumpadOffset, NumpadEnter = '\n' + NumpadOffset,
		NumpadAdd = '+' + NumpadOffset, NumpadSub = '-' + NumpadOffset,
		NumpadMul = '*' + NumpadOffset, NumpadDiv = '/' + NumpadOffset,

		// ��Ҫ���� equals to 'A' to 'Z'
		KeyA = 'A', KeyB = 'B', KeyC = 'C', KeyD = 'D', KeyE = 'E', KeyF = 'F', KeyG = 'G',
		KeyH = 'H', KeyI = 'I', KeyJ = 'J', KeyK = 'K', KeyL = 'L', KeyM = 'M', KeyN = 'N',
		KeyO = 'O', KeyP = 'P', KeyQ = 'Q', KeyR = 'R', KeyS = 'S', KeyT = 'T',
		KeyU = 'U', KeyV = 'V', KeyW = 'W', KeyX = 'X', KeyY = 'Y', KeyZ = 'Z',

		// �����λ
		OMETilde = '`', OEMSub = '-', OEMAdd = '+', OEMLBracket = '[', OEMRBracket = ']', OEMRSlash = '\\',
		OEMColon = ';', OEMQuote = '\'', OEMComma = ',', OEMPeriod = '.', OEMSlash = '/',
	};

	class Keyboard
	{
	public:
		virtual bool IsPressing(KeyCode key) = 0;
	};

	struct G2DAPI Message
	{
		Message() = default;

		Message(const Message& other) = default;

		Message(MessageEvent ev, g2d::MouseButton btn, uint32_t x, uint32_t y)
			: Event(ev), Source(MessageSource::Mouse)
			, MouseButton(btn), MousePositionX(x), MousePositionY(y)
		{		}

		Message(MessageEvent ev, KeyCode key)
			: Event(ev), Source(MessageSource::Keyboard), Key(key)
		{		}

		const MessageEvent Event = MessageEvent::Invalid;

		const MessageSource Source = MessageSource::None;

		// ����¼���Ϣ
		const MouseButton MouseButton = MouseButton::None;

		const int MousePositionX = 0;

		const int MousePositionY = 0;

		// �����¼���Ϣ
		const KeyCode Key = KeyCode::Invalid;

	public:
		// ���������Ϣ�������ƷMessage
		// ����ӿ��е�2
		Message(MessageEvent ev, g2d::MouseButton btn)
			: Event(ev), Source(MessageSource::Mouse)
			, MouseButton(btn)
		{		}

		Message(MessageEvent ev, MessageSource src)
			: Event(ev), Source(src)
		{		}

		// ���������Ϣ����Message
		Message(const Message& m, int x, int y)
			: Event(m.Event), Source(MessageSource::Mouse)
			, MouseButton(m.MouseButton), MousePositionX(x), MousePositionY(y)
		{		}

		// ���ݼ�����Ϣ����Message
		Message(const Message& m, KeyCode key)
			: Event(m.Event), Source(MessageSource::Keyboard), Key(key)
		{		}
	};

	Message G2DAPI TranslateMessageFromWin32(uint32_t message, uint32_t wparam, uint32_t lparam);
}