#pragma once

class Engine;
class RenderSystem;
class Mouse;
class Keyboard;

Engine* GetEngine();

RenderSystem& GetRenderSystem();

Mouse& GetMouse();

Keyboard& GetKeyboard();