/*
 * FILE: MsgManager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once


#include <ABaseDef.h>
#include <AList2.h>
#include "Common.h"
#include "cslock.h"
///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


// 消息类型
enum
{
	MSG_LOGIN,			// 登陆新玩家
	MSG_LOGOUT,			// 注销玩家
	MSG_C2SCOMMAND,		// 执行d_c2scmd命令
	MSG_HOSTPLAYERAI,	// 改变角色AI策略
	MSG_SHOWPOS,	    // 显示玩家位置
	MSG_LUACALL,	    // 命令行直接调用lua脚本
	MSG_FORCEATTACK,	// 改变玩家强制PK标识
	MSG_SETREGION,		// 改变玩家活动区域
	MSG_AICOMMAND,		// 执行AI命令
	MSG_RELOGINALL,		// 重新登录所有已连接玩家
	MSG_PLAYERMOVE,		// 玩家瞬移
	MSG_SHOWINFO,		// 显示玩家信息
};


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

struct MESSAGE
{
	int iMsg;			    // 消息类型
	int iParam;			    // 参数
	StringVector content;   // 附加数据
};


///////////////////////////////////////////////////////////////////////////
//  
//  class MsgManager
//  
///////////////////////////////////////////////////////////////////////////

class MsgManager
{
public:

	typedef APtrList<MESSAGE*> MessageQueue;

public:
	MsgManager();
	virtual ~MsgManager();

	bool AddMessage(int iMsg, int iParam, const StringVector& content);
	void Tick();
	
	static MsgManager& GetSingleton();

protected:
	MessageQueue m_Messages;
	CsMutex m_csMsgQueue;

	void FreeMessage( MESSAGE* pMsg );
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
