#include "pass.h"
#include "../scope_utility.h"


//===================================================================
//	overrides
//===================================================================

bool Pass::IsSame(g2d::Pass* other) const
{
	ENSURE(other != nullptr);
	if (!IsSameType(other))
		return false;

	auto p = reinterpret_cast<Pass*>(&other);

	if (this == p)
		return true;

	if (mBlendMode != p->mBlendMode ||
		mVsName != p->mVsName ||
		mPsName != p->mPsName)
		return false;

	if (mTextures.size() != p->mTextures.size() ||
		mVsConstants.size() != p->mVsConstants.size() ||
		mPsConstants.size() != p->mPsConstants.size())
	{
		return false;
	}

	for (size_t i = 0, n = mTextures.size(); i < n; i++)
	{
		if (!mTextures[i]->IsSame(p->mTextures[i]))
		{
			return false;
		}
	}

	//we have no idea how to deal with floats.
	if (mVsConstants.size() > 0 &&
		0 != memcmp(&(mVsConstants[0]), &(p->mVsConstants[0]), mVsConstants.size() * sizeof(float)))
	{
		return  false;
	}

	if (mPsConstants.size() > 0 &&
		0 != memcmp(&(mPsConstants[0]), &(p->mPsConstants[0]), mPsConstants.size() * sizeof(float)))
	{
		return false;
	}
	return true;
}

void Pass::SetTexture(unsigned int index, g2d::Texture* tex, bool autoRelease)
{
	size_t size = mTextures.size();
	if (index >= size)
	{
		mTextures.resize(index + 1);
		for (size_t i = size; i < index; i++)
		{
			mTextures[i] = nullptr;
		}
	}

	if (mTextures[index])
	{
		mTextures[index]->Release();
	}
	mTextures[index] = tex;
	if (!autoRelease)
	{
		mTextures[index]->AddRef();
	}
}

void Pass::SetVSConstant(unsigned int index, float* data, unsigned int size, unsigned int count)
{
	if (count == 0)
		return;

	if (index + count > mVsConstants.size())
	{
		mVsConstants.resize(index + count);
	}

	for (unsigned int i = 0; i < count; i++)
	{
		memcpy(&(mVsConstants[index + i]), data + i * size, size);
	}
}

void Pass::SetPSConstant(unsigned int index, float* data, unsigned int size, unsigned int count)
{
	if (count == 0)
		return;

	if (index + count > mPsConstants.size())
	{
		mPsConstants.resize(index + count);
	}

	for (unsigned int i = 0; i < count; i++)
	{
		memcpy(&(mPsConstants[index + i]), data + i * size, size);
	}
}

//===================================================================
//	functions
//===================================================================
Pass::Pass(const Pass& other)
	: mVsName(other.mVsName)
	, mPsName(other.mPsName)
	, mTextures(other.mTextures.size())
	, mVsConstants(other.mVsConstants.size())
	, mPsConstants(other.mPsConstants.size())
	, mBlendMode(other.mBlendMode)
{
	for (size_t i = 0, n = mTextures.size(); i < n; i++)
	{
		mTextures[i] = other.mTextures[i];
		mTextures[i]->AddRef();
	}

	if (other.GetVSConstantLength() > 0)
	{
		memcpy(&(mVsConstants[0]), &(other.mVsConstants[0]), other.GetVSConstantLength());
	}
	if (other.GetPSConstantLength() > 0)
	{
		memcpy(&(mPsConstants[0]), &(other.mPsConstants[0]), other.GetPSConstantLength());
	}
}

Pass::~Pass()
{
	for (auto& t : mTextures)
	{
		cxx::safe_release(t);
	}
	mTextures.clear();
}

Pass* Pass::Clone()
{
	return new Pass(*this);
}
