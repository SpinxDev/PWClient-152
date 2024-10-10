#include "stdafx.h"
#include <stdexcept>
#include <typeinfo>
#include <astring.h>
//#include <a3dtypes.h>
//#include <a3dmacros.h>
#include <a3DFontMan.h>
#include "BCGPAFileProp.h"
#include "BCGPPropertyListWrapper.h"
#include "BCGPPropertyObject.h"
#include "BCGPPropNoRedraw.h"
#include "BCGPPropListEx.h"
#include "BCGPRangeProp.h"
#include "BCGPCustomProp.h"
#include "AnyType.h"
#include "..\Global.h"

//////////////////////////////////////////////////////////////////////////
//
//	Help functions
//
/////////////////////////////////////////////////////////////////////////

void SetReadOnly_recur(CBCGPProp* pProp, BOOL bIsReadOnly)
{
	if (!pProp) return;
	pProp->Enable(!bIsReadOnly);
	for (int i = 0; i < pProp->GetSubItemsCount(); ++i)
	{
		SetReadOnly_recur(pProp->GetSubItem(i), bIsReadOnly);
	}
}

void Attach_recur(CBCGPPropertyListWrapper* pListWrapper, CBCGPProp* pCurProp, const CPropertyArray* pData)
{
	if (!pListWrapper || !pCurProp || !pData)
		throw std::runtime_error("invalid parameter in Attach_recur");

	CBCGPPropertyListWrapper::BCGPPropCreaterMap& createrFuncMap = pListWrapper->m_mapCreaterFuncs;
	size_t iCount = pData->GetCount();
	for (size_t i = 0; i < iCount; ++i)
	{
		const CPropertyItem& prop = pData->GetProperty(i);
		CBCGPProp* pPropItem = NULL;
		int iPropType = TypeInfo2PropType(prop.GetValueType());
		if (createrFuncMap.find(iPropType) != createrFuncMap.end())
			pPropItem = (*createrFuncMap[iPropType])(pListWrapper, &prop);

		if (pPropItem == NULL)
			continue;

		if (prop.IsReadonly())
			SetReadOnly_recur(pPropItem, TRUE);

		pCurProp->AddSubItem(pPropItem);
	}
}

void Update_recur(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyArray* pData, CBCGPProp* pBCGPGroup)
{
	if (!pListWrapper || !pData || !pBCGPGroup || pData->GetCount() != pBCGPGroup->GetSubItemsCount())
		throw std::runtime_error("invalid parameter in Update_recur");

	CBCGPPropertyListWrapper::BCGPPropUpdaterMap& updaterFuncMap = pListWrapper->m_mapUpdaterFuncs;
	size_t iCount = pData->GetCount();
	ASSERT(iCount == pBCGPGroup->GetSubItemsCount());
	for (size_t i = 0; i < iCount; ++i)
	{
		CPropertyItem& prop_item = pData->GetProperty(static_cast<int>(i));
		CBCGPProp* pBCGPProp = pBCGPGroup->GetSubItem(static_cast<int>(i));
		int iValType = TypeInfo2PropType(prop_item.GetValueType());

		if (updaterFuncMap.find(iValType) != updaterFuncMap.end())
			(*updaterFuncMap[iValType])(pListWrapper, bSave, &prop_item, pBCGPProp);
	}
}

