/********************************************************************
	created:	2005/10/10
	created:	10:10:2005   13:53
	file name:	TypeList.h
	author:		yaojun
	
	purpose:	TypeList
*********************************************************************/

#pragma once

#include "TPBase.h"

typedef unsigned long DWORD;

//////////////////////////////////////////////////////////////////////////
// basic def for typelist
//////////////////////////////////////////////////////////////////////////
template<typename HeadType, typename TailType>
struct TypeList
{
	typedef HeadType Head;
	typedef TailType Tail;
};


// begin namesapce TL
namespace TL
{

//////////////////////////////////////////////////////////////////////////
// helper for generating typelist
//////////////////////////////////////////////////////////////////////////

template<typename T1>
struct TypeListGenerator1
{
	typedef TypeList<T1, NullType> Result;
};

template<typename T1, typename T2>
struct TypeListGenerator2
{
	typedef TypeList<T1, typename TypeListGenerator1<T2>::Result> Result;
};

template<typename T1, typename T2, typename T3>
struct TypeListGenerator3
{
	typedef TypeList<T1, typename TypeListGenerator2<T2, T3>::Result> Result;
};

template<typename T1, typename T2, typename T3, typename T4>
struct TypeListGenerator4
{
	typedef TypeList<T1, typename TypeListGenerator3<T2, T3, T4>::Result> Result;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5>
struct TypeListGenerator5
{
	typedef TypeList<T1, typename TypeListGenerator4<T2, T3, T4, T5>::Result> Result;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
struct TypeListGenerator6
{
	typedef TypeList<T1, typename TypeListGenerator5<T2, T3, T4, T5, T6>::Result> Result;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
struct TypeListGenerator7
{
	typedef TypeList<T1, typename TypeListGenerator6<T2, T3, T4, T5, T6, T7>::Result> Result;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
struct TypeListGenerator8
{
	typedef TypeList<T1, typename TypeListGenerator7<T2, T3, T4, T5, T6, T7, T8>::Result> Result;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct TypeListGenerator9
{
	typedef TypeList<T1, typename TypeListGenerator8<T2, T3, T4, T5, T6, T7, T8, T9>::Result> Result;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
struct TypeListGenerator10
{
	typedef TypeList<T1, typename TypeListGenerator9<T2, T3, T4, T5, T6, T7, T8, T9, T10>::Result> Result;
};


//////////////////////////////////////////////////////////////////////////
// Length() operation of typelist
//////////////////////////////////////////////////////////////////////////

template<class TTypeList> struct Length
{
private:
	typedef typename TTypeList::Head Head;
	typedef typename TTypeList::Tail Tail;
public:
	enum { value = 1 + Length<Tail>::value };
};

template<> struct Length<NullType>
{
	enum { value = 0 };
};

//////////////////////////////////////////////////////////////////////////
// TypeAt() operation of typelist
//////////////////////////////////////////////////////////////////////////

namespace Private
{
	template<DWORD Index>
	struct TypeAtHelper
	{
		template<class TTypeList>
		struct In
		{
		private:
			typedef typename TTypeList::Head Head;
			typedef typename TTypeList::Tail Tail;
		public:
			typedef typename TypeAtHelper<Index - 1>::In<Tail>::Result Result;
		};
	};
	template<> struct TypeAtHelper<0> 
	{
		template<class TTypeList>
		struct In
		{
		private:
			typedef typename TTypeList::Head Head;
		public:
			typedef Head Result;
		};
	};
}

template<class TTypeList, DWORD Index> struct TypeAt
{
	typedef typename Private::TypeAtHelper<Index>::In<TTypeList>::Result Result;
};

//////////////////////////////////////////////////////////////////////////
// convert each type in typeList to its pointer type
//////////////////////////////////////////////////////////////////////////

template<class TTypeList>
struct TypeList2PtrTypeList
{
	typedef TypeList<typename TTypeList::Head *, typename TypeList2PtrTypeList<typename TTypeList::Tail>::Result> Result;
};
template<> struct TypeList2PtrTypeList<NullType>
{
	typedef NullType Result;
};


//////////////////////////////////////////////////////////////////////////
// convert each type in typeList to its const pointer type
//////////////////////////////////////////////////////////////////////////

template<class TTypeList>
struct TypeList2ConstPtrTypeList
{
	typedef TypeList<typename const TTypeList::Head *, typename TypeList2ConstPtrTypeList<typename TTypeList::Tail>::Result> Result;
};
template<> struct TypeList2ConstPtrTypeList<NullType>
{
	typedef NullType Result;
};

//////////////////////////////////////////////////////////////////////////
// get sub head typelist
//////////////////////////////////////////////////////////////////////////
namespace Private
{
	template<DWORD HeadN>
	struct HeadSubTypeListHelper
	{
		template<class TTypeList>
		struct In
		{
			typedef TypeList<typename TTypeList::Head, typename HeadSubTypeListHelper<HeadN - 1>::In<TTypeList::Tail>::Result> Result;
		};
	};
	template<> struct HeadSubTypeListHelper<0>
	{
		template<class TTypeList>
		struct In
		{
			typedef NullType Result;
		};
	};
}
template<class TTypeList, DWORD HeadN>
struct HeadSubTypeList
{
	typedef typename Private::HeadSubTypeListHelper<HeadN>::In<TTypeList>::Result Result;
};


} // end namespace TL
