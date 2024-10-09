// InnerProperty.cpp: implementation of the FWInnerProperty class.
//
//////////////////////////////////////////////////////////////////////

#include "FWInnerProperty.h"
#include "FWOutterProperty.h"
#include "A3DGFXEx.h"
#include "A3DGFXElement.h"
#include "A3DGFXKeyPoint.h"
#include "FWLog.h"

#define new A_DEBUG_NEW


#pragma warning(disable : 4715)


//////////////////////////////////////////////////////////////////////////
// inner property 
//////////////////////////////////////////////////////////////////////////

FWInnerProperty::FWInnerProperty()
{
	ZeroMemory(&m_Pointer, sizeof(m_Pointer));
	m_pointerType = POINTER_TYPE_UNKNOWN;
	m_nPropertyID = -1;
	m_nSubObjID = -1;
	m_bIsValid = false;
}

FWInnerProperty::~FWInnerProperty()
{

}

FWInnerProperty::FWInnerProperty(const FWInnerProperty &src)
{
	m_Pointer = src.GetPointer();
	m_pointerType = src.GetPointerType();
	m_nPropertyID = src.GetPropertyID();
	m_nSubObjID = src.GetSubObjID();
	m_bIsValid = src.IsValid();
}

FWInnerProperty & FWInnerProperty::operator = (const FWInnerProperty & src)
{
	return (*this = GFX_PROPERTY(src));
}

FWInnerProperty & FWInnerProperty::operator = (const GFX_PROPERTY  & val)
{
	if (!IsValid()) return *this;
	
	switch(m_pointerType) 
	{
	case POINTER_TYPE_ELEMENT:
		if (m_nSubObjID == -1)
			m_Pointer.pElem->SetProperty(m_nPropertyID, val);
		else
			m_Pointer.pElem->SetSubProperty(m_nSubObjID, 0, m_nPropertyID, val);
		break;
	case POINTER_TYPE_CTRL:
		m_Pointer.pCtrl->SetProperty(m_nPropertyID, val);
		break;
	case POINTER_TYPE_KEYPOINT:
		m_Pointer.pKP->SetProperty(m_nPropertyID, val);
		break;
	}
	return *this;
}


bool FWInnerProperty::InitWithString(A3DGFXEx *pGfx, const AString& strPropNameInINI)
{
	ASSERT(pGfx);

	m_bIsValid = false;
	m_strNameFromINI = strPropNameInINI;
	m_nSubObjID = -1;
	m_nPropertyID = -1;
	m_Pointer.pGfx = pGfx;
	m_pointerType = POINTER_TYPE_GFX;

	if (!GfxParse())
	{
		FWLog::GetInstance()->Log("FWInnerProperty::InitWithString(), error occur when parse the string.");
		return false;
	}

	return true;
}

FWInnerProperty::operator GFX_PROPERTY() const
{
	ASSERT(IsValid());
	
	switch(m_pointerType) 
	{
	case POINTER_TYPE_ELEMENT:
		if (m_nSubObjID == -1)
			return m_Pointer.pElem->GetProperty(m_nPropertyID);
		else
			return m_Pointer.pElem->GetSubProperty(m_nSubObjID, 0, m_nPropertyID);
	case POINTER_TYPE_CTRL:
		return m_Pointer.pCtrl->GetProperty(m_nPropertyID);
	case POINTER_TYPE_KEYPOINT:
		return m_Pointer.pKP->GetProperty(m_nPropertyID);
	}
	
	// this function must called after a successfully call 
	// to InitWithString(), so we should not reach here
	ASSERT(false);
}


