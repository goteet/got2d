#include "cxx_file.h"
#include "image.h"
#include "../engine.h"
#include "render_system.h"

g2d::Texture* g2d::Texture::LoadFromFile(const char* path)
{
	//std::string resourcePath = ::GetEngineImpl()->GetResourceRoot() + path;
	//return ::GetRenderSystem().CreateTextureFromFile(resourcePath.c_str());
	return NULL;
}

Texture::Texture(std::string resPath)
	: mResPath(std::move(resPath))
{

}

bool Texture::IsSame(g2d::Texture* other) const
{
	ENSURE(other != nullptr);
	if (this == other)
		return true;

	//if (!same_type(other, this))
	//	return false;
	//
	//auto timpl = reinterpret_cast<::Texture*>(other);
	//return timpl->mResPath == mResPath;
}

void Texture::AddRef()
{
	mRefCount++;
}

void Texture::Release()
{
	if (--mRefCount == 0)
	{
		delete this;
	}
}


void UploadImageRawToTexture(rhi::Texture2D* texture, const uint8_t* data, unsigned int pitch, unsigned int height)
{
	auto mappedResouce = GetRenderSystem().GetContext()->Map(texture);
	if (mappedResouce.success)
	{
		auto colorBuffer = static_cast<uint8_t*>(mappedResouce.data);
		for (unsigned int i = 0; i < height; i++)
		{
			auto dstPtr = colorBuffer + i * mappedResouce.linePitch;
			auto srcPtr = data + i * pitch;
			memcpy(dstPtr, srcPtr, pitch);
		}
		GetRenderSystem().GetContext()->Unmap(texture);
		GetRenderSystem().GetContext()->GenerateMipmaps(texture);
	}
}

void UploadImageBGRAToTexture(rhi::Texture2D* texture, const uint8_t* data, unsigned int width, unsigned int height)
{
	UploadImageRawToTexture(texture, data, width * 4, height);
}

void UploadImageBGRToTexture(rhi::Texture2D* texture, const uint8_t* data, unsigned int width, unsigned int height)
{
	auto mappedResouce = GetRenderSystem().GetContext()->Map(texture);
	if (mappedResouce.success)
	{
		auto colorBuffer = static_cast<char*>(mappedResouce.data);
		for (unsigned int i = 0; i < height; i++)
		{
			unsigned char* dstPtr = (unsigned char*)colorBuffer + i * mappedResouce.linePitch;
			auto srcPtr = data + i * width * 3;
			for (unsigned int j = 0; j < width; j++)
			{
				memcpy(dstPtr + j * 4, srcPtr + j * 3, 3);
				dstPtr[3 + j * 4] = 255;
			}
		}
		GetRenderSystem().GetContext()->Unmap(texture);
		GetRenderSystem().GetContext()->GenerateMipmaps(texture);
	}
}

void UploadImageToTexture(rhi::Texture2D* texture, const image_data& img)
{
	if (img.format == pixel_format::bgr24)
	{
		UploadImageBGRToTexture(texture, img.buffer_ptr(), img.width, img.height);
	}
	else if (img.format == pixel_format::bgra32)
	{
		UploadImageBGRAToTexture(texture, img.buffer_ptr(), img.width, img.height);
	}
	else
	{
		UploadImageRawToTexture(texture, img.buffer_ptr(), img.line_pitch, dds_block_height(img));
	}
}

bool TexturePool::CreateDefaultTexture()
{
	mDefaultTexture = GetRenderSystem().GetDevice()->CreateTexture2D(
		rhi::TextureFormat::RGBA,
		rhi::ResourceUsage::Dynamic,
		rhi::TextureBinding::ShaderResource,
		2, 2);

	if (mDefaultTexture != nullptr)
	{
		uint8_t boardData[] =
		{
			0,0,0,255,255,255,
			255,255,255,0,0,0
		};
		UploadImageBGRToTexture(mDefaultTexture.get(), boardData, 2, 2);
		return true;
	}
	else
	{
		return false;
	}
}

bool TexturePool::LoadTextureFromFile(std::string resourcePath)
{
	cxx::file_buffer f = cxx::read_file(resourcePath.c_str());
	if (!f)
		return false;

	image_data img = parse_image(f.buffer_ptr(), f.length());
	//f.destroy()
	
	bool result = false;
	if (img)
	{
		rhi::TextureFormat format;
		switch (img.format)
		{
		case pixel_format::bgr24:format = rhi::TextureFormat::BGRA; break;
		case pixel_format::bgra32:format = rhi::TextureFormat::BGRA; break;
		case pixel_format::dxt1a:
		case pixel_format::dxt1x:format = rhi::TextureFormat::DXT1; break;
		case pixel_format::dxt2:
		case pixel_format::dxt3:format = rhi::TextureFormat::DXT3; break;
		case pixel_format::dxt4:
		case pixel_format::dxt5:format = rhi::TextureFormat::DXT5; break;
		}

		auto tex = GetRenderSystem().GetDevice()->CreateTexture2D(
			format,
			rhi::ResourceUsage::Dynamic,
			rhi::TextureBinding::ShaderResource,
			img.width, img.height);

		if (tex != nullptr)
		{
			UploadImageToTexture(tex, img);
			mTextures[resourcePath] = tex;
			result = true;
		}
		//img.destroy();
	}
	return result;
}

void TexturePool::Destroy()
{
	mDefaultTexture.release();
	for (auto& t : mTextures)
	{
		t.second->Release();
	}
	mTextures.clear();
}

rhi::Texture2D* TexturePool::GetTexture(const std::string& resource)
{
	if (mTextures.count(resource) == 0)
	{
		if (!LoadTextureFromFile(resource))
		{
			return mDefaultTexture;
		}
	}

	return mTextures.at(resource);
}
