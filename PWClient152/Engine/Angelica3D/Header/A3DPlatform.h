/*
 * FILE: A3DPlatform.h
 *
 * DESCRIPTION: header for a platform related headers list
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DPLATFORM_H_
#define _A3DPLATFORM_H_

#include <Windows.h>
#include <StdIO.h>

#include <assert.h>

#ifdef _DEBUG
	#ifndef D3D_DEBUG_INFO
		#define D3D_DEBUG_INFO
	#endif
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <d3d9types.h>
#include <d3dx9math.h>
//	#include <AC.h> 
//	#include <AF.h>

#endif

