/*
 * FILE: MsgManager.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/9/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "MsgManager.h"
#include "SessionManager.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class MsgManager
//  
///////////////////////////////////////////////////////////////////////////

// 获取实例
MsgManager& MsgManager::GetSingleton()
{
	static MsgManager obj;
	return obj;
}

// 构造函数
MsgManager::MsgManager()
{
}

// 析构函数
MsgManager::~MsgManager()
{
	// 销毁未处理的消息
	for( int i=0;i<m_Messages.GetCount();i++ )
	{
		MESSAGE* pMsg = m_Messages.RemoveHead();
		FreeMessage(pMsg);
	}
}

// 添加消息
// Param: 消息ID
// Param: 附加参数
// Param: 附加content
bool MsgManager::AddMessage(int iMsg, int iParam, const StringVector& content)
{
	MESSAGE* pMsg = new MESSAGE;

	pMsg->iMsg     = iMsg;
	pMsg->iParam   = iParam;
	pMsg->content  = content;

	if( CsLockScoped lock(m_csMsgQueue) ) // 加锁
	{
		m_Messages.AddTail(pMsg);
	}
	return true;
}

// 处理消息
void MsgManager::Tick()
{
	// 先拷贝出来，因不知道ProcessMessage处理多长时间
	MessageQueue tempMsg;
	int i,iCount;
	if( CsLockScoped lock(m_csMsgQueue) ) // 加锁
	{
		iCount = m_Messages.GetCount();
		
		for( i=0;i<iCount;i++ )
		{
			MESSAGE* pMsg = m_Messages.RemoveHead();
			tempMsg.AddTail(pMsg);
		}
	}

	for( i=0;i<iCount;i++ )
	{
		MESSAGE* pMsg = tempMsg.RemoveHead();
		SessionManager::GetSingleton().ProcessMessage(pMsg);
		FreeMessage(pMsg);
	}
}

// 销毁消息
void MsgManager::FreeMessage( MESSAGE* pMsg )
{
	delete pMsg;
}