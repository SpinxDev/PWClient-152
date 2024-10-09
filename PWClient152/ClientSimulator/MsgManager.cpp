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

// ��ȡʵ��
MsgManager& MsgManager::GetSingleton()
{
	static MsgManager obj;
	return obj;
}

// ���캯��
MsgManager::MsgManager()
{
}

// ��������
MsgManager::~MsgManager()
{
	// ����δ�������Ϣ
	for( int i=0;i<m_Messages.GetCount();i++ )
	{
		MESSAGE* pMsg = m_Messages.RemoveHead();
		FreeMessage(pMsg);
	}
}

// �����Ϣ
// Param: ��ϢID
// Param: ���Ӳ���
// Param: ����content
bool MsgManager::AddMessage(int iMsg, int iParam, const StringVector& content)
{
	MESSAGE* pMsg = new MESSAGE;

	pMsg->iMsg     = iMsg;
	pMsg->iParam   = iParam;
	pMsg->content  = content;

	if( CsLockScoped lock(m_csMsgQueue) ) // ����
	{
		m_Messages.AddTail(pMsg);
	}
	return true;
}

// ������Ϣ
void MsgManager::Tick()
{
	// �ȿ�����������֪��ProcessMessage����೤ʱ��
	MessageQueue tempMsg;
	int i,iCount;
	if( CsLockScoped lock(m_csMsgQueue) ) // ����
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

// ������Ϣ
void MsgManager::FreeMessage( MESSAGE* pMsg )
{
	delete pMsg;
}