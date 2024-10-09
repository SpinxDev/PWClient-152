#pragma once

#include <AAssist.h>
#include "TPBase.h"
#include "SDBTable.h"


// begin namespace SimpleDB
namespace SimpleDB
{

namespace DatabasePrivate
{
	struct DatabaseSaveHelperFunc
	{
	private:
		AString folderPath_;
	public:
		DatabaseSaveHelperFunc(AString folderPath) : folderPath_(folderPath) {}
		
		template<class TableType>
		void operator () (const TableType& table)
		{
			// open file
			AFile dbFile;
			bool fileOpenOK = dbFile.Open(
				folderPath_ + "\\" + table.dbFileName, 
				AFILE_NOHEAD | AFILE_BINARY | AFILE_CREATENEW);
			if (!fileOpenOK) 
			{
				AString errMsg;
				errMsg.Format("can not create file %s", table.dbFileName);
				throw SimpleException(errMsg);
			}

			// write
			Archive ar(&dbFile);
			ar << table;
		}
	};

	struct DatabaseLoadHelperFunc
	{
	private:
		AString folderPath_;
		bool createIfNotExists_;
	public:
		DatabaseLoadHelperFunc(AString folderPath, bool createIfNotExists) 
			: folderPath_(folderPath), createIfNotExists_(createIfNotExists) 
		{}
		
		template<class TableType>
		void operator () (TableType& table)
		{
			// full path to file
			AString fullPath = folderPath_ + "\\" + table.dbFileName;

			if (createIfNotExists_)
			{
				// try open file. create an empty file if not exists
				HANDLE fileHandle = ::CreateFileA(
					fullPath, 
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
				if (INVALID_HANDLE_VALUE == fileHandle)
				{
					AString errMsg;
					errMsg.Format("can not open/create file %s, when loading table", table.dbFileName);
					throw SimpleException(errMsg);
				}
				::CloseHandle(fileHandle);
			}

			// open file
			AFile dbFile;
			bool fileOpenOK = dbFile.Open(
				fullPath, 
				AFILE_NOHEAD | AFILE_BINARY | AFILE_OPENEXIST);
			if (!fileOpenOK) 
			{
				AString errMsg;
				errMsg.Format("can not open file %s", table.dbFileName);
				throw SimpleException(errMsg);
			}

			// read
			Archive ar(&dbFile);
			table.DeleteAllRecords();

			// ignore empty file
			if (!ar.IsEOF())
			{
				ar >> table;
			}
		}
	};

	// inter-database
	template<class TTableValueList>
	inline void AppendDatabaseHelper(TTableValueList &dest, const TTableValueList &src)
	{
		dest.value_.AppendTable(src.value_);
		AppendDatabaseHelper(dest.nextList_, src.nextList_);
	}
	template<> inline void AppendDatabaseHelper<NullValueList>(NullValueList &dest, const NullValueList &src)
	{
	}

}

template<class TablesTypeList>
class Database
{
public:
	typedef Database<TablesTypeList> ThisType;
	typedef ValueList<TablesTypeList> TablesValueList;

	enum { tableCount = VL::Length<TablesValueList>::value };
	
	template<DWORD Index>
	struct TypeAt
	{
		typedef typename VL::TypeAt<TablesValueList, Index>::Result Result;
	};
private:
	TablesValueList tablesValueList_;
	AString folderPath_;
public:
	Database() {}
	
	// get()
	template<DWORD Index>
	typename TypeAt<Index>::Result & GetTableAt(DWORD2Type<Index>)
	{
		return VL::GetValue(tablesValueList_, DWORD2Type<Index>());
	}
	template<DWORD Index>
	typename const TypeAt<Index>::Result & GetTableAt(DWORD2Type<Index>) const
	{
		return VL::GetValueConst(tablesValueList_, DWORD2Type<Index>());
	}

	TablesValueList & GetTablesValueList()
	{
		return tablesValueList_;
	}
	const TablesValueList & GetTablesValueList() const
	{
		return tablesValueList_;
	}

	// other
	void SetFolderPath(AString folderPath)
	{
		folderPath_ = folderPath;
	}
	AString GetFolderPath()
	{
		return folderPath_;
	}

	// serialize

	void Save()
	{
		ASSERT(!folderPath_.IsEmpty());
		
		// create dir if not exists
		::CreateDirectoryA(folderPath_, NULL);

		VL::ForEachConst(tablesValueList_, DatabasePrivate::DatabaseSaveHelperFunc(folderPath_));
	}

	void Load(bool createIfNotExists = true)
	{
		ASSERT(!folderPath_.IsEmpty());

		// create dir if not exists
		::CreateDirectoryA(folderPath_, NULL);

		VL::ForEach(tablesValueList_, DatabasePrivate::DatabaseLoadHelperFunc(folderPath_, createIfNotExists));
	}
public:
	void AppendDatabase(const ThisType & src)
	{
		DatabasePrivate::AppendDatabaseHelper(tablesValueList_, src.GetTablesValueList());
	}
	
};


} // end namespace SimpleDB