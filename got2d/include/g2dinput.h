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

	typedef int KeyCode;

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
			, MouseButton(btn) , MousePositionX(x), MousePositionY(y)
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
		const KeyCode Key = 0;

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