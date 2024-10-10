/*
 * FILE: A3DWireCollector.h
 *
 * DESCRIPTION: Wire object collector class
 *
 * CREATED BY: Duyuxin, 2003/10/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DWIRECOLLECTOR_H_
#define _A3DWIRECOLLECTOR_H_

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

class A3DEngine;
class A3DStream;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DWireCollector
//
///////////////////////////////////////////////////////////////////////////

class A3DWireCollector : public A3DDevObject
{
public:		//	Types

    struct BUFFER_3D
    {
        A3DStream*		pA3DStream;
        A3DLVERTEX*		aVertBuf;		//	Locked vertex buffer
        WORD*			aIdxBuf;		//	Locked index buffer
        int				iVertCnt;		//	Vertex fill counter
        int				iIdxCnt;		//	Index fill counter

        // --- multi-thread render
        A3DLVERTEX*		aVertBufRef;	//	Locked vertex buffer
        WORD*			aIdxBufRef;	//	Locked index buffer 
    };

    struct BUFFER_2D
    {
        A3DStream*		pA3DStream;
        A3DTLVERTEX*	aVertBuf;		//	Locked vertex buffer
        WORD*			aIdxBuf;		//	Locked index buffer
        int				iVertCnt;		//	Vertex fill counter
        int				iIdxCnt;		//	Index fill counter

        // --- multi-thread render
        A3DTLVERTEX*	aVertBufRef;	//	Locked vertex buffer
        WORD*			aIdxBufRef;	//	Locked index buffer
    };

public:		//	Constructor and Destructor

	A3DWireCollector();
	virtual ~A3DWireCollector() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize wire collector
	bool Init(A3DEngine* pA3DEngine, int iVertBufSize=2048, int iIdxBufSize=8192);
	//	Release wire collector
	void Release();

	//	Add vertex and index to render buffer
	bool AddRenderData_3D(A3DVECTOR3* aVerts, int iNumVert, WORD* aIndices, int iNumIdx, DWORD dwCol);
	//	Add a line to render buffer
	bool Add3DLine(const A3DVECTOR3& v1, const A3DVECTOR3& v2, DWORD dwCol);
	//	Add a box to render buffer
	bool Add3DBox(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp, const A3DVECTOR3& vRight, const A3DVECTOR3& vExts, DWORD dwCol, const A3DMATRIX4* pMat=NULL);
	//	Add a AABB to render buffer
	bool AddAABB(const A3DAABB& aabb, DWORD dwCol, const A3DMATRIX4* pMat=NULL);
	//	Add a OBB to render buffer
	bool AddOBB(const A3DOBB& obb, DWORD dwCol, const A3DMATRIX4* pMat=NULL);
	//	Add a sphere to render buffer
	bool AddSphere(const A3DVECTOR3 vPos, float fRadius, DWORD dwCol, const A3DMATRIX4* pMat=NULL);
	//	Add a capsule to render buffer
	bool AddCapsule(const A3DCAPSULE& cc, DWORD dwCol, const A3DMATRIX4* pMat=NULL);
	//	Add a cylinder to render buffer
	bool AddCylinder(const A3DCYLINDER& c, DWORD dwCol, const A3DMATRIX4* pMat=NULL);

	//	Add vertex and index to render buffer
	bool AddRenderData_2D(A3DVECTOR3* aVerts, int iNumVert, WORD* aIndices, int iNumIdx, DWORD dwCol);
	//	Add 2D line
	bool AddLine_2D(const A3DVECTOR3& v1, const A3DVECTOR3& v2, DWORD dwCol);
	//	Add 2D rectangle
	bool AddRect_2D(int l, int t, int r, int b, DWORD dwCol, float fz=0.0f);

	//	Flush sector
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

	A3DEngine*		m_pA3DEngine;		//	Render object
	int				m_iVertBufSize;		//	Vertex buffer size in item number
	int				m_iIdxBufSize;		//	Index buffer size in item number

	BUFFER_3D		m_3dBuf;			//	3D buffer data
	BUFFER_2D		m_2dBuf;			//	2D buffer data

protected:	//	Operations

	//	Lock/Unlock 3D stream
	bool LockStream_3D(bool bLock);
	//	Lock/Unlock 2D stream
	bool LockStream_2D(bool bLock);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DWIRECOLLECTOR_H_
