/*
 * FILE: A3DTerrain2CullHC.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/8/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2CULLHC_H_
#define _A3DTERRAIN2CULLHC_H_

#include "A3DTerrain2Cull.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Identity and version of horizon cull file
#define	TRN2HCFILE_IDENTIFY		(('T'<<24) | ('2'<<16) | ('H'<<8) | 'C')
#define TRN2HCFILE_VERSION		1

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Horizon cull file header
struct TRN2HCFILEHEADER
{
	int iNumBlockRow;	//	Number of block row
	int iNumBlockCol;	//	Number of block column
	int iNumEdge;		//	Number of edge
};

//	Block square data
struct TRN2HCFILEEDGE
{
	float	y1;
	float	y2;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

class A3DHorizonCull;
class A3DTerrain2Block;

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2CullHC
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2CullHC : public A3DTerrain2Cull
{
public:		//	Types

	struct BLKEDGE
	{
		float	y1;
		float	y2;
	};

public:		//	Constructor and Destructor

	A3DTerrain2CullHC();
	virtual ~A3DTerrain2CullHC();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const char* szFile);
	//	Release resources
	void Release();

	//	Attach terrain
	virtual bool AttachTerrain(A3DTerrain2* pTerrain);
	//	Prepare to culling work
	virtual bool PrepareToCull(A3DViewport* pViewport);
	//	Do block culling
	virtual void BlocksCull(A3DViewport* pViewport, const ARectI& rcView);
	//	Enable / Disable
	virtual void Enable(bool bEnable) { m_bEnable = bEnable; };
	//	Is culler enabled ?
	virtual bool IsEnabled() { return m_bEnable; };

	//	Get block edge number
	int GetEdgeNum() const { return m_aBlkEdges.GetSize(); }
	//	Get block edge
	const BLKEDGE& GetEdge(int n) { return m_aBlkEdges[n]; }

	//	Get horizon cull object
	A3DHorizonCull* GetHorzonCull() { return m_pHoriCull; }

protected:	//	Attributes

	A3DTerrain2*	m_ptrn;				//	Terrain object
	A3DHorizonCull*	m_pHoriCull;		//	Horizon culling object
	BYTE*			m_aEdgeIns;			//	Edge inserted bits
	int				m_iNumEdgeBytes;	//	Number of edge / sizeof (BYTE)
	int				m_iNumBlockRow;		//	Number of block row
	int				m_iNumBlockCol;		//	Number of block column
	bool			m_bEnable;			//	Enable flag

	AArray<BLKEDGE, BLKEDGE&>		m_aBlkEdges;	//	Block edges info
	APtrArray<A3DTerrain2Block*>	m_aCandBlks;	//	Candidate blocks

protected:	//	Operations

	//	Load horizon culling data from file
	bool LoadHorizonCull(const char* szFile);
	//	Determine block visibility using horizon culling
	bool BlockIsVisible(A3DTerrain2Block* pBlock);

	//	Get block edge inserted flag
	inline void SetBlockEdgeInserted(int n) { m_aEdgeIns[n >> 3] |= (1 << (n & 7)); }
	//	Set block edge inserted flag
	inline bool GetBlockEdgeInserted(int n) { return m_aEdgeIns[n >> 3] & (1 << (n & 7)) ? true : false; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2CULLHC_H_
