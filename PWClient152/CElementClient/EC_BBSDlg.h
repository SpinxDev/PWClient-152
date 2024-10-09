/*
 * FILE: EC_BBSDlg.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef EC_BBSDLG_H
#define EC_BBSDLG_H

#include "AUI\\AUI.h"
#include "EC_Global.h"
#include "EC_BBSPage.h"
#include "A3DViewport.h"

class CECBBSDlg
{

public:
	CECBBSDlg();
	~CECBBSDlg();
	bool Release();
	bool InitBBSDlg(PAUIMANAGER pAUIManager,AString initurl,int nWidth,int nHeight);
	bool SetSize(int nWidth,int nHeight);
	bool OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj);
	bool OnCommand(const char *pszCommand, PAUIDIALOG pDlg);
	bool Tick();
	bool Render();
	void Show(bool bShow=true);
	PAUIDIALOG GetBBSDlg() { return BBSDlg; }
private:
	PAUIMANAGER				m_pAUIManager;
	PAUISTILLIMAGEBUTTON	m_pGoBackButton;
	PAUISTILLIMAGEBUTTON	m_pGoAheadButton;
	PAUISTILLIMAGEBUTTON	m_pRefreshButton;
	PAUISTILLIMAGEBUTTON	m_pCloseButton;
	PAUISTILLIMAGEBUTTON	m_pHomePageButton;
	PAUILABEL				m_pFontTipLabel;
	PAUICOMBOBOX			m_pFontSizeCombo;
	bool					m_bUseNewFontSize;
	PAUIOBJECT				m_pLastObj;
	PAUIOBJECT				m_pHoverObj;
	bool					m_bInit;
	AString					m_aInitUrl;
	int						m_nHeight;
	int						m_nWidth;
	bool					m_bNeedClose;
	bool					m_bFirstOpen;
};

#endif
