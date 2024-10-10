#include "StdAfx.h"
#include "A3DHLPixelShader.h"
#include "A3DEnvironment.h"

#if defined _ANGELICA22
#include "a3dhlsl.h"
#elif defined _ANGELICA21
#include <A3DHLSLWrapper.h>
#endif

static const int HL_PSSHADER_VERSION = 2;
static const char* format_HLpsShader_TagAndVersion = "HLPixelShader version: %d";
static const char* format_HLpsShader_ShaderFile = "shaderFile: %s";
static const char* format_HLpsShader_SamplerCount = "samplerCount: %d";
static const char* format_HLpsShader_SamplerTexFile = "Sampler name: %s texFile: %s";
static const char* format_HLpsShader_SamplerState = "Sampler state: %d, %d, %d, %d, %d, %d";

bool A3DHLPixelShader::Save(AFile* file) const
{
	char line[AFILE_LINEMAXLEN];

	sprintf(line, format_HLpsShader_TagAndVersion, HL_PSSHADER_VERSION);
	file->WriteLine(line);

	sprintf(line, format_HLpsShader_ShaderFile, m_shaderFile);
	file->WriteLine(line);

	sprintf(line, format_HLpsShader_SamplerCount, m_samplerInfoMap.size());
	file->WriteLine(line);

	for (SamplerInfoMap::const_iterator it=m_samplerInfoMap.begin(); it!=m_samplerInfoMap.end(); ++it)
	{
		const char* samplerName = it->first;
		const char* texFile = it->second->GetTextureFile();
		sprintf(line, format_HLpsShader_SamplerTexFile, samplerName, texFile);
		file->WriteLine(line);

		// from version 2
		const SamplerInfo::SamplerState& samplerSate = it->second->GetSamplerState();
		sprintf(line, format_HLpsShader_SamplerState, samplerSate.addrs[0], samplerSate.addrs[1], samplerSate.addrs[2], 
				samplerSate.filters[0], samplerSate.filters[1], samplerSate.filters[2]);
		file->WriteLine(line);
	}

	return true;
}

bool A3DHLPixelShader::Load(AFile* file)
{
	ReleaseShader();
	ClearAllTextureFiles();

	char line[AFILE_LINEMAXLEN];
	DWORD readLength;

	int version;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_HLpsShader_TagAndVersion, &version) != 1 || version <= 0 || version > HL_PSSHADER_VERSION)
	{
		a_LogOutput(1, "A3DHLPixelShaderA21::Load, invalid tag or version");
		return false;
	}

	{	// read shader file
		char fileNameBuffer[AFILE_LINEMAXLEN] = "";
		file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
		sscanf(line, format_HLpsShader_ShaderFile, fileNameBuffer);
		SetShaderFile(fileNameBuffer);
	}

	// read samplers
	int samplerCount;
	file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
	if (sscanf(line, format_HLpsShader_SamplerCount, &samplerCount) != 1)
	{
		a_LogOutput(1, "A3DHLPixelShaderA21::Load, invalid Sampler Count");
		return false;
	}
	for (int iSampler=0; iSampler<samplerCount; ++iSampler)
	{
		char samplerNameBuffer[AFILE_LINEMAXLEN] = "";
		char texFileNameBuffer[AFILE_LINEMAXLEN] = "";
		file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
		if (sscanf(line, format_HLpsShader_SamplerTexFile, samplerNameBuffer, texFileNameBuffer) < 1)
		{
			a_LogOutput(1, "A3DHLPixelShaderA21::Load, invalid Sampler name or texture file name");
			return false;
		}

		if (version >= 2)
		{
			file->ReadLine(line, AFILE_LINEMAXLEN, &readLength);
			int ad[3];
			int ft[3];
			if (sscanf(line, format_HLpsShader_SamplerState, &ad[0], &ad[1], &ad[2], &ft[0], &ft[1], &ft[2]) != 6)
			{
				a_LogOutput(1, "A3DHLPixelShaderA21::Load, invalid Sampler state");
				return false;
			}
			A3DTEXTUREADDR addrs[3] = {(A3DTEXTUREADDR)ad[0], (A3DTEXTUREADDR)ad[1], (A3DTEXTUREADDR)ad[2]};
			A3DTEXTUREFILTERTYPE filters[3] = {(A3DTEXTUREFILTERTYPE)ft[0], (A3DTEXTUREFILTERTYPE)ft[1], (A3DTEXTUREFILTERTYPE)ft[2]};

            //  FIX : 因为 A3DHLSLUtil解析 hlsl文件的时候，如果sampler没指定属性，就会是默认值，默认值最早是NONE
            //  后来改成了0xFF，所以GFX文件里可能是这样的错误的值，对于非法的FILTER type，则改成LINEAR(只允许POINT和LINEAR，不是POINT，就改成LINEAR)
            for (int i = 0; i < _countof(filters); ++i)
                if (filters[i] != A3DTEXF_POINT)
                    filters[i] = A3DTEXF_LINEAR;

			SetTextureFile(samplerNameBuffer, texFileNameBuffer, addrs, filters);
		}
		else
		{
			SetTextureFile(samplerNameBuffer, texFileNameBuffer);
		}
	}

	return true;
}

