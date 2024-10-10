/*
 * FILE: A3DHLSLDataSrc.h
 *
 * DESCRIPTION: DX9 High Level Shading Language Data Source
 * HLSL 数据源对象,用来提供一HLSL源文件为单位的各种功能,包括宏组合和入口函数组合
 *
 * CREATED BY: liuchenglong 2012-02-23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DHLSL_DATA_SOURCE_H_
#define _A3DHLSL_DATA_SOURCE_H_

class A3DHLSLDataSrc;

// A3DHLSLDataSrc::InitFromHLSL 参数
#define A3DHLSLDATASRC_ASMALLINST	0x00000001
#define A3DHLSLDATASRC_RELOAD		0x00000002

class A3DHLSLDataSrcInclude : public ID3DXInclude
{
private:
	A3DHLSLDataSrc*	m_pHLSLDataSrc;
	AString			m_strSystemDir;
	AString			m_strLocalDir;
	AStringArray	m_aIncludeFiles;
public:
	A3DHLSLDataSrcInclude(A3DHLSLDataSrc* pDataSrc);
	~A3DHLSLDataSrcInclude();

	bool Init();
	bool Release();

	void SetDir(D3DXINCLUDE_TYPE IncludeType, const AString& strDir);

public:
	AStringArray* GetIncludeFiles();
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	STDMETHOD(Close)(THIS_ LPCVOID pData);
};

struct SHADERINSTANCE{
	DWORD                    dwEntryID;
	DWORD                    dwProfile;
	AString					 strMacro;		// 是不是只用ID就够了?
	LPDIRECT3DPIXELSHADER9   pPShader;		// 和 pVShader 不同时有效
	LPDIRECT3DVERTEXSHADER9  pVShader;		// 和 pPShader 不同时有效
	LPD3DXBUFFER			 pShaderBuffer;
	LPD3DXCONSTANTTABLE	     pConstants;
	SHADERINSTANCE() : dwEntryID(NULL), pPShader(NULL), pVShader(NULL), pConstants(NULL), pShaderBuffer(NULL){}
};

#define MPF_SHADER20		0x1000
#define MPF_SHADER30		0x2000

class A3DHLSLDataSrc : public A3DObject
{
	friend class A3DHLSLCore;
public:
	struct MACROPROP
	{
		AString strMacroName;	// 宏的名字
		AString strDesc;		// 描述名字,如果是*开头的则是给程序用的宏
		union
		{
			struct{
				WORD	wGroup;			// 组的掩码, 最大16(0~15)组, 相同组会互斥
				WORD	wFlags;			// 预留标志, 参考 MPF_SHADER20
			};
			DWORD dwData;
		};
	};

	//struct NAMESEMANTICPAIR
	//{
	//	AString strName;
	//	AString strSemantic;
	//};

	struct ENTRYNAME
	{
		char	szProfile[8];
		AString strEntryName;
		DWORD	dwEntryID;
	};


	typedef A3DHLSLCore::ANNOTATION						ANNOTATION;
	typedef abase::vector<ANNOTATION>					AnnotationArray;
	typedef A3DHLSLCore::PARAMETER_DESC					PARAMETER_DESC;
	typedef A3DHLSLCore::SAMPLER_STATE					SAMPLER_STATE;

	typedef abase::vector<SHADERINSTANCE*>				ShaderInstArray;
	typedef abase::hash_map<AString, PARAMETER_DESC>	Name2ParamDict;
	typedef abase::vector<MACROPROP>					MacroPropArray;
	typedef abase::hash_map<AString, AString>			NameSemanticDict;
	typedef abase::vector<ENTRYNAME>					EntryNameArray;
	typedef const SAMPLER_STATE*						LPCSAMPLER_STATE;

private:
	A3DDevice*			m_pA3DDevice;
	A3DHLSLMan*			m_pHLSLMgr;
	int					m_nRefCount;
	ShaderInstArray		m_aInstance;
	Name2ParamDict*		m_pName2ParamDict;
	MacroPropArray		m_aMacroProp;
	NameSemanticDict	m_NameSemanticDict;
	EntryNameArray		m_aEntryName;
	SAMPLER_STATE		m_aSampState[MAX_SAMPLERCOUNT];
	static	ENTRYNAME	s_DefVertexEntry;
	static	ENTRYNAME	s_DefPixelEntry;
	//UINT				m_nMaxSamplerIdx;
private:
	inline Name2ParamDict* GetParamDict();
	int		ReadCodesFromFile	(AFile* pShaderFile, BYTE** ppCodes);
	bool	LoadHLSL			(AFile* pShaderFile, DWORD dwFlags);
	bool	IntLoadShaderInstMem(SHADERINSTANCE& ShaderInst, const BYTE* pShaderCode, size_t uCodeLen, const char* szFuncName, const char* szProfile, const MacroArray& aMacros, A3DHLSLDataSrcInclude* pInclude);
	bool	IntLoadShaderInst	(SHADERINSTANCE& ShaderInst, AFile* pShaderFile, const char* szFuncName, const MacroArray& aMacros, A3DHLSLDataSrcInclude* pInclude);
	void	GenerateSamplerState();
	void	GetDefaultTexturePath(const char* szTextureName, SAMPLER_STATE* lpSamplerState);
	bool	AssembleAllInst		(const BYTE* pShaderCode, size_t uCodeLen, LPD3DXINCLUDE lpInclude);
	bool	CreateShader		(SHADERINSTANCE& ShaderInst, const ENTRYNAME* pEntry);
	bool	CleanupParameter	();
	bool	ReloadAllInst		(const BYTE* pShaderCode, size_t uCodeLen, const BYTE* pErrorShaderCode, size_t uErrorCodeLen, LPD3DXINCLUDE lpInclude);
public:
	A3DHLSLDataSrc(A3DDevice* pA3DDevice, A3DHLSLMan* pHLSLMgr, const char* szShaderFile);

	bool	InitFromHLSL		(DWORD dwFlags);
	bool	InitFromObj			();

	int		AddRef				();
	int		Release				();

	bool	Cleanup				();
	const ENTRYNAME*		FindProfileByEntryName	(const char* szEntryName);
	const ENTRYNAME*		FindProfileByEntryID	(DWORD dwEntryID);
	const SHADERINSTANCE*	FindShaderInst			(const char* szFuncName, const MacroArray& aMacros);
	const SHADERINSTANCE*	LoadShaderInst			(const char* szFuncName, const MacroArray& aMacros);
	const SHADERINSTANCE*	LoadShaderInst			(const char* szFuncName, const char* szMacros);	// szMacros = "MACRO1;MACRO2=Def;MACRO3;..."

	bool	BuildEffect			(const BYTE* pShaderCode, size_t uCodeLen, LPD3DXINCLUDE lpInclude, DWORD dwFlags);
	bool	GenerateAnnotation	(LPD3DXBASEEFFECT lpD3DXEffect);

	bool	ReloadShaderInst	();
	bool	Save				(AFile* pFile);
	bool	Load				(AFile* pFile);
	
	bool	ProvingMacro		(const AStringArray& aMacros, AStringArray& aOutMacros, AStringArray* pMutexMacros = NULL);	// 对于互斥宏, 前面出现的总是排斥掉后面的宏

	const MACROPROP*			FindMacro(const char* szMacro);
	static bool					GenCompiledFilename	(const char* szShaderFile, AString& strFilename);
	static DWORD				ProfileStringToFourCC(const char* szProfile);
	static bool					ProfileFourCCToString(DWORD dwCode, char* szProfile);	// szProfile约定使用8个字节
	inline	MacroPropArray*		GetMacroArray();
	inline const SAMPLER_STATE*	GetSamplerStateArray();
	inline NameSemanticDict&	GetNameSemanticDict();
	inline int					GetShaderInstCount();
};

//////////////////////////////////////////////////////////////////////////
inline A3DHLSLDataSrc::Name2ParamDict* A3DHLSLDataSrc::GetParamDict()
{
	return m_pName2ParamDict;
}
inline A3DHLSLDataSrc::MacroPropArray* A3DHLSLDataSrc::GetMacroArray()
{
	return &m_aMacroProp;
}
inline const A3DHLSLDataSrc::SAMPLER_STATE*	A3DHLSLDataSrc::GetSamplerStateArray()
{
	return m_aSampState;
}
inline A3DHLSLDataSrc::NameSemanticDict& A3DHLSLDataSrc::GetNameSemanticDict()
{
	return m_NameSemanticDict;
}
inline int A3DHLSLDataSrc::GetShaderInstCount()
{
	return (int)m_aInstance.size();
}
#endif // _A3DHLSL_DATA_SOURCE_H_