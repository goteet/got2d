#include "../include/g2dinput.h"
#include <map>
#include <Windows.h>
#include <Windowsx.h>

struct msg_map
{
	msg_map()
	{
		reg(WM_LBUTTONDOWN, {
			g2d::MessageEvent::MouseButtonDown,
			g2d::MouseButton::Left,
		});

		reg(WM_LBUTTONUP, {
			g2d::MessageEvent::MouseButtonUp,
			g2d::MouseButton::Left,
		});

		reg(WM_LBUTTONDBLCLK, {
			g2d::MessageEvent::MouseButtonDoubleClick,
			g2d::MouseButton::Left,
		});

		reg(WM_RBUTTONDOWN, {
			g2d::MessageEvent::MouseButtonDown,
			g2d::MouseButton::Right,
		});

		reg(WM_RBUTTONUP, {
			g2d::MessageEvent::MouseButtonUp,
			g2d::MouseButton::Right,
		});

		reg(WM_RBUTTONDBLCLK, {
			g2d::MessageEvent::MouseButtonDoubleClick,
			g2d::MouseButton::Right,

		});

		reg(WM_MBUTTONDOWN, {
			g2d::MessageEvent::MouseButtonDown,
			g2d::MouseButton::Middle,
		});

		reg(WM_MBUTTONUP, {
			g2d::MessageEvent::MouseButtonUp,
			g2d::MouseButton::Middle,
		});

		reg(WM_MBUTTONDBLCLK, {
		g2d::MessageEvent::MouseButtonDoubleClick,
			g2d::MouseButton::Middle,
		});

		reg(WM_MOUSEMOVE, {
			g2d::MessageEvent::MouseMove,
			g2d::MouseButton::None,
		});

		reg(WM_KEYDOWN, {
			g2d::MessageEvent::KeyDown,
			g2d::MessageSource::Keyboard
		});

		reg(WM_KEYUP, {
			g2d::MessageEvent::KeyUp,
			g2d::MessageSource::Keyboard
		});
	}

	bool exist(uint32_t win32msg) const
	{
		return m_mapping.count(win32msg) > 0;
	}

	const g2d::Message& get(uint32_t win32msg) const
	{
		return m_mapping.at(win32msg);
	}
private:

	void reg(uint32_t win32msg, const g2d::Message& m)
	{
		m_mapping.insert({ win32msg, m });
	}
	typedef std::map<uint32_t, g2d::Message> msg_map_;
	msg_map_ m_mapping;
};

namespace g2d
{
	Message G2DAPI TranslateMessageFromWin32(uint32_t message, uint32_t wparam, uint32_t lparam)
	{
		static const msg_map mapping;
		if (mapping.exist(message))
		{
			auto& msg = mapping.get(message);
			switch (msg.Source)
			{
			case MessageSource::Mouse:
			{
				/*
				Do not use the LOWORD or HIWORD macros
				to extract the x- and y- coordinates of the cursor position
				because these macros return incorrect results on systems with multiple monitors.
				Systems with multiple monitors can have negative x- and y- coordinates,
				and LOWORD and HIWORD treat the coordinates as unsigned quantities.
				*/
				int x = GET_X_LPARAM(lparam);
				int y = GET_Y_LPARAM(lparam);
				bool ctrl = (wparam & MK_CONTROL) != 0;
				bool shift = (wparam & MK_SHIFT) != 0;
				return Message(msg, x, y);
			}

			case MessageSource::Keyboard:
			{
				int key = wparam;
				return Message(msg, key);
			}

			default:
				return Message();
			}
		}
		else if (message == WM_ACTIVATE && wparam == WA_INACTIVE)
		{
			return Message(MessageEvent::LostFocus, MessageSource::None);
		}
		else
		{
			return Message();
		}
	}
}