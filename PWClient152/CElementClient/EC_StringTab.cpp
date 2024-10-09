/*
 * FILE: EC_StringTab.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_StringTab.h"
#include "AF.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECStringTab
//	
///////////////////////////////////////////////////////////////////////////

CECStringTab::CECStringTab() : 
m_AStrTab(512),
m_WStrTab(512)
{
	m_bInit	= false;
}

CECStringTab::~CECStringTab()
{
	CECStringTab::Release();
}

//	Initialize object
bool CECStringTab::Init(const char* szFile, bool bUnicode)
{
	if (m_bInit)
		return true;

	m_bUnicode	= bUnicode;

	bool bRet;

	if (bUnicode)
		bRet = LoadWideStrings(szFile);
	else
		bRet = LoadANSIStrings(szFile);

	if (!bRet)
	{
		a_LogOutput(1, "CECStringTab::Init, Failed to open file %s", szFile);
		return false;
	}

	m_bInit = true;

	return true;
}

//	Release object
void CECStringTab::Release()
{
	//	Release all AISI strings
	if (m_AStrTab.size())
	{
		AStrTable::iterator it = m_AStrTab.begin();
		for (; it != m_AStrTab.end(); ++it)
		{
			AString* pstr = *it.value();
			delete pstr;
		}
	}

	m_AStrTab.clear();

	//	Release all unicode strings
	if (m_WStrTab.size())
	{
		WStrTable::iterator it = m_WStrTab.begin();
		for (; it != m_WStrTab.end(); ++it)
		{
			AWString* pstr = *it.value();
			delete pstr;
		}
	}

	m_WStrTab.clear();

	m_bInit = false;
}

//	Load ANSI strings from file
bool CECStringTab::LoadANSIStrings(const char* szFile)
{
	AScriptFile ScriptFile;
	if (!ScriptFile.Open(szFile))
		return false;

	bool bIndex = false;
	bool bFileEnd = true;

	//	Read configs
	while (ScriptFile.GetNextToken(true))
	{
		if (!stricmp(ScriptFile.m_szToken, "#_index"))
			bIndex = true;
		else if (!stricmp(ScriptFile.m_szToken, "#_begin"))
		{
			bFileEnd = false;
			break;
		}
	}

	if (bFileEnd)
	{
		ScriptFile.Close();
		return true;
	}

	if (bIndex)
	{
		//	Every string has a preset index
		while (ScriptFile.PeekNextToken(true))
		{
			int n = ScriptFile.GetNextTokenAsInt(true);
			ScriptFile.GetNextToken(false);

			AString* pstr = new AString(ScriptFile.m_szToken);
			if (!pstr)
			{
				ScriptFile.Close();
				glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECStringTab::LoadANSIStrings", __LINE__);
				return false;
			}

			if (!m_AStrTab.put(n, pstr))
			{
				delete pstr;
				ASSERT(0);
			}
		}
	}
	else
	{
		int iCnt = 0;

		//	Read strings sequently
		while (ScriptFile.GetNextToken(true))
		{
			AString* pstr = new AString(ScriptFile.m_szToken);
			if (!pstr)
			{
				ScriptFile.Close();
				glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECStringTab::LoadANSIStrings", __LINE__);
				return false;
			}

			if (!m_AStrTab.put(iCnt++, pstr))
			{
				delete pstr;
				ASSERT(0);
			}
		}
	}

	ScriptFile.Close();

	return true;
}

//	Load Unicode strings from file
bool CECStringTab::LoadWideStrings(const char* szFile)
{
	AWScriptFile ScriptFile;
	if (!ScriptFile.Open(szFile))
		return false;

	bool bIndex = false;
	bool bFileEnd = true;

	//	Read configs
	while (ScriptFile.GetNextToken(true))
	{
		if (!wcsicmp(ScriptFile.m_szToken, L"#_index"))
			bIndex = true;
		else if (!wcsicmp(ScriptFile.m_szToken, L"#_begin"))
		{
			bFileEnd = false;
			break;
		}
	}

	if (bFileEnd)
	{
		ScriptFile.Close();
		return true;
	}

	if (bIndex)
	{
		//	Every string has a preset index
		while (ScriptFile.PeekNextToken(true))
		{
			int n = ScriptFile.GetNextTokenAsInt(true);
			ScriptFile.GetNextToken(false);

			AWString* pstr = new AWString(ScriptFile.m_szToken);
			if (!pstr)
			{
				ScriptFile.Close();
				glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECStringTab::LoadWideStrings", __LINE__);
				return false;
			}

			if (!m_WStrTab.put(n, pstr))
			{
				delete pstr;
				ASSERT(0);
			}
		}
	}
	else
	{
		int iCnt = 0;

		//	Read strings sequently
		while (ScriptFile.GetNextToken(true))
		{
			AWString* pstr = new AWString(ScriptFile.m_szToken);
			if (!pstr)
			{
				ScriptFile.Close();
				glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECStringTab::LoadWideStrings", __LINE__);
				return false;
			}

			if (!m_WStrTab.put(iCnt++, pstr))
			{
				delete pstr;
				ASSERT(0);
			}
		}
	}

	ScriptFile.Close();

	return true;
}

