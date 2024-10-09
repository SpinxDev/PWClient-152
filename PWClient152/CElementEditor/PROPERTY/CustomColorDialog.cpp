// CustomColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CustomColorDialog.h"
#include <a3dtypes.h>
#include <a3dmacros.h>
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomColorDialog

IMPLEMENT_DYNAMIC(CCustomColorDialog, CColorDialog)

CCustomColorDialog::CCustomColorDialog(DWORD clrInit, DWORD dwFlags, CWnd* pParentWnd)
 : CColorDialog(
	RGB(A3DCOLOR_GETRED(clrInit), A3DCOLOR_GETGREEN(clrInit), A3DCOLOR_GETBLUE(clrInit)), 
	dwFlags, 
	pParentWnd),
   m_byte_alpha(A3DCOLOR_GETALPHA(clrInit))
{
	
}

BEGIN_MESSAGE_MAP(CCustomColorDialog, CColorDialog)
	//{{AFX_MSG_MAP(CCustomColorDialog)
	ON_EN_CHANGE(COLOR_ALPHA, OnChangeAlpha)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCustomColorDialog::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: 交换数据
	DDX_Text(pDX, COLOR_ALPHA, m_byte_alpha);
	CColorDialog::DoDataExchange(pDX);
}

BOOL CCustomColorDialog::OnInitDialog() 
{
	CColorDialog::OnInitDialog();
	
	// TODO: 初始化对话框
	UpdateData(FALSE);
	return TRUE;
}

void CCustomColorDialog::OnChangeAlpha() 
{
	UpdateData(TRUE);
	if(m_byte_alpha > 255) m_byte_alpha = 255;
	UpdateData(FALSE);
}

DWORD CCustomColorDialog::GetColor() const
{
	COLORREF color = CColorDialog::GetColor();
	return A3DCOLORRGBA(GetRValue(color), GetGValue(color), GetBValue(color), m_byte_alpha);
}
