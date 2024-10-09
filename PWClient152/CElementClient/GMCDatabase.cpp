#include "GMCDatabase.h"

extern const char userMsgTableDBFileName[] = "UseMsg.db";
extern const char gmCommandLogTableDBFileName[] = "GMCommandLog.db";

void GMCDatabase::AddNewReceivedPlayerMsg(int playerID, ACHAR type, ACString msg, int gmID, SimpleDB::DateTime time)
{
	UserMsgTable & userMsgTable = GetTableAt(STATIC_DWORD_INDEX(GMCDatabase::userMsgTable));
	UserMsgTable::RecordType newRecord;
	SimpleDB::SetFirstNFieldInRecord(newRecord,
		playerID,
		time,
		type,
		msg,
		gmID);
	userMsgTable.AppendRecord(newRecord);
}

void GMCDatabase::FinishPlayerMsg(
								  int playerID, 
								  SimpleDB::DateTime receivedTime, 
								  bool isProcessed, ACString memo, 
								  SimpleDB::DateTime finishTime)
{
	UserMsgTable::PrimaryKeyValueList pk;
	VL::SetFirstNContent(pk, playerID, receivedTime);

	UserMsgTable & userMsgTable = GetTableAt(STATIC_DWORD_INDEX(GMCDatabase::userMsgTable));
	UserMsgTable::Iterator iter = userMsgTable.FindRecord(pk);
	if (userMsgTable.End() == iter)
	{
		a_LogOutput(1, "GMCDatabase::FinishPlayerMsg(), can not find specified record");
		return;
	}

	iter->SetFieldValue(FIELD_INDEX(UserMsgTable::isProcessed), isProcessed);
	iter->SetFieldValue(FIELD_INDEX(UserMsgTable::memo), memo);
	iter->SetFieldValue(FIELD_INDEX(UserMsgTable::timeFinishMsg), finishTime);
}

void GMCDatabase::LogExecutedCommand(ACString commandName, ACString commandDetail, int gmID, SimpleDB::DateTime time, int assocMsgPlayerID, SimpleDB::DateTime assocMsgTimeReceived)
{
	GMCommandLogTable & gmCommandLogTable = GetTableAt(STATIC_DWORD_INDEX(GMCDatabase::gmCommandLogTable));
	GMCommandLogTable::RecordType record;
	if (-1 == assocMsgPlayerID)
	{
		SimpleDB::SetFirstNFieldInRecord(record,
			commandName,
			commandDetail,
			gmID,
			time);
	}
	else
	{
		SimpleDB::SetFirstNFieldInRecord(record,
			commandName,
			commandDetail,
			gmID,
			time,
			assocMsgPlayerID,
			assocMsgTimeReceived);
	}
	gmCommandLogTable.AppendRecord(record);
}


