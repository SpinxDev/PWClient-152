#include "stdafx.h"
#include "BCGPPropNoRedraw.h"

// Group constructor
CBCGPPropNoRedraw::CBCGPPropNoRedraw(const CString& strGroupName, DWORD_PTR dwData,
				  BOOL bIsValueList)
				  : CBCGPProp(strGroupName, dwData, bIsValueList)
{

}

// Simple property
CBCGPPropNoRedraw::CBCGPPropNoRedraw(const CString& strName, const _variant_t& varValue, 
				  LPCTSTR lpszDescr, DWORD_PTR dwData,
				  LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate,
				  LPCTSTR lpszValidChars)
				  : CBCGPProp(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{

}

void CBCGPPropNoRedraw::SetValue (const _variant_t& varValue)
{
	SetValueNoRedraw(varValue);
}

void CBCGPPropNoRedraw::SetValueNoRedraw(_variant_t varValue)
{
	ASSERT_VALID (this);

	if (m_varValue.vt != VT_EMPTY && m_varValue.vt != varValue.vt)
	{
		ASSERT (FALSE);
		return;
	}

	BOOL bInPlaceEdit = m_bInPlaceEdit;
	if (bInPlaceEdit)
	{
		OnEndEdit ();
	}

	m_varValue = varValue;

	if (bInPlaceEdit)
	{
		ASSERT_VALID (m_pWndList);
		m_pWndList->EditItem (this);
	}
}

void CBCGPPropNoRedraw::OnCloseCombo()
{
	base_class::OnCloseCombo();

	ASSERT_VALID (m_pWndList);
	m_pWndList->SetFocus ();
	m_pWndList->SetCurSel(NULL);
}


BOOL CBCGPPropNoRedraw::OnEndEdit ()
{
	return base_class::OnEndEdit();
}