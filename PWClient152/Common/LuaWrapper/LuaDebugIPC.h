/********************************************************************
	created:	2007/08/23
	author:		liudong
	
	purpose:	interprocess communication (shared memory)
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#pragma once

#include <Mmsystem.h>
#include <Windows.h>
#include <tchar.h>

class LuaDebugMsg;

class CLuaDebugIPC  
{
public:
	CLuaDebugIPC();
	virtual ~CLuaDebugIPC();

	bool Init();

	//remarks:  send message, if succeed return true,else block.
	//          if not succeed to send the message in dwMilliseconds,return false.
	//msg    :  input the message to send.
	//dwMilliseconds: input the max waiting time (miliseconds).
	//return :  if succeed to send the message return true,else false.
	bool Send(LuaDebugMsg& msg,
			  DWORD dwMilliseconds=INFINITE);
	
	//remarks:receive message and store it in the variable msg, if succeed,return true,
	//        else block the current thread. if not receive message in 
    //        dwMilliseconds(miliseconds),return false,if receive message but the message
	//        is not sent to me,push back the message as if the message has not been read
	//        and return false immediately .  
	//msg    :input which kind of message to receive,input the direction and IDs.
	//        if both ID and subID are zero ,that means all messages whose Direction is 
	//        the same as the input direction is to received.
	//dwMilliseconds: 
	//        input the max time to wait(milliseconds).INFINITE means if not receive 
	//        right message the current thread will block forever.
	//return: if succeed return true£¬else return false¡£
	bool Receive(LuaDebugMsg& msg, 
				 DWORD dwMilliseconds=INFINITE);

    // set the state of the shared memory , after set, we can write data in the
	// shared memory,and there is no  data for read. 
	// this function is only called by debugger,
	// the app(client) don't call this function.
	bool ResetMemoryState();

private:
	void Release();

private:
	HANDLE   m_hMapFile;   //handle to the map file
	HANDLE   m_hEventRead; //handle to the read event
	HANDLE   m_hEventWrite;//handle to the write event
	LPVOID   m_pData;      //pointer to data in shared memory
	
	INT64       m_frequency;   //the counter of CPU per second.
};
