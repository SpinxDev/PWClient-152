// A2DDotLineMan.cpp: implementation of the A2DDotLineMan class.
//
//////////////////////////////////////////////////////////////////////
#include "AUI.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DMacros.h"

#include "A2DDotLineMan.h"

static const int _linebuf_size = 512;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

A2DDotLineMan::A2DDotLineMan() :
m_pA3DStream(NULL)
{

}

A2DDotLineMan::~A2DDotLineMan()
{

}

void A2DDotLineMan::Release()
{
	A3DRELEASE(m_pA3DStream);
	while (!m_aLines.empty())
	{
		A2DDotLine* pLine = m_aLines[0];
		A3DRELEASE(pLine);
	}
}

bool A2DDotLineMan::Create(A3DDevice* pA3DDevice)
{
	Release();

	m_pA3DDevice = pA3DDevice;
	m_pA3DStream = CreateStream(m_pA3DDevice);
	if (!m_pA3DStream)
		return false;

	return true;
}

void A2DDotLineMan::AddLine(A2DDotLine* pLine)
{
	if (!pLine)
		return;
	for (size_t i=0; i<m_aLines.size(); i++)
	{
		if (m_aLines[i] == pLine)
			return;
	}
	m_aLines.push_back(pLine);
}

void A2DDotLineMan::RemoveLine(A2DDotLine* pLine)
{
	if (!pLine)
		return;

	for (size_t i=0; i<m_aLines.size(); i++)
	{
		if (m_aLines[i] == pLine)
		{
			m_aLines.erase(m_aLines.begin() + i);
			return;
		}
	}
}

A3DStream* A2DDotLineMan::CreateStream(A3DDevice* pA3DDevice)
{
	if (!pA3DDevice)
		return NULL;

	A3DStream* pA3DStream = new A3DStream;
	if (!pA3DStream->Init(pA3DDevice, 
		sizeof(A3DTLVERTEX), 
		A3DFVF_A3DTLVERTEX, 
		_linebuf_size * 4,
		_linebuf_size * 6,
		A3DSTRM_REFERENCEPTR,
		A3DSTRM_STATIC))
	{
		delete pA3DStream;
		return NULL;
	}

	WORD* pVBIndices = NULL;
	if (!pA3DStream->LockIndexBuffer(0, 0, (LPBYTE*) &pVBIndices, NULL))
	{
		pA3DStream->Release();
		delete pA3DStream;
		return NULL;
	}
	for (int n=0, iIdx=0, iVert=0; n < _linebuf_size; n++, iIdx+=6, iVert+=4)
	{
		pVBIndices[iIdx+0] = iVert;
		pVBIndices[iIdx+1] = iVert + 2;
		pVBIndices[iIdx+2] = iVert + 1;
		pVBIndices[iIdx+3] = iVert;
		pVBIndices[iIdx+4] = iVert + 3;
		pVBIndices[iIdx+5] = iVert + 2;
	}	
	pA3DStream->UnlockIndexBuffer();

	return pA3DStream;
}

void A2DDotLineMan::RenderStream(int iVertSize, int iNumRect, const void* pSrc)
{
	if (!pSrc || iNumRect <= 0 || !m_pA3DStream || !m_pA3DDevice)
		return;
	
	const char* pOff = (const char*)pSrc;
	while (iNumRect)
	{
		int nCount = iNumRect > _linebuf_size ? _linebuf_size : iNumRect;
		iNumRect -= nCount;

		int iNumVert = nCount * 4;
		int iNumIndex = nCount * 6;
		DWORD dwLockSize = iNumVert * iVertSize;
		A3DTLVERTEX* pDestBuf = NULL;

		if (!m_pA3DStream->LockVertexBufferDynamic(0, dwLockSize, (BYTE**) &pDestBuf, 0))
			return;

		//	Copy data
		memcpy(pDestBuf, pOff, dwLockSize);
		pOff += dwLockSize;

		//	Unlock stream
		m_pA3DStream->UnlockVertexBufferDynamic();

		m_pA3DStream->Appear();

		//	Render
		if (!m_pA3DDevice->DrawIndexedPrimitive(
			A3DPT_TRIANGLELIST,
			0,
			iNumVert,
			0,
			nCount * 2))
			return;
	}
}

void A2DDotLineMan::Flush()
{
	for (size_t i=0; i<m_aLines.size(); i++)
	{
		if (m_aLines[i])
			m_aLines[i]->Flush();
	}
}