bool FWInnerProperty::GfxParse()
{
	ASSERT(m_pointerType == POINTER_TYPE_GFX);

	BEGIN_FAKE_WHILE;

	// get name
	AString name = CutLeadingPart();
	CHECK_BREAK(!name.IsEmpty());
	
	// find element by name
	A3DGFXElement *pElem = m_Pointer.pGfx->GetElementByName(name);
	CHECK_BREAK(pElem);

	// update pointer
	m_Pointer.pElem = pElem;
	m_pointerType = POINTER_TYPE_ELEMENT;

	// parse deeper
	CHECK_BREAK(ElementParse());

	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

bool FWInnerProperty::ElementParse()
{
	ASSERT(m_pointerType == POINTER_TYPE_ELEMENT);

	BEGIN_FAKE_WHILE;

	AString name = CutLeadingPart();
	CHECK_BREAK(!name.IsEmpty());

	A3DGFXElement *pElem = m_Pointer.pElem;
	
	if (IsTheLastPart()) // the name is a property of the gfxelement
	{
		const GFX_OBJ_INFO *pObjInfo = gfx_obj_info_by_type(pElem->GetEleTypeId());
		CHECK_BREAK(pObjInfo);
		
		const GFX_PROP_INFO *pPropInfo = gfx_prop_info_by_name(pObjInfo->m_pPropInfo, pObjInfo->m_nPropCount, name);
		CHECK_BREAK(pPropInfo);

		m_nPropertyID = pPropInfo->m_OpId;
		m_bIsValid = true;
	}
	else if (name == "Keypoint") // the name is Keypoint, parse more
	{
		m_Pointer.pKP = pElem->GetKeyPoint(0);
		m_pointerType = POINTER_TYPE_KEYPOINT;

		CHECK_BREAK(KeypointParse());
	}
	else // the name is a ctrl that modify the partical 
	{
		const GFX_OBJ_INFO *pMethodInfo = gfx_method_info_by_name(name);
		CHECK_BREAK(pMethodInfo);
		
		name = CutLeadingPart();
		CHECK_BREAK(!name.IsEmpty());

		const GFX_PROP_INFO *pPropInfo = gfx_prop_info_by_name(pMethodInfo->m_pPropInfo, pMethodInfo->m_nPropCount, name);
		CHECK_BREAK(pPropInfo);

		m_nSubObjID = pMethodInfo->m_nTypeId;
		m_nPropertyID = pPropInfo->m_OpId;
		m_bIsValid = true;
	}
	
	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

bool FWInnerProperty::CtrlParse()
{
	ASSERT(m_pointerType == POINTER_TYPE_CTRL);

	BEGIN_FAKE_WHILE;

	AString name = CutLeadingPart();
	CHECK_BREAK(!name.IsEmpty());

	const GFX_OBJ_INFO *pCtrlInfo = gfx_method_info_by_type(m_Pointer.pCtrl->GetType());
	CHECK_BREAK(pCtrlInfo);

	const GFX_PROP_INFO *pPropInfo = gfx_prop_info_by_name(pCtrlInfo->m_pPropInfo, pCtrlInfo->m_nPropCount, name);
	CHECK_BREAK(pPropInfo);

	m_nPropertyID = pPropInfo->m_OpId;
	m_bIsValid = true;

	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

bool FWInnerProperty::KeypointParse()
{
	ASSERT(m_pointerType == POINTER_TYPE_KEYPOINT);

	BEGIN_FAKE_WHILE;

	AString name = CutLeadingPart();
	CHECK_BREAK(!name.IsEmpty());

	if (IsTheLastPart()) // the name is the property of the keypoint
	{
		const GFX_PROP_INFO *pKPPropInfoSet = gfx_kp_prop_info_set();
		CHECK_BREAK(pKPPropInfoSet);

		const GFX_PROP_INFO *pKPPropInfo = gfx_prop_info_by_name(pKPPropInfoSet, gfx_size_of_kp_prop_set(), name);
		CHECK_BREAK(pKPPropInfo);

		m_nPropertyID = pKPPropInfo->m_OpId;
		m_bIsValid = true;
	}
	else // the name is a ctrl
	{
		int nPostfix = DistillPostfixNum(name);
		CHECK_BREAK(nPostfix != 0);
		nPostfix = 0;

		const GFX_OBJ_INFO *pMethodInfo = gfx_method_info_by_name(name);
		CHECK_BREAK(pMethodInfo);

		CtrlList listCtrl;
		m_Pointer.pKP->GetCtrlMethodByType(pMethodInfo->m_nTypeId, listCtrl);
		CHECK_BREAK((size_t)nPostfix < listCtrl.size());
		
		m_Pointer.pCtrl = listCtrl[nPostfix];
		m_pointerType = POINTER_TYPE_CTRL;
		CHECK_BREAK(CtrlParse());
	}

	END_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

AString FWInnerProperty::CutLeadingPart()
{
	int nPos = m_strNameFromINI.Find('.');
	if (nPos == -1)
	{
		AString tmp = m_strNameFromINI;
		m_strNameFromINI.Empty();
		return tmp;
	}
	if (nPos == 0)
		return AString("");

	AString strRet = m_strNameFromINI.Left(nPos);
	m_strNameFromINI.CutLeft(nPos);
	if (!m_strNameFromINI.IsEmpty() &&
		m_strNameFromINI.Left(1) == '.')
		m_strNameFromINI.CutLeft(1);
	
	return strRet;
}

bool FWInnerProperty::IsTheLastPart() const
{
	return m_strNameFromINI.IsEmpty();
}

int FWInnerProperty::DistillPostfixNum(AString &strName)
{
	int nNum = 0;
	int nTimes = 1;
	AString strPostfix;
	while (!strName.IsEmpty())
	{
		strPostfix = strName.Right(1);
		if (strPostfix[0] >= '0' && strPostfix[0] <= '9')
		{
			strName.CutRight(1);

			nNum += nTimes * int(strPostfix[0] - '0');
			nTimes *= 10;
		}
		else
		{
			break;
		}
	}
	return nNum;
}

