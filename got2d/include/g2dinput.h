#pragma once
#include <g2dconfig.h>
#include <g2dmessage.h>
#include <gmlrect.h>

namespace g2d
{
	// ����״̬��ȡ�ӿ�
	class Keyboard
	{
	public:
		// ���̰����Ƿ񱻰���
		virtual bool IsPressing(KeyCode key) const = 0;
	};

	// ���״̬��ȡ�ӿ�
	class Mouse
	{
	public:
		// �����Ƿ񱻰���
		virtual bool IsPressing(MouseButton button) const = 0;

		// ������Ļ����
		virtual const gml::coord& CursorPosition() const = 0;
	};
}