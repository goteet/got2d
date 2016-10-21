#include "g2drender.h"
#include "render/material.h"
#include "render/pass.h"
#include "render/texture.h"

namespace g2d
{
	Material* Material::CreateColorTexture()
	{
		auto mat = new ::Material(1);
		mat->SetPass(0, new ::Pass("default", "color.texture"));
		mat->GetPassByIndex(0)->SetTexture(0, ::Texture::Default(), false);
		return mat;
	}

	Material* Material::CreateSimpleTexture()
	{
		auto mat = new ::Material(1);
		mat->SetPass(0, new ::Pass("default", "simple.texture"));
		mat->GetPassByIndex(0)->SetTexture(0, ::Texture::Default(), false);
		return mat;
	}

	Material* Material::CreateSimpleColor()
	{
		auto mat = new ::Material(1);
		mat->SetPass(0, new ::Pass("default", "simple.color"));
		return mat;
	}
}