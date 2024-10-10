/*
* FILE: A3DHLSLUtil.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/4/6
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#include "A3DHLSL.h"
#include "A3DHLSLUtil.h"
#include "A3DEngine.h"
#include "ALog.h"
#include <queue>
#include <memory> // For std::unique_ptr
#include <algorithm> // For std::copy
#include <iterator> // For std::back_inserter

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

enum ANNOTATION_TYPE
{
	ANNOTATION_TYPE_UNKNOWN = -1,
	ANNOTATION_TYPE_STRING,
	ANNOTATION_TYPE_INT,
	ANNOTATION_TYPE_BOOL,
	ANNOTATION_TYPE_FLOAT,
};

struct hlsl_parameter_t
{
	AString m_strName;					//	Annotation's name
	D3DXPARAMETER_CLASS m_Class;		//	Class
	ANNOTATION_TYPE	m_Type;			//	Type
	union {
		void*	m_pData;				//	Data storage
		BOOL	m_bValue;				
		INT		m_iValue;
		FLOAT	m_fValue;
		INT		m_aIntArray[4];
		FLOAT	m_aFltArray[4];
	};
	UINT	m_cbDateSize;				//	Data size

	hlsl_parameter_t()
		: m_pData(NULL)
		, m_Type(ANNOTATION_TYPE_UNKNOWN)
	{
		memset(m_aIntArray, 0, sizeof(m_aIntArray));
	}

	~hlsl_parameter_t()
	{
		Release();
	}

	bool InitFromConstDesc(const A3DHLSL::CONSTMAPDESC& constMapDesc);
	bool InitFromAnnotation(const A3DHLSLCore::ANNOTATION* pAnnotation);

private:
	void Release()
	{
		if (m_Type == ANNOTATION_TYPE_STRING)
		{
			delete m_pData;
			m_pData = NULL;
		}
	}
	bool InitDataFromD3DXParameterDesc(const D3DXPARAMETER_DESC& Desc);
};

bool hlsl_parameter_t::InitFromConstDesc(const A3DHLSL::CONSTMAPDESC& constMapDesc)
{
	const D3DXCONSTANT_DESC& constDesc = constMapDesc.cd.cd;
	m_strName = constDesc.Name;
	m_Class = constDesc.Class;
	m_cbDateSize = constDesc.Bytes;
	switch (constDesc.Type)
	{
	case D3DXPT_STRING:
		{
			m_pData = new char[m_cbDateSize];
			strncpy((LPSTR)m_pData, (LPCSTR)constDesc.DefaultValue, m_cbDateSize);
			m_Type = ANNOTATION_TYPE_STRING;
		}
		break;

	case D3DXPT_INT:
		if (m_Class == D3DXPC_SCALAR)
		{
			ASSERT( m_cbDateSize == sizeof(INT) );
			m_iValue = *(INT*)constDesc.DefaultValue;
		}
		else if (m_Class == D3DXPC_VECTOR)
		{
			ASSERT( m_cbDateSize % sizeof(INT) == 0 && m_cbDateSize / sizeof(INT) <= 4 );
			memcpy(m_aIntArray, constDesc.DefaultValue, m_cbDateSize);
		}
		else
		{
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}

		m_Type = ANNOTATION_TYPE_INT;
		break;

	case D3DXPT_FLOAT:
		if (m_Class == D3DXPC_SCALAR)
		{
			ASSERT( m_cbDateSize == sizeof(float) );
			m_fValue = *(FLOAT*)constDesc.DefaultValue;
		}
		else if (m_Class == D3DXPC_VECTOR)
		{
			ASSERT( m_cbDateSize % sizeof(float) == 0 && m_cbDateSize / sizeof(float) <= 4 );
			memcpy(m_aFltArray, constDesc.DefaultValue, m_cbDateSize);
		}
		else 
		{
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}
		m_Type = ANNOTATION_TYPE_FLOAT;
		break;

	case D3DXPT_BOOL:
		if (m_Class == D3DXPC_SCALAR)
		{
			ASSERT( m_cbDateSize == sizeof(BOOL) );
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}
		else
		{
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}
		m_Type = ANNOTATION_TYPE_BOOL;
		break;

	default:

		m_Type = ANNOTATION_TYPE_UNKNOWN;
		return false;
	}

	return true;
}

bool hlsl_parameter_t::InitFromAnnotation(const A3DHLSLCore::ANNOTATION* pAnnotation)
{
	if (!pAnnotation)
		return false;

	m_strName = pAnnotation->Name;
	m_Class = pAnnotation->Class;
	m_cbDateSize = pAnnotation->cbDateSize;

	switch (pAnnotation->Type)
	{
	case D3DXPT_STRING:
		{
			m_pData = new char[m_cbDateSize];
			strncpy((LPSTR)m_pData, (LPCSTR)pAnnotation->pData, m_cbDateSize);
			m_Type = ANNOTATION_TYPE_STRING;
		}
		break;

	case D3DXPT_INT:
		if (m_Class == D3DXPC_SCALAR)
		{
			ASSERT( m_cbDateSize == sizeof(int) );
			m_iValue = pAnnotation->nValue;
		}
		else if (m_Class == D3DXPC_VECTOR)
		{
			ASSERT( m_cbDateSize % sizeof(INT) == 0 && m_cbDateSize / sizeof(INT) <= 4 );
			memcpy(m_aIntArray, pAnnotation->pData, m_cbDateSize);
		}
		else
		{
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}
		m_Type = ANNOTATION_TYPE_INT;
		break;

	case D3DXPT_FLOAT:
		if (m_Class == D3DXPC_SCALAR)
		{
			ASSERT( m_cbDateSize == sizeof(float) );
			m_fValue = pAnnotation->fValue;
		}
		else if (m_Class == D3DXPC_VECTOR)
		{
			ASSERT( m_cbDateSize % sizeof(float) == 0 && m_cbDateSize / sizeof(float) <= 4 );
			memcpy(m_aFltArray, pAnnotation->pData, m_cbDateSize);
		}
		else 
		{
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}
		m_Type = ANNOTATION_TYPE_FLOAT;
		break;

	case D3DXPT_BOOL:
		if (m_Class == D3DXPC_SCALAR)
		{
			ASSERT( m_cbDateSize == sizeof(BOOL) );
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}
		else
		{
			ASSERT( FALSE && "Not yet implemented" );
			return false;
		}
		m_Type = ANNOTATION_TYPE_BOOL;
		break;

	default:

		m_Type = ANNOTATION_TYPE_UNKNOWN;
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


static bool getValueByName(A3DHLSLAnnotation* annotation, const char* szParamName, AString* pStr)
{
	if (strcmp(annotation->GetName(), szParamName) == 0)
	{
		if (annotation->GetType() == ANNOTATION_TYPE_STRING)
		{
			*pStr = annotation->GetValueAsStr();
			return true;
		}
		else
		{
			ASSERT( FALSE && "Wrong type, a string is expected here" );
			return false;
		}
	}

	return true;
}

static bool getValueByName(A3DHLSLAnnotation* annotation, const char* szParamName, bool* pBool)
{
	if (strcmp(annotation->GetName(), szParamName) == 0)
	{
		if (annotation->GetType() == ANNOTATION_TYPE_BOOL)
		{
			*pBool = (annotation->GetValueAsBool() != FALSE);
			return true;
		}
		else
		{
			ASSERT( FALSE && "Wrong type, a bool is expected here" );
			return false;
		}
	}

	return true;
}

static bool getValueByName(A3DHLSLAnnotation* annotation, const char* szParamName, float* pFloat)
{
	if (strcmp(annotation->GetName(), szParamName) == 0)
	{
		if (annotation->GetType() == ANNOTATION_TYPE_FLOAT)
		{
			*pFloat = annotation->GetValueAsFloat();
			return true;
		}
		else
		{
			ASSERT( FALSE && "Wrong type, a float here expected" );
			return false;
		}
	}

	return true;
}

bool a3d_CreateHLSLUIParamsFromHLSL(const HLSLQUERYPARAMS& params, std::list<A3DHLSLUIParam>* pUIParams, A3DHLSLTexParams* pTexParams)
{
	if (!params.pA3DEngine || !params.pA3DEngine->GetA3DHLSLMan())
		return false;

	A3DHLSLMan* pHLSLMan = params.pA3DEngine->GetA3DHLSLMan();
	A3DHLSLScopedPtr pHLSL(params.bIsVS 
		? pHLSLMan->LoadShader(params.szHLSLFile, params.szMainFunc, NULL, NULL, params.szMacro, 0)
		: pHLSLMan->LoadShader(NULL, NULL, params.szHLSLFile, params.szMainFunc, params.szMacro, 0));

	if (!pHLSL.get() || !pHLSL->GetCore())
		return false;

	pHLSLMan->RebuildByKeyword(params.szHLSLFile);

	std::list<A3DHLSLUIParam> uiParams;
	if (!a3d_CreateHLSLUIParamsFromHLSL(pHLSL.get(), &uiParams, pTexParams))
		return false;

	if (pUIParams)
	{
		std::copy(uiParams.begin(), uiParams.end(), std::back_inserter(*pUIParams));
	}
	return true;
}

bool IsTypeSupportedByUIParam(D3DXPARAMETER_TYPE paramType)
{
	switch (paramType)
	{
	case D3DXPT_BOOL:
	case D3DXPT_INT:
	case D3DXPT_FLOAT:
	case D3DXPT_STRING:
	case D3DXPT_TEXTURE:
	case D3DXPT_TEXTURE1D:
	case D3DXPT_TEXTURE2D:
	case D3DXPT_TEXTURE3D:
	case D3DXPT_TEXTURECUBE:
		return true;
	default:
		return false;
	}
}

enum 
{
	TEXTURE,
	SAMPLER,
	OTHERS,
};

static int CheckParamType(D3DXPARAMETER_TYPE paramType)
{
	switch (paramType)
	{
	case D3DXPT_TEXTURE:
	case D3DXPT_TEXTURE1D:
	case D3DXPT_TEXTURE2D:
	case D3DXPT_TEXTURE3D:
	case D3DXPT_TEXTURECUBE:
		return TEXTURE;
	case D3DXPT_SAMPLER:
	case D3DXPT_SAMPLER1D:
	case D3DXPT_SAMPLER2D:
	case D3DXPT_SAMPLER3D:
	case D3DXPT_SAMPLERCUBE:
		return SAMPLER;
	default:
		return OTHERS;
	}
}

bool a3d_CreateHLSLUIParamsFromHLSL(A3DHLSL* pHLSL, std::list<A3DHLSLUIParam>* pUIParams, A3DHLSLTexParams* pTexParams)
{
	A3DHLSLCore* pCore = pHLSL->GetCore();
	const A3DHLSLCore::Name2ParamDict* pName2ParamDict = pCore->GetParamDict();
	if (!pName2ParamDict)
		return false;

	struct PARAM_ENTRY 
	{
		PARAM_ENTRY(const char* szParamName, const A3DHLSL::CONSTMAPDESC* pDesc)
			: m_strParamName(szParamName)
			, m_pDesc(pDesc)
		{

		}

		inline const char* GetParamName() const { return m_strParamName; }
		inline const A3DHLSL::CONSTMAPDESC* GetDesc() const { return m_pDesc; }

	private:
		AString m_strParamName;
		const A3DHLSL::CONSTMAPDESC* m_pDesc;
	};

	const A3DHLSL::CONSTMAPDESC* pConstMapDesc = pHLSL->GetConstMapArray();
	const int nConstDescCnt = pHLSL->GetConstCount();
	std::queue<PARAM_ENTRY> aParmEntries;
	for (int nConstDescIdx = 0; nConstDescIdx < nConstDescCnt; ++nConstDescIdx)
	{
		const A3DHLSL::CONSTMAPDESC& constMapDesc = pConstMapDesc[nConstDescIdx];
		aParmEntries.push(PARAM_ENTRY(constMapDesc.cd.cd.Name, &constMapDesc));
	}

	while (!aParmEntries.empty())
	{
		PARAM_ENTRY pe = aParmEntries.front();
		A3DHLSLCore::Name2ParamDict::const_iterator itr = pName2ParamDict->find(pe.GetParamName());
		if (itr == pName2ParamDict->end())
		{
			aParmEntries.pop();
			continue;
		}

		//	create annotation list
		AnnotationList aAnnotationLst;
		const A3DHLSLCore::PARAMETER_DESC& paramDesc = itr->second;
		const A3DHLSLCore::AnnotationArray& aAnnotations = paramDesc.aAnnotation;
		for (A3DHLSLCore::AnnotationArray::const_iterator itr = aAnnotations.begin()
			; itr != aAnnotations.end()
			; ++itr)
		{
			std::auto_ptr<A3DHLSLAnnotation> pAnnotation(new A3DHLSLAnnotation());
			if (!pAnnotation->Init(&(*itr)))
				continue;

			aAnnotationLst.push_back(pAnnotation.release());
		}

		switch (CheckParamType(paramDesc.Type))
		{
		case TEXTURE:
			if (pTexParams)
				pTexParams->AddTextureParam(pe.GetParamName(), aAnnotationLst);
			break;
		case SAMPLER:
			if (pTexParams)
			{
				pTexParams->AddSamplerParam(pe.GetParamName(), aAnnotationLst);

				for (AnnotationList::const_iterator itr = aAnnotationLst.begin()
					; itr != aAnnotationLst.end()
					; ++itr)
				{
					if ((*itr)->GetType() != ANNOTATION_TYPE_STRING)
						continue;

					if (strcmp((*itr)->GetName(), "Texture") == 0)
					{
						AString strTexParamName = (*itr)->GetValueAsStr();
						aParmEntries.push(PARAM_ENTRY(strTexParamName, NULL));
						break;
					}
				}
			}
			break;
		case OTHERS:
			if (!aAnnotationLst.empty() && pe.GetDesc())
			{
				A3DHLSLUIParam uiParam;
				if (uiParam.Init(*pe.GetDesc(), aAnnotationLst))
					pUIParams->push_back(uiParam);
				else
				{
					a_LogOutput(1, "%s, Failed to init ui param for %s", __FUNCTION__, pe.GetParamName());
				}
			}
			else
			{
				//	no annotation
				//	skip
			}
			break;
		}

		for (AnnotationList::iterator itr = aAnnotationLst.begin(); itr != aAnnotationLst.end(); ++itr)
		{
			delete *itr;
		}
		aAnnotationLst.clear();

		aParmEntries.pop();
	}

	//for (int nIdx = 0; nIdx < nConstDescCnt; ++nIdx)
	//{
	//	const A3DHLSL::CONSTMAPDESC& constMapDesc = pConstMapDesc[nIdx];
	//	//if (constMapDesc.uMemberOffset >= 0)
	//	//	continue;

	//	A3DHLSLCore::Name2ParamDict::const_iterator itr = pName2ParamDict->find(constMapDesc.cd.cd.Name);
	//	if (itr == pName2ParamDict->end())
	//		continue;

	//	//	create annotation list
	//	AnnotationList aAnnotationLst;
	//	const A3DHLSLCore::PARAMETER_DESC& paramDesc = itr->second;
	//	const A3DHLSLCore::AnnotationArray& aAnnotations = paramDesc.aAnnotation;
	//	for (A3DHLSLCore::AnnotationArray::const_iterator itr = aAnnotations.begin()
	//		; itr != aAnnotations.end()
	//		; ++itr)
	//	{
	//		std::auto_ptr<A3DHLSLAnnotation> pAnnotation(new A3DHLSLAnnotation());
	//		if (!pAnnotation->Init(&(*itr)))
	//			continue;

	//		aAnnotationLst.push_back(pAnnotation.release());
	//	}

	//	switch (CheckParamType(constMapDesc.cd.cd.Type))
	//	{
	//	case TEXTURE:
	//		if (pTexParams)
	//			pTexParams->AddTextureParam(constMapDesc.cd.cd.Name, aAnnotationLst);
	//		break;
	//	case SAMPLER:
	//		if (pTexParams)
	//			pTexParams->AddSamplerParam(constMapDesc.cd.cd.Name, aAnnotationLst);
	//		break;
	//	case OTHERS:
	//		if (!aAnnotationLst.empty())
	//		{
	//			A3DHLSLUIParam uiParam;
	//			if (uiParam.Init(constMapDesc, aAnnotationLst))
	//				pUIParams->push_back(uiParam);
	//			else
	//			{
	//				a_LogOutput(1, "%s, Failed to init ui param for %s", __FUNCTION__, constMapDesc.cd.cd.Name);
	//			}
	//		}
	//		else
	//		{
	//			//	no annotation
	//			//	skip
	//		}
	//		break;
	//	}

	//	for (AnnotationList::iterator itr = aAnnotationLst.begin(); itr != aAnnotationLst.end(); ++itr)
	//	{
	//		delete *itr;
	//	}
	//	aAnnotationLst.clear();
	//}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DHLSLUIParam
//	
///////////////////////////////////////////////////////////////////////////

A3DHLSLUIParam::A3DHLSLUIParam()
: m_fMinVal(0.0f)
, m_fMaxVal(1.0f)
, m_fStep(0.01f)
, m_bEditable(false)
, m_iDimension(1)
, m_ParamType(PARAM_VOID)
, m_UIWidget(UIUnknown)
, m_param(NULL)
{

}


A3DHLSLUIParam::~A3DHLSLUIParam()
{
	delete m_param;
}


A3DHLSLUIParam& A3DHLSLUIParam::operator = (const A3DHLSLUIParam& rhs)
{
	if (this == &rhs)
		return *this;

	A3DHLSLUIParam(rhs).Swap(*this);
	return *this;
}

A3DHLSLUIParam::A3DHLSLUIParam(const A3DHLSLUIParam& rhs)
: m_strName(rhs.m_strName)
, m_strUIDesc(rhs.m_strUIDesc)
, m_strUIName(rhs.m_strUIName)
, m_fMinVal(rhs.m_fMinVal)
, m_fMaxVal(rhs.m_fMaxVal)
, m_fStep(rhs.m_fStep)
, m_bEditable(rhs.m_bEditable)
, m_UIWidget(rhs.m_UIWidget)
, m_iDimension(rhs.m_iDimension)
, m_ParamType(rhs.m_ParamType)
, m_param(new hlsl_parameter_t(*rhs.m_param))
{

}

void A3DHLSLUIParam::Swap(A3DHLSLUIParam& rhs)
{
	std::swap(rhs.m_strName, m_strName);
	std::swap(rhs.m_strUIName, m_strUIName);
	std::swap(rhs.m_strUIDesc, m_strUIDesc);
	std::swap(rhs.m_fMinVal, m_fMinVal);
	std::swap(rhs.m_fMaxVal, m_fMaxVal);
	std::swap(rhs.m_fStep, m_fStep);
	std::swap(rhs.m_bEditable, m_bEditable);
	std::swap(rhs.m_UIWidget, m_UIWidget);
	std::swap(rhs.m_iDimension, m_iDimension);
	std::swap(rhs.m_ParamType, m_ParamType);
	std::swap(rhs.m_param, m_param);
}

UIParamType GetUIParamTypeFromDescType(D3DXPARAMETER_TYPE paramType)
{
	switch (paramType)
	{
	case D3DXPT_VOID:
	case D3DXPT_BOOL:
	case D3DXPT_FLOAT:
	case D3DXPT_INT:
		return (UIParamType)paramType;
	default:
		return PARAM_UNSUPPORTED;
	}
}

bool A3DHLSLUIParam::Init(const A3DHLSL::CONSTMAPDESC& constMapDesc, const AnnotationList& annotations)
{
	std::auto_ptr<hlsl_parameter_t> pParam(new hlsl_parameter_t());
	if (!pParam->InitFromConstDesc(constMapDesc))
	{
		a_LogOutput(1, "%s, Failed to init ui param's param self for %s", __FUNCTION__, constMapDesc.cd.cd.Name);
		return false;
	}

	m_strName = constMapDesc.cd.cd.Name;
	m_ParamType = GetUIParamTypeFromDescType(constMapDesc.cd.cd.Type);

	if (constMapDesc.cd.cd.Class == D3DXPC_VECTOR)
	{
		m_iDimension = constMapDesc.cd.cd.Bytes / sizeof(int);
	}

	m_param = pParam.release();

	if (!InitFromAnnotationLst(annotations))
	{
		a_LogOutput(1, "%s, Failed to init uiparam %s from annotation list", __FUNCTION__, constMapDesc.cd.cd.Name);
		return false;
	}

	if (GetUIParamWidget() == UISlider && GetParamType() == PARAM_FLOAT)
	{
		float fDefaultVal = 0.0f;
		GetValueAsFloatArray(&fDefaultVal, 1);
		m_fMaxVal = a_Max(m_fMaxVal, fDefaultVal);
		m_fMinVal = a_Min(m_fMinVal, fDefaultVal);
	}

	return true;
}

bool A3DHLSLUIParam::InitFromAnnotationLst(const AnnotationList& annotations)
{
	for (AnnotationList::const_iterator itr = annotations.begin()
		; itr != annotations.end()
		; ++itr)
	{
		A3DHLSLAnnotation* annotation = *itr;

		if (!getValueByName(annotation, "bEditable", &m_bEditable))
			return false;

		if (!getValueByName(annotation, "UIName", &m_strUIName))
			return false;

		if (!getValueByName(annotation, "UIDesc", &m_strUIDesc))
			return false;

		if (!getValueByName(annotation, "UIMin", &m_fMinVal))
			return false;

		if (!getValueByName(annotation, "UIMax", &m_fMaxVal))
			return false;

		if (!getValueByName(annotation, "UIStep", &m_fStep))
			return false;

		if (strcmp(annotation->GetName(), "UIWidget") == 0)
		{
			struct UIWIDGETMETA 
			{
				char widgetName[32];
				UIParamWidget widgetType;
			};
			static const UIWIDGETMETA metas[] = {
				{ "Slider"	, UISlider		},
				{ "Float"	, UISlider		},
				{ "Color"	, UIColor		},
				{ "File"	, UIFile		},
				{ "Texture"	, UIFile		},
				{ "Vector"	, UIVector		},
				{ "float2"	, UIVector		},
				{ "float3"	, UIVector		},
				{ "float4"	, UIVector		},
				{ "Bool"	, UIBool		},
			};

			if (annotation->GetType() == ANNOTATION_TYPE_STRING)
			{
				int i;
				for (i = 0; i < _countof(metas); ++i)
				{
					if (_stricmp(annotation->GetValueAsStr(), metas[i].widgetName) == 0)
					{
						m_UIWidget = metas[i].widgetType;
						break;
					}
				}

				if (i == _countof(metas))
				{
					ASSERT( FALSE && "Unknown UIWidget type" );
					m_UIWidget = UIUnknown;
					return false;
				}

				//	Additional error checking
				if (m_UIWidget == UISlider)
				{
					ASSERT( m_iDimension == 1 && "Slider ui widget should be an exactly 1 dimension variable" );
				}

			}
			else
			{
				return false;
			}
		}	// end of if
	}	// end of for

	return true;
}

bool A3DHLSLUIParam::GetValueAsIntArray(int* pBuf, int iDim) const
{
	if (!m_param || iDim > m_iDimension)
		return false;

	memcpy(pBuf, m_param->m_aIntArray, iDim * sizeof(INT));
	return true;
}

bool A3DHLSLUIParam::GetValueAsFloatArray(float* pBuf, int iDim) const
{
	if (!m_param || iDim > m_iDimension)
		return false;

	memcpy(pBuf, m_param->m_aFltArray, iDim * sizeof(float));
	return true;
}

bool A3DHLSLUIParam::GetValueAsString(AString* pStr) const
{
	if (!m_param || !pStr)
		return false;

	if (m_param->m_Type != ANNOTATION_TYPE_STRING)
		return false;

	*pStr = (const char*)m_param->m_pData;
	return true;
}


A3DHLSLAnnotation::A3DHLSLAnnotation()
: m_param(new hlsl_parameter_t())
{

}

A3DHLSLAnnotation::~A3DHLSLAnnotation()
{
	delete m_param;
	m_param = NULL;
}

bool A3DHLSLAnnotation::Init(const A3DHLSLCore::ANNOTATION* pAnnotation)
{
	return m_param->InitFromAnnotation(pAnnotation);
}

const char* A3DHLSLAnnotation::GetName() const 
{ 
	return m_param->m_strName; 
}

int A3DHLSLAnnotation::GetType() const 
{ 
	return m_param->m_Type; 
}

const char* A3DHLSLAnnotation::GetValueAsStr() const 
{  
	ASSERT(GetType() == ANNOTATION_TYPE_STRING); 
	return (const char*) m_param->m_pData; 
}

BOOL A3DHLSLAnnotation::GetValueAsBool() const 
{ 
	ASSERT(GetType() == ANNOTATION_TYPE_BOOL); 
	return m_param->m_bValue; 
}

INT A3DHLSLAnnotation::GetValueAsInt() const 
{ 
	ASSERT(GetType() == ANNOTATION_TYPE_INT); 
	return m_param->m_iValue; 
}

FLOAT A3DHLSLAnnotation::GetValueAsFloat() const 
{ 
	ASSERT(GetType() == ANNOTATION_TYPE_FLOAT); 
	return m_param->m_fValue; 
}


A3DHLSLTexParams::SAMPLER_PARAM::SAMPLER_PARAM(const char* szParam)
: m_strParamName(szParam)
{
	memset(m_aUVWFilters, 0, sizeof(m_aUVWFilters));
	m_aUVWAddrs[0] = m_aUVWAddrs[1] = m_aUVWAddrs[2] = A3DTADDR_WRAP;
	//memset(m_aUVWAddrs, 0, sizeof(m_aUVWAddrs));
}


A3DHLSLTexParams::~A3DHLSLTexParams()
{
	for (TextureParamArray::iterator itr = m_Textures.begin()
		; itr != m_Textures.end()
		; ++itr)
	{
		delete *itr;
	}
	m_Textures.clear();

	for (SamplerParamArray::iterator itr = m_Samplers.begin()
		; itr != m_Samplers.end()
		; ++itr)
	{
		delete *itr;
	}
	m_Samplers.clear();
}

A3DHLSLTexParams::A3DHLSLTexParams(const A3DHLSLTexParams& rhs)
{
	for (TextureParamArray::const_iterator itr = rhs.m_Textures.begin()
		; itr != rhs.m_Textures.end()
		; ++itr)
	{
		m_Textures.push_back(new TEXTURE_PARAM(**itr));
	}

	for (SamplerParamArray::const_iterator itr = rhs.m_Samplers.begin()
		; itr != rhs.m_Samplers.end()
		; ++itr)
	{
		m_Samplers.push_back(new SAMPLER_PARAM(**itr));
	}
}

A3DHLSLTexParams& A3DHLSLTexParams::operator = (const A3DHLSLTexParams& rhs)
{
	if (this == &rhs)
		return *this;

	A3DHLSLTexParams(rhs).Swap(*this);
	return *this;
}

void A3DHLSLTexParams::Swap(A3DHLSLTexParams& rhs)
{
	m_Textures.swap(rhs.m_Textures);
	m_Samplers.swap(rhs.m_Samplers);
}

const A3DHLSLTexParams::TEXTURE_PARAM* A3DHLSLTexParams::GetTextureParamByName(const char* szTexParam, int * piIndex) const
{
	if (!szTexParam || !szTexParam[0])
		return NULL;

	if (piIndex)
		*piIndex = -1;

	int nIdx = 0;
	for (TextureParamArray::const_iterator itr = m_Textures.begin()
		; itr != m_Textures.end()
		; ++itr, ++nIdx)
	{
		if (strcmp((*itr)->GetParamName(), szTexParam) == 0)
		{
			if (piIndex)
				*piIndex = nIdx;

			return *itr;
		}
	}

	return NULL;
}

const A3DHLSLTexParams::SAMPLER_PARAM* A3DHLSLTexParams::GetSamplerParamByName(const char* szSamParam) const
{
	if (!szSamParam || !szSamParam[0])
		return NULL;

	for (SamplerParamArray::const_iterator itr = m_Samplers.begin()
		; itr != m_Samplers.end()
		; ++itr)
	{
		if (strcmp((*itr)->GetParamName(), szSamParam) == 0)
			return *itr;
	}

	return NULL;
}

bool A3DHLSLTexParams::AddTextureParam(const char* szParam, const AnnotationList& annotations)
{
	std::auto_ptr<TEXTURE_PARAM> pTP(new TEXTURE_PARAM(szParam));

	for (AnnotationList::const_iterator itr = annotations.begin()
		; itr != annotations.end()
		; ++itr)
	{
		AString strValue;
		if (!getValueByName(*itr, "UIObject", &strValue))
			return false;

		if (!strValue.IsEmpty())
		{
			pTP->SetTexFile(strValue);
			continue;
		}

		if (!getValueByName(*itr, "UIName", &strValue))
			return false;

		if (!strValue.IsEmpty())
		{
			pTP->SetUIName(strValue);
			continue;
		}
	}

	m_Textures.push_back(pTP.release());
	return true;
}

struct STRING_VALUE_PAIR
{
	AString strData;
	DWORD dwData;
};

static STRING_VALUE_PAIR meta_data[] = 
{
	{ "WRAP"	, A3DTADDR_WRAP },
	{ "MIRROR"	, A3DTADDR_MIRROR },
	{ "CLAMP"	, A3DTADDR_CLAMP },
	{ "BORDER"	, A3DTADDR_BORDER },
	{ "MIRRORONCE"	, A3DTADDR_MIRRORONCE },
	{ "NONE"	, A3DTEXF_NONE },
	{ "POINT"	, A3DTEXF_POINT },
	{ "LINEAR"	, A3DTEXF_LINEAR },
	{ "ANISOTROPIC"	, A3DTEXF_ANISOTROPIC },
	{ "FLATCUBIC"	, A3DTEXF_FLATCUBIC },
	{ "GAUSSIANCUBIC"	, A3DTEXF_GAUSSIANCUBIC },
};

static DWORD GetDataByString(const char* szStr)
{
	for (int i = 0; i < _countof(meta_data); ++i)
	{
		if (meta_data[i].strData.CompareNoCase(szStr) == 0)
			return meta_data[i].dwData;
	}

	return -1;
}

bool A3DHLSLTexParams::AddSamplerParam(const char* szParam, const AnnotationList& annotations)
{
	std::auto_ptr<SAMPLER_PARAM> pSP(new SAMPLER_PARAM(szParam));

	static const char* const AddressKeyWords[] = 
	{
		{ "AddressU" },
		{ "AddressV" },
		{ "AddressW" },
	};

	static const char* const FilterKeyWords[] = 
	{
		{ "MagFilter" },
		{ "MinFilter" },
		{ "MipFilter" },
	};

	for (AnnotationList::const_iterator itr = annotations.begin()
		; itr != annotations.end()
		; ++itr)
	{
		A3DHLSLAnnotation* pAnno = *itr;

		AString strValue;
		if (!getValueByName(pAnno, "Texture", &strValue))
			return false;

		if (!strValue.IsEmpty())
		{
			pSP->SetUseTex(strValue);
			continue;
		}

		for (int i = 0; i < _countof(AddressKeyWords); ++i)
		{
			AString strAddress;
			if (!getValueByName(pAnno, AddressKeyWords[i], &strAddress))
				return false;
			else if (!strAddress.IsEmpty())
			{
				pSP->SetTextureAddr(i, (A3DTEXTUREADDR)GetDataByString(strAddress));
				break;
			}
		}

		for (int i = 0; i < _countof(FilterKeyWords); ++i)
		{
			AString strFilter;
			if (!getValueByName(pAnno, FilterKeyWords[i], &strFilter))
				return false;
			else if (!strFilter.IsEmpty())
			{
				pSP->SetTextureFilterType(i, (A3DTEXTUREFILTERTYPE)GetDataByString(strFilter));
				break;
			}
		}
	}

	m_Samplers.push_back(pSP.release());
	return true;
}

A3DHLSLTexParams::SamplerTextureArray A3DHLSLTexParams::GenerateSamplerTexturePairs() const
{
	A3DHLSLTexParams::SamplerTextureArray stArray;
	for (int i = 0; i < GetSamplerParamNum(); ++i)
	{
		const SAMPLER_PARAM* pParam = GetSamplerParam(i);
		const TEXTURE_PARAM* pTexParam = GetTextureParamByName(pParam->GetUseTex());
		if (!pTexParam)
			continue;

		stArray.push_back(std::make_pair(pParam->GetParamName(), pTexParam->GetTexFile()));
	}

	return stArray;
}

