/*
 * FILE: MaskModifier.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "ARect.h"
#include "A3DVertex.h"

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

class A3DTexture;
class A3DViewport;
class CTerrain;
class A3DPixelShader;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CMaskModifier
//	
///////////////////////////////////////////////////////////////////////////

class CMaskModifier
{
public:		//	Types

	//	Currently modify
	struct MODIFY
	{
		A3DIBLVERTEX*	aVerts;		//	Vertices
		WORD*			aIndices;	//	Indices
		int				iNumVert;	//	Number of vertex
		int				iNumIndex;	//	Number of index
		ARectI			rcGrid;		//	Terrain grid area

		MODIFY();
		void Clear();
	};

public:		//	Constructor and Destructor

	CMaskModifier(CTerrain* pTerrain);
	virtual ~CMaskModifier();

public:		//	Attributes

public:		//	Operations

	//	Initalize object
	bool Init();
	//	Release object
	void Release();

	//	Change edit layer all modifies
	bool ChangeEditLayer(int iLayer);
	//	Begin modifying layer mask
	bool BeginModify();
	//	Modify routine
	bool Modify(const ARectI& rcArea, int* pModifyData);
	//	End modifying layer mask
	bool EndModify();

	//	Release all modify objects
	void ReleaseAllModifies();

	//	Render
	bool Render(A3DViewport* pA3DViewport);


	//	Has modified ?
	bool HasModify() { return m_bMoidfied; }
	void ClearModifyFlag() { m_bMoidfied = false; }

	//  Call this function after terrain loaded
	void UpdateLayerFlags();
	UINT64 GetBlockFlags(int iBlk );
	UINT64 GetBlockFlags(int r, int c);
	void UpdateMaskMapToRender(const ARectI& rcUpdate);


protected:	//	Attributes

	CTerrain*			m_pTerrain;
	MODIFY				m_CurModify;		//	Current modify
	int					m_iCurLayer;		//	Current edit layer
	int					m_iMaskTexSize;		//	Mask texture size
	A3DFORMAT			m_fmtMaskTex;		//	Mask texture format

	A3DTexture*			m_pTexture;			//	Current layer texture
	A3DTexture*			m_pMaskTexture;		//	Current mask texture
	A3DTexture*			m_pSpecularMap;		//	Current specular map
	A3DPixelShader*		m_p1LPS;			//	1 Layer pixel shader


	//	Accumulated modifies
	AArray<A3DIBLVERTEX, A3DIBLVERTEX&>	m_aAccumVerts;		//	Vertices
	AArray<WORD, WORD>					m_aAccumIdx;		//	Indices
	AArray<int, int>					m_aAccumGrids;		//	Terrain grid area
	abase::vector<UINT64>	m_aBlockFlags;		//  Terrain block mask flags
	bool				m_bMoidfied;

protected:	//	Operations

	//	Create modify mesh
	bool CreateModifyMesh(const ARectI& rcArea);
	//	Update mask texture
	bool UpdateMaskTexture(const ARectI& rcArea);
	//	Update mask texture
	bool UpdateMaskTexture(BYTE* pSrcData, const ARectI& rcArea, int iSrcPitch);

	//	Store currently modified mesh
	bool StoreCurrentModify();
	//	Try to insert a grid into m_aAccumGrids
	bool TryToAccumGrid(int iGrid);

	//	Release current layer resources
	void ReleaseCurLayerRes();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

