#pragma once

#include <g2dconfig.h>
#include <gmlvector.h>
#include <gmlmatrix.h>
#include <gmlcolor.h>
#include <gmlrect.h>

namespace g2d
{
	// ��͸���������
	enum class G2DAPI BlendMode
	{
		// �����
		None,

		// ��͸�����Ч��
		Normal,

		// ����Ч��
		Additve,

		// ��Ļ���Ч��
		Screen,
	};

	// Mesh��ʹ�õ��ڴ沼��
	struct GeometryVertex
	{
		gml::vec2 position;
		gml::vec2 texcoord;
		gml::color4 vtxcolor;
	};

	// �û��Զ���ģ�����
	// ���ݱ������ڴ��У���������Ⱦ������
	// ����ݲ���ʹ�����������ذ��ڴ������ϴ����Դ��
	class G2DAPI Mesh : public GObject
	{
	public:
		static Mesh* Create(uint32_t vertexCount, uint32_t indexCount);

		// �ͷ��ڴ棬ֻ�ܵ���һ��
		// �û���Ҫ�ֶ�����
		virtual void Release() = 0;

		// ��ö�������ָ�룬�û�ͨ�����ָ��������ݡ�
		// �û���Ҫ�Լ���֤���ݲ���������ʱָ���Ĵ�С��
		// ������������Խ��
		virtual const GeometryVertex* GetRawVertices() const = 0;
		virtual GeometryVertex* GetRawVertices() = 0;

		// �����������ָ�룬�û�ͨ�����ָ��������ݡ�
		// �û���Ҫ�Լ���֤���ݲ���������ʱָ���Ĵ�С��
		// ������������Խ��
		virtual const uint32_t* GetRawIndices() const = 0;
		virtual uint32_t* GetRawIndices() = 0;

		// ��ö������ݴ�С��ΪGeometryVertex����ĸ���
		virtual uint32_t GetVertexCount() const = 0;

		// ���������������ĸ���
		virtual uint32_t GetIndexCount() const = 0;

		// ���趥�����ݴ�С��ԭ�����ݻᱻ�Զ�Ǩ�Ƶ���������
		virtual void ResizeVertexArray(uint32_t vertexCount) = 0;

		// �����������ݴ�С��ԭ�����ݻᱻ�Զ�Ǩ�Ƶ���������
		virtual void ResizeIndexArray(uint32_t indexCount) = 0;
	};

	// �������������Ϊ��Ⱦ��Դ����һ��������Դ
	// ÿ�λ�ȡ�����ʱ����Ҫ���ֶ����� AddRef �������ü���
	// ÿ�β���Ҫ��Դ����Ҫ��ʽ���� Release �ӿ��ͷ�����
	class G2DAPI Texture : public GObject
	{
	public:
		// ���ļ��ж�ȡ��������
		// ֧�� bmp/png/tga����֧�ֵ����ͻ᷵��nullptr
		// ֧��͸���Ͳ�͸�����ļ���ʽ
		static Texture* LoadFromFile(const char* path);

		// �ͷ����ü�����ÿ�β���Ҫʹ��֮����Ҫ�ֶ�����
		virtual void Release() = 0;

		// �������ü����������������ʱ����Ҫ��ʽ���á�
		virtual void AddRef() = 0;

		// ����ı�ʶ������������ֻ�ܴ��ļ����ȡ
		// ��ʶ�������ļ�·��
		virtual const char* Identifier() const = 0;

		// �ж����������Ƿ���ͬ
		virtual bool IsSame(Texture* other) const = 0;
	};

	// ������ȾPass
	/// �˶�����֮��Ĺ����лᱻ��չ�ɶ������
	/// �û���Ҫʹ������������������
	// keep it simple and stupid.
	// there is no other more easy way to define param setting interface elegantly
	// TODO: make it more elegant.
	class G2DAPI Pass : public GObject
	{
	public:
		// ʹ�õ�Vertex Shader����
		virtual const char* GetVertexShaderName() const = 0;

		// ʹ�õ�Pixel Shader����
		virtual const char* GetPixelShaderName() const = 0;

		// �ж�����pass�Ƿ���ȫһ�£��������е�����
		virtual bool IsSame(Pass* other) const = 0;

		// ���ð�͸�����ģʽ
		virtual void SetBlendMode(BlendMode mode) = 0;

		// ���Vertex Shader��������
		virtual void SetVSConstant(uint32_t index, float* data, uint32_t size, uint32_t count) = 0;

		// ���Pixel Shader��������
		virtual void SetPSConstant(uint32_t index, float* data, uint32_t size, uint32_t count) = 0;

		// ���ò�����Ϣ����Щ������Ҫ�Զ��ͷ�
		// ��Index����չ��ʱ���м�û�б����ù���Index���Զ����nullptr
		// ����TextureCount�ᱻ����
		// �������������Ϊ�գ����治��������Ⱦ������������֮ǰ������״̬��
		virtual void SetTexture(uint32_t index, Texture*, bool autoRelease) = 0;

