/*
 * FILE: CmdProc.cpp
 *
 * DESCRIPTION: 本文件包含了各种命令的实现
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


/** 登录账号
Param1: 
Param2: 
*/
CMD_PROC(login)
{
	const char*pDefault = "123456";// 默认密码

	if( params.size() == 1)        // 只输入用户名，密码默认
	{
		params.push_back(pDefault);
		MsgManager::GetSingleton().AddMessage(MSG_LOGIN, 2, params);
	}
	else if( params.size() == 2)  // 输入一个角色的用户名，密码
	{
		MsgManager::GetSingleton().AddMessage(MSG_LOGIN, 2, params);
	}
	else if( params.size() == 3)  // 多个角色一起登陆，第一个参数是名称前缀，第二个是起始，最后一个是终止,密码默认
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
	else if( params.size() == 4)  // 多个角色一起登陆，第一个参数是名称前缀，第二个是起始，第三个是终止，最后一个是密码
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

/** 重新登录所有玩家
*/
CMD_PROC(relogin)
{
	if( params.size() == 0 )		// 重新登录所有玩家
	{
		MsgManager::GetSingleton().AddMessage(MSG_RELOGINALL, 0, params);
	}
	else if( params.size() == 1 )	// 重新登录指定玩家
	{
		MsgManager::GetSingleton().AddMessage(MSG_RELOGINALL, 1, params);
	}
	else
	{
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
	}
}

/** 退出账号
@Param1: 要推出的玩家的账号
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

/** 将所有玩家传唤到当前副本的指定地点
@Param: X
@Param: Z
*/
CMD_PROC(move)
{
	// iParam == 0, 所有玩家瞬移
	// iParam == 1, 指定玩家瞬移
	// iParam == 2, 所有玩家移动到指定点
	// iParam == 3, 指定玩家移动到指定点
	if( params.size() <= 3 )
	{
		MsgManager::GetSingleton().AddMessage(MSG_PLAYERMOVE, (int)params.size(), params);
	}
	else
	{
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
	}
}

/** 执行一段LUA脚本
*/
CMD_PROC(exec)
{
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;

	AString luaNamespace = LUA_CS_NAMESPACE_CALL;
	AString luaFileName  = "lua_test.lua"; // 默认的lua执行文件
	AString luaFuncName  = "mytest";       // 默认的lua函数名称
	if( params.size() == 0 )
	{
		
	}
	else if( params.size() == 1 ) // 更改函数名称
	{
		if(params[0] == "?") // 查询注册函数
		{
			LuaFuncFactory::GetSingleton().PrintUsage(0);
			return;
		}
		luaFuncName = params[0];
	}
	else if( params.size() == 2 ) // 更改文件名称和函数名称
	{
		if(params[0] == "?") // 查询注册函数
		{
			LuaFuncFactory::GetSingleton().PrintUsage(params[1]);
			return;
		}
		luaFileName = params[0];
		luaFuncName = params[1];
	}
	else if( params.size() == 3 ) // 更改命名空间和文件名称和函数名称
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

/** 执行Debug命令
*/
CMD_PROC(command)
{
	if( params.size() < 1) return;

	StringVector cmdParams;
	std::string scmd = params[0];
	// 去除引号
	if(scmd[0] == '"') scmd.substr(1,scmd.size() - 2);
	// 解析d_c2scmd命令
	ParseCommandLine(scmd.c_str(), cmdParams);

	if( params.size() == 1)       // 同一个命令 作用到全部角色
	{
		MsgManager::GetSingleton().AddMessage(MSG_C2SCOMMAND, 0, cmdParams);
	}
	else if( params.size() == 2)  // 命令仅作用到指定角色
	{
		cmdParams.push_back(params[1]);// 将指定角色名称放入命令的最后面
		MsgManager::GetSingleton().AddMessage(MSG_C2SCOMMAND, 1, cmdParams);
	}
}

/** 显示玩家位置
*/
CMD_PROC(showpos)
{
	if( params.size() == 0)       // 作用到全部角色
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWPOS, 0, params);
	}
	else if( params.size() == 1)  // 命令仅作用到指定角色
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWPOS, 1, params);
	}
}

