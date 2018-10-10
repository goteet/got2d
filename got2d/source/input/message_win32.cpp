#include <Windows.h>
#include <Windowsx.h>
#include <map>
#include "g2dmessage.h"

struct Mapping
{
	Mapping()
	{
		RegisterMessageMapping();
		RegisterKeyCodeMapping();
	}

	bool ExistMessage(unsigned int win32msg) const
	{
		return mMsgMapping.count(win32msg) > 0;
	}

	const g2d::Message& ToMessage(unsigned int win32msg) const
	{
		return mMsgMapping.at(win32msg);
	}

	g2d::KeyCode ToKeyCode(unsigned int vkey) const
	{
		if (mKeyMapping.count(vkey) == 0)
			return g2d::KeyCode::Invalid;
		return mKeyMapping.at(vkey);
	}

private:
	void RegisterMessageMapping()
	{

		RegisterMessage(WM_LBUTTONDOWN, {
			g2d::MessageEvent::MouseButtonDown,
			g2d::MouseButton::Left,
			});

		RegisterMessage(WM_LBUTTONUP, {
			g2d::MessageEvent::MouseButtonUp,
			g2d::MouseButton::Left,
			});

		RegisterMessage(WM_LBUTTONDBLCLK, {
			g2d::MessageEvent::MouseButtonDoubleClick,
			g2d::MouseButton::Left,
			});

		RegisterMessage(WM_RBUTTONDOWN, {
			g2d::MessageEvent::MouseButtonDown,
			g2d::MouseButton::Right,
			});

		RegisterMessage(WM_RBUTTONUP, {
			g2d::MessageEvent::MouseButtonUp,
			g2d::MouseButton::Right,
			});

		RegisterMessage(WM_RBUTTONDBLCLK, {
			g2d::MessageEvent::MouseButtonDoubleClick,
			g2d::MouseButton::Right,

			});

		RegisterMessage(WM_MBUTTONDOWN, {
			g2d::MessageEvent::MouseButtonDown,
			g2d::MouseButton::Middle,
			});

		RegisterMessage(WM_MBUTTONUP, {
			g2d::MessageEvent::MouseButtonUp,
			g2d::MouseButton::Middle,
			});

		RegisterMessage(WM_MBUTTONDBLCLK, {
			g2d::MessageEvent::MouseButtonDoubleClick,
			g2d::MouseButton::Middle,
			});

		RegisterMessage(WM_MOUSEMOVE, {
			g2d::MessageEvent::MouseMove,
			g2d::MouseButton::None,
			});

		RegisterMessage(WM_KEYDOWN, {
			g2d::MessageEvent::KeyDown,
			g2d::MessageSource::Keyboard
			});

		RegisterMessage(WM_KEYUP, {
			g2d::MessageEvent::KeyUp,
			g2d::MessageSource::Keyboard
			});
	}

