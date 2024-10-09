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

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Basic render size
#define BASERENDER_WIDTH	800
#define BASERENDER_HEIGHT	600

//	Default camera FOV
#define DEFCAMERA_FOV		56.0f

//	Define virtual key of `
#define	VK_CONSOLE			0xC0

//	Error code
enum
{
	ECERR_UNKNOWN = 0,			//	Unknown
	ECERR_INVALIDPARAMETER,		//	Invalid parameter
	ECERR_NOTENOUGHMEMORY,		//	Not enough memory
	ECERR_FILEOPERATION,		//	File operation
	ECERR_FAILEDTOCALL,			//	Failed to call function
	ECERR_WRONGVERSION,			//	Wrong version
};

class ALog;
class CECGame;

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

extern ALog				g_Log;
extern char				g_szWorkDir[];
extern char				g_szIniFile[];
extern CECGame*			g_pGame;

extern A3DVECTOR3		g_vOrigin;
extern A3DVECTOR3		g_vAxisX;
extern A3DVECTOR3		g_vAxisY;
extern A3DVECTOR3		g_vAxisZ;

extern CRITICAL_SECTION	g_csLog;
extern CRITICAL_SECTION	g_csException;

extern bool				g_bTrojanDumpLastTime;

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
unsigned long glb_HandleException(LPEXCEPTION_POINTERS pExceptionPointers);
//	Log info and print it to console at debug
void Log_Info(const char *szFormat, ...);

int		hsv2rgb( float h, float s, float v);

#if defined(HINT_TOOL_DEBUG) || defined(_DEBUG)
namespace GNET
{
	class Octets;
}
class AString;
bool putOctetToFile(const GNET::Octets &data, const AString &strFileName, bool binary = false);
void openOctet(const GNET::Octets &data);
#endif
///////////////////////////////////////////////////////////////////////////
//
//	Inline functions

#define FASHION_WORDCOLOR_TO_A3DCOLOR(c) A3DCOLORRGB(((c) & (0x1f << 10)) >> 7, ((c) & (0x1f << 5)) >> 2, ((c) & 0x1f) << 3)
#define FASHION_A3DCOLOR_TO_WORDCOLOR(c) ((((c) & 0x00f80000) >> 9) | (((c) & 0x0000f800) >> 6) | (((c) & 0x000000f8) >> 3));
#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

//
///////////////////////////////////////////////////////////////////////////


class MyCriticalSection
{
public:
	MyCriticalSection(CRITICAL_SECTION &cs) : m_cs(&cs)
	{
		::EnterCriticalSection(m_cs);
	}
	
	~MyCriticalSection()
	{
		::LeaveCriticalSection(m_cs);
	}
	
private:
	CRITICAL_SECTION *m_cs;
};

enum
{
	enumSkinShowNone = 0,
	enumSkinShowUpperBody,
	enumSkinShowWrist,
	enumSkinShowLowerBody,
	enumSkinShowFoot,
	enumSkinShowUpperAndLower,
	enumSkinShowUpperAndWrist,
	enumSkinShowLowerAndFoot,
	enumSkinShowUpperLowerAndWrist,
	enumSkinShowArmet,
	enumSkinShowHand,
};