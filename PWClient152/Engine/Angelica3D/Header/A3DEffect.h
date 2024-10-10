#ifndef _A3DEFFECT_H_
#define _A3DEFFECT_H_

class A3DEffect : public A3DTexture
{
public:
	struct SAMPLER_DESC
	{
		AString		strName;
		A3DTexture* pTexture;
	};
	typedef abase::vector<SAMPLER_DESC> SamplerDescArray;
	struct MTLPARAM
	{
		AString strKey;
		AString strValue;
	};
	typedef abase::vector<MTLPARAM> ParamArray;
	typedef abase::hash_map<AString, AString> ParamDict;
	struct EFFECTFILEHEADER
	{
		DWORD cbSize;
		int nParamCount;
	};

	struct EFFECTFILEHEADER2 : public EFFECTFILEHEADER
	{
		DWORD bAlphaTexture;
	};
public:
	A3DEffect();
	virtual ~A3DEffect();

	virtual bool Init(A3DDevice* pDevice);
	virtual bool Release();
	virtual	bool Reload(bool bForceReload);
	bool UpdateTexturePtr();
	//bool ReloadAllData();

	bool Load(AFile* pFile, const char* szOriginFile);
	bool Save(AFile* pFile);

	bool Load(const char* pszFile, const char* pszOriginFile);
	bool Save(const char* pszFile = NULL);

	// CreateHLSLShader()
	// bForceLoad: 强制加载HLSL文件,即使对象中已经存在HLSL对象也重新加载,默认为false, 注意如果为true时在多线程加载/渲染时会有问题.
	// 备注:这个函数不一定每次都全新创建一个对象,如果A3DTextureMan中映射对象已经创建,则增加引用直接返回,此时HLSL对象就不为空
	static A3DEffect* CreateHLSLShader(A3DDevice* pA3DDevice, const char* pszTextureFile, const char* pszHLSLFile, bool bForceLoad = false);


	void SetAlphaTexture(bool bAlpha);
	bool Appear(const A3DCOMMCONSTTABLE* lpCommConstTable);
	bool Commit(const A3DCOMMCONSTTABLE* lpCommConstTable);
	virtual bool Appear(int nLayer=0);
	virtual bool Disappear(int nLayer=0);

	bool SetHLSL(const char* pszHLSLFile);
	bool SetMacros(AStringArray& aMacros, DWORD dwFlags);
	bool SetMacros(const char* szMacros, DWORD dwFlags);
	bool GetMacros(AString& strMacros);

	A3DTexture* GetTextureByName(const char* pszName) const;
	bool	SetTextureByName(const char* pszName, const char* pszFilename);

	bool	SetValueByString(const A3DHLSL::CONSTMAPDESC& ConstDesc, const AString& strVaule);
	bool	GetValueByString(const A3DHLSL::CONSTMAPDESC& ConstDesc, AString& strVaule);
	AString GetMtlFilename();
	AString GetHLSLFilename();
	bool	GenerateParam(ParamArray& aParam);

	// 这个设置材质的基本信息,但是Shader里不一定总会有此信息
	// 如果shader没有使用":DiffuseFactor"和":EmissiveFactor"设置会失败并返回false, 
	// 在渲染时也不会体现出效果, 但是用GetXXX仍然能取出设置过的值
	bool	SetDiffuse	(const A3DCOLOR& clr);
	bool	SetEmissive	(const A3DCOLOR& clr);


	inline	A3DHLSL*			GetHLSL		() const;
	inline	A3DHLSLCore*		GetHLSLCore	() const;
	inline	DWORD				GetCoreID	() const;

	inline	A3DCOLOR			GetSrcColor	();
	inline	A3DCOLOR			GetDestColor();
	inline	void				SetSrcColor	(A3DCOLOR clr);
	inline	void				SetDestColor(A3DCOLOR clr);
	inline	A3DTexture*			GetOriginTex() const;
	inline	const A3DCOLOR&		GetDiffuse	() const;
	inline	const A3DCOLOR&		GetEmissive	() const;
	bool						AppearTextureOnly(int nLayer=0);
	bool						HasBloomEffect();

	virtual IDirect3DBaseTexture9* GetD3DBaseTexture() const;
private:
	bool LoadFromFile(A3DDevice* pA3DDevice, const char* szRelPath, DWORD dwTimeStamp);
private:
	//A3DDevice*			m_pA3DDevice;
	A3DHLSL*			m_pHLSL;
	SamplerDescArray	m_aSampDesc;
	A3DTexture*			m_pOriginTex;	// 这个文件映射的原来的纹理
	//A3DCOLOR			m_crSource;
	//A3DCOLOR			m_crDest;
	A3DCOLOR			Diffuse;
	A3DCOLOR			Emissive;
	bool ReleaseSamplerDesc();
};

//////////////////////////////////////////////////////////////////////////

A3DHLSL* A3DEffect::GetHLSL() const
{
	return m_pHLSL;
}

inline A3DHLSLCore* A3DEffect::GetHLSLCore() const
{
	return m_pHLSL ? m_pHLSL->GetCore() : NULL;
}

inline DWORD A3DEffect::GetCoreID() const
{
	return m_pHLSL->GetCore()->GetID();
}
inline const A3DCOLOR& A3DEffect::GetDiffuse() const
{
	return Diffuse;
}
inline const A3DCOLOR& A3DEffect::GetEmissive() const
{
	return Emissive;
}

//inline A3DCOLOR A3DEffect::GetSrcColor()
//{
//	return m_crSource;
//}
//
//inline A3DCOLOR A3DEffect::GetDestColor()
//{
//	return m_crDest;
//}
//
//inline void A3DEffect::SetSrcColor(A3DCOLOR clr)
//{
//	m_crSource = clr;
//}
//
//inline void A3DEffect::SetDestColor(A3DCOLOR clr)
//{
//	m_crDest = clr;
//}
inline A3DTexture* A3DEffect::GetOriginTex() const
{
	return m_pOriginTex;
}
#endif // _A3DMATERIAL_INSTANCE_H_