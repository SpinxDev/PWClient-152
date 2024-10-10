 /*
 * FILE: A3DStream.cpp
 *
 * DESCRIPTION: Class that standing for the vertex stream in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DStream.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DConfig.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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

int A3DStream::m_iDynDataSize		= 0;
int A3DStream::m_iStaticDataSize	= 0;
int A3DStream::m_iBackupDataSize	= 0;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

inline BOOL IsBadBufferAddress(BYTE * pBuffer, int nSize)
{
	return IsBadWritePtr((void *)pBuffer, nSize);
	/*
	int nKSize = nSize >> 12;

	for(int i=0; i<nKSize; i++)
	{
		if( IsBadWritePtr((void *)(pBuffer + (i << 12)), 1) )
			return true;
	}
	return false;
	*/
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkinModelAction
//
///////////////////////////////////////////////////////////////////////////

A3DStream::A3DStream()
{
	m_dwClassID			= A3D_CID_STREAM;
	m_pVertexBuffer		= NULL;
	m_pIndexBuffer		= NULL;
	m_pA3DDevice		= NULL;
	m_nVertexSize		= 0;
	m_dwFVFFlags		= 0;
	m_nVertexType		= A3DVT_UNKNOWN;
	m_nVertCount		= 0;
	m_nIndexCount		= 0;
	m_pVerts			= NULL;
	m_pIndices			= NULL;
	m_bHWIStream		= false;
	m_dwVertexFlags		= 0;
	m_dwIndexFlags		= 0;
	m_bVertexUnmanaged	= false;
	m_bIndexUnmanaged	= false;
	m_iIndexSize		= sizeof (WORD);
}

A3DStream::~A3DStream()
{
}