	void RegisterKeyCodeMapping()
	{
		// Functions
		RegisterKeyCode(VK_BACK, g2d::KeyCode::Backspace);
		RegisterKeyCode(VK_CONTROL, g2d::KeyCode::Control);
		RegisterKeyCode(VK_LCONTROL, g2d::KeyCode::Control);
		RegisterKeyCode(VK_RCONTROL, g2d::KeyCode::Control);
		RegisterKeyCode(VK_SHIFT, g2d::KeyCode::Shift);
		RegisterKeyCode(VK_LSHIFT, g2d::KeyCode::Shift);
		RegisterKeyCode(VK_RSHIFT, g2d::KeyCode::Shift);
		RegisterKeyCode(VK_MENU, g2d::KeyCode::Alt);
		RegisterKeyCode(VK_LMENU, g2d::KeyCode::Alt);
		RegisterKeyCode(VK_RMENU, g2d::KeyCode::Alt);
		RegisterKeyCode(VK_PAUSE, g2d::KeyCode::Pause);
		RegisterKeyCode(VK_CAPITAL, g2d::KeyCode::Capital);
		RegisterKeyCode(VK_ESCAPE, g2d::KeyCode::Escape);

		RegisterKeyCode(VK_PRIOR, g2d::KeyCode::PageUp);
		RegisterKeyCode(VK_NEXT, g2d::KeyCode::PageDown);
		RegisterKeyCode(VK_HOME, g2d::KeyCode::Home);
		RegisterKeyCode(VK_END, g2d::KeyCode::End);
		RegisterKeyCode(VK_INSERT, g2d::KeyCode::Insert);
		RegisterKeyCode(VK_DELETE, g2d::KeyCode::Delete);

		RegisterKeyCode(VK_LEFT, g2d::KeyCode::ArrowLeft);
		RegisterKeyCode(VK_UP, g2d::KeyCode::ArrowUp);
		RegisterKeyCode(VK_RIGHT, g2d::KeyCode::ArrowRight);
		RegisterKeyCode(VK_DOWN, g2d::KeyCode::ArrowDown);

		RegisterKeyCode(VK_TAB, g2d::KeyCode::Tab);
		RegisterKeyCode(VK_RETURN, g2d::KeyCode::Enter);
		RegisterKeyCode(VK_SPACE, g2d::KeyCode::Space);

		// main characters
		for (unsigned int key = 'A'; key <= 'Z'; key++)
			RegisterKeyCode(key, g2d::KeyCode(key));

		// numbers and numpads
		for (unsigned int key = '0'; key <= '9'; key++)
		{
			RegisterKeyCode(key, g2d::KeyCode(key));
			RegisterKeyCode(key + g2d::NUMPAD_OFFSET, g2d::KeyCode(key + g2d::NUMPAD_OFFSET));
		}
		RegisterKeyCode(VK_NUMLOCK, g2d::KeyCode::NumpadLock);
		RegisterKeyCode(VK_DECIMAL, g2d::KeyCode::NumpadDecimal);
		RegisterKeyCode(VK_SEPARATOR, g2d::KeyCode::NumpadEnter);
		RegisterKeyCode(VK_ADD, g2d::KeyCode::NumpadAdd);
		RegisterKeyCode(VK_SUBTRACT, g2d::KeyCode::NumpadSub);
		RegisterKeyCode(VK_MULTIPLY, g2d::KeyCode::NumpadMul);
		RegisterKeyCode(VK_DIVIDE, g2d::KeyCode::NumpadDiv);

		// Fs
		for (unsigned int key = (int)g2d::KeyCode::F1; key <= (int)g2d::KeyCode::F12; key++)
		{
			RegisterKeyCode(VK_F1 + ((int)g2d::KeyCode::F1 - key), g2d::KeyCode(key));
		}

		// OEM
		RegisterKeyCode(VK_OEM_3, g2d::KeyCode::OMETilde);
		RegisterKeyCode(VK_OEM_MINUS, g2d::KeyCode::OEMSub);
		RegisterKeyCode(VK_OEM_PLUS, g2d::KeyCode::OEMAdd);
		RegisterKeyCode(VK_OEM_4, g2d::KeyCode::OEMLBracket);
		RegisterKeyCode(VK_OEM_6, g2d::KeyCode::OEMRBracket);
		RegisterKeyCode(VK_OEM_5, g2d::KeyCode::OEMRSlash);
		RegisterKeyCode(VK_OEM_1, g2d::KeyCode::OEMColon);
		RegisterKeyCode(VK_OEM_7, g2d::KeyCode::OEMQuote);
		RegisterKeyCode(VK_OEM_COMMA, g2d::KeyCode::OEMComma);
		RegisterKeyCode(VK_OEM_PERIOD, g2d::KeyCode::OEMPeriod);
		RegisterKeyCode(VK_OEM_2, g2d::KeyCode::OEMSlash);
	}
	void RegisterMessage(unsigned int win32msg, const g2d::Message& m)
	{
		mMsgMapping.insert({ win32msg, m });
	}
	void RegisterKeyCode(unsigned int win32key, g2d::KeyCode key)
	{
		mKeyMapping.insert({ win32key, key });
	}
	typedef std::map<unsigned int, g2d::Message> MSGMap;
	typedef std::map<unsigned int, g2d::KeyCode> KEYMap;
	MSGMap mMsgMapping;
	KEYMap mKeyMapping;
};

namespace g2d
{
	Message G2DAPI TranslateMessageWin32(unsigned int message, unsigned int wparam, unsigned int lparam)
	{
		static const Mapping mapping;
		if (mapping.ExistMessage(message))
		{
			auto& msg = mapping.ToMessage(message);
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
				return Message(msg, x, y);
			}

			case MessageSource::Keyboard:
			{
				int key = wparam;
				return Message(msg, mapping.ToKeyCode(key));
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

bool KeyboardButtonDownWin32(unsigned int vkey)
{
	return (HIBYTE(GetKeyState(vkey)) & 0x80) != 0;
}

bool AltDownWin32() { return KeyboardButtonDownWin32(VK_MENU); }