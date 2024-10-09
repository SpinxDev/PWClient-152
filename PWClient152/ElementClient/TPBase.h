/********************************************************************
	created:	2005/10/12
	created:	12:10:2005   11:10
	file name:	TPBase.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

struct NullType {};

template<typename T1, typename T2>
struct IsSameType
{
private:
	template<typename U> struct Helper { enum { value = 0}; };
	template<> struct Helper<T1> { enum { value = 1 }; };
public:
	enum { value = (Helper<T2>::value == 1 ? 1 : 0) };
};

template<bool Value>
struct Bool2Type
{
	enum { value = Value };
};

template<DWORD Value>
struct DWORD2Type
{
	enum { value = Value };
};

template<int Value>
struct Int2Type
{
	enum { value = Value };
};

template<typename TType>
struct Type2Type
{
	typedef TType OriginalType;
};




template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};
#define STATIC_CHECK(expr, msg) \
{ CompileTimeError<((expr) != 0)> ERROR_##msg; (void)ERROR_##msg; } 

#define STATIC_DWORD_INDEX(index) (DWORD2Type<index>())