bool A3DStream::Init(A3DDevice* pDevice, int nVertexType, int nVertCount, int nIndexCount,
					 DWORD dwVertexFlags, DWORD dwIndexFlags)
{
	switch (nVertexType)
	{
	case A3DVT_VERTEX:
		m_nVertexSize = sizeof (A3DVERTEX);
		m_dwFVFFlags = A3DFVF_A3DVERTEX;
		break;
	case A3DVT_LVERTEX:
		m_nVertexSize = sizeof (A3DLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DLVERTEX;
		break;
	case A3DVT_TLVERTEX:
		m_nVertexSize = sizeof (A3DTLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DTLVERTEX;
		break;
	case A3DVT_TLVERTEX2:
		m_nVertexSize = sizeof (A3DTLVERTEX2);
		m_dwFVFFlags = A3DFVF_A3DTLVERTEX2;
		break;
	case A3DVT_IBLVERTEX:
		m_nVertexSize = sizeof (A3DIBLVERTEX);
		m_dwFVFFlags = A3DFVF_A3DIBLVERTEX;
		break;
	case A3DVT_BVERTEX1:
		m_nVertexSize = sizeof (A3DBVERTEX1);
		m_dwFVFFlags = A3DFVF_A3DBVERTEX1;
		break;
	case A3DVT_BVERTEX2:
		m_nVertexSize = sizeof (A3DBVERTEX2);
		m_dwFVFFlags = A3DFVF_A3DBVERTEX2;
		break;
	case A3DVT_BVERTEX3:
		m_nVertexSize = sizeof (A3DBVERTEX3);
		m_dwFVFFlags = A3DFVF_A3DBVERTEX3;
		break;
	case A3DVT_BVERTEX3TAN:
		m_nVertexSize = sizeof (A3DBVERTEX3TAN);
		m_dwFVFFlags = A3DVT_BVERTEX3TAN;
		break;
	case A3DVT_MCVERTEX:
		m_nVertexSize = sizeof (A3DMCVERTEX);
		m_dwFVFFlags = A3DFVF_A3DMCVERTEX;
		break;
	default:
		ASSERT(0);
		g_A3DErrLog.Log("A3DStream::Init(), Unknown vertex type!");
		return false;
	}

	if (!Init(pDevice, m_nVertexSize, m_dwFVFFlags, nVertCount, nIndexCount, dwVertexFlags, dwIndexFlags))
	{
		g_A3DErrLog.Log("A3DStream::Init(), Failed to initalize A3DStream!");
		return false;
	}

	m_nVertexType = nVertexType;

	return true;
}

bool A3DStream::Init(A3DDevice* pDevice, int iVertSize, DWORD dwFVF, int iVertCount,
					 int iIndexCount, DWORD dwVertexFlags, DWORD dwIndexFlags)
{
	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER || !pDevice)
		return InitAsHWI(pDevice, iVertSize, dwFVF, iVertCount, iIndexCount, dwVertexFlags, dwIndexFlags);

	if (dwIndexFlags & A3DSTRM_INDEX32)
	{
		if (!pDevice->SupportIndex32())
		{
			g_A3DErrLog.Log("A3DStream::Init(), Device doesn't support 32-bit index !");
			return false;
		}

		m_iIndexSize = sizeof (DWORD);
	}
	else
		m_iIndexSize = sizeof (WORD);

	m_pA3DDevice		= pDevice;
	m_nVertexSize		= iVertSize;
	m_dwFVFFlags		= dwFVF;
	m_nVertCount		= iVertCount;
	m_nIndexCount		= iIndexCount;
	m_dwVertexFlags		= dwVertexFlags;
	m_dwIndexFlags		= dwIndexFlags;
	m_nVertexType		= A3DVT_UNKNOWN;

	//	Craete vertex buffer
	if (iVertCount)
	{
		if (!CreateVertexBuffer())
			return false;
	}

	//	Create index buffer
	if (iIndexCount)
	{
		if (!CreateIndexBuffer())
			return false;
	}

	if (m_pA3DDevice)
	{
		if (m_bVertexUnmanaged || m_bIndexUnmanaged)
			m_pA3DDevice->AddUnmanagedDevObject(this);

		m_pA3DDevice->GetA3DEngine()->IncObjectCount(A3DEngine::OBJECTCNT_STREAM);
	}

	return true;
}

bool A3DStream::InitAsHWI(A3DDevice* pDevice, int iVertSize, DWORD dwFVF, int nVertCount, int nIndexCount, DWORD dwVertexFlags, DWORD dwIndexFlags)
{
	m_bHWIStream	= true;

	m_pA3DDevice	= pDevice;
	m_nVertexSize	= iVertSize;
	m_dwFVFFlags	= dwFVF;
	m_nVertCount	= nVertCount;
	m_nIndexCount	= nIndexCount;
	m_dwVertexFlags	= dwVertexFlags;
	m_dwIndexFlags	= dwIndexFlags;
	m_nVertexType	= A3DVT_UNKNOWN;

	if (m_dwIndexFlags & A3DSTRM_INDEX32)
		m_iIndexSize = sizeof (DWORD);
	else
		m_iIndexSize = sizeof (WORD);

	return true;
}

//	Create vertex buffer
bool A3DStream::CreateVertexBuffer()
{
	m_bVertexUnmanaged = false;

	DWORD dwUsage;
	D3DPOOL D3DPool;
	
	if (VertexIsDynamic())
	{
		
		dwUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		D3DPool = D3DPOOL_DEFAULT;
		m_bVertexUnmanaged = true;
		
		m_iDynDataSize += m_nVertCount * m_nVertexSize;
	}
	else	//	Static buffer
	{
		if (VertexUseRefPtr() || m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
		{
			dwUsage = D3DUSAGE_WRITEONLY;
			D3DPool = D3DPOOL_DEFAULT;
			m_bVertexUnmanaged = true;
		}
		else
		{
			dwUsage = D3DUSAGE_WRITEONLY;
			D3DPool = D3DPOOL_MANAGED;
		}

		m_iStaticDataSize += m_nVertCount * m_nVertexSize;
	}

	if( VertexIsSVP() )
		dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;

	//	Create vertex buffer
	HRESULT hval = m_pA3DDevice->GetD3DDevice()->CreateVertexBuffer(m_nVertCount * m_nVertexSize, dwUsage, NULL, D3DPool, &m_pVertexBuffer, NULL);
	if (D3D_OK != hval) 
	{
		g_A3DErrLog.Log("A3DStream::CreateVertexBuffer, CreateVertexBuffer Fail, with %d vertices, err=%x", m_nVertCount, hval);
		return false;
	}

	return true;
}

//	Create index buffer
bool A3DStream::CreateIndexBuffer()
{
	m_bIndexUnmanaged = false;

	if (!m_nIndexCount)
		return true;
		
	DWORD dwUsage;
	D3DPOOL D3DPool;
	bool bCreateBackBuf = false;

	if (IndexIsDynamic())
	{
		dwUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		D3DPool = D3DPOOL_DEFAULT;
		
		m_bIndexUnmanaged = true;
		
		m_iDynDataSize += m_nIndexCount * m_iIndexSize;
	}
	else	//	Static buffer
	{
		dwUsage = D3DUSAGE_WRITEONLY;

		if (IndexUseRefPtr() || m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
		{
			m_bIndexUnmanaged = true;
			D3DPool = D3DPOOL_DEFAULT;
		}
		else
			D3DPool = D3DPOOL_MANAGED;

		m_iStaticDataSize += m_nIndexCount * m_iIndexSize;
	}

	if( IndexIsSVP() )
		dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;

	//	Create index buffer
	D3DFORMAT Fmt = Index32() ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
	HRESULT hval = m_pA3DDevice->GetD3DDevice()->CreateIndexBuffer(m_nIndexCount * m_iIndexSize, dwUsage, Fmt, D3DPool, &m_pIndexBuffer, NULL);
	if (D3D_OK != hval)
	{
		g_A3DErrLog.Log("A3DStream::CreateIndexBuffer(), CreateIndexBuffer Fail, with %d indices, err=%x", m_nIndexCount, hval);
		return false;
	}

	return true;
}

bool A3DStream::Release()
{
	if (m_pA3DDevice)
	{
		m_pA3DDevice->GetA3DEngine()->DecObjectCount(A3DEngine::OBJECTCNT_STREAM);
		m_pA3DDevice->RemoveUnmanagedDevObject(this);
	}

	if (m_pVertexBuffer)
	{
		if (VertexIsDynamic())
			m_iDynDataSize -= m_nVertCount * m_nVertexSize;
		else
			m_iStaticDataSize -= m_nVertCount * m_nVertexSize;

		if( m_pA3DDevice->GetNeedResetFlag())
		{
			m_pVertexBuffer->Release();
		}
		else
		{
			m_pA3DDevice->ReleaseResource(m_pVertexBuffer);
		}
		m_pVertexBuffer = NULL;
	}

	if (m_pIndexBuffer)
	{
		if (IndexIsDynamic())
			m_iDynDataSize -= m_nIndexCount * m_iIndexSize;
		else
			m_iStaticDataSize -= m_nIndexCount * m_iIndexSize;

		if(m_pA3DDevice->GetNeedResetFlag())
		{
			m_pIndexBuffer->Release();
		}
		else
		{
			m_pA3DDevice->ReleaseResource(m_pIndexBuffer); //multithread render
		}
		m_pIndexBuffer = NULL;
	}
		
	m_pVerts	= NULL;
	m_pIndices	= NULL;

	m_pA3DDevice = NULL;

	return true;
}

bool A3DStream::SetVerts(void* pVerts, int nVertCount)
{
	if (m_bHWIStream || !m_pVertexBuffer)
		return true;

	HRESULT	hval;

	if (nVertCount > m_nVertCount)
	{
		g_A3DErrLog.Log("A3DStream::SetVerts, Vert Count Beyond m_VertCount!");
		return false;
	}

	LPBYTE pVBVerts;
	DWORD dwFlags = 0;

	if (VertexIsDynamic())
	{
#ifdef _DEBUG
		dwFlags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
#else
		dwFlags = D3DLOCK_DISCARD;
#endif
	}

	hval = m_pVertexBuffer->Lock(0, nVertCount * m_nVertexSize, (void**)&pVBVerts, dwFlags);
	if (D3D_OK != hval)
		return false;

	if( IsBadBufferAddress(pVBVerts, nVertCount * m_nVertexSize) )
	{
		m_pVertexBuffer->Unlock();
		g_A3DErrLog.Log("A3DStream::SetVerts, vertex buffer lock return bad buffer address! try to reset the device");
		
		if( m_pA3DDevice->GetAutoResetFlag() )
		{
			if( m_pA3DDevice->Reset() )
				g_A3DErrLog.Log("A3DStream::SetVerts, ... device reset done successfully!");
			else
				g_A3DErrLog.Log("A3DStream::SetVerts, ... device reset failed!");
		}
		else
		{
			m_pA3DDevice->SetNeedResetFlag(true);
		}

		return false;
	}

	memcpy(pVBVerts, pVerts, nVertCount * m_nVertexSize);

	hval = m_pVertexBuffer->Unlock();
	if (D3D_OK != hval)
		return false;

	return true;
}

bool A3DStream::SetIndices(void* pIndices, int nIndexCount)
{
	if (m_bHWIStream || !m_pIndexBuffer)
		return true;

	HRESULT hval;

	if (!m_pIndexBuffer || nIndexCount > m_nIndexCount)
	{
		g_A3DErrLog.Log("Index Count Beyond m_nIndexCount!");
		return false;
	}

	BYTE* pVBIndices;
	DWORD dwFlags = 0;

	if (IndexIsDynamic())
	{
#ifdef _DEBUG
		dwFlags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
#else
		dwFlags = D3DLOCK_DISCARD;
#endif
	}
	
	//	Update the d3d index buffer;
	hval = m_pIndexBuffer->Lock(0, 0, (void**)&pVBIndices, dwFlags);
	if (D3D_OK != hval)
		return false;

	if( IsBadBufferAddress(pVBIndices, nIndexCount * m_iIndexSize) )
	{
		m_pIndexBuffer->Unlock();
		g_A3DErrLog.Log("A3DStream::SetIndices, index buffer lock return bad buffer address! try to reset the device");
		if( m_pA3DDevice->GetAutoResetFlag() )
		{
			if( m_pA3DDevice->Reset() )
				g_A3DErrLog.Log("A3DStream::SetIndices, ... device reset done successfully!");
			else
				g_A3DErrLog.Log("A3DStream::SetIndices, ... device reset failed!");
		}
		else
		{
			m_pA3DDevice->SetNeedResetFlag(true);
		}
		return false;
	}

	memcpy(pVBIndices, pIndices, nIndexCount * m_iIndexSize);

	hval = m_pIndexBuffer->Unlock();
	if (D3D_OK != hval)
		return false;

	return true;
}

/*	Apply stream to device

	iStreamID: index of stream vertex buffer will be applied to.
	bApplyFVF: true, apply stream's FVF as vertex shader.
			   false, don't apply stream's FVF as vertex shader.
*/
bool A3DStream::Appear(int iStreamID/* 0 */, bool bApplyFVF/* true */)
{
	AppearVertexOnly(iStreamID, bApplyFVF);
	AppearIndexOnly();

	return true;
}

bool A3DStream::AppearVertexOnly(int iStreamID/* =0 */, bool bApplyFVF /* =true*/)
{
	if (m_bHWIStream || !m_pVertexBuffer)
		return true;
	
	if (!m_pA3DDevice)
		return true;
	
	if (FAILED(m_pA3DDevice->GetD3DDevice()->SetStreamSource(iStreamID, m_pVertexBuffer,0, m_nVertexSize)))
		return false;
	
	if (bApplyFVF)
	{
		//	Clear current vertex shader here is very important
		m_pA3DDevice->ClearVertexShader();
		m_pA3DDevice->SetFVF(m_dwFVFFlags);
	}

	return true;
}

bool A3DStream::AppearIndexOnly()
{
	if (m_pIndexBuffer)
	{
		if (FAILED(m_pA3DDevice->GetD3DDevice()->SetIndices(m_pIndexBuffer)))
			return false;
	}

	return true;
}

bool A3DStream::LockVertexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags)
{
	//	For a hardware independent stream, we have to let lock fail;
	if (m_bHWIStream || !m_pVertexBuffer)
		return false;

	assert((DWORD)(m_nVertCount * m_nVertexSize) >= OffsetToLock + SizeToLock);

	HRESULT hval;

	if (VertexIsDynamic())
	{
#ifdef _DEBUG
		if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
			dwFlags |= D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
		else
			dwFlags |= D3DLOCK_NOSYSLOCK;
#else
		if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
			dwFlags |= D3DLOCK_DISCARD;
#endif
	}

	hval = m_pVertexBuffer->Lock(OffsetToLock, SizeToLock, (void**)ppbData, dwFlags);
	if (D3D_OK != hval)
	{
        g_A3DErrLog.Log("A3DStream::LockVertexBuffer m_pVertexBuffer->Lock() Fail: 0x%X", hval);
		return false;
	}

	// on some ati card (such as RADEON 9500), lock return D3D_OK, but the pointer
	// point to an invalid memory address, and access violation will occurs.
	// If that happens nothing could save the game but to reset the device manually.
	int nSizeToTest = SizeToLock;
	if( nSizeToTest == 0 )
		nSizeToTest = m_nVertCount * m_nVertexSize;
	if( IsBadBufferAddress(*ppbData, nSizeToTest) )
	{
		m_pVertexBuffer->Unlock();
		g_A3DErrLog.Log("A3DStream::LockVertexBuffer, vertex buffer lock return bad buffer address!, try to do device reset!");

		if( m_pA3DDevice->GetAutoResetFlag() )
		{
			if( m_pA3DDevice->Reset() )
				g_A3DErrLog.Log("A3DStream::LockVertexBuffer, ... device reset done successfully!");
			else
				g_A3DErrLog.Log("A3DStream::LockVertexBuffer, ... device reset failed!");
		}
		else
		{
			m_pA3DDevice->SetNeedResetFlag(true);
		}

		*ppbData = NULL;
		return false;
	}
	
	return true;
}

bool A3DStream::UnlockVertexBuffer()
{
	if (m_bHWIStream)
		return false;

	HRESULT hval;

	hval = m_pVertexBuffer->Unlock();
	if (D3D_OK != hval)
	{
		g_A3DErrLog.Log("A3DStream::UnlockVertexBuffer m_pVertexBuffer->Unlock() Fail: 0x%X", hval);
		return false;
	}
	
	return true;
}

bool A3DStream::LockIndexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags)
{
	if (m_bHWIStream || !m_pIndexBuffer)
		return false;

	if (!m_nIndexCount)
	{
		ASSERT(0);
		return false;
	}

	HRESULT hval;

	if (IndexIsDynamic())
	{
#ifdef _DEBUG
		if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
			dwFlags |= D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
		else
			dwFlags |= D3DLOCK_NOSYSLOCK;
#else
		if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
			dwFlags |= D3DLOCK_DISCARD;
#endif
	}

	hval = m_pIndexBuffer->Lock(OffsetToLock, SizeToLock, (void**)ppbData, dwFlags);
	if (D3D_OK != hval)
	{				 
		g_A3DErrLog.Log("A3DStream::LockIndexBuffer m_pIndexBuffer->Lock() Fail");
		return false;
	}

	int nSizeToTest = SizeToLock;
	if( nSizeToTest == 0 )
		nSizeToTest = m_nIndexCount * m_iIndexSize;
	if( IsBadBufferAddress(*ppbData, nSizeToTest) )
	{
		m_pIndexBuffer->Unlock();
		g_A3DErrLog.Log("A3DStream::LockIndexBuffer, index buffer lock return bad buffer address! try to reset the device");
		if( m_pA3DDevice->GetAutoResetFlag() )
		{
			if( m_pA3DDevice->Reset() )
				g_A3DErrLog.Log("A3DStream::LockIndexBuffer, ... device reset done successfully!");
			else
				g_A3DErrLog.Log("A3DStream::LockIndexBuffer, ... device reset failed!");
		}
		else
		{
			m_pA3DDevice->SetNeedResetFlag(true);
		}

		*ppbData = NULL;
		return false;
	}

	return true;
}

bool A3DStream::UnlockIndexBuffer()
{
	if (m_bHWIStream)
		return true;

	if (!m_nIndexCount)
	{
		ASSERT(0);
		return false;
	}

	HRESULT hval;

	hval = m_pIndexBuffer->Unlock();
	if (D3D_OK != hval)
	{
		g_A3DErrLog.Log("A3DStream::UnlockIndexBuffer m_pIndexBuffer->Unlock() Fail");
		return false;
	}
	
	return true;
}

bool A3DStream::BeforeDeviceReset()
{
	if (m_bHWIStream ||m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
		return true;

	if (m_bVertexUnmanaged && m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

	if (m_bIndexUnmanaged && m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}

	return true;
}

bool A3DStream::AfterDeviceReset()
{
	if (m_bHWIStream ||m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag())
		return true;

	HRESULT	hval;
	DWORD dwUsage;
	D3DPOOL D3DPool;

	//	Restore vertex buffer
	if (m_bVertexUnmanaged && m_nVertCount > 0)
	{
		if (VertexIsDynamic())
		{
			dwUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
			D3DPool = D3DPOOL_DEFAULT;
		}
		else
		{
			dwUsage = D3DUSAGE_WRITEONLY;
			D3DPool = (VertexUseRefPtr()||m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag()) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		}
		if( VertexIsSVP() )
			dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;

		hval = m_pA3DDevice->GetD3DDevice()->CreateVertexBuffer(m_nVertCount * m_nVertexSize, dwUsage, NULL, D3DPool, &m_pVertexBuffer, NULL);
		if (D3D_OK != hval)
			return false;

		if (m_pVerts)
			SetVerts(m_pVerts, m_nVertCount);
	}

	//	Restore index buffer
	if (m_bIndexUnmanaged && m_nIndexCount > 0)
	{
		if (IndexIsDynamic())
		{
			dwUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
			D3DPool = D3DPOOL_DEFAULT;
		}
		else
		{
			dwUsage = D3DUSAGE_WRITEONLY;
			D3DPool = (IndexUseRefPtr()||m_pA3DDevice->GetA3DEngine()->GetSupportD3D9ExFlag()) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		}
		if( IndexIsSVP() )
			dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;

		D3DFORMAT Fmt = Index32() ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
		hval = m_pA3DDevice->GetD3DDevice()->CreateIndexBuffer(m_nIndexCount * m_iIndexSize, dwUsage, Fmt, D3DPool, &m_pIndexBuffer, NULL);
		if (D3D_OK != hval)
			return false;

		if (m_pIndices)
			SetIndices(m_pIndices, m_nIndexCount);
	}

	return true;
}

//	Bind vertex reference pointer
void A3DStream::SetVertexRef(BYTE* p)
{
	if (m_bHWIStream || !VertexUseRefPtr())
		return;

	m_pVerts = p;
}

//	Bind index reference pointer
void A3DStream::SetIndexRef(void* p)
{
	if (m_bHWIStream || !IndexUseRefPtr())
		return;

	m_pIndices = p;
}

bool A3DStream::AppearVertex(int iStreamID, UINT iStride, UINT iOffset)
{
    m_pA3DDevice->GetD3DDevice()->SetStreamSource(iStreamID, m_pVertexBuffer, iOffset, iStride);
    return true;
}

bool A3DStream::AppearIndex()
{
    m_pA3DDevice->GetD3DDevice()->SetIndices(m_pIndexBuffer);
    return true;
}

bool A3DStream::LockVertexBufferDynamic(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags)
{
    //return LockVertexBuffer(OffsetToLock, SizeToLock, ppbData, dwFlags);
    if (m_pA3DDevice->GetNeedResetFlag() && m_bVertexUnmanaged)
        return false;

    //	For a hardware independent stream, we have to let lock fail;
    if (m_bHWIStream || !m_pVertexBuffer)
        return false;

    assert((DWORD)(m_nVertCount * m_nVertexSize) >= OffsetToLock + SizeToLock);

    HRESULT hval;

    if (VertexIsDynamic())
    {
#ifdef _DEBUG
        if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
            dwFlags |= D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
        else
            dwFlags |= D3DLOCK_NOSYSLOCK;
#else
        if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
            dwFlags |= D3DLOCK_DISCARD;
#endif
    }

    if (SizeToLock == 0)
        SizeToLock = m_nVertexSize * m_nVertCount;

    hval = m_pA3DDevice->LockVertexBuffer(m_pVertexBuffer, OffsetToLock, SizeToLock, (void**)ppbData, dwFlags);
    if (D3D_OK != hval)
    {
        g_A3DErrLog.Log("A3DStream::LockVertexBuffer m_pVertexBuffer->Lock() Fail");
        return false;
    }

    return true;

}

bool A3DStream::LockIndexBufferDynamic( DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags )
{
    //return LockIndexBuffer(OffsetToLock, SizeToLock, ppbData, dwFlags);
    if (m_pA3DDevice->GetNeedResetFlag() && m_bIndexUnmanaged)
        return false;

    if (m_bHWIStream || !m_pIndexBuffer)
        return false;

    if (!m_nIndexCount)
    {
        ASSERT(0);
        return false;
    }

    HRESULT hval;

    if (IndexIsDynamic())
    {
#ifdef _DEBUG
        if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
            dwFlags |= D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
        else
            dwFlags |= D3DLOCK_NOSYSLOCK;
#else
        if( !(dwFlags & D3DLOCK_NOOVERWRITE) )
            dwFlags |= D3DLOCK_DISCARD;
#endif
    }

    if (SizeToLock == 0)
        SizeToLock = m_nIndexCount * (Index32() ? 4 : 2);

    hval = m_pA3DDevice->LockIndexBuffer(m_pIndexBuffer, OffsetToLock, SizeToLock, (void**)ppbData, dwFlags);
    if (D3D_OK != hval)
    {				 
        g_A3DErrLog.Log("A3DStream::LockIndexBuffer m_pIndexBuffer->Lock() Fail");
        return false;
    }

    return true;
}

bool A3DStream::UnlockVertexBufferDynamic()
{
    //return UnlockVertexBuffer();
    if (m_bHWIStream)
        return false;

    HRESULT hval;

    hval = m_pA3DDevice->UnlockVertexBuffer(m_pVertexBuffer);
    if (D3D_OK != hval)
    {
        g_A3DErrLog.Log("A3DStream::UnlockVertexBuffer m_pVertexBuffer->Unlock() Fail");
        return false;
    }

    return true;
}

bool A3DStream::UnlockIndexBufferDynamic()
{
    //return UnlockIndexBuffer();
    if (m_bHWIStream)
        return true;

    if (!m_nIndexCount)
    {
        ASSERT(0);
        return false;
    }

    HRESULT hval;

    hval = m_pA3DDevice->UnlockIndexBuffer(m_pIndexBuffer);
    if (D3D_OK != hval)
    {
        g_A3DErrLog.Log("A3DStream::UnlockIndexBufferDynamic m_pIndexBuffer->Unlock() Fail");
        return false;
    }

    return true;
}

bool A3DStream::SetVertsDynamic(void* pVerts, int nVertCount)
{
    if (m_bHWIStream || !m_pVertexBuffer)
        return true;

    HRESULT	hval;

    if (nVertCount > m_nVertCount)
    {
        g_A3DErrLog.Log("A3DStream::SetVerts, Vert Count Beyond m_VertCount!");
        return false;
    }

    LPBYTE pVBVerts;
    DWORD dwFlags = 0;

    if (VertexIsDynamic())
    {
#ifdef _DEBUG
        dwFlags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
#else
        dwFlags = D3DLOCK_DISCARD;
#endif
    }

    hval = m_pA3DDevice->LockVertexBuffer(m_pVertexBuffer, 0, nVertCount * m_nVertexSize, (void**)&pVBVerts, dwFlags);
    if (D3D_OK != hval)
        return false;

    memcpy(pVBVerts, pVerts, nVertCount * m_nVertexSize);

    hval = m_pA3DDevice->UnlockVertexBuffer(m_pVertexBuffer);
    if (D3D_OK != hval)
        return false;

    return true;
}

bool A3DStream::SetIndicesDynamic(void* pIndices, int nIndexCount)
{
    if (m_bHWIStream || !m_pIndexBuffer)
        return true;

    HRESULT hval;

    if (!m_pIndexBuffer || nIndexCount > m_nIndexCount)
    {
        g_A3DErrLog.Log("Index Count Beyond m_nIndexCount!");
        return false;
    }

    BYTE* pVBIndices;
    DWORD dwFlags = 0;

    if (IndexIsDynamic())
    {
#ifdef _DEBUG
        dwFlags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
#else
        dwFlags = D3DLOCK_DISCARD;
#endif
    }

    //	Update the d3d index buffer;
    hval = m_pA3DDevice->LockIndexBuffer(m_pIndexBuffer, 0, 0, (void**)&pVBIndices, dwFlags);
    if (D3D_OK != hval)
        return false;

    memcpy(pVBIndices, pIndices, nIndexCount * m_iIndexSize);

    hval = m_pA3DDevice->UnlockIndexBuffer(m_pIndexBuffer);
    if (D3D_OK != hval)
        return false;

    return true;
}