A3DHLPixelShader::A3DHLPixelShader(const A3DHLPixelShader& rhs)
: m_shaderFile(rhs.m_shaderFile)
{
	for (SamplerInfoMap::const_iterator itr = rhs.m_samplerInfoMap.begin()
		; itr != rhs.m_samplerInfoMap.end()
		; ++itr)
	{
		m_samplerInfoMap[itr->first] = new SamplerInfo(*(itr->second));
	}
}
A3DHLPixelShader::~A3DHLPixelShader()
{
	ClearAllTextureFiles();
}
void A3DHLPixelShader::ListSamplers(std::vector<AString> *pNames/*out*/) const
{
	pNames->clear();
	for (SamplerInfoMap::const_iterator it=m_samplerInfoMap.begin(); it!=m_samplerInfoMap.end(); ++it)
	{
		const AString& name = it->first;
		pNames->push_back(name);
	}
}

void A3DHLPixelShader::SetTextureFile(const char* samplerName, const char* textureFile, const A3DTEXTUREADDR addrs[3], const A3DTEXTUREFILTERTYPE filters[3])
{
	SamplerInfoMap::iterator itr = m_samplerInfoMap.find(samplerName);
	if (itr == m_samplerInfoMap.end())
	{
		m_samplerInfoMap[samplerName] = new SamplerInfo();
	}
	m_samplerInfoMap[samplerName]->SetTexture(textureFile, addrs, filters);
}

void A3DHLPixelShader::SetTextureFile(const char* samplerName, const char* textureFile)
{
	static const A3DTEXTUREADDR addrs[] = {A3DTADDR_WRAP, A3DTADDR_WRAP, A3DTADDR_WRAP};
	static const A3DTEXTUREFILTERTYPE filters[] = { A3DTEXF_LINEAR, A3DTEXF_LINEAR, A3DTEXF_LINEAR};
	return SetTextureFile(samplerName, textureFile, addrs, filters);
}

const char* A3DHLPixelShader::GetTextureFile(const char* samplerName) const
{
	SamplerInfoMap::const_iterator it = m_samplerInfoMap.find(samplerName);
	if (it == m_samplerInfoMap.end())
		return NULL;
	else
		return it->second->GetTextureFile();
}
void A3DHLPixelShader::ClearAllTextureFiles()
{
	for (SamplerInfoMap::iterator itr = m_samplerInfoMap.begin()
		; itr != m_samplerInfoMap.end()
		; ++itr)
	{
		delete itr->second;
	}

	m_samplerInfoMap.clear();	
}
void A3DHLPixelShader::AppearTextures()
{
	A3DHLSL* pHLSL = GetHLSL();
	if (!pHLSL)
		return;

	//set texture
	for (SamplerInfoMap::const_iterator itr = m_samplerInfoMap.begin()
		; itr != m_samplerInfoMap.end()
		; ++itr)
	{
		SamplerInfo* pSI = itr->second;
		ASSERT(pSI && "This must be valid" );

#if defined _ANGELICA22
		pHLSL->SetTexture(itr->first, pSI->GetTexture());
#elif defined _ANGELICA21
		pHLSL->SetupTexture(itr->first, pSI->GetTexture());
#endif
	}
}

