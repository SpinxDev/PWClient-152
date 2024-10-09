/*
 * FILE: CmdProc.cpp
 *
 * DESCRIPTION: ���ļ������˸��������ʵ��
 *
 * CREATED BY: Shizhenhua, 2013/4/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "StdAfx.h"
#include "CmdManager.h"
#include "SessionManager.h"
#include "EC_SendC2SCmds.h"
#include "MsgManager.h"
#include "LuaFunc.h"

#include <string>


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

extern volatile bool g_bGameRun;


///////////////////////////////////////////////////////////////////////////
//  
//  Command Process Functions
//  
///////////////////////////////////////////////////////////////////////////


/** ��¼�˺�
Param1: 
Param2: 
*/
CMD_PROC(login)
{
	const char*pDefault = "123456";// Ĭ������

	if( params.size() == 1)        // ֻ�����û���������Ĭ��
	{
		params.push_back(pDefault);
		MsgManager::GetSingleton().AddMessage(MSG_LOGIN, 2, params);
	}
	else if( params.size() == 2)  // ����һ����ɫ���û���������
	{
		MsgManager::GetSingleton().AddMessage(MSG_LOGIN, 2, params);
	}
	else if( params.size() == 3)  // �����ɫһ���½����һ������������ǰ׺���ڶ�������ʼ�����һ������ֹ,����Ĭ��
	{
		std::string prefix   = params[0];
		int start            = params[1].ToInt();
		int end              = params[2].ToInt();
		
		char temp[100];
		StringVector vtemp;
		for( int i=start;i<=end;i++ )
		{
			sprintf( temp, "%s%d", prefix.c_str(),i);
			vtemp.clear();
			vtemp.push_back(temp);
			vtemp.push_back(pDefault);
			MsgManager::GetSingleton().AddMessage(MSG_LOGIN, 2, vtemp);
		}
	}
	else if( params.size() == 4)  // �����ɫһ���½����һ������������ǰ׺���ڶ�������ʼ������������ֹ�����һ��������
	{
		std::string prefix   = params[0];
		int start            = params[1].ToInt();
		int end              = params[2].ToInt();
		std::string password = params[3]; 

		char temp[100];
		StringVector vtemp;
		for( int i=start;i<=end;i++ )
		{
			sprintf( temp, "%s%d", prefix.c_str(),i);
			vtemp.clear();
			vtemp.push_back(temp);
			vtemp.push_back(password.c_str());
			MsgManager::GetSingleton().AddMessage(MSG_LOGIN, 2, vtemp);
		}
	}
	else {}
}

/** ���µ�¼�������
*/
CMD_PROC(relogin)
{
	if( params.size() == 0 )		// ���µ�¼�������
	{
		MsgManager::GetSingleton().AddMessage(MSG_RELOGINALL, 0, params);
	}
	else if( params.size() == 1 )	// ���µ�¼ָ�����
	{
		MsgManager::GetSingleton().AddMessage(MSG_RELOGINALL, 1, params);
	}
	else
	{
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
	}
}

/** �˳��˺�
@Param1: Ҫ�Ƴ�����ҵ��˺�
*/
CMD_PROC(logout)
{
	if( params.size() == 0 )
	{
		MsgManager::GetSingleton().AddMessage(MSG_LOGOUT, 0, params);
	}
	else if( params.size() == 1 )
	{
		MsgManager::GetSingleton().AddMessage(MSG_LOGOUT, 1, params);
	}
	else
	{
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
	}
}

/** ��������Ҵ�������ǰ������ָ���ص�
@Param: X
@Param: Z
*/
CMD_PROC(move)
{
	// iParam == 0, �������˲��
	// iParam == 1, ָ�����˲��
	// iParam == 2, ��������ƶ���ָ����
	// iParam == 3, ָ������ƶ���ָ����
	if( params.size() <= 3 )
	{
		MsgManager::GetSingleton().AddMessage(MSG_PLAYERMOVE, (int)params.size(), params);
	}
	else
	{
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
	}
}

