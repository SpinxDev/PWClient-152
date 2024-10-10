/*
 * FILE: A3DRouteVSS.cpp
 *
 * DESCRIPTION: Fixed route scene's visible solution
 *
 * CREATED BY: duyuxin, 2002/7/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTypes.h"
#include "A3DPI.h"
#include "A3DVSSCommon.h"
#include "A3DRouteVSS.h"

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
//	Implement of A3DRouteVSS
//
///////////////////////////////////////////////////////////////////////////

A3DRouteVSS::A3DRouteVSS()
{
	m_iNumRoute		= NULL;
	m_iCurRoute		= 0;
	m_bNeedUpdate	= false;
}

/*	Initialize object

	Return true for success, otherwise return false.
*/
bool A3DRouteVSS::Init()
{
	memset(m_aRoutes, 0, sizeof (m_aRoutes));

	m_iNumRoute	= 0;

	return true;
}

//	Release resources
void A3DRouteVSS::Release()
{
	RemoveAllRoutes();
}

/*	Set current route

	Return true for success, otherwise return false.

	iIndex: route's index returned by A3DRouteVSS::LoadRoute().
	iSample: current sample of specified route
*/
bool A3DRouteVSS::SetCurrentRoute(int iIndex, int iSample)
{
	if (iIndex < 0 || iIndex >= MAXNUM_ROUTE)
		return false;

	if (!m_aRoutes[iIndex].pRoute)
		return false;

	m_iCurSample	= iSample;
	m_iCurRoute		= iIndex;
	m_bNeedUpdate	= true;

	return true;
}

/*	Set current section

	Return true for success, otherwise return false.

	iSection: section's index
*/
bool A3DRouteVSS::SetCurrentSection(int iSection)
{
	ROUTE* pRoute = &m_aRoutes[m_iCurRoute];
	if (!pRoute || !pRoute->pRoute)
		return false;

	if (iSection < 0 || iSection >= pRoute->pRoute->GetSectionNumber())
		return false;

	m_iCurSample = pRoute->pRoute->GetFirstSampleOfSection(iSection);

	return true;
}

/*	Get visible draw surfaces from specified point

	Return valued is defined in class A3DSceneVisible, GVS_*

	pViewport: current viewport
	ppSurfs (out): *ppSurfs will point to surface index buffer if GVS_OK is returned.
	piNumSurf (out): number of surface in buffer pointed by *ppSurfs.
*/
DWORD A3DRouteVSS::GetVisibleSurfs(A3DViewport* pViewport, int** ppSurfs, int* piNumSurf)
{
	if (!m_iNumRoute)
		return GVS_ERROR;

	if (m_iCurRoute < 0 || m_iCurRoute >= m_iNumRoute ||
		!m_aRoutes[m_iCurRoute].pRoute)
		return GVS_ERROR;
		
	ROUTE* pRoute = &m_aRoutes[m_iCurRoute];
		
	int iSection = pRoute->pRoute->GetSectionOfSample(m_iCurSample);
	if (iSection != pRoute->iCurSection || m_bNeedUpdate)
	{
		pRoute->pRoute->GetSectionVisSurf(iSection, pRoute->aVisSurfs, &pRoute->iNumVisSurf);
		pRoute->iCurSection = iSection;

		if (pRoute->aVisObjs)
			pRoute->pRoute->GetSectionVisObjs(iSection, pRoute->aVisObjs, &pRoute->iNumVisObj);

		m_bNeedUpdate = false;
	}
		
	*piNumSurf	= pRoute->iNumVisSurf;
	*ppSurfs	= pRoute->aVisSurfs;

	return GVS_OK;
}