void GetLogFont(LOGFONT& lf, const CPropertyItem* pProp)
{
	const A3DFontMan::FONTTYPE* pFont = any_cast<A3DFontMan::FONTTYPE>(&pProp->GetValue());
	lf.lfCharSet = GB2312_CHARSET;
	strcpy(lf.lfFaceName, pFont->szName);
	HDC hDC = GetDC(NULL);
	lf.lfHeight = -MulDiv(pFont->iHeight, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	lf.lfWeight = (pFont->dwFlags & A3DFONT_BOLD) ? FW_BOLD : 0;
	lf.lfItalic = (pFont->dwFlags & A3DFONT_ITALIC) ? TRUE : FALSE;
	ReleaseDC(NULL, hDC);
}
//////////////////////////////////////////////////////////////////////////
//
//	Creater functions
//
//////////////////////////////////////////////////////////////////////////

CBCGPProp* BCGPCreatePropException(CBCGPPropertyListWrapper* , const CPropertyItem*)
{
	throw bcgp_bad_prop("unexpected property type.");
}

CBCGPProp* BCGPCreateBoolProp(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	return new CBCGPPropNoRedraw(pProp->GetName(), (_variant_t)any_cast<bool>(pProp->GetValue()), pProp->GetDesc());
}

CBCGPProp* BCGPCreateFloatProp(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	_variant_t initVal;
	any val = pProp->GetValue();
	const prop_range* pRange = pProp->GetRange();
	ASSERT(pRange == pProp->GetRange());
	if (val.type() == typeid(float)) {
		initVal = any_cast<float>(val);
	}
	else if (val.type() == typeid(double)) {
		initVal = any_cast<double>(val);
	}
	else {
		const char* szType = val.type().name();
		const int DO_NOT_REACH_HERE = 0;
		ASSERT(szType && DO_NOT_REACH_HERE);
	}
	if (pProp->GetWay() == BCGP_PROP_WAY_DEFAULT)	
		return new CBCGPPropNoRedraw(pProp->GetName(), initVal, pProp->GetDesc());
	else if (pProp->GetWay() == BCGP_PROP_WAY_RANGE && pRange) 
	{
		return new CBCGPRangeProp(pProp->GetName()
		, initVal
		, pRange
		, pProp->GetDesc());
	}
	else
		throw bcgp_bad_prop("unsupport way of float / double prop.");
}



CBCGPProp* BCGPCreateNormalProp(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	_variant_t initVal;
	int iWay = pProp->GetWay();
	const prop_set* pOptions = pProp->GetSet();
	const prop_range* pRange = pProp->GetRange();
	any val = pProp->GetValue();

	initVal = Any2VT(val);
	if (initVal.vt == VT_EMPTY)
	{
		std::string msg = val.type().name();
		msg += "should not arrive here!";
		throw bcgp_bad_prop(msg.c_str());
	}

	if (pProp->GetWay() == BCGP_PROP_WAY_DEFAULT && !pOptions)
		return new CBCGPPropNoRedraw(pProp->GetName(), initVal, pProp->GetDesc());
	else if (pProp->GetWay() == BCGP_PROP_WAY_OPTIONS && pOptions) {
		CBCGPPropNoRedraw* pOptionProp = new CBCGPPropNoRedraw(pProp->GetName(), _variant_t(), pProp->GetDesc());
		const int MAX_BUF_LEN = 1024;
		char szBuffer[MAX_BUF_LEN];

		for (size_t idx = 0; idx < pOptions->GetCount(); ++idx)
		{
			_variant_t cur_val = Any2VT(pOptions->GetValueByIdx(idx));
			if (cur_val.vt == VT_EMPTY)
				throw bcgp_bad_prop("invalid type of value.");
			
			//	make the real value of the option in front of the string
			//	BCGPProp will do the cast and translate work for us (convert string to the value we want)
			_snprintf(szBuffer, MAX_BUF_LEN, "%s", pOptions->GetNameByIdx(idx));
			pOptionProp->AddOption(szBuffer);
			if (initVal == cur_val)
				pOptionProp->SetValue(pOptions->GetNameByIdx(idx));
		}
		pOptionProp->AllowEdit(FALSE);
		return pOptionProp;
	}
	else if (pProp->GetWay() == BCGP_PROP_WAY_RANGE && pRange) 
	{
		CBCGPRangeProp* propbox = new CBCGPRangeProp(pProp->GetName(), initVal, pRange, pProp->GetDesc());
		return propbox;
	}
	else
		throw bcgp_bad_prop("unsupported way of normal prop.");
}

CBCGPProp* BCGPCreateStringProp(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	any val = pProp->GetValue();
	std::string t = any_cast<std::string>(val);

	switch (pProp->GetWay())
	{
	case BCGP_PROP_WAY_FILENAME:
		return new CBCGPAFileProp(pProp->GetName(), t.c_str(), TRUE, pProp->GetPath(), pProp->GetFileExt(), AFileDialogWrapper::AFD_LIST, pProp->GetDesc());
	default:
		return new CBCGPPropNoRedraw(pProp->GetName(), (_variant_t) t.c_str(), pProp->GetDesc());
	}
	return NULL;
}

CBCGPProp* BCGPCreateScriptProp(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	const int NOT_IMPL_CURRENT = 0;
	ASSERT(NOT_IMPL_CURRENT);
	return NULL;
}

CBCGPProp* BCGPCreateVector3Prop(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	A3DVECTOR3 vec = any_cast<A3DVECTOR3>(pProp->GetValue());
	CBCGPProp* pBCGPProp = new CBCGPPropNoRedraw(pProp->GetName());
	pBCGPProp->AddSubItem(new CBCGPPropNoRedraw("x", (_variant_t) vec.x));
	pBCGPProp->AddSubItem(new CBCGPPropNoRedraw("y", (_variant_t) vec.y));
	pBCGPProp->AddSubItem(new CBCGPPropNoRedraw("z", (_variant_t) vec.z));
	return pBCGPProp;
}

CBCGPProp* BCGPCreateMatrix4Prop(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	const int NOT_IMPL_CURRENT = 0;
	ASSERT(NOT_IMPL_CURRENT);
	return NULL;
}

CBCGPProp* BCGPCreateFontProp(CBCGPPropertyListWrapper* , const CPropertyItem* pProp)
{
	LOGFONT lf;
	GetLogFont(lf, pProp);
	return new CBCGPFontProp(pProp->GetName(), lf, CF_SCREENFONTS, pProp->GetDesc());
}

CBCGPProp* BCGPCreateSubProp(CBCGPPropertyListWrapper* pListWrapper, const CPropertyItem* pProp)
{
	CBCGPProp* pGroup = new CBCGPPropNoRedraw(pProp->GetName());
	Attach_recur(pListWrapper, pGroup, any_cast<CPropertyArray>(&pProp->GetValue()));
	return pGroup;
}

CBCGPProp* BCGPCreateCustomProp(CBCGPPropertyListWrapper* pListWrapper, const CPropertyItem* pProp)
{
	ASSERT(pProp->GetWay() == BCGP_PROP_WAY_CUSTOM);
	return new CBCGPCustomProp(pProp->GetName(), pProp->GetCustomFuncs(), pProp->GetDesc());
}

//////////////////////////////////////////////////////////////////////////
//
//	Updater functions
//
//////////////////////////////////////////////////////////////////////////

void BCGPCUpdatePropException(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	throw bcgp_bad_prop("unexpected property type."); 
}

void BCGPUpdateBoolProp(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	if (bSave) {
		bool bVal = pBCGPProp->GetValue();
		pProp->SetValue(bVal);
	}
	else
	{
		bool bVal = any_cast<bool>(pProp->GetValue());
		pBCGPProp->SetValue(bVal);
	}
}

void BCGPUpdateFloatProp(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	const any& val = pProp->GetValue();
	if (bSave)
	{
		if (val.type() == typeid(float))
			pProp->SetValue((float)pBCGPProp->GetValue());
		else if (val.type() == typeid(double))
			pProp->SetValue((double)pBCGPProp->GetValue());
		else
			throw bcgp_bad_prop(std::string("Impossible type of float prop in save.") + val.type().name());
	}
	else
	{
		_variant_t tmpValue;
		if (val.type() == typeid(float))
			tmpValue = any_cast<float>(val);
		else if (val.type() == typeid(double))
			tmpValue = any_cast<double>(val);
		else
			throw bcgp_bad_prop(std::string("Impossible type of float prop in update.") + val.type().name());
		pBCGPProp->SetValue(tmpValue);
	}
}

void BCGPUpdateStringProp(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	if (bSave) {
		_variant_t val = pBCGPProp->GetValue();
		std::string str = (_bstr_t)val;
		pProp->SetValue(str);
	}
	else
	{
		any val = pProp->GetValue();
		if (val.type() != typeid(std::string))
			throw bcgp_bad_prop("string type of prop wanted.");
		pBCGPProp->SetValue(any_cast<std::string>(val).c_str());
	}
}

void BCGPUpdateNormalProp(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	if (bSave)
	{
		_variant_t val = pBCGPProp->GetValue();
		any any_val;
		if (pProp->GetWay() == BCGP_PROP_WAY_OPTIONS && pProp->GetSet()) {
			const prop_set* option_set = pProp->GetSet();
			_bstr_t name = val;
			size_t idx = option_set->FindName(name);
			if (idx == option_set->GetCount())
				idx = 0;
			any_val = option_set->GetValueByIdx(idx);
		}
		else
			any_val = VT2Any(val, TypeInfo2PropType(pProp->GetValueType()));
		if (any_val.empty())
			throw bcgp_bad_prop("invalid _variant_t type from option prop.");
		pProp->SetValue(any_val);
	}
	else
	{
		_variant_t val;
		if (pProp->GetWay() == BCGP_PROP_WAY_OPTIONS && pProp->GetSet()) {
			const prop_set* option_set = pProp->GetSet();
			size_t idx = option_set->FindValue(pProp->GetValue());
			if (idx == option_set->GetCount())
				throw bcgp_bad_prop("invalid data of prop.");
			val = option_set->GetNameByIdx(idx);
		}
		else
			val = Any2VT(pProp->GetValue());
		if (val.vt == VT_EMPTY)
			throw bcgp_bad_prop("invalid data type from any in property data.");
		if (pBCGPProp->GetValue() != val)
			pBCGPProp->SetValue(val);
	}
}

void BCGPUpdateVector3Prop(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	int iCount = pBCGPProp->GetSubItemsCount();
	if (iCount != 3) return;
 
	if (bSave)
	{
		float fx = pBCGPProp->GetSubItem(0)->GetValue();
		float fy = pBCGPProp->GetSubItem(1)->GetValue();
		float fz = pBCGPProp->GetSubItem(2)->GetValue();
		pProp->SetValue(A3DVECTOR3(fx, fy, fz));
	}
	else
	{
		A3DVECTOR3 vec = any_cast<A3DVECTOR3>(pProp->GetValue());
		pBCGPProp->GetSubItem(0)->SetValue(vec.x);
		pBCGPProp->GetSubItem(1)->SetValue(vec.y);
		pBCGPProp->GetSubItem(2)->SetValue(vec.z);
	}
}

void BCGPUpdateMatrix4Prop(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	const int NOT_IMPL_CURRENT = 0;
	ASSERT(NOT_IMPL_CURRENT);
}

void BCGPUpdateFontProp(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	if (bSave) {
		if (CBCGPFontProp* pFontProp = dynamic_cast<CBCGPFontProp*>(pBCGPProp))
		{
			A3DFontMan::FONTTYPE* pFont = any_cast<A3DFontMan::FONTTYPE>(&pProp->GetValue());
			LPLOGFONT plf = pFontProp->GetLogFont();
			strcpy(pFont->szName, plf->lfFaceName);
			pFont->dwFlags |= (plf->lfWeight == FW_BOLD) ? A3DFONT_BOLD : 0;
			pFont->dwFlags |= (plf->lfItalic) ? A3DFONT_ITALIC : 0;
		}
	}
	else {
		if (CBCGPFontProp* pFontProp = dynamic_cast<CBCGPFontProp*>(pBCGPProp)){
			LOGFONT& lf = *(pFontProp->GetLogFont());
			GetLogFont(lf, pProp);
		}
	}
}

void BCGPUpdateSubProp(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	Update_recur(pListWrapper, bSave, any_cast<CPropertyArray>(&pProp->GetValue()), pBCGPProp);
}

void BCGPUpdateCustomProp(CBCGPPropertyListWrapper* pListWrapper, bool bSave, CPropertyItem* pProp, CBCGPProp* pBCGPProp)
{
	// in normal update prop func
	// we need to update data from PropertyItem to BCGPProp (load) in order to update the display
	// or from BCGPProp to PropertyItem (save) in order to update the real data in the object attached
	// but we do nothing for custom prop
	// because the real data's value would be changed at once when the user clicked the button and return true
	if (bSave)
	{
		
	}
	else
	{

	}
}

void CBCGPPropertyListWrapper::Init()
{
	m_mapCreaterFuncs[ANY_VOID] = BCGPCreatePropException;
	m_mapCreaterFuncs[ANY_BOOL] = BCGPCreateBoolProp;
	m_mapCreaterFuncs[ANY_CHAR] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_UCHAR] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_SHORT] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_USHORT] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_INT] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_UINT] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_LONG] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_ULONG] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_INT64] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_UINT64] = BCGPCreateNormalProp;
	m_mapCreaterFuncs[ANY_FLOAT] = BCGPCreateFloatProp;
	m_mapCreaterFuncs[ANY_DOUBLE] = BCGPCreateFloatProp;
	m_mapCreaterFuncs[ANY_A3DVECTOR3] = BCGPCreateVector3Prop;
	m_mapCreaterFuncs[ANY_A3DMATRIX4] = BCGPCreateMatrix4Prop;
	m_mapCreaterFuncs[ANY_STRING] = BCGPCreateStringProp;
	m_mapCreaterFuncs[ANY_FONTTYPE] = BCGPCreateFontProp;
	m_mapCreaterFuncs[ANY_SUBPROPARRAY] = BCGPCreateSubProp;
	m_mapCreaterFuncs[ANY_CUSTOMFUNCPTR] = BCGPCreateCustomProp;


	m_mapUpdaterFuncs[ANY_VOID] = BCGPCUpdatePropException;
	m_mapUpdaterFuncs[ANY_BOOL] = BCGPUpdateBoolProp;
	m_mapUpdaterFuncs[ANY_CHAR] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_UCHAR] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_SHORT] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_USHORT] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_INT] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_UINT] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_LONG] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_ULONG] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_INT64] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_UINT64] = BCGPUpdateNormalProp;
	m_mapUpdaterFuncs[ANY_FLOAT] = BCGPUpdateFloatProp;
	m_mapUpdaterFuncs[ANY_DOUBLE] = BCGPUpdateFloatProp;
	m_mapUpdaterFuncs[ANY_A3DVECTOR3] = BCGPUpdateVector3Prop;
	m_mapUpdaterFuncs[ANY_A3DMATRIX4] = BCGPUpdateMatrix4Prop;
	m_mapUpdaterFuncs[ANY_STRING] = BCGPUpdateStringProp;
	m_mapUpdaterFuncs[ANY_FONTTYPE] = BCGPUpdateFontProp;
	m_mapUpdaterFuncs[ANY_SUBPROPARRAY] = BCGPUpdateSubProp;
	m_mapUpdaterFuncs[ANY_CUSTOMFUNCPTR] = BCGPUpdateCustomProp;
}

