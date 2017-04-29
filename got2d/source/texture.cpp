#include <img/file_data.h>
#include <img/img_data.h>
#include "engine.h"
#include "render_system.h"

g2d::Texture* g2d::Texture::LoadFromFile(const char* path)
{
	std::string resourcePath = ::GetEngineImpl()->GetResourceRoot() + path;
	return ::GetRenderSystem()->CreateTextureFromFile(resourcePath.c_str());
}

Texture::Texture(std::string resPath)
	: m_resPath(std::move(resPath))
{

}

bool Texture::IsSame(g2d::Texture* other) const
{
	ENSURE(other != nullptr);
	if (this == other)
		return true;

	if (!same_type(other, this))
		return false;

	auto timpl = reinterpret_cast<::Texture*>(other);
	return timpl->m_resPath == m_resPath;
}

void Texture::AddRef()
{
	m_refCount++;
}

void Texture::Release()
{
	if (--m_refCount == 0)
	{
		delete this;
	}
}


void UploadImageRawToTexture(rhi::Texture2D* texture, uint8_t* data, uint32_t pitch, uint32_t height)
{
	auto mappedResouce = GetRenderSystem()->GetContext()->Map(texture);
	if (mappedResouce.success)
	{
		auto colorBuffer = static_cast<uint8_t*>(mappedResouce.data);
		for (uint32_t i = 0; i < height; i++)
		{
			auto dstPtr = colorBuffer + i * mappedResouce.linePitch;
			auto srcPtr = data + i * pitch;
			memcpy(dstPtr, srcPtr, pitch);
		}
		GetRenderSystem()->GetContext()->Unmap(texture);
		GetRenderSystem()->GetContext()->GenerateMipmaps(texture);
	}
}

void UploadImageBGRAToTexture(rhi::Texture2D* texture, uint8_t* data, uint32_t width, uint32_t height)
{
	UploadImageRawToTexture(texture, data, width * 4, height);
}

void UploadImageBGRToTexture(rhi::Texture2D* texture, uint8_t* data, uint32_t width, uint32_t height)
{
	auto mappedResouce = GetRenderSystem()->GetContext()->Map(texture);
	if (mappedResouce.success)
	{
		auto colorBuffer = static_cast<uint8_t*>(mappedResouce.data);
		for (uint32_t i = 0; i < height; i++)
		{
			auto dstPtr = colorBuffer + i * mappedResouce.linePitch;
			auto srcPtr = data + i * width * 3;
			for (uint32_t j = 0; j < width; j++)
			{
				memcpy(dstPtr + j * 4, srcPtr + j * 3, 3);
				dstPtr[3 + j * 4] = 255;
			}
		}
		GetRenderSystem()->GetContext()->Unmap(texture);
		GetRenderSystem()->GetContext()->GenerateMipmaps(texture);
	}
}

void UploadImageToTexture(rhi::Texture2D* texture, const img_data& img)
{
	if (img.format == color_format::bgr24)
	{
		UploadImageBGRToTexture(texture, img.buffer, img.width, img.height);
	}
	else if (img.format == color_format::bgra32)
	{
		UploadImageBGRAToTexture(texture, img.buffer, img.width, img.height);
	}
	else
	{
		UploadImageRawToTexture(texture, img.buffer, img.pitch, img.block_height);
	}
}

bool TexturePool::CreateDefaultTexture()
{
	m_defaultTexture = GetRenderSystem()->GetDevice()->CreateTexture2D(
		rhi::TextureFormat::RGBA,
		rhi::ResourceUsage::Dynamic,
		rhi::TextureBinding::ShaderResource,
		2, 2);

	if (m_defaultTexture != nullptr)
	{
		uint8_t boardData[] =
		{
			0,0,0,255,255,255,
			255,255,255,0,0,0
		};
		UploadImageBGRToTexture(m_defaultTexture, boardData, 2, 2);
		return true;
	}
	else
	{
		return false;
	}
}

bool TexturePool::LoadTextureFromFile(std::string resourcePath)
{
	file_data f = read_file(resourcePath.c_str());
	if (!f.is_valid())
		return false;

	img_data img = read_image(f.buffer);
	f.destroy();
	bool result = false;
	if (img.is_valid())
	{
		rhi::TextureFormat format;
		switch (img.format)
		{
		case color_format::bgr24:format = rhi::TextureFormat::BGRA; break;
		case color_format::bgra32:format = rhi::TextureFormat::BGRA; break;
		case color_format::dxt1:format = rhi::TextureFormat::DXT1; break;
		case color_format::dxt2:
		case color_format::dxt3:format = rhi::TextureFormat::DXT3; break;
		case color_format::dxt4:
		case color_format::dxt5:format = rhi::TextureFormat::DXT5; break;
		}

		auto tex = GetRenderSystem()->GetDevice()->CreateTexture2D(
			format,
			rhi::ResourceUsage::Dynamic,
			rhi::TextureBinding::ShaderResource,
			img.width, img.height);

		if (tex != nullptr)
		{
			UploadImageToTexture(tex, img);
			m_textures[resourcePath] = tex;
			result = true;
		}
		img.destroy();
	}
	return result;
}

void TexturePool::Destroy()
{
	m_defaultTexture.release();
	for (auto& t : m_textures)
	{
		t.second->Release();
	}
	m_textures.clear();
}

rhi::Texture2D* TexturePool::GetTexture(const std::string& resource)
{
	if (m_textures.count(resource) == 0)
	{
		if (!LoadTextureFromFile(resource))
		{
			return m_defaultTexture;
		}
	}

	return m_textures.at(resource);
}
