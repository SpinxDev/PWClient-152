/*
 * FILE: ATime.h
 *
 * DESCRIPTION: A class used to get some kind of time in A3D Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ATIME_H_
#define _ATIME_H_

#include "ACPlatform.h"

void a_GetSystemTime(WORD * pYear, WORD * pMonth, WORD * pDay, WORD * pDate, WORD * pHour, WORD * pMinute, WORD * pSecond);
DWORD a_GetTime();

#endif
