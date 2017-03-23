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
		virtual SwitchState GetPressState(KeyCode key) const = 0;

		// 某个按键的重复次数
		virtual uint32_t GetRepeatingCount(KeyCode key) const = 0;

		bool IsReleasing(KeyCode key) { return GetPressState(key) == SwitchState::Releasing; }

		bool IsPressing(KeyCode key) { return GetPressState(key) == SwitchState::Pressing; }

		bool IsJustPressed(KeyCode key) { return GetPressState(key) == SwitchState::JustPressed; }
	};

	// 鼠标状态获取接口
	class Mouse
	{
	public:
		// 光标的屏幕坐标
		virtual const gml::coord& GetCursorPosition() const = 0;

		// 按下鼠标按键的时候，光标的屏幕坐标
		virtual const gml::coord& GetCursorPressPosition(MouseButton button) const = 0;

		// 按键是否被按下
		virtual SwitchState GetPressState(MouseButton button) const = 0;

		// 按键的持续重复次数
		virtual uint32_t GetRepeatingCount(MouseButton button) const = 0;

		bool IsReleasing(MouseButton button) { return GetPressState(button) == SwitchState::Releasing; }

		bool IsPressing(MouseButton button) { return GetPressState(button) == SwitchState::Pressing; }

		bool IsJustPressed(MouseButton button) { return GetPressState(button) == SwitchState::JustPressed; }
	};
}