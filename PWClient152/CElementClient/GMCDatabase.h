/********************************************************************
	created:	2005/10/11
	created:	11:10:2005   10:06
	file name:	GMCDatabaseDef.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "SDBTable.h"
#include "SDBDateTime.h"
#include "SDBDatabase.h"
#include <AAssist.h>

#ifdef _ELEMENTCLIENT
	#include "EC_Global.h"
	#include "EC_Game.h"
	#include "EC_GameRun.h"
	#include "EC_UIManager.h"
	#include "EC_GameUIMan.h"

	#define GETSTRINGFROMTABLE(i) g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetStringFromTable(i)
#else
	#include "AWScriptFile.h"
	#include "hashmap.h"
	#include "AUICTranslate.h"
#endif

/********************************************************************
	Tables
*********************************************************************/



//////////////////////////////////////////////////////////////////////////
// TableUserMessage
//////////////////////////////////////////////////////////////////////////
extern const char userMsgTableDBFileName[];
typedef TL::TypeListGenerator8<int, SimpleDB::DateTime, ACHAR, ACString, int, bool, SimpleDB::DateTime, ACString>::Result UserMsgTableStructure;
struct UserMsgTable : public SimpleDB::Table<UserMsgTableStructure, 2, userMsgTableDBFileName>
{
	enum FieldNames
	{
		playerID = 0,  
		timeReceivedMsg,
		msgType,
		playerMsg,
		gmID,
		isProcessed,
		timeFinishMsg,
		memo
	};

	UserMsgTable()
	{
#ifdef _ELEMENTCLIENT
		VL::SetFirstNContent(fieldStrNames_,
			GETSTRINGFROMTABLE(6101),
			GETSTRINGFROMTABLE(6102),
			GETSTRINGFROMTABLE(6103),
			GETSTRINGFROMTABLE(6104),
			GETSTRINGFROMTABLE(6105),
			GETSTRINGFROMTABLE(6106),
			GETSTRINGFROMTABLE(6107),
			GETSTRINGFROMTABLE(6108));
#else
		AWScriptFile s;
		char fn[300];
		GetModuleFileNameA(NULL,fn,300);
		int i = strlen(fn) - 1;
		while(i >= 0 && fn[i] != '\\' && fn[i] != '/' )
			fn[i--] = 0;
		strcpy(fn + i + 1, "Interfaces\\ingame.stf");
		bool bval = s.Open(fn);
		if( !bval ) return;

		abase::hash_map<int, AWString> stringTable;
		while( !s.IsEnd() )
		{
			bval = s.GetNextToken(true);
			if( !bval ) break;		// End of file.
			int idString = a_atoi(s.m_szToken);

			bval = s.GetNextToken(true);
			if( !bval ) return;

			AUICTranslate tran;
			ACString str(tran.Translate(s.m_szToken));
			stringTable[idString] = str;
		}
		s.Close();
		VL::SetFirstNContent(fieldStrNames_,
			stringTable[6101],
			stringTable[6102],
			stringTable[6103],
			stringTable[6104],
			stringTable[6105],
			stringTable[6106],
			stringTable[6107],
			stringTable[6108]);
#endif
	}
};
typedef UserMsgTable::RecordType RecordUserMsg;


//////////////////////////////////////////////////////////////////////////
// GMCommandLogTable
//////////////////////////////////////////////////////////////////////////
extern const char gmCommandLogTableDBFileName[];
typedef TL::TypeListGenerator6<ACString, ACString, int, SimpleDB::DateTime, int, SimpleDB::DateTime>::Result TableGMCommandLogStructure;
struct GMCommandLogTable : public SimpleDB::Table<TableGMCommandLogStructure, 2, gmCommandLogTableDBFileName>
{
	enum FieldNames
	{
		commandName = 0,
		commandDetail,
		gmID,
		timeExcuted,
		assocMsgPlayerID,
		assocMsgTimeReceived
	};

	GMCommandLogTable()
	{
#ifdef _ELEMENTCLIENT
		VL::SetFirstNContent(fieldStrNames_,
			GETSTRINGFROMTABLE(6201),
			GETSTRINGFROMTABLE(6202),
			GETSTRINGFROMTABLE(6203),
			GETSTRINGFROMTABLE(6204),
			GETSTRINGFROMTABLE(6205),
			GETSTRINGFROMTABLE(6206));
#else
		AWScriptFile s;
		char fn[300];
		GetModuleFileNameA(NULL,fn,300);
		int i = strlen(fn) - 1;
		while(i >= 0 && fn[i] != '\\' && fn[i] != '/' )
			fn[i--] = 0;
		strcpy(fn + i + 1, "Interfaces\\ingame.stf");
		bool bval = s.Open(fn);
		if( !bval ) return;

		abase::hash_map<int, AWString> stringTable;
		while( !s.IsEnd() )
		{
			bval = s.GetNextToken(true);
			if( !bval ) break;		// End of file.
			int idString = a_atoi(s.m_szToken);

			bval = s.GetNextToken(true);
			if( !bval ) return;

			AUICTranslate tran;
			ACString str(tran.Translate(s.m_szToken));
			stringTable[idString] = str;
		}
		s.Close();
		VL::SetFirstNContent(fieldStrNames_,
			stringTable[6201],
			stringTable[6202],
			stringTable[6203],
			stringTable[6204],
			stringTable[6205],
			stringTable[6206]);
#endif
	}
};
typedef GMCommandLogTable::RecordType RecordGMCommandLog;


/********************************************************************
	Database
*********************************************************************/

typedef TL::TypeListGenerator2
<
	UserMsgTable, 
	GMCommandLogTable
>::Result GMCTablesTypeList;

struct GMCDatabase : public SimpleDB::Database<GMCTablesTypeList>
{
	enum TableName
	{
		userMsgTable = 0, 
		gmCommandLogTable
	};

	void AddNewReceivedPlayerMsg(int playerID, ACHAR type, ACString msg, int gmID, SimpleDB::DateTime time);
	void FinishPlayerMsg(int playerID, SimpleDB::DateTime receivedTime, bool isProcessed, ACString memo = _AL(""), SimpleDB::DateTime finishTime = SimpleDB::DateTime::GetSystemTime());
	void LogExecutedCommand(ACString commandName, ACString commandDetail, int gmID, SimpleDB::DateTime time = SimpleDB::DateTime::GetSystemTime(), int assocMsgPlayerID = -1, SimpleDB::DateTime assocMsgTimeReceived = SimpleDB::DateTime());
};
