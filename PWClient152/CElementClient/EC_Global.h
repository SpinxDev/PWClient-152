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
#include "AAssist.h"

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

#ifdef ANGELICA_2_2
#define SHADER_DIR		"Shaders\\2.2\\"
#else
#define SHADER_DIR		"Shaders\\"
#endif // ANGELICA_2_2

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
class AString;
class AWString;

namespace GNET
{
	class Octets;
}

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

extern ALog				g_Log;
extern char				g_szWorkDir[];
extern char				g_szIniFile[];
extern CECGame*			g_pGame;
extern bool				g_bRenderNoFocus;
extern bool				g_bEnableFortressDeclareWar;
extern bool				g_bIgnoreURLNavigate;

extern A3DVECTOR3		g_vOrigin;
extern A3DVECTOR3		g_vAxisX;
extern A3DVECTOR3		g_vAxisY;
extern A3DVECTOR3		g_vAxisZ;

extern CRITICAL_SECTION	g_csException;
extern CRITICAL_SECTION	g_csSession;
extern CRITICAL_SECTION	g_csRTDebug;

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

void glb_LogURL(const AString &strURL);
AString glb_FormatOctets(const GNET::Octets &o);
AString glb_ConverToUTF8(const AWString &str);
AString glb_ConverToHex(const AString &str);
int		glb_Random(int iMin, int iMax);
int		hsv2rgb( float h, float s, float v);

void	glb_LogDebugInfo(const AString &str);
#ifdef LOG_PROTOCOL
#define LOG_DEBUG_INFO(s) glb_LogDebugInfo(s)
#else
#define LOG_DEBUG_INFO(s)
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions

inline void _cp_str(ACString& str, const void* s, int len)
{
	len /= sizeof(ACHAR);
	ACHAR* p = str.GetBuffer(len + 1);
	memcpy(p, s, len * sizeof(ACHAR));
	p[len] = _AL('\0');
	str.ReleaseBuffer();
}

#define FASHION_WORDCOLOR_TO_A3DCOLOR(c) A3DCOLORRGB(((c) & (0x1f << 10)) >> 7, ((c) & (0x1f << 5)) >> 2, ((c) & 0x1f) << 3)
#define FASHION_A3DCOLOR_TO_WORDCOLOR(c) ((((c) & 0x00f80000) >> 9) | (((c) & 0x0000f800) >> 6) | (((c) & 0x000000f8) >> 3));
#define ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

//	ÉùÃ÷ Singleton
#define ELEMENTCLIENT_DECLARE_SINGLETON(CLASS_NAME)	\
private:\
CLASS_NAME();\
CLASS_NAME(const CLASS_NAME &);\
CLASS_NAME & operator = (const CLASS_NAME &);\
public:\
static CLASS_NAME &Instance()

//	¶¨Òå Singleton
#define ELEMENTCLIENT_DEFINE_SINGLETON(CLASS_NAME)\
CLASS_NAME::CLASS_NAME(){}\
CLASS_NAME & CLASS_NAME::Instance(){\
	static CLASS_NAME s_instance;\
	return s_instance;\
}

#define ELEMENTCLIENT_DEFINE_SINGLETON_NO_CTOR(CLASS_NAME)\
	CLASS_NAME & CLASS_NAME::Instance(){\
	static CLASS_NAME s_instance;\
	return s_instance;\
}

//
///////////////////////////////////////////////////////////////////////////