/** ִ��һ��LUA�ű�
*/
CMD_PROC(exec)
{
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;

	AString luaNamespace = LUA_CS_NAMESPACE_CALL;
	AString luaFileName  = "lua_test.lua"; // Ĭ�ϵ�luaִ���ļ�
	AString luaFuncName  = "mytest";       // Ĭ�ϵ�lua��������
	if( params.size() == 0 )
	{
		
	}
	else if( params.size() == 1 ) // ���ĺ�������
	{
		if(params[0] == "?") // ��ѯע�ắ��
		{
			LuaFuncFactory::GetSingleton().PrintUsage(0);
			return;
		}
		luaFuncName = params[0];
	}
	else if( params.size() == 2 ) // �����ļ����ƺͺ�������
	{
		if(params[0] == "?") // ��ѯע�ắ��
		{
			LuaFuncFactory::GetSingleton().PrintUsage(params[1]);
			return;
		}
		luaFileName = params[0];
		luaFuncName = params[1];
	}
	else if( params.size() == 3 ) // ���������ռ���ļ����ƺͺ�������
	{
		luaNamespace = params[0];
		luaFileName  = params[1];
		luaFuncName  = params[2];
	}
	else{}

	StringVector cmdParams;

	cmdParams.push_back(luaNamespace);
	cmdParams.push_back(luaFileName);
	cmdParams.push_back(luaFuncName);

	MsgManager::GetSingleton().AddMessage(MSG_LUACALL, 0, cmdParams);
}

/** ִ��Debug����
*/
CMD_PROC(command)
{
	if( params.size() < 1) return;

	StringVector cmdParams;
	std::string scmd = params[0];
	// ȥ������
	if(scmd[0] == '"') scmd.substr(1,scmd.size() - 2);
	// ����d_c2scmd����
	ParseCommandLine(scmd.c_str(), cmdParams);

	if( params.size() == 1)       // ͬһ������ ���õ�ȫ����ɫ
	{
		MsgManager::GetSingleton().AddMessage(MSG_C2SCOMMAND, 0, cmdParams);
	}
	else if( params.size() == 2)  // ��������õ�ָ����ɫ
	{
		cmdParams.push_back(params[1]);// ��ָ����ɫ���Ʒ�������������
		MsgManager::GetSingleton().AddMessage(MSG_C2SCOMMAND, 1, cmdParams);
	}
}

/** ��ʾ���λ��
*/
CMD_PROC(showpos)
{
	if( params.size() == 0)       // ���õ�ȫ����ɫ
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWPOS, 0, params);
	}
	else if( params.size() == 1)  // ��������õ�ָ����ɫ
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWPOS, 1, params);
	}
}

/** ��ʾ�����Ϣ
*/
CMD_PROC(showinfo)
{
	if( params.size() == 0 )		// ���õ�ȫ����ɫ
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWINFO, 0, params);
	}
	else if( params.size() == 1 )	// ���õ�ָ����ɫ
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWINFO, 1, params);
	}
}

/** ������һ����
@remarks
	ĿǰMoveAgent, ��¼������Ϣ�Ĵ�СΪ���ͼ(-2000, -2000) ~ (2000, 2000)
	������������������ڴ˷�Χ�ڵ���
*/
CMD_PROC(region)
{
	if( params.size() >= 4 )
	{
		MsgManager::GetSingleton().AddMessage(MSG_SETREGION, 0, params);
	}
	else
	{
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
	}
}

/** �ı����AIģʽ
*/
CMD_PROC(aimode)
{
	if( params.size() == 1 ) // AI��������
	{
		if(params[0] == "-") params[0] = "";
		MsgManager::GetSingleton().AddMessage(MSG_HOSTPLAYERAI, 0, params);
	}
	else if( params.size() == 2 ) // AI�������ƣ�ָ�����
	{
		if(params[0] == "-") params[0] = "";
		MsgManager::GetSingleton().AddMessage(MSG_HOSTPLAYERAI, 1, params);
	}
	else
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
}

/** ִ��AI����
*/
CMD_PROC(aicmd)
{
	if( params.size() < 1) return;

	StringVector cmdParams;
	std::string scmd = params[0];
	// ȥ������
	if(scmd[0] == '"') scmd.substr(1,scmd.size() - 2);
	ParseCommandLine(scmd.c_str(), cmdParams);

	if( params.size() == 1 )
	{
		MsgManager::GetSingleton().AddMessage(MSG_AICOMMAND, 0, cmdParams);
	}
	else if( params.size() == 2 )
	{
		cmdParams.push_back(params[1]);
		MsgManager::GetSingleton().AddMessage(MSG_AICOMMAND, 1, cmdParams);
	}
}

