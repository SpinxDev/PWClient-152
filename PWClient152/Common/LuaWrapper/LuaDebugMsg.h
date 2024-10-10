/********************************************************************
	created:	2007/08/16
	author:		liudong
	
	purpose:	lua debug msg
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#pragma  once

#include <Windows.h>
#include <tchar.h>

class LD_TreeRoot;

#ifdef _UNICODE
#define  DEBUGGER_PROCESS_NAME      "LuaDebugger_u.exe"
#else
#define  DEBUGGER_PROCESS_NAME      "LuaDebugger.exe"
#endif

#define  DEBUGGER_WIN_NAME          _T("LuaDebugger")

#define  DEBUGGER_FILEMAP_NAME      _T("LuaDebugger_FileMap")
#define  DEBUGGER_FILEMAP_SIZE      8*1024*1024
#define  DEBUGGER_EVENT_WRITE       _T("LuaDebugger_Event_Write")
#define  DEBUGGER_EVENT_READ        _T("LuaDebugger_Event_Read")
#define  DEBUGGER_TOKEN_LEN         255
#define  DEBUGGER_INFO_LEN			1024

typedef enum {
	D_AI,
	D_CFG,
	D_NONE
} DEBUG_TYPE;


enum LDM_Type
{
	//both send
	LDM_SEND_BUFFER,
	LDM_APP_EXIT,

	//app send
	LDM_REPORT_ERROR,
	LDM_DEBUG_START,
	LDM_BREAK,
	LDM_RT_ERROR,
	LDM_SEND_ALL_GLOBAL_VAR,
	LDM_SEND_ALL_LOCAL_VAR,
	LDM_SEND_WATCH_VAR,

	//show which buffer and show which line,called when user double click the stack trace.
	LDM_SHOW_BUFFER_LINE,

	//debugger send
	LDM_REQUEST_BUFFER,
	LDM_DEBUGGER_EXIT,
	LDM_REQUEST_WATCH_VAR,
	LDM_REQUEST_ALL_GLOBAL_VAR,
	LDM_REQUEST_ALL_LOCAL_VAR,
	LDM_STACK_CHANGE,
	LDM_GO,
	LDM_BREAK_POINT,
	LDM_STEP_OVER,
	LDM_STEP_INTO,
	LDM_STEP_OUT,
	LDM_RUN_TO_CURSOR,
	
	//no message
	LDM_NONE,
};

enum LDM_Direction
{
	LDM_APP2DEBUGGER,
	LDM_DEBUGGER2APP,
};

class   LDM_Header
{
public:
	LDM_Header(){ 
		timeDiscard= INFINITE;
		ZeroMemory(appName,DEBUGGER_TOKEN_LEN*sizeof(_TCHAR));
	}

	unsigned long  appId;
	unsigned long  appSubId;
	int            direction;
	int		       msgType;
	//the length of the message's body.(bytes,not include the header's length).
	int            bodyLength; 
	unsigned long  timeStamp;
	//the time(milliseconds) means the message is dirty and should be discard.
	unsigned long  timeDiscard;
    _TCHAR         appName[DEBUGGER_TOKEN_LEN];
};
 
//the message sent by IPC
class LuaDebugMsg
{
public:
	LuaDebugMsg();
	~LuaDebugMsg();
    void Release();
	LuaDebugMsg& operator=(const LuaDebugMsg&);

	LDM_Header header;
	char*    data;
};

class LDM_Interface
{
public:
	virtual void ToLDM(LuaDebugMsg& msg) = 0;
	virtual void FromLDM(LuaDebugMsg& msg) = 0;
	virtual ~LDM_Interface(){}
};

//-------------------both app and debugger send-----------------
//sent buffer of lua
class LDM_SendBuffer : public LDM_Interface
{
public:
	LDM_SendBuffer();
	~LDM_SendBuffer();
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
	void Release();

	_TCHAR name[DEBUGGER_TOKEN_LEN];
	int  bufLen;
	char * buf;
};

//App exit
class LDM_AppExit : public LDM_Interface
{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg){};
};


//-----------------------app send-----------------------------------
//app inform debugger compile error
class LDM_ReportError : public LDM_Interface
{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
	bool FromLuaError(const _TCHAR * err);

	_TCHAR name[DEBUGGER_TOKEN_LEN];
	int    line;
	_TCHAR   info[DEBUGGER_INFO_LEN];
};

//App inform debugger start debugging now
class LDM_DebugStart : public LDM_Interface{
public:
	void   ToLDM(LuaDebugMsg& msg);
	void   FromLDM(LuaDebugMsg& msg);

	_TCHAR name[DEBUGGER_TOKEN_LEN];//buf name
};

//app inform debugger the information of current break point
class LDM_Break : public LDM_Interface{
public:
	LDM_Break();
	~LDM_Break();
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
	void Release();

	int     lineNum;//the line breaking now,zero based
    _TCHAR  name[DEBUGGER_TOKEN_LEN];//buf name
	int     stackLen;//call stack length in characters
	_TCHAR* callStack;//may not include _T('\0') in the end
};

//app inform debugger to show the buffer and the line
class LDM_ShowBufferLine : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);

	int     line;//当前显示行的行号,zero based
    _TCHAR  name[DEBUGGER_TOKEN_LEN];//buf name
};

//app inform debugger run time error
class LDM_RTError : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
};


//app send all global variables to debugger
class LDM_SendAllGlobalVar : public LDM_Interface{
public:
	//input the tree to be sent when sending message.
	//input the root of the tree to receive the data.
	//should never input NULL pointer
	LDM_SendAllGlobalVar(LD_TreeRoot* pRoot){
		m_pRoot = pRoot;
	}

	void ToLDM(LuaDebugMsg& msg);

	void FromLDM(LuaDebugMsg& msg);

	LD_TreeRoot* m_pRoot;
};
 

//app send all local variables to debugger
class LDM_SendAllLocalVar : public LDM_Interface{
public:
	//input the tree to be sent when sending message.
	//input the root of the tree to receive the data.
	//should never input NULL pointer
	LDM_SendAllLocalVar(LD_TreeRoot* pRoot){
		m_pRoot = pRoot;
	}

	void ToLDM(LuaDebugMsg& msg);

	void FromLDM(LuaDebugMsg& msg);

	LD_TreeRoot* m_pRoot;
};

//app send a variables that can be seen in current state to debugger
class LDM_SendWatchVar : public LDM_Interface{
public:
	//input the tree to be sent when sending message.
	//input the root of the tree to receive the data.
	//should never input NULL pointer
	LDM_SendWatchVar(LD_TreeRoot* pRoot){
		m_pRoot = pRoot;
	}

	void ToLDM(LuaDebugMsg& msg);

	void FromLDM(LuaDebugMsg& msg);

	LD_TreeRoot* m_pRoot;
};



//------------------------debugger send------------------------------
//debugger inform app to execute to next line
class LDM_StepOver : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg){};
};

//debugger inform app to execute to next line ,
//if meet with a function ,enter into it.
class LDM_StepInto : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg){};
};

//debugger inform app to run out of the current function
class LDM_StepOut : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg){};
};

//debugger inform app to execute to the selected buffer and line
class LDM_RunToCursor : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
    
	_TCHAR name[DEBUGGER_TOKEN_LEN];//buffer name to run to
	int    line;//line in the buffer to run to
};


// node to store the name of watched variable
class WatchVarNameNode{
public:
	WatchVarNameNode();
	~WatchVarNameNode();
    _TCHAR            name[DEBUGGER_TOKEN_LEN];
	WatchVarNameNode* pNext;
};

//debugger request variables can be seen in current state from app
class LDM_RequestWatchVar : public LDM_Interface{
public:
	LDM_RequestWatchVar();
	~LDM_RequestWatchVar();
    void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
	void Release();
	
	int               m_NameNum;//num of variable to watch
	WatchVarNameNode* m_pHead;
};

//debugger request all global variables from app
class LDM_RequestAllGlobalVar : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
};

//debugger request all local variables from app
class LDM_RequestAllLocalVar : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);
};

//debugger inform app the state is changed according to the stack trace .
class LDM_StackChange : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);

    int  level;//now level of the stack trace,the root is zero.
};

//debugger exit
class LDM_DebuggerExit : public LDM_Interface
{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg){};
};

//debugger ask for lua buffer from app.
class LDM_RequestBuffer : public LDM_Interface
{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);

	_TCHAR name[DEBUGGER_TOKEN_LEN];
};

//debugger inform app to execute until meet with a break point.
class LDM_Go : public LDM_Interface{
public:
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg){};
};

//debugger inform app a break point is added or deleted.
class LDM_BreakPoint: public LDM_Interface{
public:
	LDM_BreakPoint(){ isAdd =true; };
	void ToLDM(LuaDebugMsg& msg);
	void FromLDM(LuaDebugMsg& msg);

	bool   isAdd;//true:add break point, false: delete an exited break point.
	int    lineNum;//the line of the breakpoint.
	_TCHAR name[DEBUGGER_TOKEN_LEN];//the buffer name of the break point.
};