A3DHLPixelShader::SamplerInfo::~SamplerInfo()
{
	if (m_pTexture && m_pA3DEngine)
	{
		m_pA3DEngine->GetA3DTextureMan()->ReleaseTexture(m_pTexture);
	}
}

bool A3DHLPixelShader::SamplerInfo::LoadTexture(A3DEngine* pA3DEngine)
{
	if (!pA3DEngine || m_texturFile.IsEmpty()) 
		return false;

	m_pA3DEngine = pA3DEngine;
	A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();
	if (m_pTexture)
		pTexMan->ReleaseTexture(m_pTexture);

#ifdef _ANGELICA21
	pTexMan->LoadTextureFromFile(m_texturFile, &m_pTexture, A3DTF_NODOWNSAMPLE);
#else
	pTexMan->LoadTextureFromFile(m_texturFile, &m_pTexture);
#endif
	return m_pTexture != NULL;
}

class A3DHLPixelShaderNull : public A3DHLPixelShader
{
public:
	virtual ~A3DHLPixelShaderNull() {};

	virtual bool LoadShader(A3DDevice *pDevice) { return false; };
	// unload file
	virtual void ReleaseShader() {};
	virtual bool IsValid() const { return false; };
	virtual bool Appear() { return false; };
	virtual bool Disappear() { return false; };
	virtual bool ApplyConstValues() { return false; };
	virtual A3DHLPixelShader* Clone() const { return new A3DHLPixelShaderNull(); };
	virtual bool Save(AFile* file) const { return true; };
	virtual bool Load(AFile* file) { return false; };
	virtual A3DHLSL* GetHLSL() const { return NULL; }
	virtual bool IsConstantExist(const char* name) const { return false; }

protected:
	// valueCount: 1 ~ 4
	virtual bool SetConstValueFloatArr(const char* name, const float valueArr[], size_t valueCount) { return false; };
	virtual bool SetTextureFromRT(const char* name, A3DRenderTarget* rt) { return false; }
	virtual bool SetTexture(const char* samplerName, A3DTexture* pTex) { return false; }

};

#ifdef _ANGELICA21		// A3DHLPixelShader implement with Angelica 2.1
#include <A3DHLSLWrapper.h>

class A3DHLPixelShaderA21 : public A3DHLPixelShader
{
public:
	A3DHLPixelShaderA21();
	A3DHLPixelShaderA21(const A3DHLPixelShaderA21& rhs);
	A3DHLPixelShaderA21& operator = (const A3DHLPixelShaderA21& rhs);
	void Swap(A3DHLPixelShaderA21& rhs)
	{
		A3DHLPixelShader::Swap(rhs);
		std::swap(m_pHLSL, rhs.m_pHLSL);
	}
	virtual ~A3DHLPixelShaderA21();
	virtual bool LoadShader(A3DDevice *pDevice);
	virtual void ReleaseShader();
	virtual bool IsValid() const;
	virtual bool Appear();
	virtual bool Disappear();
	virtual bool ApplyConstValues();
	virtual A3DHLPixelShader* Clone() const;
	virtual A3DHLSL* GetHLSL() const { return m_pHLSL; }

	virtual void SetShaderNoReleaseFlag(bool b)
	{
		if (m_pHLSL)
			m_pHLSL->SetShaderNoRelease(b);
	}

protected:
	// valueCount: 1 ~ 4
	virtual bool SetConstValueFloatArr(const char* name, const float valueArr[], size_t valueCount);
	virtual bool SetTextureFromRT(const char* name, A3DRenderTarget* rt);
	virtual bool IsConstantExist(const char* name) const { return m_pHLSL && m_pHLSL->IsConstantExist(name); }
	virtual bool SetTexture(const char* samplerName, A3DTexture* pTex)
	{
		return m_pHLSL && m_pHLSL->SetupTexture(samplerName, pTex);
	}

private:
	A3DHLSL* m_pHLSL;
};

A3DHLPixelShaderA21::A3DHLPixelShaderA21() : m_pHLSL(NULL)
{
}

