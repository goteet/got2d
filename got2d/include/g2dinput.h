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
	// ����״̬��ȡ�ӿ�
	class Keyboard
	{
	public:
		// ���̰����Ƿ񱻰���
		virtual SwitchState PressState(KeyCode key) const = 0;

		virtual uint32_t GetRepeatingCount(KeyCode key) const = 0;

		bool IsReleasing(KeyCode key) { return PressState(key) == SwitchState::Releasing; }

		bool IsPressing(KeyCode key) { return PressState(key) == SwitchState::Pressing; }

		bool IsJustPressed(KeyCode key) { return PressState(key) == SwitchState::JustPressed; }
	};

	// ���״̬��ȡ�ӿ�
	class Mouse
	{
	public:
		// ������Ļ����
		virtual const gml::coord& CursorPosition() const = 0;

		// �����Ƿ񱻰���
		virtual SwitchState PressState(MouseButton button) const = 0;

		bool IsReleasing(MouseButton button) { return PressState(button) == SwitchState::Releasing; }

		bool IsPressing(MouseButton button) { return PressState(button) == SwitchState::Pressing; }

		bool IsJustPressed(MouseButton button) { return PressState(button) == SwitchState::JustPressed; }
	};
}