/*	Allocate a route slot

	Return route slot's address for success, otherwise return NULL.
*/
A3DRouteVSS::ROUTE* A3DRouteVSS::AllocRouteSlot()
{
	int i;

	//	Search a empty slot
	for (i=0; i < MAXNUM_ROUTE; i++)
	{
		if (!m_aRoutes[i].pRoute)
			break;
	}

	if (i >= MAXNUM_ROUTE)
	{
		g_A3DErrLog.Log("A3DRouteVSS::AllocRouteSlot, too many routes.");
		return NULL;
	}
	
	ROUTE* pRoute = &m_aRoutes[i];
	memset(pRoute, 0, sizeof (ROUTE));

	return pRoute;
}

/*	Load one route

	Return route's index for success, otherwise return -1.

	szFileName: route file's name
	bAll: true, load all route at once.
*/
int A3DRouteVSS::LoadRoute(char* szFileName, bool bAll)
{
	int iNumFace, iNumObj;

	ROUTE* pRoute = AllocRouteSlot();
	if (!pRoute)
	{
		g_A3DErrLog.Log("A3DRouteVSS::Init, failed to allocate route slot!");
		return -1;
	}

	//	Create route object
	if (!(pRoute->pRoute = new A3DVSSRouteLoader))
	{
		g_A3DErrLog.Log("A3DRouteVSS::Init, failed to create route object!");
		return -1;
	}

	if (!(pRoute->pRoute->Load(szFileName, bAll)))
	{
		g_A3DErrLog.Log("A3DRouteVSS::LoadRoute, failed to load route.");
		goto Error;
	}

	iNumFace = pRoute->pRoute->GetFaceNum();

	//	Create visible surface index buffer
	if (!(pRoute->aVisSurfs = new int [iNumFace]))
	{
		g_A3DErrLog.Log("A3DRouteVSS::LoadRoute, not enough memory.");
		goto Error;
	}

	memset(pRoute->aVisSurfs, 0, sizeof (int) * iNumFace);

	//	Create visible object ID buffer
	if ((iNumObj = pRoute->pRoute->GetObjectNum()))
	{
		if (!(pRoute->aVisObjs = new int [iNumObj]))
		{
			g_A3DErrLog.Log("A3DRouteVSS::LoadRoute, not enough memory.");
			goto Error;
		}

		memset(pRoute->aVisObjs, 0, iNumObj * sizeof (int));
	}

	pRoute->dwID		= pRoute->pRoute->GetRouteID();
	pRoute->iCurSection	= -1;
	pRoute->iNumVisSurf	= 0;
	pRoute->iNumVisObj	= 0;

	if (++m_iNumRoute == 1)		//	Is the first route
		m_iCurRoute = pRoute - m_aRoutes;
	
	return pRoute - m_aRoutes;

Error:

	if (pRoute->pRoute)
	{
		pRoute->pRoute->Release();
		delete pRoute->pRoute;
		pRoute->pRoute = NULL;
	}

	if (pRoute->aVisSurfs)
		delete [] pRoute->aVisSurfs;

	return -1;
}

/*	Load one route from memory file

	Return route's index for success, otherwise return -1.

	pBuf: memory file's address
*/
int	A3DRouteVSS::LoadRoute(BYTE* pBuf)
{
	int iNumFace, iNumObj;

	ROUTE* pRoute = AllocRouteSlot();
	if (!pRoute)
	{
		g_A3DErrLog.Log("A3DRouteVSS::Init, failed to allocate route slot!");
		return -1;
	}

	//	Create route object
	if (!(pRoute->pRoute = new A3DVSSRouteLoader))
	{
		g_A3DErrLog.Log("A3DRouteVSS::Init, failed to create route object!");
		return -1;
	}

	if (!(pRoute->pRoute->Load(pBuf)))
	{
		g_A3DErrLog.Log("A3DRouteVSS::LoadRoute, failed to load route.");
		goto Error;
	}

	iNumFace = pRoute->pRoute->GetFaceNum();

	//	Create visible surface index buffer
	if (!(pRoute->aVisSurfs = new int [iNumFace]))
	{
		g_A3DErrLog.Log("A3DRouteVSS::LoadRoute,  not enough memory.");
		goto Error;
	}

	memset(pRoute->aVisSurfs, 0, sizeof (int) * iNumFace);

	//	Create visible object ID buffer
	if ((iNumObj = pRoute->pRoute->GetObjectNum()))
	{
		if (!(pRoute->aVisObjs = new int [iNumObj]))
		{
			g_A3DErrLog.Log("A3DRouteVSS::LoadRoute, not enough memory.");
			goto Error;
		}

		memset(pRoute->aVisObjs, 0, iNumObj * sizeof (int));
	}

	pRoute->dwID		= pRoute->pRoute->GetRouteID();
	pRoute->iCurSection	= -1;
	pRoute->iNumVisSurf	= 0;
	pRoute->iNumVisObj	= 0;

	if (++m_iNumRoute == 1)		//	Is the first route
		m_iCurRoute = pRoute - m_aRoutes;
	
	return pRoute - m_aRoutes;

Error:

	if (pRoute->pRoute)
	{
		pRoute->pRoute->Release();
		delete pRoute->pRoute;
	}

	if (pRoute->aVisSurfs)
		delete [] pRoute->aVisSurfs;

	return -1;
}

