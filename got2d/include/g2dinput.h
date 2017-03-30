#pragma once
#include <gml/gmlrect.h>
#include "g2dconfig.h"
#include "g2dmessage.h"

namespace g2d
{
	// ���̣���갴����״̬
	enum class G2DAPI SwitchState : int
	{
		Releasing,		// ��ť����״̬
		JustPressed,	// �ոհ��²��ͻ�û��Ӧ
		Pressing,		// һֱ�����µ�״̬
	};

	// ����״̬��ȡ�ӿ�
	class G2DAPI Keyboard
	{
	public:
		static Keyboard& Instance();

		// ���̰����Ƿ񱻰���
		virtual SwitchState GetPressState(KeyCode key) const = 0;

		// ĳ���������ظ�����
		virtual uint32_t GetRepeatingCount(KeyCode key) const = 0;

		// ���а�����û�б�����
		virtual bool IsFree() const = 0;

		bool IsReleasing(KeyCode key) { return GetPressState(key) == SwitchState::Releasing; }

		bool IsPressing(KeyCode key) { return GetPressState(key) == SwitchState::Pressing; }

		bool IsJustPressed(KeyCode key) { return GetPressState(key) == SwitchState::JustPressed; }
	};

	// ���״̬��ȡ�ӿ�
	class G2DAPI Mouse
	{
	public:
		static Mouse& Instance();

		// ������Ļ����
		virtual const gml::coord& GetCursorPosition() const = 0;

		// ������갴����ʱ�򣬹�����Ļ����
		virtual const gml::coord& GetCursorPressPosition(MouseButton button) const = 0;

		// �����Ƿ񱻰���
		virtual SwitchState GetPressState(MouseButton button) const = 0;

		// �����ĳ����ظ�����
		virtual uint32_t GetRepeatingCount(MouseButton button) const = 0;

		// ���а�����û�б�����
		virtual bool IsFree() const = 0;

		bool IsReleasing(MouseButton button) { return GetPressState(button) == SwitchState::Releasing; }

		bool IsPressing(MouseButton button) { return GetPressState(button) == SwitchState::Pressing; }

		bool IsJustPressed(MouseButton button) { return GetPressState(button) == SwitchState::JustPressed; }
	};

	inline Keyboard& GetKeyboard()
	{
		return Keyboard::Instance();
	}

	inline Mouse& GetMouse()
	{
		return Mouse::Instance();
	}
}