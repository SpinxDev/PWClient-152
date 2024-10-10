/********************************************************************
	created:	2007/09/13
	author:		kuiwu
	
	purpose:	aui lua manager
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#include "AUI.h"

#include "AUIDef.h"
#include "AUILuaManager.h"
#include "AUILuaDialog.h"
#include "AUICommon.h"
#include "LuaState.h"
#include "LuaScript.h"
#include "ScriptValue.h"
#include "AFI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "AIniFile.h"

AUILuaManager::AUILuaManager()
:m_pLuaScript(NULL)
{
	m_bTickCB = false;
	m_bLuaInited = false;
}

AUILuaManager::~AUILuaManager()
{

}

bool AUILuaManager::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, 
					  const char *pszFilename, int nDefaultWidth, int nDefaultHeight)
{
	CLuaState * pLuaState = g_LuaStateMan.GetAIState();
	if (!pLuaState)
	{
		return	AUI_ReportError(__LINE__, 1, "AUILuaManager::Init, Failed to get luastate");
	}

	m_pLuaScript = NULL;
	if( pszFilename )
	{
		char szName[MAX_PATH];
		af_GetFileTitle(pszFilename, szName, sizeof(szName));
		AIniFile scripte_file;
		if( scripte_file.Open("Configs\\UIScriptPath.ini") )
		{
			AUILuaDialog::lua_filepath = scripte_file.GetValueAsString("UI", "ScriptPath", AUILuaDialog::lua_filepath);
			scripte_file.Close();
		}

		m_strFilename = szName;
		if (!RetrieveLuaFileName())
		{
			return	AUI_ReportError(__LINE__, 1, "AUILuaManager::Init, Failed to retrieve lua file name");
		}

		if( af_IsFileExist(m_strLuaFilename) )
		{
			pLuaState->Lock();
			m_pLuaScript = pLuaState->RegisterFile(m_strLuaFilename);
			pLuaState->Unlock();
		}
	}


	if( !AUIManager::Init(pA3DEngine, pA3DDevice, pszFilename, nDefaultWidth, nDefaultHeight) )
		return false;

	if (!m_pLuaScript)
	{
		return true;  //???  true?
	}

	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;

	m_pLuaScript->GetLuaState()->LockCall(m_szName, "IsTick", args, results);
	if(results.size() > 0)
		m_bTickCB = results[0].GetBool();

	m_pLuaScript->GetLuaState()->LockCall(m_szName, "Init", args, results);
	m_bLuaInited = true;
	if( pszFilename )
	{
		A3DRECT rcOld, rcNew;
		A3DDEVFMT fmt = m_pA3DEngine->GetA3DDevice()->GetDevFormat();
		rcOld.SetRect(0, 0, m_nDefaultWidth, m_nDefaultHeight);
		rcNew.SetRect(0, 0, fmt.nWidth, fmt.nHeight);
		LuaResizeWindows(rcOld, rcNew);
	}

	return true;
}

bool AUILuaManager::Tick(DWORD dwTime)
{
	if(m_bLuaInited && m_pLuaScript && m_bTickCB)
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;
		args.push_back((double)dwTime);
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "Tick", args, results);
	}

	return AUIManager::Tick(dwTime);
}

bool AUILuaManager::Release()
{
	if (m_pLuaScript)
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "Release", args, results);
		CLuaState * pLuaState = m_pLuaScript->GetLuaState();
		pLuaState->Lock();
		pLuaState->UnRegister(m_pLuaScript->GetName());
		pLuaState->Unlock();
		m_pLuaScript = NULL;
	}
	m_bLuaInited = false;

	return AUIManager::Release();
}

void AUILuaManager::ResizeWindows(A3DRECT rcOld, A3DRECT rcNew)
{
	AUIManager::ResizeWindows(rcOld, rcNew);

	if (m_bLuaInited)
		LuaResizeWindows(rcOld, rcNew);
}

void AUILuaManager::LuaResizeWindows(A3DRECT rcOld, A3DRECT rcNew)
{
	if( m_pLuaScript )
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;
		args.push_back(CScriptValue((double)rcOld.left));
		args.push_back(CScriptValue((double)rcOld.top));
		args.push_back(CScriptValue((double)rcOld.Width()));
		args.push_back(CScriptValue((double)rcOld.Height()));
		args.push_back(CScriptValue((double)rcNew.left));
		args.push_back(CScriptValue((double)rcNew.top));
		args.push_back(CScriptValue((double)rcNew.Width()));
		args.push_back(CScriptValue((double)rcNew.Height()));
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "ResizeWindows", args, results);
	}
}

bool AUILuaManager::DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( AUIManager::DealWindowsMessage(uMsg, wParam, lParam) )
		return true;

	if (!m_bLuaInited || !m_pLuaScript)
		return false;

	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;

	args.push_back(CScriptValue((double)uMsg));
	

	CScriptValue val;

	switch(uMsg) 
	{
	case WM_MOUSEWHEEL:
		{
			short	fwKeys = (short)LOWORD(wParam);
			short   zDelta = (short)HIWORD(wParam);
			args.push_back(CScriptValue((double)fwKeys));
			args.push_back(CScriptValue((double)zDelta));
			short    lo  = (short) LOWORD(lParam);  //maybe x 
			short    hi  = (short) HIWORD(lParam);  //maybe y
			args.push_back(CScriptValue((double)lo));
			args.push_back(CScriptValue((double)hi));
		}
		break;
	
	default:
		{
			args.push_back(CScriptValue((double)wParam));
			short    lo  = (short) LOWORD(lParam);  //maybe x 
			short    hi  = (short) HIWORD(lParam);  //maybe y
			args.push_back(CScriptValue((double)lo));
			args.push_back(CScriptValue((double)hi));
		}
		break;
	}

	m_pLuaScript->GetLuaState()->LockCall(m_szName, "OnEventMap", args, results);
	if( results.size() >= 1 && results[0].m_Type == CScriptValue::SVT_BOOL )
		return results[0].GetBool();
	else
	{
		AUI_ReportError("%s:OnEventMap message %d no return value!", m_szName, uMsg);
	}
	
// 	return true;
	return false;
}

void AUILuaManager::SendLuaEvent(const abase::vector<CScriptValue> &args, abase::vector<CScriptValue> *pResults/* = 0*/)
{
	if (m_bLuaInited && m_pLuaScript)
	{
		if (!pResults) {
			abase::vector<CScriptValue> results;
			m_pLuaScript->GetLuaState()->LockCall(m_szName, "OnEventMap", args, results);
		}
		else {
			m_pLuaScript->GetLuaState()->LockCall(m_szName, "OnEventMap", args, *pResults);
		}	
	}
}

bool AUILuaManager::OnMessageBox(int nRetVal, PAUIDIALOG pDlg)
{
	if (m_bLuaInited && m_pLuaScript)
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;

		CScriptValue val;
		val.SetVal(pDlg->GetName());
		args.push_back(val);
		val.SetVal(nRetVal);
		args.push_back(val);
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "OnMessageBox", args, results);
	}
	return AUIManager::OnMessageBox(nRetVal, pDlg);
}

PAUIDIALOG AUILuaManager::CreateDlgInstance(const AString strTemplName)
{
	return new AUILuaDialog;
}

bool AUILuaManager::RetrieveLuaFileName()
{
	m_strLuaFilename = AUILuaDialog::lua_filepath + m_strFilename;
	int index = m_strLuaFilename.ReverseFind('.');
	if (index < 0)
	{
		m_strLuaFilename = "";
		return false;
	}

	m_strLuaFilename = m_strLuaFilename.Left(index) + ".lua";
	return true;
}