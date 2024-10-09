/********************************************************************
	created:	2005/10/14
	created:	14:10:2005   10:35
	file name:	SDBRecord.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "ValueList.h"
#include "SDBArchive.h"
#include "SDBMisc.h"
#include "SDBField.h"

// begin namespace SimpleDB
namespace SimpleDB
{

	
//////////////////////////////////////////////////////////////////////////
// Record def
//////////////////////////////////////////////////////////////////////////

namespace RecordPrivate
{
	template<class TTypeList>
	struct TypeList2FieldTypeList
	{
		typedef TypeList<Field<typename TTypeList::Head>, typename TypeList2FieldTypeList<typename TTypeList::Tail>::Result> Result;
	};
	template<> struct TypeList2FieldTypeList<NullType>
	{
		typedef NullType Result;
	};
}

template<class TTypeList>
class Record : public ValueList<typename RecordPrivate::TypeList2FieldTypeList<TTypeList>::Result>
{
public:
	// type info
	typedef TTypeList StructTypeList;
	typedef Record<TTypeList> ThisType;
	typedef typename RecordPrivate::TypeList2FieldTypeList<TTypeList>::Result FieldTypeList;
	typedef ValueList<FieldTypeList> FieldValueList;

	// field count
	enum { fieldCount = VL::Length<FieldValueList>::value };

	// TypeAt
	template<DWORD Index>
	struct TypeAt
	{
		typedef typename VL::TypeAt<FieldValueList, Index>::Result Result;
	};
public:
	// constructor/destructor
	Record()
	{
	}
	Record(const ThisType& src)
	{
		*this = src;
	}
	
	// serialize
	void Save(Archive &ar) const
	{
		VL::ForEachConst(*this, CommonPrivate::FuncSaveAnyTypeToArchive(ar));
	}
	
	void Load(Archive &ar)
	{
		VL::ForEach(*this, CommonPrivate::FuncLoadAnyTypeToArchive(ar));
	}
	
	// null related
	template<DWORD Index>
	bool IsFieldNull(DWORD2Type<Index>) const
	{
		return VL::GetValueConst(*this, DWORD2Type<Index>()).IsNull();
	}

	template<DWORD Index>
	void SetFieldToNull(DWORD2Type<Index>)
	{
		VL::SetValue(*this, DWORD2Type<Index>(), DBNull());
	}

	void SetAllFieldToNull()
	{
		VL::SetValueAll(*this, DBNull());
	}

	// get/set col value
	template<DWORD Index>
	typename TypeAt<Index>::Result & FieldAt(DWORD2Type<Index>)
	{
		return VL::GetValue(*this, DWORD2Type<Index>());
	}
	template<DWORD Index>
	typename const TypeAt<Index>::Result & FieldAt(DWORD2Type<Index>) const
	{
		return VL::GetValueConst(*this, DWORD2Type<Index>());
	}

	template<DWORD Index>
	typename TypeAt<Index>::Result & operator [] (DWORD2Type<Index>)
	{
		return FieldAt(DWORD2Type<Index>());
	}
	template<DWORD Index>
	typename const TypeAt<Index>::Result & operator [] (DWORD2Type<Index>) const
	{
		return FieldAt(DWORD2Type<Index>());
	}

	template<DWORD Index>
	typename const TL::TypeAt<TTypeList, Index>::Result & GetFieldValue(DWORD2Type<Index>) const
	{
		return FieldAt(STATIC_DWORD_INDEX(Index)).Value();
	}

	template<DWORD Index, typename ValueType>
	void SetFieldValue(DWORD2Type<Index>, const ValueType & value)
	{
		FieldAt(STATIC_DWORD_INDEX(Index)) = value;
	}

	// operator
	bool operator == (const ThisType & anotherRecord) const
	{
		if (&anotherRecord == this)
			return true;

		return VL::IsEqual(*this, anotherRecord);
	}
	bool operator != (const ThisType & anotherRecord) const
	{
		return !(anotherRecord == *this);
	}

	ThisType & operator = (const ThisType & anotherRecord)
	{
		if (&anotherRecord != this)
			VL::Assign(*this, anotherRecord);
		return *this;
	}

	template<class Functor>
	void ForEachField(Functor & func)
	{
		VL::ForEach(*this, func);
	}
	template<class Functor>
	void ForEachField(Functor & func) const
	{
		VL::ForEachConst(*this, func);
	}
};

// generate index from const integer
#define FIELD_INDEX(index) (STATIC_DWORD_INDEX(index))

//////////////////////////////////////////////////////////////////////////
// GetFirstNFieldInRecord
//////////////////////////////////////////////////////////////////////////

template
<
	class TRecord, 
	typename ValueType0
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0)
{
	val0 = row.GetFieldValue(DWORD2Type<0>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1)
{
	GetFirstNFieldInRecord(row, val0);
	val1 = row.GetFieldValue(DWORD2Type<1>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2 
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2)
{
	GetFirstNFieldInRecord(row, val0, val1);
	val2 = row.GetFieldValue(DWORD2Type<2>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2,
	ValueType3 & val3)
{
	GetFirstNFieldInRecord(row, val0, val1, val2);
	val3 = row.GetFieldValue(DWORD2Type<3>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2,
	ValueType3 & val3,
	ValueType4 & val4)
{
	GetFirstNFieldInRecord(row, val0, val1, val2, val3);
	val4 = row.GetFieldValue(DWORD2Type<4>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2,
	ValueType3 & val3,
	ValueType4 & val4,
	ValueType5 & val5)
{
	GetFirstNFieldInRecord(row, val0, val1, val2, val3, val4);
	val5 = row.GetFieldValue(DWORD2Type<5>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2,
	ValueType3 & val3,
	ValueType4 & val4,
	ValueType5 & val5,
	ValueType6 & val6)
{
	GetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5);
	val6 = row.GetFieldValue(DWORD2Type<6>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6,
	typename ValueType7
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2,
	ValueType3 & val3,
	ValueType4 & val4,
	ValueType5 & val5,
	ValueType6 & val6,
	ValueType7 & val7)
{
	GetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5, val6);
	val7 = row.GetFieldValue(DWORD2Type<7>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6,
	typename ValueType7,
	typename ValueType8
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2,
	ValueType3 & val3,
	ValueType4 & val4,
	ValueType5 & val5,
	ValueType6 & val6,
	ValueType7 & val7,
	ValueType8 & val8)
{
	GetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5, val6, val7);
	val8 = row.GetFieldValue(DWORD2Type<8>());
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6,
	typename ValueType7,
	typename ValueType8,
	typename ValueType9
> 
inline void GetFirstNFieldInRecord(
	TRecord &row, 
	ValueType0 & val0,
	ValueType1 & val1,
	ValueType2 & val2,
	ValueType3 & val3,
	ValueType4 & val4,
	ValueType5 & val5,
	ValueType6 & val6,
	ValueType7 & val7,
	ValueType8 & val8,
	ValueType9 & val9)
{
	GetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5, val6, val7, val8);
	val9 = row.GetFieldValue(DWORD2Type<9>());
}

//////////////////////////////////////////////////////////////////////////
// SetFirstNFieldInRecord
//////////////////////////////////////////////////////////////////////////

template
<
	class TRecord,
	typename ValueType0
>
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0)
{
	row.SetFieldValue(DWORD2Type<0>(), val0);
}

template
<
	class TRecord, 
	typename ValueType0,
	typename ValueType1
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1)
{
	SetFirstNFieldInRecord(row, val0);
	row.SetFieldValue(DWORD2Type<1>(), val1);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2)
{
	SetFirstNFieldInRecord(row, val0, val1);
	row.SetFieldValue(DWORD2Type<2>(), val2);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2,
	const ValueType3 & val3)
{
	SetFirstNFieldInRecord(row, val0, val1, val2);
	row.SetFieldValue(DWORD2Type<3>(), val3);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2,
	const ValueType3 & val3,
	const ValueType4 & val4)
{
	SetFirstNFieldInRecord(row, val0, val1, val2, val3);
	row.SetFieldValue(DWORD2Type<4>(), val4);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2,
	const ValueType3 & val3,
	const ValueType4 & val4,
	const ValueType5 & val5)
{
	SetFirstNFieldInRecord(row, val0, val1, val2, val3, val4);
	row.SetFieldValue(DWORD2Type<5>(), val5);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2,
	const ValueType3 & val3,
	const ValueType4 & val4,
	const ValueType5 & val5,
	const ValueType6 & val6)
{
	SetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5);
	row.SetFieldValue(DWORD2Type<6>(), val6);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6,
	typename ValueType7
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2,
	const ValueType3 & val3,
	const ValueType4 & val4,
	const ValueType5 & val5,
	const ValueType6 & val6,
	const ValueType7 & val7)
{
	SetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5, val6);
	row.SetFieldValue(DWORD2Type<7>(), val7);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6,
	typename ValueType7,
	typename ValueType8
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2,
	const ValueType3 & val3,
	const ValueType4 & val4,
	const ValueType5 & val5,
	const ValueType6 & val6,
	const ValueType7 & val7,
	const ValueType8 & val8)
{
	SetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5, val6, val7);
	row.SetFieldValue(DWORD2Type<8>(), val8);
}

template
<
	class TRecord, 
	typename ValueType0, 
	typename ValueType1, 
	typename ValueType2,
	typename ValueType3,
	typename ValueType4,
	typename ValueType5,
	typename ValueType6,
	typename ValueType7,
	typename ValueType8,
	typename ValueType9
> 
inline void SetFirstNFieldInRecord(
	TRecord &row, 
	const ValueType0 & val0,
	const ValueType1 & val1,
	const ValueType2 & val2,
	const ValueType3 & val3,
	const ValueType4 & val4,
	const ValueType5 & val5,
	const ValueType6 & val6,
	const ValueType7 & val7,
	const ValueType8 & val8,
	const ValueType9 & val9)
{
	SetFirstNFieldInRecord(row, val0, val1, val2, val3, val4, val5, val6, val7, val8);
	row.SetFieldValue(DWORD2Type<9>(), val9);
}

} // end namespace SimpleDB