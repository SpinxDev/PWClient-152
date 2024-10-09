/*
 * FILE: AutoHomeCommon.h
 *
 * DESCRIPTION: Class for general data
 *
 * CREATED BY: Jiang Dalong, 2006/06/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <ABaseDef.h>

// ID and corresponding index for tree and grass
struct PLANTIDINDEXINFO
{
	DWORD		dwID;
	int			nIndex;
};

// Plant tree type
enum PLANTTREETYPE
{
	PT_RANDOM	= 0,
	PT_REGULAR	= 1,
};

enum GRASSKIND
{
	GK_LAND		= 0,
	GK_WATER	= 1,
};
