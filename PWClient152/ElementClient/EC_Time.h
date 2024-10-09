// File		: EC_Time.h
// Creator	: Xu Wenbin
// Date		: 2015/1/4

#ifndef _ELEMENTCLIENT_EC_TIME_H
#define _ELEMENTCLIENT_EC_TIME_H

#include <time.h>

namespace CECTime{
	bool IsValidSingle(int value, short low, short high);
	bool IsValidContinuous(int y, int m, short y1, short m1, short y2, short m2);
	bool IsValidContinuous(int y, int m, int d, short y1, short m1, short d1, short y2, short m2, short d2);
	int GetWeek(int y, int m, int d);
	int GetMonthDayCount(int y, int m);

	const struct tm &GetZeroTime();
	bool IsZero(const struct tm &t);
	bool IsValidTime(const struct tm &t);
	bool IsHappenBefore(const struct tm &t, const struct tm &tRef);	
	bool IsHappenAfter(const struct tm &t, const struct tm &tRef);
	bool IsHappenOn(const struct tm &t, const struct tm &tBegin, const struct tm &tEnd);
	bool IsHappenOnWithZeroRange(const struct tm &t, const struct tm &tBegin, const struct tm &tEnd);
}

#endif		//	_ELEMENTCLIENT_EC_TIME_H