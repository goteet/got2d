#pragma once
#include <vector>
#include "g2drender.h"

class Pass;

class Material : public g2d::Material
{
	RTTI_IMPL;

public:
	virtual g2d::Pass* GetPassByIndex(unsigned int index) const override;

	virtual unsigned int GetPassCount() const override;

	virtual bool IsSame(g2d::Material* other) const override;

	virtual g2d::Material* Clone() const override;

	virtual void Release()  override;

public:
	Material(unsigned int passCount);

	Material(const Material& other);

	~Material();

	void SetPass(unsigned int index, Pass* p);

private:
	std::vector<::Pass*> mPasses;
};