#include "material.h"
#include "../scope_utility.h"
#include "pass.h"

//===================================================================
//	overrides
//===================================================================
g2d::Pass* Material::GetPassByIndex(unsigned int index) const
{
	ENSURE(index < GetPassCount());
	return mPasses.at(index);
}

unsigned int Material::GetPassCount() const
{
	return static_cast<unsigned int>(mPasses.size());
}

bool Material::IsSame(g2d::Material* other) const
{
	ENSURE(other != nullptr);

	if (this == other)
		return true;

	if (!IsSameType(other))
		return false;

	if (other->GetPassCount() != GetPassCount())
		return false;

	for (unsigned int i = 0; i < GetPassCount(); i++)
	{
		if (!mPasses[i]->IsSame(other->GetPassByIndex(i)))
		{
			return false;
		}
	}
	return true;
}

g2d::Material* Material::Clone() const
{
	return new Material(*this);
}

void Material::Release()
{
	delete this;
}

//===================================================================
//	functions
//===================================================================
Material::Material(unsigned int passCount)
	: mPasses(passCount)
{

}

Material::Material(const Material& other)
	: mPasses(other.mPasses.size())
{
	for (size_t i = 0, n = mPasses.size(); i < n; i++)
	{
		mPasses[i] = other.mPasses[i]->Clone();
	}
}

Material::~Material()
{
	for (Pass* pPass : mPasses)
	{
		pPass->Release();
	}
	mPasses.clear();
}

void Material::SetPass(unsigned int index, Pass* p)
{
	ENSURE(index < GetPassCount());
	mPasses[index] = p;
}