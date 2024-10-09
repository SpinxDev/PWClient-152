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


// ��Ϣ����
enum
{
	MSG_LOGIN,			// ��½�����
	MSG_LOGOUT,			// ע�����
	MSG_C2SCOMMAND,		// ִ��d_c2scmd����
	MSG_HOSTPLAYERAI,	// �ı��ɫAI����
	MSG_SHOWPOS,	    // ��ʾ���λ��
	MSG_LUACALL,	    // ������ֱ�ӵ���lua�ű�
	MSG_FORCEATTACK,	// �ı����ǿ��PK��ʶ
	MSG_SETREGION,		// �ı���һ����
	MSG_AICOMMAND,		// ִ��AI����
	MSG_RELOGINALL,		// ���µ�¼�������������
	MSG_PLAYERMOVE,		// ���˲��
	MSG_SHOWINFO,		// ��ʾ�����Ϣ
};


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

struct MESSAGE
{
	int iMsg;			    // ��Ϣ����
	int iParam;			    // ����
	StringVector content;   // ��������
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
