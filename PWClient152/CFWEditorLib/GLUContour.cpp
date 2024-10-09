// GLUContour.cpp: implementation of the CGLUContour class.
//
//////////////////////////////////////////////////////////////////////

#include <A3DMacros.h>
#include <AMemory.h>
#include "GLUContour.h"
#include "CharContour.h"
#include "CharVerticesBuffer.h"
#include "CodeTemplate.h"


#define new A_DEBUG_NEW

 


CGLUContour::CGLUContour()
{
	m_nVerticesCount = 0;
	m_paGLUVertices = NULL;
}

CGLUContour::~CGLUContour()
{
	Destory();
}

bool CGLUContour::CreateFromCharContour(const CCharContour *pCharContour)
{
	ASSERT(pCharContour);
	ASSERT(m_nVerticesCount == 0);
	ASSERT(!m_paGLUVertices);

	const CCharVerticesBuffer *pCharVerticesBuffer = pCharContour->GetCharVerticesBuffer();
	if (pCharVerticesBuffer->GetVerticesCount() == 0)
		return true;

	m_nVerticesCount = pCharVerticesBuffer->GetVerticesCount();
	m_paGLUVertices = (double (*)[3]) new double[ComputeVerticesArraySize()];

	for (int i = 0, nIndex = 0; i <= pCharVerticesBuffer->GetVerticesCount(); i++)
	{
		double *pSrcVertices = pCharVerticesBuffer->m_paVertices[i];
		if (!FindInVertices(pSrcVertices[0], pSrcVertices[1], 0))
		{
			m_paGLUVertices[nIndex][0] = pSrcVertices[0];
			m_paGLUVertices[nIndex][1] = pSrcVertices[1];
			m_paGLUVertices[nIndex][2] = 0;
			nIndex++;
		}
	}

	return true;
}

void CGLUContour::Destory()
{
	SAFE_DELETE_ARRAY(m_paGLUVertices);
	m_nVerticesCount = 0;
}

int CGLUContour::GetVerticesArraySize() const
{
	if (m_nVerticesCount > 0 && m_paGLUVertices)
		return ComputeVerticesArraySize();
	else
		return 0;
}

int CGLUContour::ComputeVerticesArraySize() const
{
	return sizeof(double) * 3 * m_nVerticesCount;
}

bool CGLUContour::FindInVertices(double x, double y, int nCount)
{
	for (int i = 0; i < nCount; i++)
	{
		if (m_paGLUVertices[i][0] == x &&
			m_paGLUVertices[i][1] == y)
			return true;
	}
	return false;
}


