#include "g2dmessage.h"

g2d::Message::Message(MessageEvent ev, g2d::MouseButton btn, unsigned int x, unsigned int y)
	: Event(ev)
	, Source(MessageSource::Mouse)
	, MouseButton(btn)
	, CursorPositionX(x)
	, CursorPositionY(y)
{

}


g2d::Message::Message(MessageEvent ev, KeyCode key)
	: Event(ev)
	, Source(MessageSource::Keyboard)
	, Key(key)
{

}

g2d::Message::Message(MessageEvent ev, MessageSource src)
	: Event(ev)
	, Source(src)
{

}

g2d::Message::Message(MessageEvent ev, g2d::MouseButton btn)
	: Event(ev)
	, Source(MessageSource::Mouse)
	, MouseButton(btn)
{

}

g2d::Message::Message(const Message& m, int x, int y)
	: Event(m.Event)
	, Source(MessageSource::Mouse)
	, MouseButton(m.MouseButton)
	, CursorPositionX(x)
	, CursorPositionY(y)
{

}

g2d::Message::Message(const Message& m, KeyCode key)
	: Event(m.Event)
	, Source(MessageSource::Keyboard)
	, Key(key)
{

}
