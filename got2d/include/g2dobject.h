#pragma once
#include "g2dconfig.h"
#include "cxx_math.h"

namespace g2d
{
	using namespace cxx;

	struct G2DAPI Object
	{
		virtual unsigned int GetClassID() const = 0;

		bool IsSameType(Object* other) const;

	protected:
		Object() = default;

		virtual ~Object() = default;

		Object(const Object&) = delete;

		Object& operator=(const Object&) = delete;
	};

	unsigned int G2DAPI GenerateClassID();

	template<typename T>
	bool Is(Object* o)
	{
		return o->GetClassID() == T::GetStaticClassID();
	}

#define RTTI_IMPL \
public:\
	static unsigned int GetStaticClassID()\
	{\
		static unsigned int sStaticClassID = g2d::GenerateClassID();\
		 return sStaticClassID;\
	}\
	virtual unsigned int GetClassID() const override\
	{\
		 return GetStaticClassID();\
	}\

}