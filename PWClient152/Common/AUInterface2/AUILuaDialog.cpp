/********************************************************************
	created:	2007/09/13
	author:		kuiwu
	
	purpose:	aui lua dialog
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#include "AUI.h"

#include "AUILuaDialog.h"
#include "LuaState.h"
#include "LuaScript.h"
#include "ScriptValue.h"
#include "AIniFile.h"
#include "AFI.h"

#include "AUIManager.h"
#include "AUICommon.h"
#include "AUIDef.h"

AString AUILuaDialog::lua_filepath = "script\\interfaces\\";

AUILuaDialog::AUILuaDialog()
:m_pLuaScript(NULL), m_bRenderCB(false), m_bTickCB(false), m_bTimer(false), m_szLuaFilename("")
{
	
}

AUILuaDialog::~AUILuaDialog()
{

}

bool AUILuaDialog::RetrieveLuaFileName()
{
	m_szLuaFilename = lua_filepath + m_szFilename;
	int index = m_szLuaFilename.ReverseFind('.');
	if (index < 0)
	{
		m_szLuaFilename = "";
		return false;
	}

	m_szLuaFilename = m_szLuaFilename.Left(index) + ".lua";
	return true;
}

bool AUILuaDialog::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszTempName)
{
	if (!AUIDialog::Init(pA3DEngine, pA3DDevice, pszTempName))
	{
		return false;
	}

	CLuaState * pLuaState = g_LuaStateMan.GetAIState();
	if (!pLuaState)
	{
		return	AUI_ReportError(__LINE__, 1, "AUILuaDialog::OnInitDialog, Failed to get luastate");
	}
	
	if( !RetrieveLuaFileName() )
		return	AUI_ReportError(__LINE__, 1, "AUILuaDialog::OnInitDialog, Failed to retrieve lua file name");
	
	m_pLuaScript = NULL;
	if( af_IsFileExist(m_szLuaFilename) )
	{
		pLuaState->Lock();
		m_pLuaScript = pLuaState->RegisterFile(m_szLuaFilename);
		pLuaState->Unlock();
	}

	if (!m_pLuaScript)
	{
		return true;  //???  true?
	}

	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;

	
	m_pLuaScript->GetLuaState()->LockCall(m_szName, "IsTick", args, results);
	if(results.size() > 0)
		m_bTickCB = results[0].GetBool();
	m_pLuaScript->GetLuaState()->LockCall(m_szName, "IsRender", args, results);
	if(results.size() > 0)
		m_bRenderCB = results[0].GetBool();

	m_pLuaScript->GetLuaState()->LockCall(m_szName, "Init", args, results);
	return true;
}


bool AUILuaDialog::Release()
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
	return AUIDialog::Release();
}

bool AUILuaDialog::Render()
{
   	if (!AUIDialog::Render())
		return false;

	if (m_pLuaScript && m_bRenderCB)
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "Render", args, results);
	}	

	return true;
}

bool AUILuaDialog::Tick()
{
	if( !AUIDialog::Tick() )
		return false;

	if (m_pLuaScript && m_bTickCB)
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;
		args.push_back((double)m_pAUIManager->GetTickTime());
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "Tick", args, results);
	}

	if( m_pLuaScript && m_bTimer )
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;
		args.push_back(double(GetTickCount() - m_dwTimerStart));
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "OnTimer", args, results);
	}

	return true;
}


void AUILuaDialog::Show(bool bShow, bool bModal, bool bActive)
{
	AUIDialog::Show(bShow, bModal, bActive);

	if (!m_pLuaScript)
	{
		return;
	}
	
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;
	if (bShow)
	{
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "ShowDialog", args, results);
	}
	else
	{
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "HideDialog", args, results);
	}
	
}


bool AUILuaDialog::OnEventMap(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	bool bval = AUIDialog::OnEventMap(uMsg, wParam, lParam, pObj);

	if (!m_pLuaScript)
	{
		return bval;
	}

	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;

	args.push_back(CScriptValue((double)uMsg));

	const char * name = (pObj)? (pObj->GetName()) : ("");
	CScriptValue val;
	val.SetVal(name);
	args.push_back(val);

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
	
	return false;
}


bool AUILuaDialog::OnCommandMap(const char *szCommand)
{
	bool bval = AUIDialog::OnCommandMap(szCommand);
	
	if (!m_pLuaScript)
	{
		return bval;
	}
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;

	CScriptValue val;
	val.SetVal(szCommand);
	args.push_back(val);	
	m_pLuaScript->GetLuaState()->LockCall(m_szName, "OnEventMap", args, results);
	if( results.size() >= 1 && results[0].m_Type == CScriptValue::SVT_BOOL )
		return results[0].GetBool();
	else
	{
		AUI_ReportError("%s:OnEventMap message %d no return value!", m_szName, szCommand);
	}
	
	return true;
}

void AUILuaDialog::SetTimer(bool bTimer)
{
	m_bTimer = bTimer;
	m_dwTimerStart = GetTickCount();

	if( m_pLuaScript && m_bTimer )
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> results;
		args.push_back(double(0));
		m_pLuaScript->GetLuaState()->LockCall(m_szName, "OnTimer", args, results);
	}

}

