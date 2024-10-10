/********************************************************************
	created:	2005/10/13
	created:	13:10:2005   20:20
	file name:	GMCDBMan.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "GMCDatabase.h"

class GMCDBMan  
{
private:
	GMCDatabase database_;
	int gmID_;
private:
	AString Date2FolderName(const SimpleDB::DateTime &date);
	AString GetMainDBFolderPath(bool bMakeSureItExist = true);
	AString GetUnfinishedDBPath(bool bMakeSureItExists = true);
	AString GetCommonDBPath(SimpleDB::DateTime date, bool bMakeSureItExists = true);
public:
	GMCDBMan(int gmID);
	~GMCDBMan();
	bool Load();
	bool Save();
	GMCDatabase & GetDatabase(){ return database_; }
};
