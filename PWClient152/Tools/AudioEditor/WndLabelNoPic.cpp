#include "stdafx.h"
#include "WndLabelNoPic.h"

CWndLabelNoPic::CWndLabelNoPic(void)
{
}

CWndLabelNoPic::~CWndLabelNoPic(void)
{
}

void CWndLabelNoPic::DrawRealize(CDC* pDC)
{
	CRect rc;
	rc.left = 10;
	rc.right = 500;
	rc.top = 0;
	rc.bottom = 25;
	DrawText(pDC, rc, m_csLabel, RGB(255,255,255));
}