		// ����Inedx��ȡ���õ�����
		virtual Texture* GetTextureByIndex(uint32_t index) const = 0;

		// ��ȡ��������������������ÿһ��������������
		virtual uint32_t GetTextureCount() const = 0;

		// ��ȡ Vertex Shader ��������
		virtual const float* GetVSConstant() const = 0;

		// ��ȡ Vertex Shader �������ݴ�С
		virtual uint32_t GetVSConstantLength() const = 0;

		// ��ȡ Pixel Shader ��������
		virtual const float* GetPSConstant() const = 0;

		// ��ȡ Pixel Shader �������ݴ�С
		virtual uint32_t GetPSConstantLength() const = 0;

		// ��ǰ�İ�͸�����״̬
		virtual BlendMode GetBlendMode() const = 0;
	};

	// ������ʣ����������������Ⱦ��Դ�Ķ���
	// һ�����ʿ��ܻ��Ϊ�����ͬ��Pass��������Ӱ����shaodowmap pass �� rendering pass��
	// ���ʻᱣ���������ݣ�һ�㲻�ܲ�ͬ������ͨ��һ��mesh
	class G2DAPI Material : public GObject
	{
	public:
		// ��Ⱦ������ɫ������Ĳ���
		static Material* CreateColorTexture();

		//ֻ��Ⱦ����Ĳ���
		static Material* CreateSimpleTexture();

		// ֻ��Ⱦ������ɫ�Ĳ���
		static Material* CreateSimpleColor();

		// �ͷ��ڴ棬ֻ�ܵ���һ��
		// �û���Ҫ�ֶ�����
		virtual void Release() = 0;

		// ����������ȡPass
		// �����ṩ��(һ��)ֻ��һ��Pass
		// ��Filter��أ��ữ��ߣ����ж��Pass
		virtual Pass* GetPassByIndex(uint32_t index) const = 0;

		// ��ȡ������Pass����
		virtual uint32_t GetPassCount() const = 0;

		// �ж����������Ƿ���ȫһ��
		virtual bool IsSame(Material* other) const = 0;

		// ����һ�����ݡ�������ȫһ�²���
		virtual Material* Clone() const = 0;
	};

	// ��Ⱦ�ֲ㣬2D��Ⱦ������Ҫ��һ�����ڣ�һ����Ⱦ�ǿ���ָ����ĳ�����֮ǰ������֮��ġ�
	/// �����ʵ����ͨ��SceneNode�Ĺ�ϵȥ������������ʱû����Ƿ���Ҫ�������
	/// ���������ľ����ȼ���
	class G2DAPI RenderLayer
	{
	public:
		constexpr static uint32_t PreZ = 0x1000;
		constexpr static uint32_t BackGround = 0x2000;
		constexpr static uint32_t Default = BackGround + 0x2000;
		constexpr static uint32_t ForeGround = BackGround + 0x4000;
		constexpr static uint32_t Overlay = 0x8000;
	};

	// ��Ⱦϵͳ�ӿ�
	class G2DAPI RenderSystem : public GObject
	{
	public:
		// ��nativeWindow�����ı��ʱ����Ҫ��ʾ��������ӿ�
		// ������Ⱦ ϵͳ��ص�״̬������ͶӰ����
		// �����ĳ� �����ʼ����ʱ���ṩһ���ӿڣ��������ڲ��Լ�ע��listener��Ϣ
		virtual bool OnResize(uint32_t width, uint32_t height) = 0;

		// ��Ⱦ��ʼ�ĳ�ʼ������
		// ���е�Render�ӿڶ���Ҫ������ӿڱ�����֮��ʹ��
		virtual void BeginRender() = 0;

		// ��Ⱦ��������β����
		// ���е�Render�ӿڶ���Ҫ������ӿڱ�����֮ǰʹ��
		virtual void EndRender() = 0;

		// �ṩȷ����Material Meshע����Ⱦ����
		// һ�����û��Զ����Entity��OnRender�¼���ʹ��
		virtual void RenderMesh(uint32_t layer, Mesh*, Material*, const gml::mat32&) = 0;

		// nativeWindow ��ǰ�Ĵ�С
		// �����ĳ� �����ʼ����ʱ���ṩһ���ӿ�
		virtual uint32_t GetWindowWidth() const = 0;
		virtual uint32_t GetWindowHeight() const = 0;

		// ����Ļ����ת�����������ϵ
		virtual gml::vec2 ScreenToView(const gml::coord& screen) const = 0;

		// �����������ϵת������Ļ����ϵ
		virtual gml::coord ViewToScreen(const gml::vec2 & view) const = 0;
	};
}