A3DHLPixelShaderA21::A3DHLPixelShaderA21(const A3DHLPixelShaderA21& rhs)
: A3DHLPixelShader(rhs)
, m_pHLSL(rhs.m_pHLSL ? rhs.m_pHLSL->Clone() : NULL)
{
}

A3DHLPixelShaderA21& A3DHLPixelShaderA21::operator = (const A3DHLPixelShaderA21& rhs)
{
	if (this == &rhs)
		return *this;

	A3DHLPixelShaderA21(rhs).Swap(*this);
	return *this;
}

A3DHLPixelShaderA21::~A3DHLPixelShaderA21()
{
	ReleaseShader();
}

bool A3DHLPixelShaderA21::LoadShader(A3DDevice* pDevice)
{
	ReleaseShader();
	if (!HasShaderFile())
		return false;

	if (!m_pHLSL)
	{
		m_pHLSL = new A3DHLSL();
	}

	if (!m_pHLSL->Load(pDevice, "", GetShaderFile(), NULL))
	{
		A3DRELEASE(m_pHLSL);
		return false;
	}

	//set texture
	for (SamplerInfoMap::const_iterator itr = GetSamplerInfoMap().begin()
		; itr != GetSamplerInfoMap().end()
		; ++itr)
	{
		SamplerInfo* pSI = itr->second;
		ASSERT(pSI && "This must be valid" );
		A3DTexture* pTex = pSI->GetTexture();
		if (!pTex)
			pSI->LoadTexture(pDevice->GetA3DEngine());

		const SamplerInfo::SamplerState& samplerState = pSI->GetSamplerState();
		m_pHLSL->SetupTexture(itr->first, pSI->GetTexture(), samplerState.filters, samplerState.addrs);
	}

	return IsValid();
}

void A3DHLPixelShaderA21::ReleaseShader()
{
	A3DRELEASE(m_pHLSL);
}

bool A3DHLPixelShaderA21::IsValid() const
{
	return m_pHLSL != NULL && m_pHLSL->IsValid();
}

bool A3DHLPixelShaderA21::Appear()
{
	AppearTextures();

	if (m_pHLSL)
		return m_pHLSL->Appear();

	return false;
}

bool A3DHLPixelShaderA21::Disappear()
{
	if (m_pHLSL)
		return m_pHLSL->Disappear();
	else
		return false;
}

bool A3DHLPixelShaderA21::ApplyConstValues()
{
	return true;
}

A3DHLPixelShader* A3DHLPixelShaderA21::Clone() const
{
	return new A3DHLPixelShaderA21(*this);
}

bool A3DHLPixelShaderA21::SetConstValueFloatArr(const char* name, const float valueArr[], size_t valueCount)
{
	if (m_pHLSL && m_pHLSL->IsConstantExist(name))
	{
		return m_pHLSL->SetupArray(name, valueArr, valueCount);
	}
	else
		return false;
}

bool A3DHLPixelShaderA21::SetTextureFromRT(const char* name, A3DRenderTarget* rt)
{
	if (m_pHLSL && m_pHLSL->IsConstantExist(name))
	{
		return m_pHLSL->SetupTextureFromRT(name, rt);
	}
	else
		return false;
}

#endif //def _ANGELICA21

#ifdef _ANGELICA22

#include "a3dhlsl.h"
class A3DHLPixelShaderA22 : public A3DHLPixelShader
{
public:
	A3DHLPixelShaderA22()
		: m_pHLSL(NULL)
		, m_pA3DDevice(NULL)
	{

	}

	A3DHLPixelShaderA22(const A3DHLPixelShaderA22& rhs)
		: A3DHLPixelShader(rhs)
		, m_pA3DDevice(rhs.m_pA3DDevice)
		, m_pHLSL(NULL)
	{
		LoadShader(rhs.m_pA3DDevice);
	}

	A3DHLPixelShaderA22& operator = (const A3DHLPixelShaderA22& rhs)
	{
		if (this == &rhs)
			return *this;

		A3DHLPixelShaderA22(rhs).Swap(*this);
		return *this;
	}

