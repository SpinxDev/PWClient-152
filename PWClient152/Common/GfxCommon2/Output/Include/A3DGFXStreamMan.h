#ifndef _A3DGFXSTREAMMAN_H_
#define _A3DGFXSTREAMMAN_H_

#include "A3DStream.h"
#include "A3DDevice.h"

#define A3DFVF_GFXVERTEX (D3DFVF_XYZB1 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

struct A3DGFXVERTEX
{
	FLOAT x, y, z;
	DWORD dwMatIndices;
	DWORD diffuse;
	DWORD specular;
	FLOAT tu, tv;
};


#define A3DFVF_POINTVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct A3DPOINTVERTEX
{
	FLOAT x, y, z;
	DWORD diffuse;
};


#define A3DFVF_POINTEXVERTEX (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE)

struct A3DPOINTEXVERTEX
{
	FLOAT x, y, z;
	FLOAT size;
	DWORD diffuse;
};

class A3DGFXStreamMan;
class A3DSharedStream : public A3DStream
{
public:
	A3DSharedStream() : m_dwCurLockOffset(0), m_dwCurLockSize(0) {}
	virtual ~A3DSharedStream() {}

protected:
	friend class A3DGFXStreamMan;
	DWORD m_dwCurLockOffset;
	DWORD m_dwCurLockSize;

	bool Init(A3DDevice* pDevice);
	bool QuerySharedVertBuf(int nVertBufSize);
	bool QuerySharedIndexBuf(int nIndexCount);
	bool PresetIndexBuf();

public:

	void Appear(int nVertType, int nVertSize, bool bUseIndex);
	bool IsVertexBufValid() const { return m_pVertexBuffer != 0; }
};

inline bool A3DSharedStream::QuerySharedVertBuf(int nVertBufSize)
{
	if (nVertBufSize <= m_nVertCount) return true;

	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}

#ifdef _DEBUG
	a_LogOutput(0, "GFX VertBuf Max Size: %d\n", nVertBufSize);
#endif

	m_nVertCount = nVertBufSize;
	if (!CreateVertexBuffer())
	{
		m_nVertCount = 0;
		m_pVertexBuffer = NULL;
		return false;
	}
	return true;
}

inline bool A3DSharedStream::QuerySharedIndexBuf(int nIndexCount)
{
	if (nIndexCount <= m_nIndexCount) return true;

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}

#ifdef _DEBUG
	a_LogOutput(0, "GFX IndexBuf Max Count: %d\n", nIndexCount);
#endif

	m_nIndexCount = nIndexCount;
	if (!CreateIndexBuffer())
	{
		m_nIndexCount = 0;
		m_pIndexBuffer = NULL;
		return false;
	}

	if (!PresetIndexBuf())
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
		return false;
	}

	return true;
}

inline bool A3DSharedStream::PresetIndexBuf()
{
	// preset index buf
	WORD* pIndices;
	if (!LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0))
		return false;

	int nRectNum = m_nIndexCount / 6;
	for (int i = 0; i < nRectNum; i++)
	{
		int index = i * 6;
		WORD nBase = (WORD)(i * 4);
		pIndices[index  ] = nBase;//0
		pIndices[index+1] = nBase + 1;
		pIndices[index+2] = nBase + 2;
		pIndices[index+3] = nBase + 1;
		pIndices[index+4] = nBase + 3;
		pIndices[index+5] = nBase + 2;
	}

	UnlockIndexBuffer();
	return true;
}

class A3DGFXStreamMan
{
public:
	A3DGFXStreamMan() : m_nRef(0), m_pDeferedLockBuffer(NULL) { ::InitializeCriticalSection(&m_csStream); }
	virtual ~A3DGFXStreamMan() { ::DeleteCriticalSection(&m_csStream); }

protected:
	A3DSharedStream		m_SharedStream;
	int					m_nRef;
	CRITICAL_SECTION	m_csStream;
    BYTE*               m_pDeferedLockBuffer;
    DWORD               m_dwLockStart;

protected:
	void Lock() { ::EnterCriticalSection(&m_csStream); }
	void Unlock() { ::LeaveCriticalSection(&m_csStream); }

public:
	bool QueryVertBuf(int nSize)
	{
		Lock();
		bool bRet = m_SharedStream.QuerySharedVertBuf(nSize);
		Unlock();
		return bRet;
	}

	bool QueryIndexBuf(int nSize)
	{
		Lock();
		bool bRet = m_SharedStream.QuerySharedIndexBuf(nSize);
		Unlock();
		return bRet;
	}

	void Appear(int nVertType, int nVertSize, bool bUseIndex)
	{
		Lock();
		m_SharedStream.Appear(nVertType, nVertSize, bUseIndex);
		Unlock();
	}

	bool LockVertexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags);

	void UnlockVertexBuffer();

    bool LockVertexBufferDeferred(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags);

    void UnlockVertexBufferDeferred();

    bool Init(A3DDevice* pDevice);
	void Release();

	static int GetMaxVertexBufferSize();
	static int GetMaxIndexCount();
};

#endif
