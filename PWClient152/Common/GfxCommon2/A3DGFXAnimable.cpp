/*
* FILE: A3DGFXAnimable.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/8/12
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "A3DGFXAnimable.h"
#include <AFile.h>
#include "A3DGFXFloatValueTrans.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static const char* _format_animtype		= "AnimType: %d";


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DGFXAnimable
//	
///////////////////////////////////////////////////////////////////////////

void A3DGFXAnimable::SetPropertyConstrains(const PropertyConstrains* pConstrains)
{
#ifdef GFX_EDITOR
	m_pConstrains = pConstrains;
#endif
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DGFXAnimable
//	
///////////////////////////////////////////////////////////////////////////

A3DGFXAnimable* A3DGFXAnimable::CreateAnimable(AnimableType at)
{
	switch (at)
	{
	case ANIMTYPE_FLOATTRANS:
		return new A3DGFXFloatValueTrans();
	case ANIMTYPE_FLOATTRANS_NOINTERPOLATE:
		return new A3DGFXFloatValueTrans(false);
	default:
		return NULL;
	}
}

void A3DGFXAnimable::DestroyAnimable(A3DGFXAnimable* pAnimable)
{
	delete pAnimable;
}

bool A3DGFXAnimable::SaveAnimable(AFile* pFile, A3DGFXAnimable* pAnimable)
{
	char szLine[AFILE_LINEMAXLEN];

	sprintf_s(szLine, _format_animtype, (int)pAnimable->GetAnimableType());
	pFile->WriteLine(szLine);

	return pAnimable->Save(pFile);
}

bool A3DGFXAnimable::LoadAnimable(AFile* pFile, A3DGFXAnimable** ppAnimable, DWORD dwVersion)
{
	if (!ppAnimable)
		return false;

	char szLine[AFILE_LINEMAXLEN];
	DWORD dwRead;

	int at;
	pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
	sscanf(szLine, _format_animtype, &at);
	
	if (*ppAnimable)
	{
		if ((*ppAnimable)->GetAnimableType() != (AnimableType)at)
		{
			const PropertyConstrains* pConstrains = (*ppAnimable)->m_pConstrains;
			delete *ppAnimable;
			*ppAnimable = CreateAnimable((AnimableType)at);
			(*ppAnimable)->m_pConstrains = pConstrains;
		}
	}
	else
	{
		*ppAnimable = CreateAnimable((AnimableType)at);
	}
	
	return (*ppAnimable)->Load(pFile, dwVersion);
}