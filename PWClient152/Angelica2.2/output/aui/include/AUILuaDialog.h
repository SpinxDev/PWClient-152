/********************************************************************
	created:	2007/09/13
	author:		kuiwu
	
	purpose:	aui lua dialog
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#ifndef _AUILUADIALOG_H_
#define _AUILUADIALOG_H_

#include "AUIDialog.h"

class CLuaScript;

class AUILuaDialog : public AUIDialog
{
public:
	AUILuaDialog();
	virtual ~AUILuaDialog();
	virtual bool Release();
	virtual bool Render();
	virtual void Show(bool bShow, bool bModal = false, bool bActive = true);
	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName);
	virtual bool Tick();
	
	void SetTimer(bool bTimer);
	void SetTickCB(const bool bTickCB){ m_bTickCB = bTickCB; }

	static AString lua_filepath;
	
protected:
	virtual bool OnEventMap(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	virtual bool OnCommandMap(const char *szCommand);
	virtual bool RetrieveLuaFileName();

	CLuaScript * m_pLuaScript;
	bool         m_bRenderCB;   //indicate whether script has render callback
	bool         m_bTickCB;     //indicate whether script has render callback
	bool         m_bTimer; 
	DWORD        m_dwTimerStart;
	AString		 m_szLuaFilename;
};





#endif  