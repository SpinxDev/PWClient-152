/********************************************************************
	created:	2005/10/11
	created:	11:10:2005   14:56
	file name:	SDBMisc.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include <AAssist.h>

// begin namespace SimpleDB
namespace SimpleDB
{

struct DBNull
{
};
template<typename TType>
inline bool operator == (const TType& left, const DBNull& right)
{
	return false;
}
template<typename TType>
inline bool operator == (const DBNull& left, const TType& right)
{
	return false;
}
inline bool operator == (const DBNull& left, const DBNull& right)
{
	return true;
}

struct MemoryTableDBFileNameHolder
{
	static const char * value;
};

struct SimpleException
{
	AString msg_;
	SimpleException(const AString& msg) : msg_(msg) {}
};


} // end namespace SimpleDB


