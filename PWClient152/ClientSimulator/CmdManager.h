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
	// 命令处理函数映射
	typedef void (*FN_CMD_PROC) ( StringVector& params );
	typedef abase::hash_map<AString,FN_CMD_PROC> CommandMap;

public:
	~CmdManager();

	bool AddCommand(const AString& cmd);

	// 注册命令
	bool Register( const AString& cmd,FN_CMD_PROC pfnProc );
	void Unregister( const AString& cmd );

	// 获取上一条历史
	AString GetPrevHistory();
	// 获取下一条历史
	AString GetNextHistory();

	static CmdManager& GetSingleton();

protected:
	CommandMap m_CmdMap;		// 命令处理函数映射
	StringVector m_CmdHistory;	// 命令记录
	int m_iCurHistory;			// 当前历史记录

private:
	CmdManager();

	// 添加到历史记录中
	void AddHistory( const AString& cmd );
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
