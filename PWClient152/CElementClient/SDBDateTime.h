/********************************************************************
	created:	2005/10/11
	created:	11:10:2005   10:27
	file name:	DateTime.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#ifdef UNICODE
	#ifndef _UNICODE
		#define _UNICODE
	#endif
#endif

#include <ATime.h>
#include <AAssist.h>
#include "SDBArchive.h"
#include <tchar.h>

#pragma warning (disable : 4786)

// begin namespace SimpleDB
namespace SimpleDB
{

struct DateTime
{
public:
	union
	{
		struct
		{
			WORD year_;
			WORD month_;
			WORD day_;
			WORD week_;
			WORD hour_;
			WORD minute_;
			WORD second_;
		};
		WORD data_[7];
	};
	enum { size = sizeof(WORD) * 7 };
public:
	DateTime()
	{
		memset(&data_, 0, size);
	}
	DateTime(const DateTime & src)
	{
		*this = src;
	}
	DateTime & operator = (const DateTime & src)
	{
		if (&src != this) 
			memcpy(data_, src.data_, size);
		return *this;
	}
	void LoadSystemTime()
	{
		a_GetSystemTime(&year_, &month_, &day_, &week_, &hour_, &minute_, &second_);
	}
	bool operator == (const DateTime & src) const
	{
		return (0 == memcmp(data_, src.data_, size));
	}
	bool operator != (const DateTime & src) const
	{
		return !(*this == src);
	}
	bool operator < (const DateTime & src) const
	{
		if (year_ < src.year_)			return true;
		else if (year_ > src.year_)		return false;

		if (month_ < src.month_)		return true;
		else if (month_ > src.month_)	return false;
			
		if (day_ < src.day_)			return true;
		else if (day_ > src.day_)		return false;

		if (hour_ < src.hour_)			return true;
		else if (hour_ > src.hour_)		return false;

		if (minute_ < src.minute_)		return true;
		else if (minute_ > src.minute_)	return false;

		if (second_ < src.second_)		return true;
		else if (second_ > src.second_)	return false;

		return false;
	}
	bool operator <= (const DateTime & src) const
	{
		return (*this < src || *this == src);
	}
	bool operator > (const DateTime & src) const
	{
		return !(*this <= src);
	}
	bool operator >= (const DateTime &src) const
	{
		return !(*this < src);
	}
	bool IsDatePartEqual(const DateTime & src) const
	{
		return (src.year_ == year_ && src.month_ == month_ && src.day_ == day_);
	}
	bool IsTimePartEqual(const DateTime & src) const
	{
		return (src.hour_ == hour_ && src.minute_ == minute_ && src.second_ == second_);
	}
	void Save(Archive & ar) const
	{
		ar.Write(data_, size);
	}
	void Load(Archive & ar)
	{
		ar.Read(data_, size);
	}
	static DateTime GetSystemTime()
	{
		DateTime dateTime;
		dateTime.LoadSystemTime();
		return dateTime;
	}
	ACString ToString() const
	{
		ACString stringValue;
		stringValue.Format(_AL("%04u-%02u-%02u %02u:%02u:%02u"),
			year_, month_, day_, hour_, minute_, second_);
		return stringValue;
	}
	ACString ToStringDatePart() const
	{
		ACString stringValue;
		stringValue.Format(_AL("%04u-%02u-%02u"),
			year_, month_, day_);
		return stringValue;
	}
	ACString ToStringTimePart() const
	{
		ACString stringValue;
		stringValue.Format(_AL("%02u:%02u:%02u"),
			hour_, minute_, second_);
		return stringValue;
	}
	void FromString(ACString stringValue)
	{
		_stscanf(stringValue, _AL("%04u-%02u-%02u %02u:%02u:%02u"), 
			&year_, &month_, &day_, &hour_, &minute_, &second_);
	}
	void FromStringDatePart(ACString stringValue)
	{
		_stscanf(stringValue, _AL("%04u-%02u-%02u"), 
			&year_, &month_, &day_);
		hour_ = minute_ = second_ = 0;
	}
	void FromStringTimePart(ACString stringValue)
	{
		_stscanf(stringValue, _AL("%02u:%02u:%02u"), 
			&hour_, &minute_, &second_);
		year_ = month_ = day_ = 0;
	}
	void ClearDatePart()
	{
		year_ = month_ = day_ = 0;
	}
	void ClearTimePart()
	{
		hour_ = minute_ = second_ = 0;
	}

};

} // end namespace SimpleDB
