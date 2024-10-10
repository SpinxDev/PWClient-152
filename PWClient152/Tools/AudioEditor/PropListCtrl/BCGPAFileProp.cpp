#include "stdafx.h"
#include "BCGPAFileProp.h"
#include "..\Render.h"
#include "..\Global.h"

CBCGPAFileProp::CBCGPAFileProp(const CString& strName, 
			   const CString& strFileName, 
			   BOOL bOpenFileDialog /*= TRUE*/,
			   LPCTSTR lpszRelativePath /*= _T("")*/,
			   LPCTSTR lpszDefExt /*= _T("")*/,
			   int iListStyle /*= AFileDialogWrapper::AFD_USEDEFAULT*/,
			   LPCTSTR lpszDescr /*= _T("")*/, 
			   DWORD_PTR dwData /*= 0*/)
	: CBCGPPropNoRedraw(strName, _variant_t((LPCTSTR)strFileName), lpszDescr, dwData)
	, m_bIsOpenFileDialog(bOpenFileDialog)
	, m_strRelativePath(lpszRelativePath)
	, m_strDefExt(lpszDefExt)
	, m_iListStyle(iListStyle)
{
	m_dwFlags = PROP_HAS_BUTTON;
}

CBCGPAFileProp::~CBCGPAFileProp()
{

}

void CBCGPAFileProp::OnClickButton (CPoint point)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	ASSERT_VALID (m_pWndInPlace);
	ASSERT (::IsWindow (m_pWndInPlace->GetSafeHwnd ()));

	m_bButtonIsDown = TRUE;
	Redraw ();

	CString strPath = (LPCTSTR)(_bstr_t)m_varValue;
	BOOL bUpdate = FALSE;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(), m_bIsOpenFileDialog, m_strRelativePath, GetName(), m_strDefExt, m_iListStyle, m_pWndList);
	if (dlg.DoModal () == IDOK)
	{
		bUpdate = TRUE;
		strPath = dlg.GetRelativePathName();
	}

	if (bUpdate)
	{
		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetWindowText (strPath);
		}

		m_varValue = (LPCTSTR) strPath;
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
//		m_pWndList->SetCurSel(NULL);
	}

	m_pWndList->PostMessage(WM_KILLBCGPPROPSEL, 0, 0);
}