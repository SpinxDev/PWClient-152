/*
 * FILE: AMPI.h
 *
 * DESCRIPTION: private interface functions or variables for Angelica Media engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */


#ifndef _AMPI_H_
#define _AMPI_H_

#include "ALog.h"
extern ALog * g_pAMErrLog;

#define AMERRLOG(fmt) {if(g_pAMErrLog) g_pAMErrLog->Log fmt;}

#endif//_AMPI_H_