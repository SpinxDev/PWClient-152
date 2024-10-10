// VerticesBuffer.cpp: implementation of the CCharVerticesBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "CharVerticesBuffer.h"
#include "CodeTemplate.h"
#include "Global.h"
#include <AList2.h>

#define new A_DEBUG_NEW


CCharVerticesBuffer::CCharVerticesBuffer()
{
	m_nVerticesCount = -1;
	m_paVertices = NULL;
}

CCharVerticesBuffer::~CCharVerticesBuffer()
{
	Destroy();
}

int CCharVerticesBuffer::ComputeVerticesArraySize() const
{
	ASSERT(-1 != m_nVerticesCount);

	return m_nVerticesCount * (2 * sizeof(double));
}

void CCharVerticesBuffer::Destroy()
{
	SAFE_DELETE_ARRAY(m_paVertices);
	m_nVerticesCount = -1;
}

bool CCharVerticesBuffer::Create(int nVerticesCount)
{
	ASSERT(nVerticesCount > 0);
	ASSERT(-1 == m_nVerticesCount);
	ASSERT(NULL == m_paVertices);

	m_nVerticesCount = nVerticesCount;

	m_paVertices = (double (*)[2]) new double[ComputeVerticesArraySize()];

	return (NULL != m_paVertices);
}

bool CCharVerticesBuffer::Create(APtrList<CCharVerticesBuffer *> * pListVerticesBuffer)
{
	ASSERT(pListVerticesBuffer);
	ASSERT(-1 == m_nVerticesCount);
	ASSERT(NULL == m_paVertices);

	if (0 == pListVerticesBuffer->GetCount())
		return false;

	bool bSucceed = false;
	do {
		if (!BuildVerticesCountFromOtherVerticesList(pListVerticesBuffer))
			break;

		if (!BuildVerticesArrayFromOtherVerticesList(pListVerticesBuffer))
			break;

		bSucceed = true;
		
	} while(false);

	if (!bSucceed)
		Destroy();

	return true;
}


bool CCharVerticesBuffer::BuildVerticesCountFromOtherVerticesList(APtrList<CCharVerticesBuffer *> * pListVerticesBuffer)
{
	ASSERT(pListVerticesBuffer);
	ASSERT(-1 == m_nVerticesCount);
	ASSERT(NULL == m_paVertices);

	CCharVerticesBuffer *pSrcCharVerticesBuffer = NULL;
	m_nVerticesCount = 0;
	ALISTPOSITION iter = pListVerticesBuffer->GetHeadPosition();
	while (iter)
	{
		pSrcCharVerticesBuffer = pListVerticesBuffer->GetNext(iter);
		m_nVerticesCount += pSrcCharVerticesBuffer->m_nVerticesCount;
	}

	return (m_nVerticesCount > 1);
}

bool CCharVerticesBuffer::BuildVerticesArrayFromOtherVerticesList(APtrList<CCharVerticesBuffer *> * pListVerticesBuffer)
{
	ASSERT(pListVerticesBuffer);
	ASSERT(-1 != m_nVerticesCount);
	ASSERT(NULL == m_paVertices);

	m_paVertices = (double (*)[2]) new double [ComputeVerticesArraySize()];

	CCharVerticesBuffer *pSrcCharVerticesBuffer = NULL;
	
	BYTE * pPos = (BYTE *)m_paVertices;

	ALISTPOSITION iter = pListVerticesBuffer->GetHeadPosition();
	while (iter)
	{
		pSrcCharVerticesBuffer = pListVerticesBuffer->GetNext(iter);
		memcpy(pPos, pSrcCharVerticesBuffer->m_paVertices, pSrcCharVerticesBuffer->ComputeVerticesArraySize());
		pPos += pSrcCharVerticesBuffer->ComputeVerticesArraySize();
	}
	return true;
}

int CCharVerticesBuffer::GetBufferSize() const
{
	if (0 == m_nVerticesCount || !m_paVertices)
		return 0;
	else
		return ComputeVerticesArraySize();
}

bool CCharVerticesBuffer::IsCreated() const
{
	return (m_nVerticesCount != -1 && m_paVertices);
}

void CCharVerticesBuffer::ReversVertices()
{
	if (0 == m_nVerticesCount)
		return;

	CCharVerticesBuffer tempBuffer;
	tempBuffer.Create(m_nVerticesCount);
	for (int i = 0; i < m_nVerticesCount; i++)
	{
		tempBuffer.m_paVertices[i][0] = m_paVertices[m_nVerticesCount - i - 1][0];
		tempBuffer.m_paVertices[i][1] = m_paVertices[m_nVerticesCount - i - 1][1];
	}
	memcpy(m_paVertices, tempBuffer.m_paVertices, tempBuffer.GetBufferSize());
}

bool CCharVerticesBuffer::SetVerticesAt(int nIndex, double *p)
{
	ASSERT(m_nVerticesCount > 0);
	ASSERT(p);

	if (nIndex < 0 || nIndex > m_nVerticesCount - 1)
		return false;

	m_paVertices[nIndex][0] = p[0];
	m_paVertices[nIndex][1] = p[1];

	return true;
}
