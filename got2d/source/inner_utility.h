#pragma once

#define RTTI_INNER_IMPL \
public:\
	unsigned int GetClassID() const\
	{\
		static unsigned int sClassID = g2d::GenerateClassID();\
		return sClassID;\
	}\
private:

template<typename T1, typename T2>
bool IsSameType(T1* a, T2* b)
{
	return (a->GetClassID() == b->GetClassID());
}

namespace g2d
{
	struct Engine;
	struct RenderSystem;

	struct SceneNode;
	struct Scene;

	struct Component;
	struct Camera;
	struct Quad;
}
