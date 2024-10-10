#include "GMCDBMan.h"
#include "EC_Global.h"

#pragma warning(disable : 4630)

#define UNFINISHED_FOLDER	"UnFinished"
#define MAIN_DB_FOLDER		"GMLog"

GMCDBMan::GMCDBMan(int gmID) :
	gmID_(gmID)
{

}

GMCDBMan::~GMCDBMan()
{

}

AString GMCDBMan::Date2FolderName(const SimpleDB::DateTime &date)
{
	AString folderName;
	folderName.Format("%u-%u-%u", date.year_, date.month_, date.day_);
	return folderName;
}

namespace GMCDBManPrivate
{


}

AString GMCDBMan::GetMainDBFolderPath(bool bMakeSureItExists)
{
	AString dbMainFolderPath1;
	dbMainFolderPath1.Format(
		"%s\\userdata\\%s", 
		g_szWorkDir, 
		MAIN_DB_FOLDER,
		gmID_);
	if (bMakeSureItExists)
	{
		::CreateDirectoryA(dbMainFolderPath1, NULL);
	}

	AString dbMainFolderPath2;
	dbMainFolderPath2.Format("%s\\%d", dbMainFolderPath1, gmID_);
	if (bMakeSureItExists)
	{
		::CreateDirectoryA(dbMainFolderPath2, NULL);
	}

	return dbMainFolderPath2;
}

AString GMCDBMan::GetUnfinishedDBPath(bool bMakeSureItExists)
{
	return GetMainDBFolderPath(bMakeSureItExists) + "\\" + UNFINISHED_FOLDER;
}

AString GMCDBMan::GetCommonDBPath(SimpleDB::DateTime date, bool bMakeSureItExists)
{
	return GetMainDBFolderPath(bMakeSureItExists) + "\\" + Date2FolderName(date);
}


bool GMCDBMan::Load()
{
	// set path
	GMCDatabase unFinishedDB;
	unFinishedDB.SetFolderPath(GetUnfinishedDBPath(true));
	database_.SetFolderPath(GetCommonDBPath(SimpleDB::DateTime::GetSystemTime(), true));
	
	// open working database and unfinished database
	try
	{
		unFinishedDB.Load();
		database_.Load();
	}
	catch(SimpleDB::SimpleException & e)
	{
		AString err;
		err.Format("GMCDBMan::Load(), failed to load database. detail : %s", e.msg_);
		a_LogOutput(1, err);
		return false;
	}

	// move all unfinished msg to current working database
	UserMsgTable &destTable = database_.GetTableAt(DWORD2Type<GMCDatabase::userMsgTable>());
	UserMsgTable &srcTable = unFinishedDB.GetTableAt(DWORD2Type<GMCDatabase::userMsgTable>());
	destTable.AppendTable(srcTable);

	return true;
}


namespace GMCDBManPrivate
{
	struct IsUserMsgNotFinished
	{
		bool operator() (const UserMsgTable::RecordType &record)
		{
			return record.IsFieldNull(DWORD2Type<UserMsgTable::isProcessed>());
		}
	};

}

bool GMCDBMan::Save()
{
	GMCDatabase unFinishedDB;
	unFinishedDB.SetFolderPath(GetUnfinishedDBPath(true));
	
	UserMsgTable & workingUsrMsgTable = 
		database_.GetTableAt(DWORD2Type<GMCDatabase::userMsgTable>());
	UserMsgTable & destUsrMsgTable =
		unFinishedDB.GetTableAt(DWORD2Type<GMCDatabase::userMsgTable>());

	// get all unfinished msg
	UserMsgTable tempTableOfUnFinishedMsg;
	workingUsrMsgTable.GetSubRecordSet(
		GMCDBManPrivate::IsUserMsgNotFinished(), 
		tempTableOfUnFinishedMsg);

	// copy unfinished msg to unfinishedDB
	destUsrMsgTable.AppendTable(tempTableOfUnFinishedMsg);
	
	// delete unfinished msg from working db
	workingUsrMsgTable.DeleteIf(GMCDBManPrivate::IsUserMsgNotFinished());


	try
	{
		unFinishedDB.Save();
		database_.Save();
	}
	catch(SimpleDB::SimpleException & e)
	{
		AString err;
		err.Format("GMCDBMan::Save(), failed to save database. detail : %s", e.msg_);
		a_LogOutput(1, err);
		
		return false;
	}

	// copy unfinished msg back to working table
	// they are still in use in game
	workingUsrMsgTable.AppendTable(tempTableOfUnFinishedMsg);

	return true;
}