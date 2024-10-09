/********************************************************************
	created:	2005/10/10
	created:	10:10:2005   14:02
	file name:	Table.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include <vector.h>
#include "EC_Algorithm.h"
#include "SDBRecord.h"
#include "SDBArchive.h"
//#include <crtdbg.h>
#include <AAssist.h>
#include "SDBMisc.h"

// begin namespace SimpleDB

namespace SimpleDB
{

//////////////////////////////////////////////////////////////////////////
// Table def
//////////////////////////////////////////////////////////////////////////

namespace TablePrivate
{
	// ReplaceHeadWithACString it used to avoid bug of VC6
	// we can not replace type with ACString in StructureTypeList2FieldStrNameValueListHelper directly
	template<class TTypeList>
	struct ReplaceHeadWithACString
	{
		typedef TypeList<ACString, typename TTypeList::Tail> Result;
	};
	template<class TTypeList>
	struct StructureTypeList2FieldStrNameValueListHelper
	{
	private:
		typedef typename ReplaceHeadWithACString<TTypeList>::Result HeadReplacedTypeList;
	public:
		typedef TypeList<typename HeadReplacedTypeList::Head, typename StructureTypeList2FieldStrNameValueListHelper<typename HeadReplacedTypeList::Tail>::Result> Result;
	};
	template<> struct StructureTypeList2FieldStrNameValueListHelper<NullType>
	{
		typedef NullType Result;
	};

	template<class TTypeList>
	struct StructureTypeList2FieldStrNameValueList
	{
		typedef ValueList<typename StructureTypeList2FieldStrNameValueListHelper<TTypeList>::Result> Result;
	};
}


// note : the template parameter "class DBFileNameHolder" should have 
// been "const char * DBFileName". but VC6 does not support a pointer type value 
// as template parameter, so we have to this form.
// the caller have to provide a struct that holder a member "static const char * value;"
// the "value" will be recognized as DBFileName

template<class TTypeList, DWORD FirstNFieldIsPrimaryKey = typename TL::Length<TTypeList>::value, const char * DBFileName = 0>
class Table
{
// typedef
protected:
	typedef abase::vector< Record<TTypeList> > BaseVector;
public:
	typedef TTypeList StructTypeList;
	typedef Table<TTypeList, FirstNFieldIsPrimaryKey, DBFileName> ThisType;
	typedef Record<TTypeList> RecordType;
	typedef typename BaseVector::iterator Iterator;
	typedef typename BaseVector::const_iterator ConstIterator;
	typedef typename TL::HeadSubTypeList<TTypeList, FirstNFieldIsPrimaryKey>::Result PrimaryKeyTypeList;
	typedef ValueList<PrimaryKeyTypeList> PrimaryKeyValueList;
	typedef typename TablePrivate::StructureTypeList2FieldStrNameValueList<TTypeList>::Result FieldStrNameValueList;
// member variable
public:
	static const char * dbFileName;
protected:
	BaseVector recordSet_;
	RecordType nullRecord_;
	FieldStrNameValueList fieldStrNames_;
public:
	// constructor/destructor
	Table()
	{
		nullRecord_.SetAllFieldToNull();
	}
	~Table() {}
	Table(const ThisType & src)
	{
		*this = src;
	}

	// set operator
	Iterator Begin() { return recordSet_.begin(); }
	ConstIterator Begin() const { return recordSet_.begin(); }
	Iterator End() { return recordSet_.end(); }
	ConstIterator End() const { return recordSet_.end(); }

	DWORD GetRecordCount() const { return static_cast<DWORD>(recordSet_.size()); }
	Iterator AppendRecord(const RecordType& record) 
	{ 
		// record can not be duplicated
		Iterator iter = FindRecord(record);
		if (iter != End()) return iter;
			
		recordSet_.push_back(record); 
		return Begin() + GetRecordCount() - 1;
	}
	Iterator InsertRecord(Iterator iter, const RecordType& record) 
	{ 
		// record can not be duplicated
		Iterator iter = FindRecord(record);
		if (iter != End()) return iter;

		return recordSet_.insert(iter, record); 
	}
	Iterator DeleteRecord(Iterator iter) { return recordSet_.erase(iter); }
	void QuickDeleteRecord(Iterator iter) { recordSet_.erase_noorder(iter); }
	void DeleteAllRecords() { recordSet_.clear(); }
	
	Iterator FindRecord(const PrimaryKeyValueList & pk)
	{
		Iterator iter;
		for (iter = Begin(); End() != iter; ++iter)
			if (VL::Compare(*iter, pk))
				break;
		return iter;
	}
	ConstIterator FindRecord(const PrimaryKeyValueList & pk) const
	{
		ConstIterator iter;
		for (iter = Begin(); End() != iter; ++iter)
			if (VL::Compare(*iter, pk))
				break;
		return iter;
	}

	Iterator FindRecord(const RecordType& record)
	{
		Iterator iter;
		for (iter = Begin(); End() != iter; ++iter)
			if (record == *iter)
				break;
		return iter;
	}
	ConstIterator FindRecord(const RecordType& record) const
	{
		ConstIterator iter;
		for (iter = Begin(); End() != iter; ++iter)
			if (record == *iter)
				break;
		return iter;
	}

	template<class CompareFunc>
	Iterator FindRecordWhen(CompareFunc & func)
	{
		Iterator iter;
		for (iter = Begin(); End() != iter; ++iter)
			if (func(*iter))
				break;
		return iter;
	}
	template<class CompareFunc>
	ConstIterator FindRecordWhen(CompareFunc & func) const
	{
		ConstIterator iter;
		for (iter = Begin(); End() != iter; ++iter)
			if (func(*iter))
				break;
		return iter;
	}
	
	
	template<class ConditionFunctor>
	void GetSubRecordSet(ConditionFunctor condFunc, ThisType & destTable) const
	{
		destTable.DeleteAllRecords();
		for (ConstIterator iter = Begin(); End() != iter; ++iter)
			if (condFunc(*iter))
				destTable.AppendRecord(*iter);
	}
	template<class ConditionFunctor>
	DWORD GetSubRecordSetCount(ConditionFunctor condFunc) const
	{
		DWORD count = 0;
		for (ConstIterator iter = Begin(); End() != iter; ++iter)
			if (condFunc(*iter))
				++count;
		return count;
	}



	template<class ConditionFunctor>
	void DeleteIf(ConditionFunctor condFunc)
	{
		// note : this function is depend on the implement of 
		// abase::vecter::erase_noorder(). change the former
		// if the later is changed.
		Iterator iter = Begin();
		while (End() != iter)
		{
			if (condFunc(*iter))
				QuickDeleteRecord(iter);
			else
				++iter;
		}
	}

	// serialize
	void Save(Archive &ar) const
	{
		ar << GetRecordCount();
		for (ConstIterator iter = Begin(); End() != iter; ++iter)
			ar << *iter;				
	}
	void Load(Archive &ar)
	{
		DWORD recordCount = 0;
		ar >> recordCount;
		ASSERT(recordCount >= 0);

		recordSet_.clear();
		recordSet_.reserve(static_cast<size_t>(recordCount));
		RecordType recordTemp;
		while (recordCount--)
		{
			ar >> recordTemp;
			AppendRecord(recordTemp);
		}
	}

	// other
	const RecordType & GetNullRecord() const
	{
		return nullRecord_;
	}
	const FieldStrNameValueList & GetFieldStrNames() const
	{
		return fieldStrNames_;
	}

	// inter-table
	void AppendTable(const ThisType & anotherTable)
	{
		ASSERT(&anotherTable != this);
		for (ConstIterator iter = anotherTable.Begin(); anotherTable.End() != iter; ++iter)
			AppendRecord(*iter);
	}
	void Union(const ThisType & anotherTable)
	{
		ASSERT(&anotherTable != this);
		for (ConstIterator iter = anotherTable.Begin(); anotherTable.End() != iter; ++iter)
			if (FindRecord(*iter) != End())
				AppendRecord(*iter);
	}

	// operator
	ThisType & operator = (const ThisType & src)
	{
		if (&src == this)
			return (*this);
		
		DeleteAllRecords();
		AppendTable(src);
	}

	bool operator == (const ThisType & src) const
	{
		if (&src == this)
			return true;
		
		if (src.GetRecordCount() != GetRecordCount())
			return false;

		ConstIterator iter1 = Begin();
		ConstIterator iter2 = src.Begin();
		const ConstIterator iterEnd1 = End();
		const ConstIterator iterEnd2 = src.End();
		for (; iterEnd1 != iter1 && iterEnd2 != iter2; ++iter1, ++iter2)
		{
			if (*iter1 != *iter2)
				return false;
		}
		return true;
	}
	bool operator != (const ThisType & src) const
	{
		return !(src == *this);
	}

};

template<class TTypeList, DWORD FirstNFieldIsPrimaryKey, const char * DBFileName>
const char * Table<TTypeList, FirstNFieldIsPrimaryKey, DBFileName>::dbFileName = DBFileName;


//////////////////////////////////////////////////////////////////////////
// Select From ... implement and its helper
//////////////////////////////////////////////////////////////////////////


// ******** helper ********

template<class TFromTablesTypeList>
struct SelectFromHelperTypeGenerator
{
private:
	template<class TFromTablesTypeList2>
	struct FromTablesTypeList2FromRecordsTypeList
	{
	private:
		typedef typename TFromTablesTypeList2::Head HeadTableType;
		typedef typename TFromTablesTypeList2::Tail TailTalbeType;
	public:
		typedef TypeList<typename HeadTableType::RecordType, typename FromTablesTypeList2FromRecordsTypeList<TailTalbeType>::Result > Result;
	};
	template<> struct FromTablesTypeList2FromRecordsTypeList<NullType>
	{
		typedef NullType Result;
	};
public:
	// original 
	typedef TFromTablesTypeList OriginalTypeList;

	// from tables
	typedef typename TL::TypeList2PtrTypeList<TFromTablesTypeList>::Result FromTablesPtrTypeList;
	typedef typename TL::TypeList2ConstPtrTypeList<TFromTablesTypeList>::Result FromTablesConstPtrTypeList;

	typedef ValueList<FromTablesPtrTypeList> FromTablesPtrValueList;
	typedef ValueList<FromTablesConstPtrTypeList> FromTablesConstPtrValueList;

	// from records
	typedef typename FromTablesTypeList2FromRecordsTypeList<TFromTablesTypeList>::Result FromRecordsTypeList;
	
	typedef typename TL::TypeList2PtrTypeList<FromRecordsTypeList>::Result FromRecordsPtrTypeList;
	typedef typename TL::TypeList2ConstPtrTypeList<FromRecordsTypeList>::Result FromRecordsConstPtrTypeList;

	typedef ValueList<FromRecordsPtrTypeList> FromRecordsPtrValueList;
	typedef ValueList<FromRecordsConstPtrTypeList> FromRecordsConstPtrValueList;

};


// ***** Select From ... implement ********
namespace TablePrivate
{

	/*
	1. generate DescartesProduct for records in tables
	eg. table t1{ r1 }, t2{ r1 }, t3{ r1, r2 }
	will generate 
	dpTable 
	{ 
		(t1.nullRecord & t2.nullRecord & t3.nullRecord),
		(t1.nullRecord & t2.nullRecord & t3.r1),
		(t1.nullRecord & t2.nullRecord & t3.r2),
		(t1.nullRecord & t2.r1 & t3.nullRecord),
		(t1.nullRecord & t2.r1 & t3.r1),
		(t1.nullRecord & t2.r1 & t3.r2),
		(t1.r1 & t2.nullRecord & t3.nullRecord),
		(t1.r1 & t2.nullRecord & t3.r1),
		(t1.r1 & t2.nullRecord & t3.r2),
		(t1.r1 & t2.r1 & t3.nullRecord),
		(t1.r1 & t2.r1 & t3.r1),
		(t1.r1 & t2.r1 & t3.r2),
	}
	2. for each record in dpTable, call ProcessFunctor to process it
	*/
	template<class TFromTablesTypeList>
	struct SelectFromHelper
	{
	private:
		typedef typename TFromTablesTypeList::Head FromTablesHeadType;
		typedef typename TFromTablesTypeList::Tail FromTablesTailType;
		typedef ValueList<typename TL::TypeList2ConstPtrTypeList<TFromTablesTypeList>::Result> FromTablesConstPtrValueList;
		typedef SelectFromHelperTypeGenerator<TFromTablesTypeList> TypeGenerator;
		typedef typename TypeGenerator::FromRecordsConstPtrValueList FromRecordsConstPtrValueList;
	public:
		template<class ProcessFunctor, class FullFromRecordsConstPtrValueList, class TResultTable>
		static void In(const FromTablesConstPtrValueList &fromTablesConstPtr, 
				FromRecordsConstPtrValueList & fromRecordsConstPtr, 
				ProcessFunctor & processor,
				FullFromRecordsConstPtrValueList & fullFromRecordsConstPtr,
				TResultTable &resultTable)
		{
//			_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Enter SelectFromHelper::In()\n");
			
			const FromTablesHeadType & headTable = *fromTablesConstPtr.value_;
			FromTablesHeadType::ConstIterator iter = headTable.Begin();

			// recursivly generate the Descartes Product
			// NullRecord is also a part of Descartes Product
			bool nullRecordApplyed = false;
			while (!nullRecordApplyed || iter != headTable.End())
			{
				if (!nullRecordApplyed)
				{
					fromRecordsConstPtr.value_ = &headTable.GetNullRecord();		
					SelectFromHelper<FromTablesTailType>::
							In(fromTablesConstPtr.nextList_, fromRecordsConstPtr.nextList_, processor, fullFromRecordsConstPtr, resultTable);
					nullRecordApplyed = true;
				}
				else
				{
					while (iter != headTable.End())
					{
						//_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Enter SelectFromHelper::In()::while\n");
						
						fromRecordsConstPtr.value_ = &(*iter);		
						SelectFromHelper<FromTablesTailType>::
								In(fromTablesConstPtr.nextList_, fromRecordsConstPtr.nextList_, processor, fullFromRecordsConstPtr, resultTable);
						++iter;
					}
				} // end if
			} // end while
		} // end constructor In()
	};
	template<> struct SelectFromHelper<NullType>
	{
	public:
		template<class ProcessFunctor, class FullFromRecordsConstPtrValueList, class TResultTable>
		static void In(const NullValueList & fromTablesConstPtr, 
			NullValueList & fromRecordsConstPtr, 
			ProcessFunctor & processor, 
			FullFromRecordsConstPtrValueList & fullFromRecordsConstPtr,
			TResultTable &resultTable)
		{
			processor(fullFromRecordsConstPtr, resultTable);
		}
	};

}