CBCGPPropertyListWrapper::CBCGPPropertyListWrapper()
: m_pList(new CBCGPPropListEx(this))
, m_pData(NULL)
{
	Init();
}

CBCGPPropertyListWrapper::~CBCGPPropertyListWrapper()
{
	m_pList->EnablePropertyChanged(false);
	delete m_pList;
}

//	Create proplist
BOOL CBCGPPropertyListWrapper::OnCreate(const CRect& rc, CWnd* pParent)
{
	if (!m_pList->Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
		TVS_LINESATROOT | TVS_HASBUTTONS, rc, pParent, 1))
		return FALSE;
	m_pList->EnableDescriptionArea();
	m_pList->SetVSDotNetLook();
	return TRUE;
}

void CBCGPPropertyListWrapper::OnSize(UINT flag, int cx, int cy)
{
	m_pList->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CBCGPPropertyListWrapper::SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
{
	return m_pList->SetWindowPos(NULL, x, y, cx, cy, nFlags);
}

//	bSave: true - from user interface to data
//	bSave: false - from data to user interface
void CBCGPPropertyListWrapper::UpdateData(bool bSave, bool bIsCommit)
{
	if (!m_pData)
		return;

	for (Data2UI::iterator itr = m_mapData2UI.begin()
		; itr != m_mapData2UI.end()
		; ++itr)
	{
		try
		{
			CPropertyItem& propItem = m_pData->GetProperty(itr->first);
			int iPropType = TypeInfo2PropType(propItem.GetValueType());

			if (m_mapUpdaterFuncs.find(iPropType) != m_mapUpdaterFuncs.end())
				(*m_mapUpdaterFuncs[iPropType])(this, bSave, &propItem, m_pList->GetProperty(static_cast<int>(itr->second)));
		}
		catch (std::exception& e)
		{
			CString str;
			str.Format("Unexpected Error Happens: %s", e.what());
			AfxMessageBox(str);
			return;
		}
		catch (...)
		{
			CString str;
			str.Format("Unexpected Error Happens: Unknown error.");
			AfxMessageBox(str);
			return;
		}
	}

	if (bSave)
	{
		m_pData->RaisePropUpdatedMsg(bIsCommit);
	}
	else
		m_pList->InvalidateRect(m_pList->GetListRect());
}

void CBCGPPropertyListWrapper::AttachTo(CPropertyArray* pData)
{
	try
	{
		m_pList->RemoveAllItems();
		m_mapData2UI.clear();
		m_pData = pData;

		if (!m_pData) {
			m_pList->Invalidate();
			return;
		}

		size_t iCount = pData->GetCount();
		for (size_t i = 0 ; i < iCount; ++i)
		{
			const CPropertyItem& prop = pData->GetProperty(i);
			CBCGPProp* pPropItem = NULL;
			int iPropType = TypeInfo2PropType(prop.GetValueType());
			if (m_mapCreaterFuncs.find(iPropType) != m_mapCreaterFuncs.end())
				pPropItem = (*m_mapCreaterFuncs[iPropType])(this, &prop);

			if (pPropItem == NULL)
				continue;

			if (prop.IsReadonly() || pData->IsReadOnly())
				SetReadOnly_recur(pPropItem, TRUE);

			size_t iPropBoxIdx = m_pList->AddProperty(pPropItem, FALSE, FALSE);
			m_mapData2UI[i] = iPropBoxIdx;
		}

		m_pList->AdjustLayout();
		m_pList->Invalidate();
	}
	catch (std::exception& e)
	{
		AfxMessageBox(e.what());
		GF_Log(LOG_ERROR, e.what());
	}
	catch (...)
	{
		AfxMessageBox("发生了未知的异常! 请联系程序人员解决");
	}
}
