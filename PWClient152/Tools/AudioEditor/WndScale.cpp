#include "StdAfx.h"
#include "WndScale.h"

CWndScale::CWndScale(void)
: m_fMinValue(0)
, m_fMaxValue(1)
{
	m_clBK = RGB(222,233,253);
}

CWndScale::~CWndScale(void)
{
}

void CWndScale::DrawRealize(CDC* pDC)
{
	DrawScale(pDC);
}

void CWndScale::DrawScale(CDC* pDC)
{
	ASSERT(pDC != NULL);

	CRect rcClient;
	GetClientRect(&rcClient);

	int iHeight = rcClient.Height();
	int iScaleY = iHeight - (int)(iHeight*0.5f + 0.5f);
	int y = iHeight - (int)(iHeight*0.3f);
	int iNum = (int)(rcClient.Width()/m_fInterval+0.5f);
	for (int i=0; i<iNum; ++i)
	{
		int x = (int)(i*m_fInterval+0.5f);
		if(i%10 == 0)
		{
			CRect rcText;
			rcText.left = x+3;
			rcText.top = 0;
			rcText.bottom = 10;
			rcText.right = x+35;
			CString strText;
			strText.Format("%.2f", i*m_fInterval*m_fPerPixelValue + m_fMinValue);
			DrawLine(pDC, x, iScaleY, x, iHeight, 0);
			DrawText(pDC, rcText, strText, 0);
		}
		else
			DrawLine(pDC, x, y, x, iHeight, 0);
	}
}

void CWndScale::SetData(float fMinValue, float fMaxValue, float fPerPixelValue, float fInterval)
{
	m_fMinValue = fMinValue;
	m_fMaxValue = fMaxValue;
	m_fPerPixelValue = fPerPixelValue;
	m_fInterval = fInterval;
	Update();
}