template<class TFromTablesTypeList, class ProcessFunctor, class TResultTable>
inline void SelectFrom(
	typename const SelectFromHelperTypeGenerator<TFromTablesTypeList>::FromTablesConstPtrValueList & fromTablesConstPtr, 
	ProcessFunctor & processor, 
	TResultTable & resultTable,
	Type2Type<TFromTablesTypeList>)
{
//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Enter SelectFrom()\n");
	typedef typename SelectFromHelperTypeGenerator<TFromTablesTypeList>::FromRecordsConstPtrValueList FullFromRecordsConstPtrValueList;
	FullFromRecordsConstPtrValueList fullFromRecordsConstPtr;
	typename TablePrivate::
		SelectFromHelper<TFromTablesTypeList>::
				In(fromTablesConstPtr, fullFromRecordsConstPtr, processor, fullFromRecordsConstPtr, resultTable);
//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Leave SelectFrom()\n");
}

template<class TTable, class ProcessFunctor, class TResultTable>
inline void SelectFrom(
	const TTable & fromTable,
	ProcessFunctor & processor, 
	TResultTable & resultTable)
{
	typedef typename TL::TypeListGenerator1<TTable>::Result SingleTableTypeList;
	typedef typename SelectFromHelperTypeGenerator<SingleTableTypeList>::FromTablesConstPtrValueList SingleTableValueList;
	
	SingleTableValueList singleTableValueList;
	VL::SetFirstNContent(singleTableValueList, &fromTable);

	SelectFrom(singleTableValueList, processor, resultTable, Type2Type<SingleTableTypeList>());
}
	
} // end namespace SimpleDB


