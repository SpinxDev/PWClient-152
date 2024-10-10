/*
 * FILE: A3DFlatCollector.h
 *
 * DESCRIPTION: Flat object collector class
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DFLATCOLLECTOR_H_
#define _A3DFLATCOLLECTOR_H_

#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "A3DDevObject.h"

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

class A3DStream;
class A3DEngine;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DFlatCollector
//
///////////////////////////////////////////////////////////////////////////

class A3DFlatCollector : public A3DDevObject
{
public:		//	Types

	enum
	{
		SIZE_VERTEXBUF	= 1024,
		SIZE_INDEXBUF	= 4096,
	};

public:		//	Constructor and Destructor

	A3DFlatCollector();
	virtual ~A3DFlatCollector() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize wire collector
	bool Init(A3DEngine* pA3DEngine, int iVertBufSize=2048, int iIdxBufSize=8192);
	//	Release wire collector
	void Release();

	//	Add vertex and index to render buffer
	bool AddRenderData_3D(const A3DVECTOR3* aVerts, int iNumVert, const WORD* aIndices, int iNumIdx, DWORD dwCol);
	//	Add a box to render buffer
	bool AddBox_3D(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, const A3DVECTOR3& vRight, const A3DVECTOR3& vExts, DWORD dwCol, const A3DMATRIX4* pMat=NULL);
	//	Add a AABB to render buffer
	bool AddAABB_3D(const A3DAABB& aabb, DWORD dwCol, const A3DMATRIX4* pMat=NULL);
	//	Add a OBB to render buffer
	bool AddOBB_3D(const A3DOBB& obb, DWORD dwCol, const A3DMATRIX4* pMat=NULL);

	//	Add vertex and index to render buffer
	bool AddRenderData_2D(const A3DVECTOR3* aVerts, int iNumVert, const WORD* aIndices, int iNumIdx, DWORD dwCol);
	//	Add a rectangle to render buffer
	bool AddRect_2D(int l, int t, int r, int b, DWORD dwCol, float fz=0.0f);

	//	Flush buffers
	bool Flush_3D();
	bool Flush_2D();

	bool Flush()
	{
		Flush_3D();
		Flush_2D();
		return true;
	}

	//	Before device reset
	virtual bool BeforeDeviceReset();

protected:	//	Attributes

	A3DEngine*		m_pA3DEngine;	//	A3DEngine object
	A3DStream*		m_pStream3D;
	A3DStream*		m_pStream2D;
	int				m_iVertBufSize;	//	Vertex buffer size in item number
	int				m_iIdxBufSize;	//	Index buffer size in item number
	A3DLVERTEX*		m_aVertBuf3D;	//	Locked vertex buffer
	WORD*			m_aIdxBuf3D;	//	Locked index buffer
	int				m_iVertCnt3D;	//	Vertex fill counter
	int				m_iIdxCnt3D;	//	Index fill counter
	A3DTLVERTEX*	m_aVertBuf2D;	//	Locked vertex buffer
	WORD*			m_aIdxBuf2D;	//	Locked index buffer
	int				m_iVertCnt2D;	//	Vertex fill counter
	int				m_iIdxCnt2D;	//	Index fill counter

    // --- multi-thread render
    A3DLVERTEX*		m_aVertBuf3DRef;	//	Locked vertex buffer
    WORD*			m_aIdxBuf3DRef;	    //	Locked index buffer
    A3DTLVERTEX*	m_aVertBuf2DRef;	//	Locked vertex buffer
    WORD*			m_aIdxBuf2DRef;	    //	Locked index buffer
    // --- multi-thread render

protected:	//	Operations

	//	Lock / Unlock stream
	bool LockStream_3D(bool bLock);
	//	Lock / Unlock stream
	bool LockStream_2D(bool bLock);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DFLATCOLLECTOR_H_
