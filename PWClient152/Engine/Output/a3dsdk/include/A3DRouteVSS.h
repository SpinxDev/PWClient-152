/*
 * FILE: A3DRouteVSS.h
 *
 * DESCRIPTION: Fixed route scene's visible solution
 *
 * CREATED BY: duyuxin, 2002/7/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DROUTEVSS_H_
#define _A3DROUTEVSS_H_

#include "A3DVSSRouteLoader.h"
#include "A3DSceneVisible.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DRouteVSS
//
///////////////////////////////////////////////////////////////////////////

class A3DRouteVSS : public A3DSceneVisible
{
public:		//	Types

	enum
	{
		MAXNUM_ROUTE	= 1024		//	Maximum number of route can be handled at same time
	};

	struct ROUTE
	{
		A3DVSSRouteLoader*	pRoute;			//	Route object's address

		DWORD	dwID;			//	Route ID
		int*	aVisSurfs;		//	Visible surface indices buffer
		int		iNumVisSurf;	//	Number of visible surface
		int		iCurSection;	//	Current section
		int*	aVisObjs;		//	Visible scene object ID buffer
		int		iNumVisObj;		//	Number of visible scene object
	};

public:		//	Constructors and Destructors

	A3DRouteVSS();
	virtual ~A3DRouteVSS() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release resources
	virtual void Release();

	//	Get visible surfaces
	virtual DWORD GetVisibleSurfs(A3DViewport* pViewport, int** ppSurfs, int* piNumSurf);

	//	Load one route from disk file
	int	LoadRoute(char* szFileName, bool bAll);
	//	Load one route from memory file
	int	LoadRoute(BYTE* pBuf);			
	//	Remove one route
	bool RemoveRoute(int iIndex);		
	//	Remove all routes
	void RemoveAllRoutes();				
	//	Set current route
	bool SetCurrentRoute(int iIndex, int iSample);
	//	Set current section
	bool SetCurrentSection(int iSection);			

	//	Get scene object number
	int	GetSceneObjectNum();	
	//	Get visible object's ID of specified section
	bool GetCurrentVisObjs(int* pBuf, int* piNumVisObj);
	//	Get route loader object
	A3DVSSRouteLoader* GetRouteLoader(int iIndex);	

	void SetCurrentSample(int iSample) { m_iCurSample = iSample; }
	int	GetCurrentSample() { return m_iCurSample; }

protected:	//	Attributes

	ROUTE	m_aRoutes[MAXNUM_ROUTE];	//	Routes
	int		m_iNumRoute;				//	Number valid route in m_aRoutes
	int		m_iCurRoute;				//	Current route
	int		m_iCurSample;				//	Current sample
	bool	m_bNeedUpdate;				//	Need update vss ?

protected:	//	Operations

	//	Allocate a route slot
	ROUTE* AllocRouteSlot();
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DROUTEVSS_H_

