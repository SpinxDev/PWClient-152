/*
* FILE: A3DHLSLUtil.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/4/6
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DHLSLUtil_H_
#define _A3DHLSLUtil_H_

#include <list>
#include <vector>
#include "A3DPlatform.h"
#include "AString.h"
#include "A3DHLSL.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

enum UIParamWidget
{
	UIUnknown = -1,
	UISlider,
	UIColor,
	UIFile,
	UIVector,
	UIBool,
};

enum UIParamType
{
	PARAM_VOID  = D3DXPT_VOID,
	PARAM_BOOL  = D3DXPT_BOOL,
	PARAM_FLOAT = D3DXPT_FLOAT,
	PARAM_INT   = D3DXPT_INT,
	PARAM_UNSUPPORTED = D3DXPT_UNSUPPORTED,
};

class A3DEngine;
struct hlsl_parameter_t;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DHLSLAnnotation
//	
///////////////////////////////////////////////////////////////////////////

class A3DHLSLAnnotation
{
private:

	hlsl_parameter_t* m_param;

public:

	A3DHLSLAnnotation();
	~A3DHLSLAnnotation();

	bool Init(const A3DHLSLCore::ANNOTATION* pAnnotation);
	const char* GetName() const;
	int GetType() const;
	const char* GetValueAsStr() const;
	BOOL GetValueAsBool() const;
	INT GetValueAsInt() const;
	FLOAT GetValueAsFloat() const;

private:
	A3DHLSLAnnotation(const A3DHLSLAnnotation&);
	A3DHLSLAnnotation& operator = (const A3DHLSLAnnotation&);
};

typedef std::list<A3DHLSLAnnotation*> AnnotationList;

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DHLSLUIParam
//	
///////////////////////////////////////////////////////////////////////////

class A3DHLSLUIParam
{
	AString m_strName;					//	The parameter's name
	AString m_strUIName;				//	string UIName = ""
	AString m_strUIDesc;				//	string UIDesc = ""
	//	Useful when UI widget is slider
	float m_fMinVal;					//	float fMinVal = ...
	float m_fMaxVal;					//	float fMaxVal = ...
	float m_fStep;						//	float fStep	  = ...
	bool m_bEditable;					//	bool bEditable = true / false;
	UIParamWidget m_UIWidget;			//	string UIWidget = "Slider/Color/File/Vector/Bool";

	//	Other parameters parsed from other part of information
	int m_iDimension;					//	the vector dimension of the param itself
	UIParamType m_ParamType;			//	parameter type, currently only int, float are possible
	hlsl_parameter_t* m_param;			//	the parameter's value

public:
	A3DHLSLUIParam();
	~A3DHLSLUIParam();

	A3DHLSLUIParam& operator = (const A3DHLSLUIParam& rhs);
	A3DHLSLUIParam(const A3DHLSLUIParam& rhs);

	void Swap(A3DHLSLUIParam& rhs);

	bool Init(const A3DHLSL::CONSTMAPDESC& d3dxDesc, const AnnotationList& annotations);

	const char* GetName() const { return m_strName; }
	const char* GetUIName() const { return m_strUIName; }
	const char* GetUIDesc() const { return m_strUIDesc; }
	UIParamWidget GetUIParamWidget() const { return m_UIWidget; }
	float GetMinVal() const { return m_fMinVal; }
	float GetMaxVal() const { return m_fMaxVal; }
	float GetStep() const { return m_fStep; }
	bool GetEditable() const { return m_bEditable; }
	//	the dimension if the parameter is a vector type (float4, int4, etc)
	int GetDimension() const { return m_iDimension; }
	bool GetValueAsIntArray(int* pBuf, int iDim) const;
	bool GetValueAsFloatArray(float* pBuf, int iDim) const;
	bool GetValueAsString(AString* pStr) const;
	UIParamType GetParamType() const { return m_ParamType; }

private:
	bool InitFromAnnotationLst(const AnnotationList& annotations);
};

class A3DHLSLTexParams
{
public:
	class TEXTURE_PARAM 
	{
	public:
		explicit TEXTURE_PARAM(const char* szParam) : m_strParamName(szParam)
			, m_bEditable(false)
		{

		}

		void SetTexFile(const char* szDefTex) { m_strTexture = szDefTex; m_bEditable = true; }
		const char* GetTexFile() const { return m_strTexture; }
		void SetUIName(const char* szUIName) { m_strUIName = szUIName; m_bEditable = true; }
		const char* GetUIName() const { return m_strUIName; }
		bool GetEditable() const { return m_bEditable; }
		const char* GetParamName() const { return m_strParamName; }
	private:
		AString m_strParamName;
		AString m_strTexture;
		bool m_bEditable;
		AString m_strUIName;
	};

	class SAMPLER_PARAM
	{
	public:
		explicit SAMPLER_PARAM(const char* szParam);
		void SetTextureFilterType(int nIdx, A3DTEXTUREFILTERTYPE filter) { m_aUVWFilters[nIdx] = filter; }
		A3DTEXTUREFILTERTYPE GetTextureFilterType(int nIdx) const { return m_aUVWFilters[nIdx]; }
		void SetTextureAddr(int nIdx, A3DTEXTUREADDR addr) { m_aUVWAddrs[nIdx] = addr; }
		A3DTEXTUREADDR GetTextureAddr(int nIdx) const { return m_aUVWAddrs[nIdx]; }
		const char* GetParamName() const { return m_strParamName; }
		void SetUseTex(const char* szUseTex) { m_strUseTex = szUseTex; }
		const char* GetUseTex() const { return m_strUseTex; }
		const A3DTEXTUREFILTERTYPE* GetTexFilters() const { return m_aUVWFilters; }
		const A3DTEXTUREADDR* GetTexAddr() const { return m_aUVWAddrs; }

	private:
		AString m_strParamName;
		AString m_strUseTex;
		A3DTEXTUREFILTERTYPE m_aUVWFilters[3];
		A3DTEXTUREADDR m_aUVWAddrs[3];
	};

public:
	A3DHLSLTexParams() {}
	virtual ~A3DHLSLTexParams();
	A3DHLSLTexParams(const A3DHLSLTexParams&);
	A3DHLSLTexParams& operator = (const A3DHLSLTexParams&);

	void Swap(A3DHLSLTexParams& rhs);

	int GetTextureParamNum() const { return static_cast<int>(m_Textures.size()); }
	const TEXTURE_PARAM* GetTextureParam(int nIdx) const { return m_Textures[nIdx]; }
	int GetSamplerParamNum() const { return static_cast<int>(m_Samplers.size()); }
	const SAMPLER_PARAM* GetSamplerParam(int nIdx) const { return m_Samplers[nIdx]; }
	const TEXTURE_PARAM* GetTextureParamByName(const char* szTexParam) const { return GetTextureParamByName(szTexParam, NULL); }
	const TEXTURE_PARAM* GetTextureParamByName(const char* szTexParam, int* piIndex) const;
	const SAMPLER_PARAM* GetSamplerParamByName(const char* szSamParam) const;

	bool AddTextureParam(const char* szParam, const AnnotationList& annotations);
	bool AddSamplerParam(const char* szParam, const AnnotationList& annotations);

	//	Editor interfaces
	typedef std::pair<AString, AString> SamplerTexturePair;
	typedef std::vector<SamplerTexturePair> SamplerTextureArray;
	SamplerTextureArray GenerateSamplerTexturePairs() const;

	const char* GetTextureUIName(int nIdx) const { return GetTextureParam(nIdx)->GetUIName(); }
	const char* GetTextureFile(int nIdx) const { return GetTextureParam(nIdx)->GetTexFile(); }
	void SetTextureFile(int nIdx, const char* szTexFile) { m_Textures[nIdx]->SetTexFile(szTexFile); }

private:
	typedef std::vector<TEXTURE_PARAM*> TextureParamArray;
	TextureParamArray m_Textures;

	typedef std::vector<SAMPLER_PARAM*> SamplerParamArray;
	SamplerParamArray m_Samplers;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

struct HLSLQUERYPARAMS
{
	A3DEngine* pA3DEngine;			//	initialized engine ( we need to create shader instance to know more information )
	const char* szHLSLFile;			//	hlsl file name with macro
	const char* szMacro;			//	macro
	const char* szMainFunc;			//	selected main func, if keep empty, the default "ps_main" will be used
	bool	bIsVS;					//	bIsVS : true - vertex shader, false - pixel shader
};

//	Compile the hlsl file as A3DPixelShader/A3DVertexShader and only take out the params that are actually used 
bool a3d_CreateHLSLUIParamsFromHLSL(const HLSLQUERYPARAMS& params, std::list<A3DHLSLUIParam>* pUIParams, A3DHLSLTexParams* pTexParams);
bool a3d_CreateHLSLUIParamsFromHLSL(A3DHLSL* pHLSL, std::list<A3DHLSLUIParam>* pUIParams, A3DHLSLTexParams* pTexParams);

#endif	//	_A3DHLSLUtil_H_

