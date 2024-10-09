// CharOutline.cpp: implementation of the CCharOutline class.
//
//////////////////////////////////////////////////////////////////////

#include "CharOutline.h"
#include "CharContour.h"
#include "CodeTemplate.h"
#include "Global.h"
#include <AMemory.h>
#include <AList2.h>

#define new A_DEBUG_NEW


CCharOutline::CCharOutline()
{
	m_pListCharContour = new APtrList<CCharContour *>;
}

CCharOutline::~CCharOutline()
{
	Destroy();
	SAFE_DELETE(m_pListCharContour);
}

void CCharOutline::Destroy()
{
	CCharContour *pContour = NULL;
	ALISTPOSITION pos = m_pListCharContour->GetHeadPosition();
	while (pos)
	{
		pContour = m_pListCharContour->GetNext(pos);
		SAFE_DELETE(pContour);
	}
	m_pListCharContour->RemoveAll();
}

bool CCharOutline::CreateFromChar(int nChar, HDC hDC)
{
	ASSERT(hDC);
	ASSERT(m_pListCharContour->GetCount() == 0);

	BYTE *pBuffer = NULL;
	DWORD dwBufferSize = 0;
	if (!GetOutlineBuffer(nChar, hDC, &pBuffer, &dwBufferSize))
		return false;

	TTPOLYGONHEADER *pPolyHeader = NULL;
	BYTE *pCurrentPos = pBuffer;
	int nCount = 0;
	bool bNeedRevers = false;
	while (pCurrentPos - pBuffer < (int)dwBufferSize)
	{
		pPolyHeader = (TTPOLYGONHEADER *)pCurrentPos;
		pCurrentPos = (BYTE *)(pPolyHeader + 1);

		CCharContour *pContour = new CCharContour;
		if (!pContour->CreateFromFont(pPolyHeader, pCurrentPos))
		{
			// ignor current contour
			SAFE_DELETE(pContour);
		}
		else
		{
//			if (bNeedRevers || 0 == nCount && nChar >> 8 && pContour->IsAntiClockwise())
//			{
//				pContour->ReversVertices();
//				bNeedRevers = true;
//			}
			m_pListCharContour->AddTail(pContour);
			nCount++;
		}
	}

	SAFE_DELETE(pBuffer);


	return true;
}

bool CCharOutline::GetOutlineBuffer(int nChar, HDC hDC, BYTE **ppBuffer, DWORD *pdwBufferSize)
{
	ASSERT(hDC);
	ASSERT(ppBuffer);
	ASSERT(pdwBufferSize);

	BYTE *pBuffer = NULL;

	// transform matrix
	MAT2 matTransform;
	ZeroMemory(&matTransform, sizeof(matTransform));
	matTransform.eM11.value = 1;
	matTransform.eM22.value = 1;

	// get buffer size
	DWORD dwBufferSize = ::GetGlyphOutline(hDC, nChar, GGO_NATIVE, &m_metrics, 0, NULL, &matTransform);
	if (dwBufferSize == GDI_ERROR)
	{
		return false;
	}

	// get buffer data
	pBuffer = new BYTE[dwBufferSize];
	if (GDI_ERROR == ::GetGlyphOutline(hDC, nChar, GGO_NATIVE, &m_metrics, dwBufferSize, pBuffer, &matTransform))
	{
		SAFE_DELETE(pBuffer);
		return false;
	}

	*ppBuffer = pBuffer;
	*pdwBufferSize = dwBufferSize;

	return true;
}

void CCharOutline::Simplify(float fSimplifyFactor, float fMinLenPerCurve)
{
	ALISTPOSITION pos = m_pListCharContour->GetHeadPosition();
	while (pos)
	{
		CCharContour *pContour = m_pListCharContour->GetNext(pos);
		pContour->Simplify(fSimplifyFactor, fMinLenPerCurve);
	}
}
