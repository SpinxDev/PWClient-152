/*
 * FILE: CmdManager.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "CmdManager.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Local types and variables
//  
///////////////////////////////////////////////////////////////////////////

extern bool _InitCmdProc();


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class CmdManager
//  
///////////////////////////////////////////////////////////////////////////

// ��ȡ����
CmdManager& CmdManager::GetSingleton()
{
	static CmdManager mgr;
	return mgr;
}

// ���캯��
CmdManager::CmdManager() : m_iCurHistory(0)
{
	_InitCmdProc();
}

// ��������
CmdManager::~CmdManager()
{
}

// ע���������
bool CmdManager::Register( const AString& cmd,FN_CMD_PROC pfnProc )
{
	ASSERT( !cmd.IsEmpty() && pfnProc );

	if( m_CmdMap.find(cmd) != m_CmdMap.end() )
	{
		a_LogOutput(1, "Command %s duplicated!", cmd);
		return false;
	}

	m_CmdMap[cmd] = pfnProc;
	return true;
}

// ɾ���������
void CmdManager::Unregister( const AString& cmd )
{
	CommandMap::iterator it = m_CmdMap.find(cmd);
	if( it != m_CmdMap.end() )
	{
		m_CmdMap.erase(it);
	}
}

// ����һ������
bool CmdManager::AddCommand(const AString& cmd)
{
	StringVector params;
	ParseCommandLine(cmd, params);
	if( params.empty() )
		return false;

	CommandMap::iterator it = m_CmdMap.find(params[0]);
	if( it != m_CmdMap.end() )
	{
		params.erase(params.begin());
		it->second(params);
		AddHistory(cmd);
	}
	else
	{
		AddConsoleText("<!> Invalid command!!\n", LOGCOLOR_RED);
	}

	return true;
}

// ���һ����ʷ��¼
void CmdManager::AddHistory( const AString& cmd )
{
	if( m_CmdHistory.size() < MAX_COMMAND_HISTORY )
		m_CmdHistory.push_back(cmd);
	else
	{
		m_CmdHistory.push_back(cmd);
		m_CmdHistory.erase(m_CmdHistory.begin());
	}

	m_iCurHistory = (int)m_CmdHistory.size();
}

// ��ȡǰһ����ʷ
AString CmdManager::GetPrevHistory()
{
	if( m_iCurHistory - 1 < 0 )
		m_iCurHistory = 0;
	else
		m_iCurHistory -= 1;

	if( m_iCurHistory >= 0 && m_iCurHistory < (int)m_CmdHistory.size() )
		return m_CmdHistory[m_iCurHistory];
	else
		return "";
}

// ��ȡ��һ����ʷ
AString CmdManager::GetNextHistory()
{
	if( m_iCurHistory + 1 >= (int)m_CmdHistory.size() )
		m_iCurHistory = m_CmdHistory.size() - 1;
	else
		m_iCurHistory += 1;

	if( m_iCurHistory >= 0 && m_iCurHistory < (int)m_CmdHistory.size() )
		return m_CmdHistory[m_iCurHistory];
	else
		return "";
}