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
		virtual SwitchState GetPressState(KeyCode key) const = 0;

		// ĳ���������ظ�����
		virtual uint32_t GetRepeatingCount(KeyCode key) const = 0;

		bool IsReleasing(KeyCode key) { return GetPressState(key) == SwitchState::Releasing; }

		bool IsPressing(KeyCode key) { return GetPressState(key) == SwitchState::Pressing; }

		bool IsJustPressed(KeyCode key) { return GetPressState(key) == SwitchState::JustPressed; }
	};

	// ���״̬��ȡ�ӿ�
	class Mouse
	{
	public:
		// ������Ļ����
		virtual const gml::coord& GetCursorPosition() const = 0;

		// ������갴����ʱ�򣬹�����Ļ����
		virtual const gml::coord& GetCursorPressPosition(MouseButton button) const = 0;

		// �����Ƿ񱻰���
		virtual SwitchState GetPressState(MouseButton button) const = 0;

		// �����ĳ����ظ�����
		virtual uint32_t GetRepeatingCount(MouseButton button) const = 0;

		bool IsReleasing(MouseButton button) { return GetPressState(button) == SwitchState::Releasing; }

		bool IsPressing(MouseButton button) { return GetPressState(button) == SwitchState::Pressing; }

		bool IsJustPressed(MouseButton button) { return GetPressState(button) == SwitchState::JustPressed; }
	};
}