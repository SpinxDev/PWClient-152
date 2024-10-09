/********************************************************************
	created:	2005/10/10
	created:	10:10:2005   14:58
	file name:	ValueList.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include <ABaseDef.h>
#include "TypeList.h"
#include "TPBase.h"
//#include <crtdbg.h>

//////////////////////////////////////////////////////////////////////////
// basic def for ValueList 
//////////////////////////////////////////////////////////////////////////
template<class TTypeList>
struct ValueList
{
	typedef typename TTypeList TheTypeList;
	typedef typename TTypeList::Head TypeListHead;
	typedef typename TTypeList::Tail TypeListTail;
	typedef ValueList<TypeListTail> NextValueListType;
	
	TypeListHead value_;
	NextValueListType nextList_;
};
template<> struct ValueList<NullType>
{
};
typedef ValueList<NullType> NullValueList;


// Begin namespace VL
namespace VL
{


//////////////////////////////////////////////////////////////////////////
// TypeAt() operation
//////////////////////////////////////////////////////////////////////////
template<class TValueList, DWORD Index>
struct TypeAt
{
	typedef typename TL::TypeAt<typename TValueList::TheTypeList, Index>::Result Result;
};

//////////////////////////////////////////////////////////////////////////
// Length() operation
//////////////////////////////////////////////////////////////////////////
template<class TValueList>
struct Length
{
	enum { value = TL::Length<TValueList::TheTypeList>::value };
};


//////////////////////////////////////////////////////////////////////////
// GetValue() operation
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<DWORD Index>
	struct GetValueHelper
	{
		template<class TValueList>
		static void In(TValueList &valueList, typename VL::TypeAt<TValueList, Index>::Result *& pValue)
		{
			GetValueHelper<Index - 1>::In(valueList.nextList_, pValue);
		}
	};
	template<> struct GetValueHelper<0>
	{
		template<class TValueList>
		static void In(TValueList &valueList, typename VL::TypeAt<TValueList, 0>::Result *& pValue)
		{
			pValue = &valueList.value_;
		}
	};
} // end name sapce Private

template<DWORD Index, class TValueList>
inline typename VL::TypeAt<TValueList, Index>::Result & GetValue(TValueList & valueList, DWORD2Type<Index>)
{
//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Enter VL::GetValue()\n");
	VL::TypeAt<TValueList, Index>::Result *pValue = NULL;
	Private::GetValueHelper<Index>::In(valueList, pValue);
	ASSERT(pValue);
	return *pValue;
}

//////////////////////////////////////////////////////////////////////////
// GetValueConst() operation
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<DWORD Index>
	struct GetValueConstHelper
	{
		template<class TValueList>
		static void In(const TValueList &valueList, typename const VL::TypeAt<TValueList, Index>::Result *& pValue)
		{
			GetValueConstHelper<Index - 1>::In(valueList.nextList_, pValue);
		}
	};
	template<> struct GetValueConstHelper<0>
	{
		template<class TValueList>
		static void In(const TValueList &valueList, typename const VL::TypeAt<TValueList, 0>::Result *& pValue)
		{
			pValue = &valueList.value_;
		}
	};
} // end name sapce Private

template<DWORD Index, class TValueList>
inline typename const VL::TypeAt<TValueList, Index>::Result & GetValueConst(const TValueList & valueList, DWORD2Type<Index>)
{
//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Enter VL::GetValueConst()\n");
	const VL::TypeAt<TValueList, Index>::Result *pValue = NULL;
	Private::GetValueConstHelper<Index>::In(valueList, pValue);
	ASSERT(pValue);
	return *pValue;
}

//////////////////////////////////////////////////////////////////////////
// SetValue() operation
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<DWORD Index>
	struct SetValueHelper
	{
		template<class TValueList, typename ValueType>
		static void In(TValueList &valueList, const ValueType & value)
		{
			SetValueHelper<Index - 1>::In(valueList.nextList_, value);
		}
	};
	template<> struct SetValueHelper<0>
	{
		template<class TValueList, typename ValueType>
		static void In(TValueList &valueList, const ValueType & value)
		{
			valueList.value_ = value;
		}
	};
} // end name sapce Private

template<DWORD Index, class TValueList, typename ValueType>
inline void SetValue(TValueList &valueList, DWORD2Type<Index>, const ValueType & value)
{
//	_CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "Enter VL::SetValue()\n");
	Private::SetValueHelper<Index>::In(valueList, value);
}



//////////////////////////////////////////////////////////////////////////
// SetToDefaultValue() operation
// call each type's default constructor to assign its value
//////////////////////////////////////////////////////////////////////////
template<DWORD Index, class TValueList>
inline void SetToDefaultValue(TValueList & valueList, DWORD2Type<Index>)
{
	typedef VL::TypeAt<TValueList, Index>::Result ValueType;
	VL::SetValue(valueList, DWORD2Type<Index>(), ValueType());
}

namespace Private
{
	template<class TValueList>
	inline void static SetAllToDefaultValueHelper(TValueList & valueList)
	{
		SetAllToDefaultValueHelper(valueList.nextList_);
	}
	template<>
	inline void static SetAllToDefaultValueHelper<NullValueList>(NullValueList & valueList)
	{

	}
	
}

template<class TValueList>
inline void SetAllToDefaultValue(TValueList & valueList)
{
	Private::SetAllToDefaultValueHelper(valueList);
}



//////////////////////////////////////////////////////////////////////////
// IsEqual
//////////////////////////////////////////////////////////////////////////

namespace Private
{
	template<class TValueList>
	inline bool IsEqualHelper(const TValueList & valueList1, const TValueList & valueList2)
	{
		if (valueList1.value_ != valueList2.value_)
			return false;
		else
			return IsEqualHelper(valueList1.nextList_, valueList2.nextList_);
	}
	template<> 
	inline bool IsEqualHelper<NullValueList>(const NullValueList & valueList1, const NullValueList & valueList2)
	{
		return true;
	}
}

template<class TValueList>
inline bool IsEqual(const TValueList & valueList1, const TValueList & valueList2)
{
	if (&valueList1 == &valueList2)
		return true;
	
	return Private::IsEqualHelper(valueList1, valueList2);
}

//////////////////////////////////////////////////////////////////////////
// compare two valuelist from head until reach the end of the shorter one
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<class TValueList1>
	struct CompareHelper
	{
		template<class TValueList2>
		static bool In(const TValueList1 & valueList1, const TValueList2 & valueList2)
		{
			if (valueList1.value_ != valueList2.value_)
				return false;
			else
				return CompareHelper<TValueList1::NextValueListType>::In(valueList1.nextList_, valueList2.nextList_);
		}
		template<> static bool In(const TValueList1 & valueList1, const NullValueList & valueList2)
		{
			return true;
		}
	};
	template<> struct CompareHelper<NullValueList>
	{
		template<class TValueList2>
		static bool In(const NullValueList & valueList1, const TValueList2 & valueList2)
		{
			return true;
		}
	};
}

template<class TValueList1, class TValueList2>
inline bool Compare(const TValueList1 & valueList1, const TValueList2 & valueList2)
{
	return Private::CompareHelper<TValueList1>::In(valueList1, valueList2);
}

//////////////////////////////////////////////////////////////////////////
// copy valueList
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<class TValueList>
	inline void AssignHelper(TValueList & destValueList, const TValueList & srcValueList)
	{
		destValueList.value_ = srcValueList.value_;
		Private::AssignHelper(destValueList.nextList_, srcValueList.nextList_);
	}
	template<>
	inline void AssignHelper<NullValueList>(NullValueList & destValueList, const NullValueList & srcValueList)
	{
	}
}

template<class TValueList>
inline TValueList & Assign(TValueList & destValueList, const TValueList & srcValueList)
{
	if (&srcValueList == &destValueList)
		return destValueList;

	Private::AssignHelper(destValueList, srcValueList);
	return destValueList;
}

//////////////////////////////////////////////////////////////////////////
// for_each() operation
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<class TValueList>
	struct ForEachHelper
	{
		template<class TFunctor>
		static void In(TValueList &valueList, TFunctor &func)
		{
			func(valueList.value_);
			ForEachHelper<TValueList::NextValueListType>::In(valueList.nextList_, func);
		}
	};
	template<> struct ForEachHelper<NullValueList>
	{
		template<class TFunctor>
		static void In(NullValueList &valueList, TFunctor &func)
		{
		}
	};
	
}

template<class TValueList, class TFunctor>
inline void ForEach(TValueList & valueList, TFunctor &func)
{
	Private::ForEachHelper<TValueList>::In(valueList, func);
}

//////////////////////////////////////////////////////////////////////////
// for_each() const operation
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<class TValueList>
	struct ForEachHelperConst
	{
		template<class TFunctor>
		static void In(const TValueList &valueList, TFunctor &func)
		{
			func(valueList.value_);
			ForEachHelperConst<TValueList::NextValueListType>::In(valueList.nextList_, func);
		}
	};
	template<> struct ForEachHelperConst<NullValueList>
	{
		template<class TFunctor>
		static void In(const NullValueList &valueList, TFunctor &func)
		{
		}
	};
	
}

template<class TValueList, class TFunctor>
inline void ForEachConst(const TValueList & valueList, TFunctor &func)
{
	Private::ForEachHelperConst<TValueList>::In(valueList, func);
}

//////////////////////////////////////////////////////////////////////////
// SetValueAll() operation
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<typename RightValueType>
	struct SetValueAllFunctor
	{
	private:
		const RightValueType & rightValue_;
	public:
		SetValueAllFunctor(const RightValueType & rightValue) : rightValue_(rightValue) {}
		template<typename LeftValueType>
		void operator () (LeftValueType & leftValue)
		{
			leftValue = rightValue_;
		}
	};
} // end name sapce Private

template<class TValueList, typename ValueType>
inline void SetValueAll(TValueList &valueList, const ValueType & value)
{
	VL::ForEach(valueList, Private::SetValueAllFunctor<ValueType>(value));
}

//////////////////////////////////////////////////////////////////////////
// Get first N fields' values of valueList
//////////////////////////////////////////////////////////////////////////

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0)
{
	val0 = valueList.value_;
}

template<class TValueList>
void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2,
						 typename VL::TypeAt<TValueList, 3>::Result & val3)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2, val3);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2,
						 typename VL::TypeAt<TValueList, 3>::Result & val3,
						 typename VL::TypeAt<TValueList, 4>::Result & val4)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2, val3, val4);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2,
						 typename VL::TypeAt<TValueList, 3>::Result & val3,
						 typename VL::TypeAt<TValueList, 4>::Result & val4,
						 typename VL::TypeAt<TValueList, 5>::Result & val5)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2,
						 typename VL::TypeAt<TValueList, 3>::Result & val3,
						 typename VL::TypeAt<TValueList, 4>::Result & val4,
						 typename VL::TypeAt<TValueList, 5>::Result & val5,
						 typename VL::TypeAt<TValueList, 6>::Result & val6)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2,
						 typename VL::TypeAt<TValueList, 3>::Result & val3,
						 typename VL::TypeAt<TValueList, 4>::Result & val4,
						 typename VL::TypeAt<TValueList, 5>::Result & val5,
						 typename VL::TypeAt<TValueList, 6>::Result & val6,
						 typename VL::TypeAt<TValueList, 7>::Result & val7)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6, val7);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2,
						 typename VL::TypeAt<TValueList, 3>::Result & val3,
						 typename VL::TypeAt<TValueList, 4>::Result & val4,
						 typename VL::TypeAt<TValueList, 5>::Result & val5,
						 typename VL::TypeAt<TValueList, 6>::Result & val6,
						 typename VL::TypeAt<TValueList, 7>::Result & val7,
						 typename VL::TypeAt<TValueList, 8>::Result & val8)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6, val7, val8);
}

template<class TValueList>
inline void GetFirstNContent(
						 TValueList & valueList, 
						 typename VL::TypeAt<TValueList, 0>::Result & val0,
						 typename VL::TypeAt<TValueList, 1>::Result & val1,
						 typename VL::TypeAt<TValueList, 2>::Result & val2,
						 typename VL::TypeAt<TValueList, 3>::Result & val3,
						 typename VL::TypeAt<TValueList, 4>::Result & val4,
						 typename VL::TypeAt<TValueList, 5>::Result & val5,
						 typename VL::TypeAt<TValueList, 6>::Result & val6,
						 typename VL::TypeAt<TValueList, 7>::Result & val7,
						 typename VL::TypeAt<TValueList, 8>::Result & val8,
						 typename VL::TypeAt<TValueList, 9>::Result & val9)
{
	val0 = valueList.value_;
	GetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6, val7, val8, val9);
}

//////////////////////////////////////////////////////////////////////////
// Set first N fields' values of valueList
//////////////////////////////////////////////////////////////////////////

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0)
{
	valueList.value_ = val0;
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2,
						 typename const VL::TypeAt<TValueList, 3>::Result & val3)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2, val3);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2,
						 typename const VL::TypeAt<TValueList, 3>::Result & val3,
						 typename const VL::TypeAt<TValueList, 4>::Result & val4)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2, val3, val4);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2,
						 typename const VL::TypeAt<TValueList, 3>::Result & val3,
						 typename const VL::TypeAt<TValueList, 4>::Result & val4,
						 typename const VL::TypeAt<TValueList, 5>::Result & val5)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2,
						 typename const VL::TypeAt<TValueList, 3>::Result & val3,
						 typename const VL::TypeAt<TValueList, 4>::Result & val4,
						 typename const VL::TypeAt<TValueList, 5>::Result & val5,
						 typename const VL::TypeAt<TValueList, 6>::Result & val6)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2,
						 typename const VL::TypeAt<TValueList, 3>::Result & val3,
						 typename const VL::TypeAt<TValueList, 4>::Result & val4,
						 typename const VL::TypeAt<TValueList, 5>::Result & val5,
						 typename const VL::TypeAt<TValueList, 6>::Result & val6,
						 typename const VL::TypeAt<TValueList, 7>::Result & val7)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6, val7);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2,
						 typename const VL::TypeAt<TValueList, 3>::Result & val3,
						 typename const VL::TypeAt<TValueList, 4>::Result & val4,
						 typename const VL::TypeAt<TValueList, 5>::Result & val5,
						 typename const VL::TypeAt<TValueList, 6>::Result & val6,
						 typename const VL::TypeAt<TValueList, 7>::Result & val7,
						 typename const VL::TypeAt<TValueList, 8>::Result & val8)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6, val7, val8);
}

template<class TValueList>
void SetFirstNContent(
						 TValueList & valueList, 
						 typename const VL::TypeAt<TValueList, 0>::Result & val0,
						 typename const VL::TypeAt<TValueList, 1>::Result & val1,
						 typename const VL::TypeAt<TValueList, 2>::Result & val2,
						 typename const VL::TypeAt<TValueList, 3>::Result & val3,
						 typename const VL::TypeAt<TValueList, 4>::Result & val4,
						 typename const VL::TypeAt<TValueList, 5>::Result & val5,
						 typename const VL::TypeAt<TValueList, 6>::Result & val6,
						 typename const VL::TypeAt<TValueList, 7>::Result & val7,
						 typename const VL::TypeAt<TValueList, 8>::Result & val8,
						 typename const VL::TypeAt<TValueList, 9>::Result & val9)
{
	valueList.value_ = val0;
	SetFirstNContent(valueList.nextList_, val1, val2, val3, val4, val5, val6, val7, val8, val9);
}

} // end namespace VL

