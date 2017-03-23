#pragma once
#include <g2dconfig.h>

namespace g2d
{
	// ��Ϣ����
	enum class G2DAPI MessageEvent : int
	{
		Invalid,					// δ֪��Ϣ
		LostFocus,					// �л����ڵ�ʱ��
		MouseMove,					// ����ƶ�
		MouseButtonDown,			// ��갴ť������
		MouseButtonUp,				// ��갴ť�ͷ�
		MouseButtonDoubleClick,		// ��갴ť˫��
		KeyDown,					// ���̰�ť����
		KeyUp,						// ���̰�ť�ͷ�
	};


	// �����豸���¼���Դ
	enum class G2DAPI MessageSource : int
	{
		Mouse, Keyboard, None
	};

	// ��갴����ʶ
	enum class G2DAPI MouseButton : int
	{
		Left = 0, Middle = 1, Right = 2, None = 3
	};

	constexpr int NUMPAD_OFFSET = 0x1000;

	enum class G2DAPI KeyCode : int
	{
		Invalid = 0,

		// ���ܰ���
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

		// F��
		F1 = 0x1101, F2 = 0x1102, F3 = 0x1103,
		F4 = 0x1104, F5 = 0x1105, F6 = 0x1106,
		F7 = 0x1107, F8 = 0x1108, F9 = 0x1109,
		F10 = 0x110A, F11 = 0x110B, F12 = 0x110C,

		// ���� equals to '0' to '9'
		Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
		Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',

		// С���� equals to 0x100+'0' to 0x100+'9'
		Numpad0 = '0' + NUMPAD_OFFSET, Numpad1 = '1' + NUMPAD_OFFSET, Numpad2 = '2' + NUMPAD_OFFSET,
		Numpad3 = '3' + NUMPAD_OFFSET, Numpad4 = '4' + NUMPAD_OFFSET, Numpad5 = '5' + NUMPAD_OFFSET,
		Numpad6 = '6' + NUMPAD_OFFSET, Numpad7 = '7' + NUMPAD_OFFSET, Numpad8 = '8' + NUMPAD_OFFSET,
		Numpad9 = '9' + NUMPAD_OFFSET, NumpadLock,
		NumpadDecimal = '.' + NUMPAD_OFFSET, NumpadEnter = '\n' + NUMPAD_OFFSET,
		NumpadAdd = '+' + NUMPAD_OFFSET, NumpadSub = '-' + NUMPAD_OFFSET,
		NumpadMul = '*' + NUMPAD_OFFSET, NumpadDiv = '/' + NUMPAD_OFFSET,

		// ��Ҫ���� equals to 'A' to 'Z'
		KeyA = 'A', KeyB = 'B', KeyC = 'C', KeyD = 'D', KeyE = 'E', KeyF = 'F', KeyG = 'G',
		KeyH = 'H', KeyI = 'I', KeyJ = 'J', KeyK = 'K', KeyL = 'L', KeyM = 'M', KeyN = 'N',
		KeyO = 'O', KeyP = 'P', KeyQ = 'Q', KeyR = 'R', KeyS = 'S', KeyT = 'T',
		KeyU = 'U', KeyV = 'V', KeyW = 'W', KeyX = 'X', KeyY = 'Y', KeyZ = 'Z',

		// �����λ
		OMETilde = '`', OEMSub = '-', OEMAdd = '+', OEMLBracket = '[', OEMRBracket = ']', OEMRSlash = '\\',
		OEMColon = ';', OEMQuote = '\'', OEMComma = ',', OEMPeriod = '.', OEMSlash = '/',
	};

	// ϵͳʹ�õ��¼�����ʹ��TranslateMessage����ϵͳ�¼�
	// һ��ֻ�������ڲ��ṹ��������Ϣ
	class G2DAPI Message
	{
	public:
		const MessageEvent Event = MessageEvent::Invalid;

		const MessageSource Source = MessageSource::None;

		// ����¼���Ϣ
		const MouseButton MouseButton = MouseButton::None;

		const int CursorPositionX = 0;

		const int CursorPositionY = 0;

		// �����¼���Ϣ
		const KeyCode Key = KeyCode::Invalid;

	public:
		Message() = default;

		Message(const Message& other) = default;

		// ���������Ϣ
		Message(MessageEvent ev, g2d::MouseButton btn, uint32_t x, uint32_t y);

		// ����������Ϣ
		Message(MessageEvent ev, KeyCode key);

		// ����LostFocus��Ϣ
		Message(MessageEvent ev, MessageSource src);

	public:
		// ���½ӿ��е�������ڲ�ʹ��
		// �������������ϢMessage
		Message(MessageEvent ev, g2d::MouseButton btn);

		Message(const Message& m, int x, int y);

		// ���ݼ�����Ϣ����Message
		Message(const Message& m, KeyCode key);
	};

	Message G2DAPI TranslateMessageWin32(uint32_t message, uint32_t wparam, uint32_t lparam);
}