/*
 * FILE: EC_Utility.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DVector.h"
#include "AAssist.h"
#include "A3DTypes.h"
#include <time.h>
#include <AUIObject.h>
#include <hashtab.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Macros used to implement 16.16 fix point calculation
#define	INTTOFIX16(x)		(((int)(x)) << 16)
#define FIX16TOINT(x)		(((x) + 0x00008000) >> 16)
#define FIX16TOINTCEIL(x)	(((x) + 0x0000ffff) >> 16)
#define FIX16TOINTFLOOR(x)	((x) >> 16)
#define FLOATTOFIX16(x)		((int)((x) * 65536.0f + 0.5f))
#define FIX16TOFLOAT(x)		((x) / 65536.0f)

#define FLOATTOFIX8(x)		((short)((x) * 256.0f + 0.5f))
#define FIX8TOFLOAT(x)		((x) / 256.0f)

#define FIX16MUL(M1, M2)	(int)(((__int64)(M1) * (M2) + 0x00008000) >> 16)
#define FIX16DIV(D1, D2)	(int)(((__int64)(D1) << 16) / (D2))

//	Check whether a position value is valid
#define INVALIDPOS			9000000.0f
#define POSISVALID(x)		((x) < INVALIDPOS-1.0f && (x) > -INVALIDPOS+1.0f)

#ifdef UNICODE
#define glb_vsnprintf	_vsnwprintf
#else
#define glb_vsnprintf	_vsnprintf
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Struct used by gl_StepSlideMove and gl_SlideMove
struct SLIDEMOVE
{
	A3DVECTOR3	vStart;			//	(in) Start position
	A3DVECTOR3	vExts;			//	(in) Extents
	A3DVECTOR3	vVelocity;		//	(in) Velocity
	float		fTime;			//	(in) Time
	float		fStepHeight;	//	(in) Step height
	float		fVertVel;		//	(in) Additional vertical velocity
	float		fClipY;			//	(in) If 0 < Normal.y of side < fClipY, vel.y will be clipped

	A3DVECTOR3	vDest;			//	(out) Destination point
};

class CECViewport;
class A3DSkin;
class A3DShader;

//	在 abase::hash_map 中封装 const char * 作为 key 的辅助类
struct	ConstChar
{
	const char *s;
	ConstChar():s(NULL){}
	ConstChar(const char *str) : s(str){}
	bool operator == (const ConstChar &rhs) const{
		return (!s && !rhs.s) || (s && rhs.s && !strcmp(s, rhs.s));
	}
};
struct ConstCharHashFunc : public abase::_hash_function
{
	unsigned long operator()(const ConstChar &rhs)const
	{
		return rhs.s ? _hash_function::operator()(rhs.s) : 0;
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

//	File operations
bool glb_CreateDirectory(const char* szDir);
bool glb_FileExist(const char* szFile);
void glb_ChangeExtension(char* szFile, char* szNewExt);
void glb_ClearExtension(char* szFile);
bool glb_WriteString(FILE* fp, const char* str);
ACString glb_GetExeDirectory();
void glb_GetExeDirectory(char *buffer, int bufferSize);

bool glb_PercentProbability(int iPercent);
bool glb_PercentProbability(float fPercent);
A3DVECTOR3 glb_RandomVectorH(int* piDegree=NULL);
A3DVECTOR3 glb_RandomVector();

BYTE glb_CompressDirH(float x, float z);
A3DVECTOR3 glb_DecompressDirH(BYTE byDir);

int glb_IntCompare(const void* arg1, const void* arg2);
int glb_WordCompare(const void* arg1, const void* arg2);
int glb_IconStateCompare(const void* arg1, const void* arg2);

extern bool g_bExceptionOccured;
unsigned long glb_HandleException(LPEXCEPTION_POINTERS pExceptionPointers);

//	Get command line parameters
bool glb_IsConsoleEnable();
void glb_EnableConsole(bool bEnable);

tm* glb_GetFormatLocalTime();

//	Build pvp mask
BYTE glb_BuildPVPMask(bool bForceAttack);
//  Build bless mask
BYTE glb_BuildBLSMask();
//	Build refuse bless mask
BYTE glb_BuildRefuseBLSMask();
//	Get force attack flag
bool glb_GetForceAttackFlag(const DWORD* pdwParam);

#define FATAL_ERROR_LOAD_BUILDING 1
#define FATAL_ERROR_WRONG_CONFIGDATA 2
extern DWORD g_dwFatalErrorFlag;

void glb_ArmorReplaceShader(A3DSkin * pSkin);
void glb_ArmorReplaceShader_ReflectPrefix(A3DSkin * pSkin);
void glb_FashionReplaceShader(A3DSkin * pSkin);
A3DShader* glb_LoadBodyShader(A3DSkin* pSkin, const char* szTexMap);
void glb_BoothReplaceShader(A3DSkin *pSkin);

ACString A3DCOLOR_TO_STRING(A3DCOLOR clr);
bool STRING_TO_A3DCOLOR(ACString str, A3DCOLOR &clr);

//  Is the host player in the map for mini client
bool glb_IsInMapForMiniClient();

void glb_ExploreFile(const char *szFullPath);

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
inline bool	glb_IsTextEmpty(PAUIOBJECT pObj)
{
	ASSERT(pObj != NULL);

	return (pObj == NULL) || (pObj->GetText() == NULL) || pObj->GetText()[0] == _AL('\0');
}

inline bool glb_IsTextNotEmpty(PAUIOBJECT pObj)
{
	return !glb_IsTextEmpty(pObj);
}