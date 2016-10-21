#include "system_blackboard.h"
#include "engine.h"



Engine* GetEngine()
{
	return Engine::Instance;
}

RenderSystem& GetRenderSystem()
{
	return Engine::Instance->GetRenderSystemImpl();
}

Mouse& GetMouse()
{
	return Engine::Instance->GetMouseImpl();
}

Keyboard& GetKeyboard()
{
	return Engine::Instance->GetKeyboardImpl();
}