/*	Remove one route

	Return true for success, otherwise return false.

	iIndex: route's index returned by A3DRouteVSS::LoadRoute().
*/
bool A3DRouteVSS::RemoveRoute(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAXNUM_ROUTE)
		return false;

	ROUTE* pRoute = &m_aRoutes[iIndex];
			
	if (pRoute->pRoute)
	{
		pRoute->pRoute->Release();
		delete pRoute->pRoute;
		pRoute->pRoute = NULL;
	}
	
	if (pRoute->aVisSurfs)
	{
		delete [] pRoute->aVisSurfs;
		pRoute->aVisSurfs = NULL;
	}

	if (pRoute->aVisObjs)
	{
		delete [] pRoute->aVisObjs;
		pRoute->aVisObjs = NULL;
	}

	m_iNumRoute--;

	return true;
}

//	Remove all routes
void A3DRouteVSS::RemoveAllRoutes()
{
	for (int i=0; i < MAXNUM_ROUTE; i++)
	{
		if (m_aRoutes[i].pRoute)
			RemoveRoute(i);
	}

	m_iNumRoute = 0;
}

/*	Get route loader object

	Return route loader object's address for success, otherwise return NULL.

	iIndex: route's index returned by A3DRouteVSS::LoadRoute().
*/
A3DVSSRouteLoader* A3DRouteVSS::GetRouteLoader(int iIndex)
{
	if (iIndex < 0 || iIndex >= MAXNUM_ROUTE)
		return NULL;

	return m_aRoutes[iIndex].pRoute;
}

/*	Get scene object number. Call this function after at least one route has
	been loaded.

	Return scene object's number.
*/
int	A3DRouteVSS::GetSceneObjectNum()
{
	if (!m_iNumRoute)
		return 0;

	return m_aRoutes[m_iCurRoute].pRoute->GetObjectNum();
}

/*	Get visible object's ID of specified section

	Return true for success, otherwise return false.

	pBuf (out): buffer used to receive visible object's ID, the buffer must be
				length enough, cannot be NULL. You can through
				A3DVSSRouteLoader::GetObjectNum() to get the maximum possible 
				number of ID will be put in this buffer.
	piNumVisObj (out): used to receive the number of object ID that can be seen 
					   in this section, also it's the number of valid ID in pBuf.
					   This parameter can be NULL
*/
bool A3DRouteVSS::GetCurrentVisObjs(int* pBuf, int* piNumVisObj)
{
	if (!m_aRoutes[m_iCurRoute].aVisObjs)
	{
		if (piNumVisObj)
			*piNumVisObj = 0;

		return false;
	}

	memcpy(pBuf, m_aRoutes[m_iCurRoute].aVisObjs, m_aRoutes[m_iCurRoute].iNumVisObj * sizeof (int));

	if (piNumVisObj)
		*piNumVisObj = m_aRoutes[m_iCurRoute].iNumVisObj;

	return true;
}



