/*
* FILE: A3DGFXPIFuncs.h
*
* DESCRIPTION: PI means platform independent (platform means 2.0, 2.1, 2.2)
*				this class provide a set of shim functions to make up level code out of bothered by differences from 2.0, 2.1, 2.2
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/3/1
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DGFXPIFuncs_H_
#define _A3DGFXPIFuncs_H_

#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class A3DRenderTarget;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DGFXPIFuncs
//	
///////////////////////////////////////////////////////////////////////////

class A3DGFXPIFuncs
{
public:		//	Types

public:		//	Constructor and Destructor

public:		//	Attributes

public:		//	Operations

	static A3DRenderTarget* CreateRenderTarget(A3DDevice* pDev, int iWid, int iHei, bool bTarget, bool bDepth, A3DFORMAT fmtTarget, A3DFORMAT fmtDepth);
	static void DestroyRenderTarget(A3DRenderTarget* pRenderTarget);

	static bool ApplyRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt);
	static void RestoreRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt);
	static bool CopyRenderTargetToBackBuffer(A3DRenderTarget* pFrameTarget, A3DDevice* pA3DDevice);

	static DWORD GetSkinModelNoPixelShaderFlag();

private:	//	Attributes

private:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DGFXPIFuncs_H_

