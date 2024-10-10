#include "StdAfx.h"
#include "A3DGFXStreamMan.h"

#ifdef _ANGELICA21
	static const int _default_rect_count = 2560;
#else
	static const int _default_rect_count = 2560;
#endif
static const int _default_vert_buf = 32 * 4 * _default_rect_count;
static const int _default_index_count = 6 * _default_rect_count;

bool A3DSharedStream::Init(A3DDevice* pDevice)
{
	if (A3DStream::Init(
		pDevice,
		1,
		0,
		_default_vert_buf,
		_default_index_count,
		A3DSTRM_REFERENCEPTR,
		A3DSTRM_STATIC))
	{
		PresetIndexBuf();
		return true;
	}
	else
		return false;
}

void A3DSharedStream::Appear(int nVertType, int nVertSize, bool bUseIndex)
{
    AppearVertex(0, nVertSize, m_dwCurLockOffset);
    m_dwCurLockOffset += m_dwCurLockSize;

    if (bUseIndex)
        AppearIndex();
}

int A3DGFXStreamMan::GetMaxVertexBufferSize()
{
	return _default_vert_buf;
}

int A3DGFXStreamMan::GetMaxIndexCount()
{
	return _default_index_count;
}

bool A3DGFXStreamMan::Init(A3DDevice* pDevice)
{
	Lock();
	if (m_nRef == 0)
    {
        m_pDeferedLockBuffer = new BYTE[_default_vert_buf];
        if (!m_SharedStream.Init(pDevice))
	    {
		    Unlock();
		    return false;
	    }
    }
	m_nRef++;

	Unlock();
	return true;
}

void A3DGFXStreamMan::Release()
{
	if (m_nRef == 0) return;
    
	Lock();
	if (--m_nRef == 0)
    {
        m_SharedStream.Release();
        delete[] m_pDeferedLockBuffer;
        m_pDeferedLockBuffer = NULL;
    }
	Unlock();
}

bool A3DGFXStreamMan::LockVertexBuffer(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags)
{
    return LockVertexBufferDeferred(OffsetToLock, SizeToLock, ppbData, dwFlags);
#if defined(_ANGELICA21) || defined(_ANGELICA22)

	bool bRet;
	DWORD dwLockFlag = 0;
	Lock();

	if (m_SharedStream.IsVertexBufValid())
	{
		if (SizeToLock + m_SharedStream.m_dwCurLockOffset > _default_vert_buf)
			m_SharedStream.m_dwCurLockOffset = 0;
		else
			dwLockFlag = D3DLOCK_NOOVERWRITE;
		bRet = m_SharedStream.LockVertexBufferDynamic(m_SharedStream.m_dwCurLockOffset, SizeToLock, ppbData, dwLockFlag);
		m_SharedStream.m_dwCurLockSize = SizeToLock;
	}
	else
		bRet = false;

	if (!bRet)
		Unlock();

	return bRet;

#else
	Lock();
	bool bRet = (m_SharedStream.IsVertexBufValid() && m_SharedStream.LockVertexBuffer(OffsetToLock, SizeToLock, ppbData, dwFlags));
	if (!bRet) Unlock();
	return bRet;
#endif
}

bool A3DGFXStreamMan::LockVertexBufferDeferred(DWORD OffsetToLock, DWORD SizeToLock, BYTE** ppbData, DWORD dwFlags)
{
#if defined(_ANGELICA21) || defined(_ANGELICA22)

    bool bRet = true;
    Lock();

    if (m_SharedStream.IsVertexBufValid())
    {
        if (SizeToLock + m_SharedStream.m_dwCurLockOffset > _default_vert_buf)
            m_SharedStream.m_dwCurLockOffset = 0;

        *ppbData = m_pDeferedLockBuffer + m_SharedStream.m_dwCurLockOffset;
        m_dwLockStart = m_SharedStream.m_dwCurLockOffset;
        m_SharedStream.m_dwCurLockSize = SizeToLock;
    }
    else
        bRet = false;

    if (!bRet)
        Unlock();

    return bRet;

#else
    Lock();
    bool bRet = (m_SharedStream.IsVertexBufValid() && m_SharedStream.LockVertexBuffer(OffsetToLock, SizeToLock, ppbData, dwFlags));
    if (!bRet) Unlock();
    return bRet;
#endif
}

void A3DGFXStreamMan::UnlockVertexBuffer()
{
    UnlockVertexBufferDeferred();
}

void A3DGFXStreamMan::UnlockVertexBufferDeferred()
{
    DWORD dwLockFlag = 0;
    if (m_dwLockStart > 0)
        dwLockFlag = D3DLOCK_NOOVERWRITE;
    bool bRet;
    BYTE* pDst = NULL;
    bRet = m_SharedStream.LockVertexBufferDynamic(m_dwLockStart, m_SharedStream.m_dwCurLockSize, &pDst, dwLockFlag);

    if (bRet && pDst)
    {
		__try
		{
			memcpy(pDst, m_pDeferedLockBuffer + m_dwLockStart, m_SharedStream.m_dwCurLockSize);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}

        m_SharedStream.UnlockVertexBufferDynamic();
    }
    Unlock();
}