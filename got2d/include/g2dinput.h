#pragma once
#include <g2dconfig.h>
#include <g2dmessage.h>
#include <gmlrect.h>

namespace g2d
{
	// 键盘状态获取接口
	class Keyboard
	{
	public:
		// 键盘按键是否被按下
		virtual bool IsPressing(KeyCode key) const = 0;
	};

	// 鼠标状态获取接口
	class Mouse
	{
	public:
		// 按键是否被按下
		virtual bool IsPressing(MouseButton button) const = 0;

		// 光标的屏幕坐标
		virtual const gml::coord& CursorPosition() const = 0;
	};
}