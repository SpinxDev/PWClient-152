/*
* FILE: A3DGFXFuncs.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/9/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "A3DGFXFuncs.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static bool UpdatePositionParams(A3DGFXElement* pEle, const A3DVECTOR3 aPos[2], int aParams[2])
{
	for (int iParamIdx = 0; iParamIdx < 2; ++iParamIdx)
	{
		if (!pEle->NeedUpdateParam(aParams[iParamIdx]))
			continue;

		pEle->UpdateParam(aParams[iParamIdx], GFX_PROPERTY(aPos[iParamIdx]));
	}

	return true;
}

//	Some Special usage of A3DGFXEx
//	Update the gfx's lighting element's two egde positions
bool GFX_UpdateLightingEdgePos(A3DGFXEx* pGfx, const char* szEleName, const A3DVECTOR3 aEdge[2])
{
	ASSERT( pGfx && "Caller's response to make sure not putting in NULL pointer" );

	A3DGFXElement* pEle = pGfx->GetElementByName(szEleName);
	if (!pEle || (pEle->GetEleTypeId() != ID_ELE_TYPE_LIGHTNING
		&& pEle->GetEleTypeId() != ID_ELE_TYPE_LIGHTNINGEX))
	{
		a_LogOutput(1, "Element (%s) is not exist in this GFX or is not a lightning element", szEleName);
		return false;
	}

	int aUpdateParams[2] = 
	{
		ID_PARAM_LTN_POS1,
		ID_PARAM_LTN_POS2
	};
	
	return UpdatePositionParams(pEle, aEdge, aUpdateParams);
}


bool GFX_UpdateTrailEdgePos(A3DGFXEx* pGfx, const char* szEleName, const A3DVECTOR3 aEdge[2])
{
	ASSERT( pGfx && "Caller's response to make sure not putting in NULL pointer" );
	
	A3DGFXElement* pEle = pGfx->GetElementByName(szEleName);
	if (!pEle
		|| (pEle->GetEleTypeId() != ID_ELE_TYPE_TRAIL && pEle->GetEleTypeId() != ID_ELE_TYPE_TRAIL_EX)
        )
	{
		a_LogOutput(1, "Element (%s) is not exist or not a trail element.", szEleName);
		return false;
	}

	int aUpdateParams[2] = 
	{
		ID_PARAM_TRAIL_POS1,
		ID_PARAM_TRAIL_POS2,
	};

	return UpdatePositionParams(pEle, aEdge, aUpdateParams);
}

bool GFX_IsUseAudioEvent()
{
#if defined(_ANGELICA22) && defined(_USEAUDIOENGINE)
	if (g_pA3DConfig)
		return g_pA3DConfig->GetFlagFAudioEnable();
#endif

	return false;
}
