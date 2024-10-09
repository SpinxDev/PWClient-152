// File		: EC_Time.cpp
// Creator	: Xu Wenbin
// Date		: 2015/1/4

#include "EC_Time.h"

#include <memory.h>

namespace CECTime{
	bool IsValidSingle(int value, short low, short high)
	{
		// Check whether value is within range [low, high]
		//
		return value >= low
			&& value <= high;
	}
	
	bool IsValidContinuous(int y, int m, short y1, short m1, short y2, short m2)
	{
		// Check whether (y, m) is within range [(y1, m1), (y2, m2)]
		// This is useful for time-like range check such as 2009-10 ~ 2011-2
		//
		bool valid(false);
		
		if (y1 == y2)
		{
			if (y == y1)
				valid = IsValidSingle(m, m1, m2);
		}
		else
		{
			if (y == y1)
				valid = (m >= m1);
			else if (y == y2)
				valid = (m <= m2);
			else
				valid = (y>y1 && y<y2);
		}
		
		return valid;
	}
	
	bool IsValidContinuous(int y, int m, int d, short y1, short m1, short d1, short y2, short m2, short d2)
	{
		// Check whether (y, m, d) is within range [(y1, m1, d1), (y2, m2, d2)]
		// This is useful for time-like range check such as 2009-10-15 ~ 2011-8-2
		//
		bool valid = false;
		
		if (IsValidSingle(y, y1, y2))
		{
			if (y1 == y2)
			{
				if (y == y1)
					valid = IsValidContinuous(m, d, m1, d1, m2, d2);
			}
			else
			{
				if (y == y1)
				{
					if (m >= m1)
					{
						if (m == m1)
						{
							valid = (d >= d1);
						}
						else
						{
							valid = true;
						}
					}
				}
				else if (y == y2)
				{
					if (m <= m2)
					{
						if (m == m2)
						{
							valid = (d <= d2);
						}
						else
						{
							valid = true;
						}
					}
				}
				else
				{
					valid = (y>y1 && y<y2);
				}
			}
		}
		
		return valid;
	}
	
	int GetWeek(int y, int m, int d)
	{
		// Return 0-6 correspond to Monday-Sunday given year/month/day
		//
		
		if (m==1 || m==2)
		{
			m += 12;
			y --;
		}			
		int week = (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
		return week;
	}
	
	int GetMonthDayCount(int y, int m)
	{
		// Get day count of given month in given year
		//
		static int s_days[]= {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		
		int nDay(0);
		if (m != 2)
		{
			nDay = s_days[m];
		}
		else
		{
			if ((y%400) == 0 ||	(y%4) == 0 && (y%100) != 0)
				nDay = 29;
			else
				nDay = 28;
		}
		return nDay;
	}

	
	const struct tm &GetZeroTime(){
		static struct tm s_dummy;
		return s_dummy;
	}
	
	bool IsZero(const struct tm &t){
		return !memcmp(&t, &GetZeroTime(), sizeof(t));
	}

	bool IsValidTime(const struct tm &t){
		struct tm tTemp = t;
		return ::mktime(&tTemp) != -1;
	}

	bool IsHappenBefore(const struct tm &t, const struct tm &tRef){
		struct tm tTemp = t;
		time_t value = ::mktime(&tTemp);
		if (value == -1){
			return false;
		}
		struct tm tRefTemp = tRef;
		time_t valueRef = ::mktime(&tRefTemp);
		if (valueRef == -1){
			return false;
		}
		return value <= valueRef;
	}

	bool IsHappenAfter(const struct tm &t, const struct tm &tRef){
		return IsHappenBefore(tRef, t);
	}
	
	bool IsHappenOn(const struct tm &t, const struct tm &tBegin, const struct tm &tEnd){
		return IsHappenAfter(t, tBegin)
			&& IsHappenBefore(t, tEnd);
	}

	bool IsHappenOnWithZeroRange(const struct tm &t, const struct tm &tBegin, const struct tm &tEnd){
		//	tBegin 或 tEnd 为 Zero 时，忽略边界
		bool result(false);
		if (IsZero(tBegin)){
			if (IsZero(tEnd)){
				result = true;
			}else{
				result = IsHappenBefore(t, tEnd);
			}
		}else{
			if (IsZero(tEnd)){
				result = IsHappenAfter(t, tBegin);
			}else{
				result = IsHappenOn(t, tBegin, tEnd);
			}
		}
		return result;
	}
}
