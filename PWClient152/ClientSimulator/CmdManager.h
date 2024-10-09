/*
 * FILE: CmdManager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once


#include "Common.h"

///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define CMD_PROC(cmd) void cmd##_proc( StringVector& params )
#define REGISTER_CMD_PROC(cmd, proc) CmdManager::GetSingleton().Register(#cmd, &proc)
#define REGISTER_CMD(cmd) CmdManager::GetSingleton().Register(#cmd, &cmd##_proc)

#define MAX_COMMAND_HISTORY 20


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  class CmdManager
//  
///////////////////////////////////////////////////////////////////////////

class CmdManager
{
public:
	// �������ӳ��
	typedef void (*FN_CMD_PROC) ( StringVector& params );
	typedef abase::hash_map<AString,FN_CMD_PROC> CommandMap;

public:
	~CmdManager();

	bool AddCommand(const AString& cmd);

	// ע������
	bool Register( const AString& cmd,FN_CMD_PROC pfnProc );
	void Unregister( const AString& cmd );

	// ��ȡ��һ����ʷ
	AString GetPrevHistory();
	// ��ȡ��һ����ʷ
	AString GetNextHistory();

	static CmdManager& GetSingleton();

protected:
	CommandMap m_CmdMap;		// �������ӳ��
	StringVector m_CmdHistory;	// �����¼
	int m_iCurHistory;			// ��ǰ��ʷ��¼

private:
	CmdManager();

	// ��ӵ���ʷ��¼��
	void AddHistory( const AString& cmd );
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
