/*
* FILE: A3DGFXFuncs.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/9/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DGFXFuncs_H_
#define _A3DGFXFuncs_H_


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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

//	Some Special usage of A3DGFXEx
//	Update the gfx's lighting element's two edge positions
//	Call this function before calling A3DGFXEx* pGfx's TickAnimation
//	Return value: return false if element is not lightning or lightningex, or is not exist
//	True if succeed
bool GFX_UpdateLightingEdgePos(A3DGFXEx* pGfx, const char* szEleName, const A3DVECTOR3 aEdge[2]);

//	Update the gfx's trail element's two edge point positions
//	Call this function before calling A3DGFXEx* pGfx's TickAnimation
bool GFX_UpdateTrailEdgePos(A3DGFXEx* pGfx, const char* szEleName, const A3DVECTOR3 aEdge[2]);

//	Is engine using audio event
//	When In Angelia2.2 and angelica.cfg [a3d] faudio_enable = 1, we use audio engine based on fmod
//	Else we use the old sound
bool GFX_IsUseAudioEvent();

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DGFXFuncs_H_



