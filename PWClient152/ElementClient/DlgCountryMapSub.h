// Filename	: DlgCountryMapSub.h
// Creator	: Han Guanghui
// Date		: 2013/4/3

#ifndef _DLGCOUNTRYMAPSUB_H_
#define _DLGCOUNTRYMAPSUB_H_ 

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "EC_DomainCountry.h"

class CDlgCountryMap;
class AUISubDialog;
class CDlgCountryMapSub : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCountryMapSub();
	virtual ~CDlgCountryMapSub();

	void OnCommandCancel(const char * szCommand);

	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void DropCapture();
	void OnResize();
	SIZE GetMinImageSize();
	void UpdateImage(bool dataChanged);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual bool Render();
	virtual void OnShowDialog();
	virtual void OnHideDialog();

	static void DomainRenderCallback(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);
	void RenderDomain();
	void UpdateHint(bool bDomainDesc=true);

	PAUIIMAGEPICTURE			m_pImg_CountryMap;
	CECDomainCountry		    m_DomainCountry;
	AUISubDialog*				m_pSubDlg;
	CDlgCountryMap*				m_pDlgCountryMap;
	int							m_nHoverAreaIndex;
	int							m_nLastHoverIndex;
	A3DPOINT2					m_DraggingStartPoint;
	int							m_DragHBarPosStart;
	int							m_DragVBarPosStart;
	bool						m_bIsDragging;
	bool						m_bLButtonDown;
};

#endif