/** �ı����ǿ��PKģʽ
*/
CMD_PROC(pkmode)
{
	if( params.size() == 1 )		// PKģʽ
	{
		MsgManager::GetSingleton().AddMessage(MSG_FORCEATTACK, 0, params);
	}
	else if( params.size() == 2 )	// PKģʽ��ָ�����
	{
		MsgManager::GetSingleton().AddMessage(MSG_FORCEATTACK, 1, params);
	}
	else
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
}


/** �鿴������Ϣ
*/
CMD_PROC(help)
{
	if( params.size() == 0)
	{// �г�����
		AddConsoleText("-----------------------���������б�:------------------------\n", LOGCOLOR_GREEN);
		AddConsoleText("login		��½һ������ҡ�\n", LOGCOLOR_GREEN);
		AddConsoleText("logout		�ǳ�һ����ҡ�\n", LOGCOLOR_GREEN);
		AddConsoleText("relogin		���µ�¼��\n", LOGCOLOR_GREEN);
		AddConsoleText("command		ִ��һ��C2S�����\n", LOGCOLOR_GREEN);
		AddConsoleText("move		����ҽ���˲�ơ�\n", LOGCOLOR_GREEN);
		AddConsoleText("exec		ִ��LUA�ű���\n", LOGCOLOR_GREEN);
		AddConsoleText("showpos		��ʾ���λ�á�\n", LOGCOLOR_GREEN);
		AddConsoleText("showinfo	��ʾ�����Ϣ��\n", LOGCOLOR_GREEN);
		AddConsoleText("pkmode		�������PKģʽ��\n", LOGCOLOR_GREEN);
		AddConsoleText("aimode		������Ҳ���ģʽ��\n", LOGCOLOR_GREEN);
		AddConsoleText("aicmd		ִ��һ������������\n", LOGCOLOR_GREEN);
		AddConsoleText("region		������һ����\n", LOGCOLOR_GREEN);
		AddConsoleText("help		��ʾ������Ϣ��\n", LOGCOLOR_GREEN);
		AddConsoleText("cls		�������̨��Ļ��\n", LOGCOLOR_GREEN);
		AddConsoleText("quit		�˳�����\n", LOGCOLOR_GREEN);
		AddConsoleText("----------------��ʹ�����·�ʽ��ѯ��������:-----------------\n", LOGCOLOR_GREEN);
		AddConsoleText("help login\n", LOGCOLOR_GREEN);
		AddConsoleText("------------------------����--------------------------------\n", LOGCOLOR_GREEN);
	}
	else if( params.size() == 1)
	{// �г�ָ������ʾ��
		AString cmdName = params[0];
		AddConsoleText("-------------------------ʹ�÷���:--------------------------\n", LOGCOLOR_GREEN);

		if( cmdName.CompareNoCase("login") == 0 )
		{
			AddConsoleText("�������û���(Ĭ��������123456���û�)\n", LOGCOLOR_GREEN);
			AddConsoleText("login tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("�������û��� �û�����\n", LOGCOLOR_GREEN);
			AddConsoleText("login tj1001 123456\n", LOGCOLOR_GREEN);
			AddConsoleText("�������û���ǰ׺ �û�����׺���ֿ�ʼ �û�����׺���ֽ���(Ĭ��������123456���û�)\n", LOGCOLOR_GREEN);
			AddConsoleText("login tj 1001 1009\n", LOGCOLOR_GREEN);
			AddConsoleText("�������û���ǰ׺ �û�����׺���ֿ�ʼ �û�����׺���ֽ��� �û�����(����Ҫһ������) \n", LOGCOLOR_GREEN);
			AddConsoleText("login tj 1001 1009 123456\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("logout") == 0 )
		{
			AddConsoleText("�������û���\n", LOGCOLOR_GREEN);
			AddConsoleText("logout tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("�������ޣ��ǳ�������ң�\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("relogin") == 0 )
		{
			AddConsoleText("�������ޣ����µ�¼�����û���\n", LOGCOLOR_GREEN);
			AddConsoleText("�������û������ǳ�ָ���û���\n", LOGCOLOR_GREEN);
			AddConsoleText("logout tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("command") == 0 )
		{
			AddConsoleText("������˫���Ž�d_c2scmd������������Ϊһ�����������������õ�������login�û���\n", LOGCOLOR_GREEN);
			AddConsoleText("command \"d 10800 11212 1\"\n", LOGCOLOR_GREEN);
			AddConsoleText("�������ڶ���������ָ���������õ����û�����\n", LOGCOLOR_GREEN);
			AddConsoleText("command \"d 10800 11212 1\" tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("help") == 0 )
		{
			AddConsoleText("�������޲������г��������\n", LOGCOLOR_GREEN);
			AddConsoleText("help\n", LOGCOLOR_GREEN);
			AddConsoleText("������ϵͳע�����������\n", LOGCOLOR_GREEN);
			AddConsoleText("help login\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("quit") == 0 )
		{
			AddConsoleText("quit �˳�Ӧ�ó���\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("cls") == 0 )
		{
			AddConsoleText("cls �������̨��Ļ\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("move") == 0 )
		{
			AddConsoleText("�������ޣ��������˲�ƣ�\n", LOGCOLOR_GREEN);
			AddConsoleText("�������û�����ָ�����˲�ƣ�\n", LOGCOLOR_GREEN);
			AddConsoleText("move tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("������������ �����꣨��������ƶ����˵㣩\n", LOGCOLOR_GREEN);
			AddConsoleText("move 100 100\n", LOGCOLOR_GREEN);
			AddConsoleText("������������ ������ �û�����ָ������ƶ����˵㣩\n", LOGCOLOR_GREEN);
			AddConsoleText("move 144 100 tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("exec") == 0 )
		{
			AddConsoleText("�޲��������õ�ǰĿ¼�µ�lua_test.lua �е�mytest����\n", LOGCOLOR_GREEN);
			AddConsoleText("exec\n", LOGCOLOR_GREEN);
			AddConsoleText("������?,д�����ʺš����ڲ�ѯϵͳ��ע��ĺ���\n", LOGCOLOR_GREEN);
			AddConsoleText("exec ?\n", LOGCOLOR_GREEN);
			AddConsoleText("������? �����������ռ�[��ģ��ƥ��]\n", LOGCOLOR_GREEN);
			AddConsoleText("exec ? Cmd\n", LOGCOLOR_GREEN);
			AddConsoleText("��������������Ĭ��Ϊlua_test.lua��\n", LOGCOLOR_GREEN);
			AddConsoleText("exec mytest\n", LOGCOLOR_GREEN);
			AddConsoleText("�������ű��ļ��� ������ \n", LOGCOLOR_GREEN);
			AddConsoleText("exec lua_test.lua mytest\n", LOGCOLOR_GREEN);
			AddConsoleText("�����������ռ� �ű��ļ��� ������ \n", LOGCOLOR_GREEN);
			AddConsoleText("exec Cmd lua_test.lua mytest\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("showpos") == 0 )
		{
			AddConsoleText("�޲�������ʾ������ҵ�ǰλ��\n", LOGCOLOR_GREEN);
			AddConsoleText("showpos\n", LOGCOLOR_GREEN);
			AddConsoleText("����������˺�����\n", LOGCOLOR_GREEN);
			AddConsoleText("showpos tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("showinfo") == 0 )
		{
			AddConsoleText("�޲�������ʾ���������Ϣ\n", LOGCOLOR_GREEN);
			AddConsoleText("showinfo\n", LOGCOLOR_GREEN);
			AddConsoleText("����������˺�����\n", LOGCOLOR_GREEN);
			AddConsoleText("showinfo tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("pkmode") == 0 )
		{
			AddConsoleText("������PKģʽ1��0\n", LOGCOLOR_GREEN);
			AddConsoleText("pkmode 1\n", LOGCOLOR_GREEN);
			AddConsoleText("������PKģʽ ָ�����\n", LOGCOLOR_GREEN);
			AddConsoleText("pkmode 1 tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("aimode") == 0 )
		{
			AddConsoleText("������AI��������\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode FoolAI\n", LOGCOLOR_GREEN);
			AddConsoleText("������AI�������� �û���\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode ProtectAI tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("������������в���\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode -\n", LOGCOLOR_GREEN);
			AddConsoleText("��������ո��û����� �û���\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode - tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("aicmd") == 0 )
		{
			AddConsoleText("������AI����\n", LOGCOLOR_GREEN);
			AddConsoleText("aicmd \"follow 123456\" \n", LOGCOLOR_GREEN);
			AddConsoleText("������AI���� �û���\n", LOGCOLOR_GREEN);
			AddConsoleText("aicmd \"protect 123456\" tj72000 \n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("region") == 0 )
		{
			AddConsoleText("��������������пɻ�ķ�Χ\n", LOGCOLOR_GREEN);
			AddConsoleText("��������СXֵ����СZֵ�����Xֵ�����Zֵ\n", LOGCOLOR_GREEN);
			AddConsoleText("region -1000 -500 800 1200\n", LOGCOLOR_GREEN);
		}
		else {}
		AddConsoleText("------------------------����--------------------------------\n", LOGCOLOR_GREEN);
	}
	else{}
	
}

/** �˳�����
*/
CMD_PROC(quit)
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
}

/** �������̨��Ļ
*/
CMD_PROC(cls)
{
	ClearConsoleText();
}

/** ����
*/
CMD_PROC(about)
{
	AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND, 0x0010, 0);
}

///////////////////////////////////////////////////////////////////////////
// lua ע�ắ��

// ��½
int login(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	login_proc(params);
	return 1;
}

// �ǳ�
int logout(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	logout_proc(params);
	return 1;
}

// �ƶ�
int move(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	move_proc(params);
	return 1;
}

// ����
int command(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	command_proc(params);
	return 1;
}

// lua�ű�ִ�����������Ƕ��ִ��exec��
int exec(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	exec_proc(params);
	return 1;
}

// ����
int quit(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	quit_proc(params);
	return 1;
}

// �ı��ɫAI����
int aimode(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	aimode_proc(params);
	return 1;
}

// �ı��ɫPKģʽ
int pkmode(lua_State* L)
{
	StringVector params;
	LuaGetParam(L, params);
	pkmode_proc(params);
	return 1;
}

// ִ��AI����
int aicmd(lua_State* L)
{
	StringVector params;
	LuaGetParam(L, params);
	aicmd_proc(params);
	return 1;
}

// ������һ����
int region(lua_State* L)
{
	StringVector params;
	LuaGetParam(L, params);
	region_proc(params);
	return 1;
}

///////////////////////////////////////////////////////////////////////////

bool _InitCmdProc()
{
	REGISTER_CMD(login);
	REGISTER_CMD(logout);
	REGISTER_CMD(help);
	REGISTER_CMD(quit);
	REGISTER_CMD(relogin);
	REGISTER_CMD(cls);
	REGISTER_CMD(about);

	//////////////////////////////////

	// debug ����
	REGISTER_CMD(command); 
	// �ƶ���ָ���ص�
	REGISTER_CMD(move);
	// LUA�ű�ִ��
	REGISTER_CMD(exec);
	// ��ʾ���λ��
	REGISTER_CMD(showpos);
	// ��ʾ�����Ϣ
	REGISTER_CMD(showinfo);
	// �ı����PKģʽ
	REGISTER_CMD(pkmode);
	// �ı����AIģʽ
	REGISTER_CMD(aimode);
	// ִ��AI����
	REGISTER_CMD(aicmd);
	// ������һ����
	REGISTER_CMD(region);

	///////////////////////////////////////////////
	// �����¶��lua�ű�

	const char* pnamespace = "Cmd";
	LUA_CS_REGISTER(pnamespace, login,   "��ɫ��½��"   );
	LUA_CS_REGISTER(pnamespace, logout,  "��ɫ�ǳ���"   );
	LUA_CS_REGISTER(pnamespace, command, "debug���"  );
	LUA_CS_REGISTER(pnamespace, pkmode,  "�ı����PKģʽ��");
	LUA_CS_REGISTER(pnamespace, aimode,  "�ı����AIģʽ��");
	LUA_CS_REGISTER(pnamespace, aicmd,	 "ִ��AI���");
	LUA_CS_REGISTER(pnamespace, region,  "������һ����");
	LUA_CS_REGISTER(pnamespace, move,    "�ƶ���ɫ��"   );
	LUA_CS_REGISTER(pnamespace, exec,    "ִ��lua�ű���");
	LUA_CS_REGISTER(pnamespace, quit,    "�˳�����"   );

	// ȫ��һ��ע��
	LuaFuncFactory::GetSingleton().Flash();
	return true;
}