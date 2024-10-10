#pragma once
#include "wndbase.h"

class CWndScale : public CWndBase
{
public:
	CWndScale(void);
	~CWndScale(void);
public:
	void					SetData(float fMinValue, float fMaxValue, float fPerPixelValue, float fInterval);

protected:
	virtual void			DrawRealize(CDC* pDC);
	void					DrawScale(CDC* pDC);
protected:
	float					m_fMinValue;
	float					m_fMaxValue;
	float					m_fPerPixelValue;
	float					m_fInterval;
};