/** 显示玩家信息
*/
CMD_PROC(showinfo)
{
	if( params.size() == 0 )		// 作用到全部角色
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWINFO, 0, params);
	}
	else if( params.size() == 1 )	// 作用到指定角色
	{
		MsgManager::GetSingleton().AddMessage(MSG_SHOWINFO, 1, params);
	}
}

/** 设置玩家活动区域
@remarks
	目前MoveAgent, 记录区域信息的大小为大地图(-2000, -2000) ~ (2000, 2000)
	该命令给定参数可以在此范围内调整
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

/** 改变玩家AI模式
*/
CMD_PROC(aimode)
{
	if( params.size() == 1 ) // AI策略名称
	{
		if(params[0] == "-") params[0] = "";
		MsgManager::GetSingleton().AddMessage(MSG_HOSTPLAYERAI, 0, params);
	}
	else if( params.size() == 2 ) // AI策略名称，指定玩家
	{
		if(params[0] == "-") params[0] = "";
		MsgManager::GetSingleton().AddMessage(MSG_HOSTPLAYERAI, 1, params);
	}
	else
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
}

/** 执行AI命令
*/
CMD_PROC(aicmd)
{
	if( params.size() < 1) return;

	StringVector cmdParams;
	std::string scmd = params[0];
	// 去除引号
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

/** 改变玩家强制PK模式
*/
CMD_PROC(pkmode)
{
	if( params.size() == 1 )		// PK模式
	{
		MsgManager::GetSingleton().AddMessage(MSG_FORCEATTACK, 0, params);
	}
	else if( params.size() == 2 )	// PK模式，指定玩家
	{
		MsgManager::GetSingleton().AddMessage(MSG_FORCEATTACK, 1, params);
	}
	else
		AddConsoleText("<!> Invalid parameter count!\n", LOGCOLOR_RED);
}


/** 查看帮助信息
*/
CMD_PROC(help)
{
	if( params.size() == 0)
	{// 列出命令
		AddConsoleText("-----------------------所有命令列表:------------------------\n", LOGCOLOR_GREEN);
		AddConsoleText("login		登陆一个新玩家。\n", LOGCOLOR_GREEN);
		AddConsoleText("logout		登出一个玩家。\n", LOGCOLOR_GREEN);
		AddConsoleText("relogin		重新登录。\n", LOGCOLOR_GREEN);
		AddConsoleText("command		执行一个C2S的命令。\n", LOGCOLOR_GREEN);
		AddConsoleText("move		对玩家进行瞬移。\n", LOGCOLOR_GREEN);
		AddConsoleText("exec		执行LUA脚本。\n", LOGCOLOR_GREEN);
		AddConsoleText("showpos		显示玩家位置。\n", LOGCOLOR_GREEN);
		AddConsoleText("showinfo	显示玩家信息。\n", LOGCOLOR_GREEN);
		AddConsoleText("pkmode		设置玩家PK模式。\n", LOGCOLOR_GREEN);
		AddConsoleText("aimode		设置玩家策略模式。\n", LOGCOLOR_GREEN);
		AddConsoleText("aicmd		执行一个策略里的命令。\n", LOGCOLOR_GREEN);
		AddConsoleText("region		设置玩家活动区域。\n", LOGCOLOR_GREEN);
		AddConsoleText("help		显示帮助信息。\n", LOGCOLOR_GREEN);
		AddConsoleText("cls		清除控制台屏幕。\n", LOGCOLOR_GREEN);
		AddConsoleText("quit		退出程序。\n", LOGCOLOR_GREEN);
		AddConsoleText("----------------可使用如下方式查询以上命令:-----------------\n", LOGCOLOR_GREEN);
		AddConsoleText("help login\n", LOGCOLOR_GREEN);
		AddConsoleText("------------------------结束--------------------------------\n", LOGCOLOR_GREEN);
	}
	else if( params.size() == 1)
	{// 列出指定命令示范
		AString cmdName = params[0];
		AddConsoleText("-------------------------使用方法:--------------------------\n", LOGCOLOR_GREEN);

		if( cmdName.CompareNoCase("login") == 0 )
		{
			AddConsoleText("参数：用户名(默认密码是123456的用户)\n", LOGCOLOR_GREEN);
			AddConsoleText("login tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：用户名 用户密码\n", LOGCOLOR_GREEN);
			AddConsoleText("login tj1001 123456\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：用户名前缀 用户名后缀数字开始 用户名后缀数字结束(默认密码是123456的用户)\n", LOGCOLOR_GREEN);
			AddConsoleText("login tj 1001 1009\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：用户名前缀 用户名后缀数字开始 用户名后缀数字结束 用户密码(密码要一样才行) \n", LOGCOLOR_GREEN);
			AddConsoleText("login tj 1001 1009 123456\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("logout") == 0 )
		{
			AddConsoleText("参数：用户名\n", LOGCOLOR_GREEN);
			AddConsoleText("logout tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：无（登出所有玩家）\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("relogin") == 0 )
		{
			AddConsoleText("参数：无（重新登录所有用户）\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：用户名（登出指定用户）\n", LOGCOLOR_GREEN);
			AddConsoleText("logout tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("command") == 0 )
		{
			AddConsoleText("参数：双引号将d_c2scmd命令括起来作为一个参数（该命令作用到所有已login用户）\n", LOGCOLOR_GREEN);
			AddConsoleText("command \"d 10800 11212 1\"\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：第二个参数是指定命令作用到的用户名称\n", LOGCOLOR_GREEN);
			AddConsoleText("command \"d 10800 11212 1\" tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("help") == 0 )
		{
			AddConsoleText("参数：无参数（列出所有命令）\n", LOGCOLOR_GREEN);
			AddConsoleText("help\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：系统注册的命令名称\n", LOGCOLOR_GREEN);
			AddConsoleText("help login\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("quit") == 0 )
		{
			AddConsoleText("quit 退出应用程序\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("cls") == 0 )
		{
			AddConsoleText("cls 清除控制台屏幕\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("move") == 0 )
		{
			AddConsoleText("参数：无（所有玩家瞬移）\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：用户名（指定玩家瞬移）\n", LOGCOLOR_GREEN);
			AddConsoleText("move tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：横坐标 纵坐标（所有玩家移动到此点）\n", LOGCOLOR_GREEN);
			AddConsoleText("move 100 100\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：横坐标 纵坐标 用户名（指定玩家移动到此点）\n", LOGCOLOR_GREEN);
			AddConsoleText("move 144 100 tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("exec") == 0 )
		{
			AddConsoleText("无参数：调用当前目录下的lua_test.lua 中的mytest函数\n", LOGCOLOR_GREEN);
			AddConsoleText("exec\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：?,写死的问号。用于查询系统内注册的函数\n", LOGCOLOR_GREEN);
			AddConsoleText("exec ?\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：? 函数或命名空间[可模糊匹配]\n", LOGCOLOR_GREEN);
			AddConsoleText("exec ? Cmd\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：函数名（默认为lua_test.lua）\n", LOGCOLOR_GREEN);
			AddConsoleText("exec mytest\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：脚本文件名 函数名 \n", LOGCOLOR_GREEN);
			AddConsoleText("exec lua_test.lua mytest\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：命名空间 脚本文件名 函数名 \n", LOGCOLOR_GREEN);
			AddConsoleText("exec Cmd lua_test.lua mytest\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("showpos") == 0 )
		{
			AddConsoleText("无参数：显示所有玩家当前位置\n", LOGCOLOR_GREEN);
			AddConsoleText("showpos\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：玩家账号名称\n", LOGCOLOR_GREEN);
			AddConsoleText("showpos tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("showinfo") == 0 )
		{
			AddConsoleText("无参数：显示所有玩家信息\n", LOGCOLOR_GREEN);
			AddConsoleText("showinfo\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：玩家账号名称\n", LOGCOLOR_GREEN);
			AddConsoleText("showinfo tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("pkmode") == 0 )
		{
			AddConsoleText("参数：PK模式1或0\n", LOGCOLOR_GREEN);
			AddConsoleText("pkmode 1\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：PK模式 指定玩家\n", LOGCOLOR_GREEN);
			AddConsoleText("pkmode 1 tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("aimode") == 0 )
		{
			AddConsoleText("参数：AI策略名字\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode FoolAI\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：AI策略名字 用户名\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode ProtectAI tj1001\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：清空所有策略\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode -\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：清空该用户策略 用户名\n", LOGCOLOR_GREEN);
			AddConsoleText("aimode - tj1001\n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("aicmd") == 0 )
		{
			AddConsoleText("参数：AI命令\n", LOGCOLOR_GREEN);
			AddConsoleText("aicmd \"follow 123456\" \n", LOGCOLOR_GREEN);
			AddConsoleText("参数：AI命令 用户名\n", LOGCOLOR_GREEN);
			AddConsoleText("aicmd \"protect 123456\" tj72000 \n", LOGCOLOR_GREEN);
		}
		else if( cmdName.CompareNoCase("region") == 0 )
		{
			AddConsoleText("设置玩家在世界中可活动的范围\n", LOGCOLOR_GREEN);
			AddConsoleText("参数：最小X值，最小Z值，最大X值，最大Z值\n", LOGCOLOR_GREEN);
			AddConsoleText("region -1000 -500 800 1200\n", LOGCOLOR_GREEN);
		}
		else {}
		AddConsoleText("------------------------结束--------------------------------\n", LOGCOLOR_GREEN);
	}
	else{}
	
}

/** 退出程序
*/
CMD_PROC(quit)
{
	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
}

/** 清除控制台屏幕
*/
CMD_PROC(cls)
{
	ClearConsoleText();
}

/** 关于
*/
CMD_PROC(about)
{
	AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND, 0x0010, 0);
}

///////////////////////////////////////////////////////////////////////////
// lua 注册函数

// 登陆
int login(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	login_proc(params);
	return 1;
}

// 登出
int logout(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	logout_proc(params);
	return 1;
}

// 移动
int move(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	move_proc(params);
	return 1;
}

// 命令
int command(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	command_proc(params);
	return 1;
}

// lua脚本执行命令（即可以嵌套执行exec）
int exec(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	exec_proc(params);
	return 1;
}

// 结束
int quit(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	quit_proc(params);
	return 1;
}

// 改变角色AI策略
int aimode(lua_State * L)
{
	StringVector params;
	LuaGetParam(L,params);
	aimode_proc(params);
	return 1;
}

// 改变角色PK模式
int pkmode(lua_State* L)
{
	StringVector params;
	LuaGetParam(L, params);
	pkmode_proc(params);
	return 1;
}

// 执行AI命令
int aicmd(lua_State* L)
{
	StringVector params;
	LuaGetParam(L, params);
	aicmd_proc(params);
	return 1;
}

// 设置玩家活动区域
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

	// debug 命令
	REGISTER_CMD(command); 
	// 移动到指定地点
	REGISTER_CMD(move);
	// LUA脚本执行
	REGISTER_CMD(exec);
	// 显示玩家位置
	REGISTER_CMD(showpos);
	// 显示玩家信息
	REGISTER_CMD(showinfo);
	// 改变玩家PK模式
	REGISTER_CMD(pkmode);
	// 改变玩家AI模式
	REGISTER_CMD(aimode);
	// 执行AI命令
	REGISTER_CMD(aicmd);
	// 设置玩家活动区域
	REGISTER_CMD(region);

	///////////////////////////////////////////////
	// 将命令暴露给lua脚本

	const char* pnamespace = "Cmd";
	LUA_CS_REGISTER(pnamespace, login,   "角色登陆。"   );
	LUA_CS_REGISTER(pnamespace, logout,  "角色登出。"   );
	LUA_CS_REGISTER(pnamespace, command, "debug命令。"  );
	LUA_CS_REGISTER(pnamespace, pkmode,  "改变玩家PK模式。");
	LUA_CS_REGISTER(pnamespace, aimode,  "改变玩家AI模式。");
	LUA_CS_REGISTER(pnamespace, aicmd,	 "执行AI命令。");
	LUA_CS_REGISTER(pnamespace, region,  "设置玩家活动区域。");
	LUA_CS_REGISTER(pnamespace, move,    "移动角色。"   );
	LUA_CS_REGISTER(pnamespace, exec,    "执行lua脚本。");
	LUA_CS_REGISTER(pnamespace, quit,    "退出程序。"   );

	// 全部一起注册
	LuaFuncFactory::GetSingleton().Flash();
	return true;
}