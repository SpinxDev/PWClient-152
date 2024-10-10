#ifndef _A3DHLPIXELSHADER_H_
#define _A3DHLPIXELSHADER_H_

#include <map>
#include <vector>
#include <utility>
#include <AString.h>
#include <A3DTypes.h>

#define HLSL_CONST_TEXTRANS		"g_TexTrans"

//	.x key point time line (time)
#define HLSL_CONST_TIME			"g_Time"

//	.x width .y height .zw not used
#define HLSL_CONST_SCREENSIZE	"g_ScreenSize"

//	alpha mask for alpha map with different channel (in shader, dot tex result with this mask)
//	only one channel would be used (may be .a or .r which depends)
#define HLSL_CONST_ALPHAMASK	"g_AlphaMask"

//	light direction in screen space (x(right) y(up), z(inside))
#define HLSL_CONST_SCREENLIGHTDIR	"g_ScrLightDir"

#define HLSL_CONST_GFX_DIFFUSE	"g_GfxDiffuse"

class A3DDevice;
class A3DEngine;
class AFile;
class A3DHLSL;

enum E_CMP_SHADER_TYPE
{
	ECST_NONE = 0,
	ECST_HILIGHT,
	ECST_DISSOLVE,
	ECST_DISSOLVE_EDGECOLOR,
	ECST_FLUID,
	ECST_FLUID_ADD,

	ECST_COUNT
};


//	A Wrapper class for hlsl 
class A3DHLPixelShader
{
protected:
	class SamplerInfo
	{
	public:
		explicit SamplerInfo()
			: m_pTexture(NULL)
			, m_pA3DEngine(NULL)
		{

		}

		SamplerInfo(const SamplerInfo& rhs)
			: m_pA3DEngine(rhs.m_pA3DEngine)
			, m_texturFile(rhs.m_texturFile)
			, m_pTexture(NULL)
			, m_samplerState(rhs.m_samplerState)
		{
			if (m_pA3DEngine)
			{
				// if rhs has been loaded, then rhs's m_pA3DEngine must be valid, then we load our texture here
				LoadTexture(m_pA3DEngine);
			}
		}

		SamplerInfo& operator = (const SamplerInfo& rhs)
		{
			if (this == &rhs)
				return *this;

			SamplerInfo(rhs).Swap(*this);
			return *this;
		}

		void Swap(SamplerInfo& rhs)
		{
			std::swap(m_pA3DEngine, rhs.m_pA3DEngine);
			std::swap(m_pTexture, rhs.m_pTexture);
			std::swap(m_texturFile, rhs.m_texturFile);
			std::swap(m_samplerState, rhs.m_samplerState);
		}

		~SamplerInfo();
		bool LoadTexture(A3DEngine* pA3DEngine);

		// addrs: u, v, w, filters: mag, min, mip
		void SetTexture(const char* szTexture, const A3DTEXTUREADDR addrs[3], const A3DTEXTUREFILTERTYPE filters[3])
		{
			m_texturFile = szTexture;
			for (size_t i=0; i<3; ++i)
				m_samplerState.addrs[i] = addrs[i];
			for (size_t i=0; i<3; ++i)
				m_samplerState.filters[i] = filters[i];
		}

		inline const char* GetTextureFile() const { return m_texturFile; }
		inline A3DTexture* GetTexture() const { return m_pTexture; }

		struct SamplerState
		{
			A3DTEXTUREADDR addrs[3];
			A3DTEXTUREFILTERTYPE filters[3];

			SamplerState()
			{
				for (int i=0; i<3; ++i)
					addrs[i] = A3DTADDR_WRAP;
				for (int i=0; i<3; ++i)
					filters[i] = A3DTEXF_LINEAR;
			}
		};
		const SamplerState& GetSamplerState() const { return m_samplerState; }

	private:
		A3DEngine* m_pA3DEngine;
		A3DTexture* m_pTexture;
		AString m_texturFile;
		SamplerState m_samplerState;
	};

	// sampler name -> Texture
	typedef std::map<AString, SamplerInfo*> SamplerInfoMap;
protected:
	A3DHLPixelShader(const A3DHLPixelShader& rhs);

	void Swap(A3DHLPixelShader& rhs)
	{
		AString strTmp = rhs.m_shaderFile;
		rhs.m_shaderFile = m_shaderFile;
		m_shaderFile = strTmp;
		m_samplerInfoMap.swap(rhs.m_samplerInfoMap);
	}

public:
	A3DHLPixelShader() : m_shaderFile(NULL) {};
	virtual ~A3DHLPixelShader();

	void SetShaderFile(const char* file) { m_shaderFile = file; };
	bool HasShaderFile() const { return !m_shaderFile.IsEmpty(); }

	//if shader file does not set, return ""
	const char* GetShaderFile() const { return m_shaderFile; }

	virtual bool LoadShader(A3DDevice *pDevice)=0;
	// unload file, file name is preserved
	virtual void ReleaseShader()=0;
	virtual void SetShaderNoReleaseFlag(bool b) {}

	virtual bool IsValid() const=0;
	virtual bool Appear()=0;
	void AppearTextures();
	virtual bool Disappear()=0;
	virtual bool Save(AFile* file) const;
	virtual bool Load(AFile* file);

	// buffer const value, need to ApplyConstValues()
	bool SetConstValueFloat(const char* name, float value)
	{
		float valueArr[] = {value};
		return SetConstValueFloatArr(name, valueArr, 1);
	}
	bool SetConstValueFloat2(const char* name, float value1, float value2)
	{
		float valueArr[] = {value1, value2};
		return SetConstValueFloatArr(name, valueArr, 2);
	}
	bool SetConstValueFloat3(const char* name, float value1, float value2, float value3)
	{
		float valueArr[] = {value1, value2, value3};
		return SetConstValueFloatArr(name, valueArr, 3);
	}
	bool SetConstValueFloat4(const char* name, float value1, float value2, float value3, float value4)
	{
		float valueArr[] = {value1, value2, value3, value4};
		return SetConstValueFloatArr(name, valueArr, 4);
	}

	virtual bool ApplyConstValues()=0;
	virtual A3DHLPixelShader* Clone() const=0;
	virtual A3DHLSL* GetHLSL() const = 0;
	virtual bool SetTextureFromRT(const char* name, A3DRenderTarget* rt) = 0;
	virtual bool SetTexture(const char* samplerName, A3DTexture* pTex) = 0;
	virtual bool IsConstantExist(const char* name) const = 0;

	// For editor
	void ListSamplers(std::vector<AString> *pNames/*out*/) const;
	// addrs: u, v, w, filters: mag, min, mip
	void SetTextureFile(const char* samplerName, const char* textureFile, const A3DTEXTUREADDR addrs[3], const A3DTEXTUREFILTERTYPE filters[3]);
	void SetTextureFile(const char* samplerName, const char* textureFile);
	const char* GetTextureFile(const char* samplerName) const;
	void ClearAllTextureFiles();

protected:
	const SamplerInfoMap& GetSamplerInfoMap() const { return m_samplerInfoMap; }
	// valueCount: 1 ~ 4
	virtual bool SetConstValueFloatArr(const char* name, const float valueArr[], size_t valueCount)=0;

private:
	A3DHLPixelShader& operator = (const A3DHLPixelShader& rhs);
	AString m_shaderFile;
	SamplerInfoMap m_samplerInfoMap;
};

A3DHLPixelShader* a3d_CreateHLPixelShader();

#endif //ndef A3DPixelShader_H_
