#pragma once
#include "afxwin.h"

class CBounceButton : public CButton
{
public:
	CBounceButton(UINT uiDownResID, UINT uiUpResID);
	~CBounceButton(void);
public:
	void SetButtonState(bool bDown);
	bool IsButtonDown() const { return m_bDown; }
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
protected:
	bool m_bDown;
	CBitmap m_bmpDown;
	CBitmap m_bmpUp;
	UINT m_uiDownResID;
	UINT m_uiUpResID;
	int m_iWidth;
	int m_iHeight;
	CDC memdc,memdc2;
};
