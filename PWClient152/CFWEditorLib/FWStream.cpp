// FWStream.cpp: implementation of the FWStream class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStream.h"
#include "CodeTemplate.h"

#define new A_DEBUG_NEW


static const int DEFAULT_VERT_COUNT = 500;
static const int DEFAULT_INDEX_COUNT = 500;



FWStream::FWStream()
{

}

FWStream::~FWStream()
{
	Release();
}

bool FWStream::QueryVertBuf(int nVertBufSize)
{
	ASSERT(nVertBufSize >= 0);

	if (nVertBufSize <= m_nVertCount) return true;
	
	SAFE_RELEASE(m_pVertexBuffer);
	
	m_nVertCount = nVertBufSize;
	if (!CreateVertexBuffer())
	{
		m_nVertCount = 0;
		m_pVertexBuffer = NULL;
		return false;
	}
	return true;
}

bool FWStream::QueryIndexBuf(int nIndexBufSize)
{
	ASSERT(nIndexBufSize >= 0);
	
	if (nIndexBufSize <= m_nIndexCount) return true;

	SAFE_RELEASE(m_pIndexBuffer);

	m_nIndexCount = nIndexBufSize;
	if (!CreateIndexBuffer())
	{
		m_nIndexCount = 0;
		m_pIndexBuffer = NULL;
		return false;
	}
	return true;
}


bool FWStream::Init(A3DDevice* pDevice)
{
	ASSERT(pDevice);

	return A3DStream::Init(
		pDevice,
		A3DVT_TLVERTEX,
		DEFAULT_VERT_COUNT,
		DEFAULT_INDEX_COUNT,					
		A3DSTRM_REFERENCEPTR,	
		A3DSTRM_REFERENCEPTR);
}
