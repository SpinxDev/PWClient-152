/*
 * FILE: GL_Global.h
 *
 * DESCRIPTION: Element client, Global definition and declaration
 *
 * CREATED BY: duyuxin, 2003/12/11
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#pragma warning (disable: 4786)

#include "ABaseDef.h"
#include "ALog.h"
#include "AMemory.h"
#include "A3DVector.h"

class ALog;

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

extern ALog				g_Log;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

//	Initialize log system
bool glb_InitLogSystem(const char* szFile);
//	Close log system
void glb_CloseLogSystem();
//	Output predefined error
void glb_ErrorOutput(int iErrCode, const char* szFunc, int iLine);
//	Calculate a file's md5, return false if file missing or true with md5 returned
bool glb_CalcFileMD5(const char * szFile, BYTE md5[16]);
//	RepairExe
void glb_RepairExeInMemory();
int		hsv2rgb( float h, float s, float v);
///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


