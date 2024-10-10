/*
 * FILE: A3DVertexCollector.h
 *
 * DESCRIPTION: Used to manager vertices which will be rendered
 *
 * CREATED BY: duyuxin, 2001/12/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DVERTEXCOLLECTOR_H_
#define _A3DVERTEXCOLLECTOR_H_

#include "A3DDevice.h"
#include "A3DVertex.h"
#include "A3DTexture.h"
#include "A3DStream.h"
#include "A3DMaterial.h"
#include "AList.h"

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
//	Class A3DVertexCollector
//
///////////////////////////////////////////////////////////////////////////

class A3DVertexCollector
{
public:		//	Types

	//	Some limits of A3DVertexCollector
	enum
	{
		MAXNUM_VERTINSLOT	= 256,		//	Maximum number of vertex in a texture slot buffer
		MAXNUM_INDEXINSLOT	= 1024		//	Maximum number of index in a texture slot buffer
	};

	//	Vertex type used by A3DVertexCollector
	enum
	{
		VERTEX		= 0,		//	Untransformed and unlit vertex
		LVERTEX		= 1,		//	Untransformed but lit vertex
		TLVERTEX	= 2,		//	Transformed and lit vertex
		NUMVERTTYPE	= 3			//	Vertex type number
	};

	//	Slot buffer
	typedef struct _SLOTBUFFER
	{
		bool		bValid;				//	Buffer valid flag
		A3DStream*	pA3DStream;			//	Vertex and index buffer
		int			iNumVert;			//	Current number of vertex
		int			iNumIndex;			//	Current number of index
	
	} SLOTBUFFER, *PSLOTBUFFER;

	//	Texture slot
	typedef struct _TEXTURESLOT
	{
		A3DTexture*		pTexture;		//	Texture's ID, in fact it is a pointer
		A3DMaterial*	pMaterial;		//	Material pointer
		A3DSHADER		Shader;			//	Shader
		ALISTELEMENT*	pRecordElement;	//	Used by AList
		SLOTBUFFER		aBufs[NUMVERTTYPE];	//	Slot buffers

	} TEXTURESLOT, *PTEXTURESLOT;

public:		//	Constructors and Destructors

	A3DVertexCollector();
	virtual ~A3DVertexCollector();

public:		//	Attributes

public:		//	Operations

	bool		Init(A3DDevice* pA3DDevice);	//	Initialize object
	void		Release();		//	Release resources
	bool		Reset();		//	Reset collector

	DWORD		RegisterTexture(A3DVERTEXTYPE iVertexType, A3DTexture* pTexture, 
								A3DMaterial* pMaterial, A3DSHADER* pShader);	//	Add texture information

	bool		PushVertices(DWORD hTexture, A3DVERTEXTYPE iType, void* aInVerts, int iNumVert,
							 WORD* aInIndices, int iNumIdx);	//	Push vertices and ready to render
	bool		Flush(A3DViewport* pCurViewport);	//	Render all remained vertices

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;			//	A3D device object
	AList		m_TextureList;			//	Texture list
	int			m_aSizes[NUMVERTTYPE];	//	Size of three type vertex

protected:	//	Operations

	bool			AllocateSlotBuffer(PTEXTURESLOT pSlot, int iVertexType);	//	Allocate buffer for a texture slot
	TEXTURESLOT*	AllocateSlot(int iVertexType);	//	Allocate a new texture slot
	bool			FlushBuffer(PTEXTURESLOT pSlot, PSLOTBUFFER pSlotBuf);		//	Flush specified vertex buffer
	void			ReleaseTextureSlots();			//	Release texture slots

	inline int	VTA3DToCollector(A3DVERTEXTYPE iVertex);	//	Convert A3D vertex type to A3DVertexCollector vertex type
	inline A3DVERTEXTYPE VTCollectorToA3D(int iVertex);		//	Convert A3DVertexCollector vertex type to A3D vertex type
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

//	Convert A3D vertex type to A3DVertexCollector vertex type
int	A3DVertexCollector::VTA3DToCollector(A3DVERTEXTYPE iVertex)
{
	switch (iVertex)
	{
	case A3DVT_VERTEX:		return VERTEX;
	case A3DVT_LVERTEX:		return LVERTEX;
	case A3DVT_TLVERTEX:	return TLVERTEX;
	}

	return -1;
}

//	Convert A3DVertexCollector vertex type to A3D vertex type
A3DVERTEXTYPE A3DVertexCollector::VTCollectorToA3D(int iVertex)
{
	switch (iVertex)
	{
	case VERTEX:		return A3DVT_VERTEX;
	case LVERTEX:		return A3DVT_LVERTEX;
	case TLVERTEX:		return A3DVT_TLVERTEX;
	}

	return A3DVT_VERTEX;
}


#endif	//	_A3DVERTEXCOLLECTOR_H_


