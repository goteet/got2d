#include "g2dobject.h"

namespace g2d
{
	bool Object::IsSameType(Object* other) const
	{
		return GetClassID() == other->GetClassID();
	}

	unsigned int G2DAPI GenerateClassID()
	{
		static unsigned int sNextClassID = 0;
		return ++sNextClassID;
	}
}