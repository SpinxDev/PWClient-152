/********************************************************************
	created:	2007/09/13
	author:		kuiwu
	
	purpose:	aui lua manager
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#ifndef _AUILUAMANAGER_H_
#define _AUILUAMANAGER_H_


#include "AUIManager.h"
#include "ScriptValue.h"

class CLuaScript;

class AUILuaManager: public AUIManager
{
public:
	AUILuaManager();
	virtual ~AUILuaManager();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, 
		const char *pszFilename = NULL, int nDefaultWidth = AUIMANAGER_DEFAULTWIDTH, int nDefaultHeight = AUIMANAGER_DEFAULTHEIGHT);
	virtual bool Tick(DWORD dwTime);
	virtual bool Release();
	virtual bool DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool OnMessageBox(int nRetVal, PAUIDIALOG pDlg);
	virtual void ResizeWindows(A3DRECT rcOld, A3DRECT rcNew);
	void SendLuaEvent(const abase::vector<CScriptValue>& args, abase::vector<CScriptValue> *pResults = 0);

protected:
	virtual PAUIDIALOG CreateDlgInstance(const AString strTemplName);

	void LuaResizeWindows(A3DRECT rcOld, A3DRECT rcNew);

	virtual bool RetrieveLuaFileName();

	CLuaScript * m_pLuaScript;
	bool         m_bTickCB;
	bool		 m_bLuaInited;

	AString	     m_strLuaFilename;
};





#endif