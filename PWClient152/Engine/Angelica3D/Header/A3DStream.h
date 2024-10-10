/*
 * FILE: A3DStream.h
 *
 * DESCRIPTION: Class that standing for the vertex stream in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSTREAM_H_
#define _A3DSTREAM_H_

#include "A3DDevObject.h"
#include "A3DVertex.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

/*	A3D stream flags. Some tricks about using this flags

	* 0: Create dynamic buffer managed by A3DDevice. A extra vertex and index (if has)
		backup buffer will be created to store data which is used to
		restore buffer data after device lost. This flag is seldom used, because
		a dynamic buffer is often a frame-refreshed (update every frame) buffer, it
		doesn't need to restore it's data when device lose.

	* A3DSTRM_STATIC: Create static buffer automatically managed by D3D. Backup buffer
		in this case are created by D3D and user needn't to care about buffer's data
		restoring.
	
	* A3DSTRM_REFERENCEPTR: Create dynamic buffer managed by A3DDevice. Buffer's data
		restoring depends on external vertex and index buffers. Those external buffers
		can be set through A3DStream::SetVertexRef() and A3DStream::SetIndexRef(). If
		these two function never be called, buffer data won't be restored. But it's
		just the way most dynamic works like -- they don't need to restore data at all,
		because they are frame-refreshed.

	* A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR: Create static buffer managed by A3DDevice.
		Buffer's data restoring depends on external vertex and index buffers. Those
		external buffers can be set through A3DStream::SetVertexRef() and
		A3DStream::SetIndexRef(). If these two function never be called, buffer data
		won't be restored. This thing is usually terrible for static buffers, so don't
		forget to set external data buffer !
*/
#define A3DSTRM_STATIC			0x0001		//	Static buffer
#define A3DSTRM_REFERENCEPTR	0x0002		//	Reference pointers
#define A3DSTRM_INDEX32			0x0100		//	32-bit index, index flag only
#define A3DSTRM_SVP				0x0200		//	will be used with software vertex processing

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DDevice;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DStream
//
///////////////////////////////////////////////////////////////////////////

class A3DStream : public A3DDevObject
{
public:		//	Types

public:		//	Constructions and Destructions

	A3DStream();
	virtual ~A3DStream();

public:		//	Attributes

	static int	m_iDynDataSize;		//	Total dynamic data size
	static int	m_iStaticDataSize;	//	Total static data size
	static int	m_iBackupDataSize;	//	Backup data size

public:		//	Operations

	//	Initalize stream
	bool Init(A3DDevice* pDevice, int nVertexType, int nVertCount, int nIndexCount, DWORD dwVertexFlags, DWORD dwIndexFlags);
	bool Init(A3DDevice* pDevice, int iVertSize, DWORD dwFVF, int iVertCount, int iIndexCount, DWORD dwVertexFlags, DWORD dwIndexFlags);
	//	Release stream
	bool Release();

	bool LockVertexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags);
	bool LockIndexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags);
	bool UnlockVertexBuffer();
	bool UnlockIndexBuffer();

	bool SetVerts(void* pVerts, int nVertCount);
	bool SetIndices(void* pIndices, int nIndexCount);

	//	Bind vertex reference pointer
	void SetVertexRef(BYTE* p);
	//	Bind index reference pointer
	void SetIndexRef(void* p);

	bool Appear(int iStreamID=0, bool bApplyFVF=true);
	bool AppearVertexOnly(int iStreamID=0, bool bApplyFVF=true);
	bool AppearIndexOnly();
    bool AppearVertex(int iStreamID, UINT iStride, UINT iOffset);
    bool AppearIndex();

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	int GetVertCount() const { return m_nVertCount; }
	int GetIndexCount() const { return m_nIndexCount; }
	int GetVertexType() const { return m_nVertexType; }
	int GetVertexSize() const { return m_nVertexSize; }
	int GetIndexSize() const { return m_iIndexSize; }
	int GetVertBufSize() const { return m_nVertCount * m_nVertexSize; }
	int GetIndexBufSize() const { return m_iIndexSize * m_nIndexCount; }

	bool VertexIsDynamic() const { return (m_dwVertexFlags & A3DSTRM_STATIC) ? false : true; }
	bool VertexUseRefPtr() const { return (m_dwVertexFlags & A3DSTRM_REFERENCEPTR) ? true : false; }
	bool VertexIsSVP() const { return (m_dwVertexFlags & A3DSTRM_SVP) ? true : false; }
	DWORD GetVertexFlags() const { return m_dwVertexFlags; }
	
	bool IndexIsDynamic() const { return (m_dwIndexFlags & A3DSTRM_STATIC) ? false : true; }
	bool IndexUseRefPtr() const { return (m_dwIndexFlags & A3DSTRM_REFERENCEPTR) ? true : false; }
	bool Index32() const { return (m_dwIndexFlags & A3DSTRM_INDEX32) ? true : false; }
	bool IndexIsSVP() const { return (m_dwIndexFlags & A3DSTRM_SVP) ? true : false; }
	DWORD GetIndexFlags() const { return m_dwIndexFlags; }

    //multiThread Render --- begin
    bool LockVertexBufferDynamic(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags);
    bool LockIndexBufferDynamic(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags);
    bool UnlockVertexBufferDynamic();
    bool UnlockIndexBufferDynamic();
    bool SetVertsDynamic(void* pVerts, int nVertCount);
    bool SetIndicesDynamic(void* pIndices, int nIndexCount);

    //multiThread Render --- end

protected:	//	Attributes

	IDirect3DVertexBuffer9*		m_pVertexBuffer;
	IDirect3DIndexBuffer9*		m_pIndexBuffer;

	A3DDevice*	m_pA3DDevice;
	bool		m_bHWIStream;

	BYTE*		m_pVerts;
	void*		m_pIndices;

	DWORD		m_dwVertexFlags;
	DWORD		m_dwIndexFlags;

	int			m_nVertexType;
	int			m_nVertexSize;
	DWORD		m_dwFVFFlags;
	int			m_nVertCount;
	int			m_nIndexCount;
	int			m_iIndexSize;		//	Size of index in bytes (2 or 4)

	bool		m_bVertexUnmanaged;
	bool		m_bIndexUnmanaged;

protected:	//	Operations

	bool InitAsHWI(A3DDevice* pDevice, int iVertSize, DWORD dwFVF, int nVertCount, int nIndexCount, DWORD dwVertexFlags, DWORD dwIndexFlags);

	//	Create vertex buffer
	bool CreateVertexBuffer();
	//	Create index buffer
	bool CreateIndexBuffer();
};


#endif	//	_A3DSTREAM_H_

