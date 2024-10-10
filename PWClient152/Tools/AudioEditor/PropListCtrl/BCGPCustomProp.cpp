#include "stdafx.h"
#include <stdexcept>
#include "BCGPPropertyObject.h"
#include "BCGPCustomProp.h"
#include "prop_custom_funcs.h"
#include "AnyType.h"

CBCGPCustomProp::CBCGPCustomProp(const CString& strName, prop_custom_funcs* pCustomFuncs,
				LPCTSTR lpszDescr /*= NULL*/, DWORD_PTR dwData /*= 0*/,
				LPCTSTR lpszEditMask /*= NULL*/, LPCTSTR lpszEditTemplate /*= NULL*/,
				LPCTSTR lpszValidChars /*= NULL*/)
				: CBCGPPropNoRedraw(strName
				, _variant_t()
				, lpszDescr
				, dwData
				, lpszEditMask
				, lpszEditTemplate
				, lpszValidChars)
				, m_pCustomFuncs(pCustomFuncs)
{
	m_dwFlags = PROP_HAS_BUTTON;
	if(NULL == pCustomFuncs->OnGetValue())
		m_varValue = (int)0;
	else
		m_varValue = Any2VT(*pCustomFuncs->OnGetValue());
	AllowEdit(FALSE);
}

any CBCGPCustomProp::GetCustomValue() const
{
	return m_pCustomFuncs->OnGetValue();
}

void CBCGPCustomProp::SetCustomValue(any val)
{
	m_pCustomFuncs->OnSetValue(val);
}

void CBCGPCustomProp::SetValue(const _variant_t& varValue)
{
	throw std::runtime_error("not allow to call CBCGPCustomProp::SetValue.");
}

const _variant_t& CBCGPCustomProp::GetValue () const
{
	throw std::runtime_error("not allow to call CBCGPCustomProp::GetValue.");
	return m_varValue;
}

CString CBCGPCustomProp::FormatProperty ()
{
	return m_pCustomFuncs->OnGetShowString();
}

void CBCGPCustomProp::OnDrawValue (CDC* pDC, CRect rect)
{
	CBCGPProp::OnDrawValue(pDC, rect);

	m_pCustomFuncs->OnDrawValue(pDC, rect);
}

void CBCGPCustomProp::OnClickButton (CPoint point)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	m_bButtonIsDown = TRUE;
	if (!m_pCustomFuncs->OnBtnClicked())
		return;
	else
	{
		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetWindowText (FormatProperty ());
		}
		
		m_pWndList->OnPropertyChanged(this);
	}

	m_bButtonIsDown = FALSE;

	Redraw ();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		m_pWndList->SetFocus ();
	}
}