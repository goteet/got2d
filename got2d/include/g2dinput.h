#pragma once
#include <g2dconfig.h>
#include <gmlrect.h>

namespace g2d
{
	enum class G2DAPI MessageEvent
	{
		Invalid,
		MouseMove,
		MouseButtonDown,
		MouseButtonUp,
		MouseButtonDoubleClick,
	};

	// 输入设备，事件来源
	enum class MessageSource
	{
		Invalid, Mouse, Keyboard
	};

	enum class MouseButton
	{
		Left, Middle, Right, None
	};

	struct G2DAPI Message
	{
		Message() = default;

		Message(const Message& other) = default;

		Message(MessageEvent ev, g2d::MouseButton btn, uint32_t x, uint32_t y)
			: Event(ev), Source(MessageSource::Mouse)
			, MouseButton(btn)
			, MousePositionX(x)
			, MousePositionY(y)
		{		}

		Message(MessageEvent ev, int key)
			: Event(ev), Source(MessageSource::Keyboard)
			, KeyButton(key)
		{		}
		
		const MessageEvent Event = MessageEvent::Invalid;

		const MessageSource Source = MessageSource::Invalid;

		// 光标事件信息
		const MouseButton MouseButton = MouseButton::None;

		const uint32_t MousePositionX = 0;

		const uint32_t MousePositionY = 0;

		// 键盘事件信息，没完整实现
		const int KeyButton = 0;

	public:
		// 根据鼠标信息构建半成品Message
		// 这个接口有点2
		Message(MessageEvent ev, g2d::MouseButton btn)
			: Event(ev), Source(MessageSource::Mouse)
			, MouseButton(btn)
		{		}

		Message(const Message& m, uint32_t x, uint32_t y)
			: Event(m.Event), Source(MessageSource::Mouse)
			, MouseButton(m.MouseButton)
			, MousePositionX(x)
			, MousePositionY(y)
		{		}

		// 根据键盘信息构建半成品Message
		// 这个接口有点2
		Message(MessageEvent ev)
			: Event(ev), Source(MessageSource::Keyboard)
		{		}

		Message(const Message& m, int key)
			: Event(m.Event), Source(MessageSource::Keyboard)
			, KeyButton(key)
		{		}
	};

	Message G2DAPI TranslateMessageFromWin32(uint32_t message, uint32_t wparam, uint32_t lparam);
}