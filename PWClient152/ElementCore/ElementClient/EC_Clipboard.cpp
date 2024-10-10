/*
 * FILE: EC_Clipboard.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/8/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Clipboard.h"

#define new A_DEBUG_NEW

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


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECClipboard
//	
///////////////////////////////////////////////////////////////////////////

CECClipboard::CECClipboard()
{
	memset(m_aDataSlots, 0, sizeof (m_aDataSlots));
}

CECClipboard::~CECClipboard()
{
	for (int i=0; i < NUM_DATASLOT; i++)
	{
		EraseData(i);
	}
}

//	Set data
bool CECClipboard::SetData(int iIndex, void* pData, int iSize)
{
	if (iIndex < 0 || iIndex >= NUM_DATASLOT)
	{
		ASSERT(0);
		return false;
	}

	DATASLOT& ds = m_aDataSlots[iIndex];

	if (ds.pData && ds.iBufSize >= iSize)
	{
		memcpy(ds.pData, pData, iSize);
		ds.iDataSize = iSize;
		return true;
	}
	
	if (ds.pData)
	{
		a_free(ds.pData);

		ds.iBufSize		= 0;
		ds.iDataSize	= 0;
	}

	if (!(ds.pData = a_malloc(iSize)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECClipboard::SetData", __LINE__);
		return false;
	}

	memcpy(ds.pData, pData, iSize);

	ds.iBufSize		= iSize;
	ds.iDataSize	= iSize;

	return true;
}

//	Get data
void* CECClipboard::GetData(int iIndex, int* piSize)
{
	if (iIndex < 0 || iIndex >= NUM_DATASLOT)
	{
		ASSERT(0);
		return NULL;
	}

	DATASLOT& ds = m_aDataSlots[iIndex];

	if (piSize)
		*piSize = ds.iDataSize;

	return ds.pData;
}

//	Erase data
void CECClipboard::EraseData(int iIndex)
{
	if (iIndex < 0 || iIndex >= NUM_DATASLOT)
	{
		ASSERT(0);
		return;
	}

	DATASLOT& ds = m_aDataSlots[iIndex];
	if (ds.pData)
	{
		a_free(ds.pData);
		ds.pData = NULL;
	}

	ds.iBufSize		= 0;
	ds.iDataSize	= 0;
}