	void Swap(A3DHLPixelShaderA22& rhs)
	{
		A3DHLPixelShader::Swap(rhs);
		std::swap(m_pHLSL, rhs.m_pHLSL);
		std::swap(m_pA3DDevice, rhs.m_pA3DDevice);
	}

private:
	virtual bool LoadShader(A3DDevice *pDevice);
	// unload file
	virtual void ReleaseShader();
	virtual bool IsValid() const;
	virtual bool Appear();
	virtual bool Disappear();
	virtual bool ApplyConstValues();
	virtual A3DHLPixelShader* Clone() const;
	// valueCount: 1 ~ 4
	virtual bool SetConstValueFloatArr(const char* name, const float valueArr[], size_t valueCount);
	virtual bool SetTextureFromRT(const char* name, A3DRenderTarget* rt);
	virtual A3DHLSL* GetHLSL() const { return m_pHLSL; }
	virtual bool IsConstantExist(const char* name) const 
	{ 
		return m_pHLSL && m_pHLSL->GetConstMapByName(name) != NULL;
	}
	virtual bool SetTexture(const char* samplerName, A3DTexture* pTex)
	{
		return m_pHLSL && m_pHLSL->SetTexture(samplerName, pTex);
	}

private:
	A3DHLSL* m_pHLSL;
	A3DDevice* m_pA3DDevice;
};


bool A3DHLPixelShaderA22::LoadShader(A3DDevice *pDevice) 
{
	ReleaseShader();
	m_pA3DDevice = pDevice;

	const char* szShaderFile = GetShaderFile();
	if (!szShaderFile || !szShaderFile[0] || !pDevice)
		return false;

	m_pHLSL = pDevice->GetA3DEngine()->GetA3DHLSLMan()->LoadShader(NULL, szShaderFile, NULL);
	if (!m_pHLSL)
		return false;

	A3DEnvironment* pEnvironment = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment();
	m_pHLSL->BindCommConstData(pEnvironment->GetCCDBinder());
	//set texture
	for (SamplerInfoMap::const_iterator itr = GetSamplerInfoMap().begin()
		; itr != GetSamplerInfoMap().end()
		; ++itr)
	{
		SamplerInfo* pSI = itr->second;
		ASSERT(pSI && "This must be valid" );
		A3DTexture* pTex = pSI->GetTexture();
		if (!pTex)
			pSI->LoadTexture(pDevice->GetA3DEngine());
	}
	
	return true;
}

void A3DHLPixelShaderA22::ReleaseShader() 
{
	if (m_pA3DDevice && m_pHLSL)
		m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan()->ReleaseShader(m_pHLSL);

	m_pHLSL = NULL;
}

bool A3DHLPixelShaderA22::IsValid() const 
{ 
	return m_pHLSL != NULL; 
}

bool A3DHLPixelShaderA22::Appear() 
{ 
	if (!m_pHLSL)
		return false;

	AppearTextures();

	ASSERT( m_pA3DDevice && m_pA3DDevice->GetA3DEngine() && m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment() );
	return m_pHLSL->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());
}

bool A3DHLPixelShaderA22::Disappear() 
{ 
	if (!m_pHLSL)
		return false;

	return m_pHLSL->Disappear();
}

bool A3DHLPixelShaderA22::ApplyConstValues() 
{ 
	return true;
}

bool A3DHLPixelShaderA22::SetConstValueFloatArr(const char* name, const float valueArr[], size_t valueCount) 
{ 
	if (!m_pHLSL)
		return false;

	return m_pHLSL->SetConstantArrayF(name, reinterpret_cast<const A3DVECTOR4*>(valueArr), (valueCount + 3) / 4);
}

bool A3DHLPixelShaderA22::SetTextureFromRT(const char* name, A3DRenderTarget* rt)
{
	if (!m_pHLSL)
		return false;

	return m_pHLSL->SetTextureFromRT(name, rt);
}

A3DHLPixelShader* A3DHLPixelShaderA22::Clone() const
{ 
	return new A3DHLPixelShaderA22(*this); 
}

#endif



A3DHLPixelShader* a3d_CreateHLPixelShader()
{
#ifdef _ANGELICA21
	return new A3DHLPixelShaderA21();
#elif defined (_ANGELICA22)
	return new A3DHLPixelShaderA22();
#endif
}

