/*
 * FILE: ATime.h
 *
 * DESCRIPTION: A class used to get some kind of time in Angelica Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "ATime.h"
#include "ACounter.h"

#pragma comment(lib, "winmm.lib")

void a_GetSystemTime(WORD * pYear, WORD * pMonth, WORD * pDay, WORD * pDate, WORD * pHour, WORD * pMinute, WORD * pSecond)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	if( pYear ) *pYear = st.wYear;
	if( pMonth ) *pMonth = st.wMonth;
	if( pDay ) *pDay = st.wDay;
	if( pDate ) *pDate = st.wDayOfWeek;
	if( pHour ) *pHour = st.wHour;
	if( pMinute ) *pMinute = st.wMinute;
	if( pSecond ) *pSecond = st.wSecond;
}

DWORD a_GetTime()
{
	return (DWORD)(ACounter::GetMicroSecondNow() / 1000);
}

