#pragma once
#include <g2dconfig.h>
#include <g2dmessage.h>
#include <gmlrect.h>

namespace g2d
{
	enum class G2DAPI SwitchState : int
	{
		Releasing, JustPressed, Pressing,
	};
	// 键盘状态获取接口
	class Keyboard
	{
	public:
		// 键盘按键是否被按下
		virtual SwitchState PressState(KeyCode key) const = 0;

		virtual uint32_t GetRepeatingCount(KeyCode key) const = 0;

		bool IsReleasing(KeyCode key) { return PressState(key) == SwitchState::Releasing; }

		bool IsPressing(KeyCode key) { return PressState(key) == SwitchState::Pressing; }

		bool IsJustPressed(KeyCode key) { return PressState(key) == SwitchState::JustPressed; }
	};

	// 鼠标状态获取接口
	class Mouse
	{
	public:
		// 光标的屏幕坐标
		virtual const gml::coord& CursorPosition() const = 0;

		// 按键是否被按下
		virtual SwitchState PressState(MouseButton button) const = 0;

		bool IsReleasing(MouseButton button) { return PressState(button) == SwitchState::Releasing; }

		bool IsPressing(MouseButton button) { return PressState(button) == SwitchState::Pressing; }

		bool IsJustPressed(MouseButton button) { return PressState(button) == SwitchState::JustPressed; }
	};
}