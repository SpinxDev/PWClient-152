/*
 * FILE: Staff.cpp
 *
 * DESCRIPTION: Staff class
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "Render.h"
#include "Staff.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CStaff
//
///////////////////////////////////////////////////////////////////////////

CStaff::CStaff()
{
	m_pRender	= NULL;
	m_dwColor	= 0;
	m_vPos		= g_vOrigin;
	m_vDir		= g_vAxisZ;
	m_vUp		= g_vAxisY;
	m_vRight	= g_vAxisX;
	m_fLength	= 1.0f;
	m_aIndices	= NULL;
	m_iNumSeg	= 5;
	m_iNumIdx	= 0;
	m_aVerts	= NULL;
	m_fRadius	= 0.2f;

	m_matTM.Identity();
}

//	Initialize sector
bool CStaff::Init(CRender* pRender, int iSegment, float fLength, float fRadius)
{
	m_pRender	= pRender;
	m_fLength	= fLength;
	m_fRadius	= fRadius;
	m_iNumSeg	= iSegment >= 3 ? iSegment : 3;
	m_iNumIdx	= m_iNumSeg * 6 + (m_iNumSeg - 2) * 3 * 2;

	//	Create index buffer
	if (!(m_aIndices = new WORD[m_iNumIdx]))
	{
		g_Log.Log("CStaff::Init: Not enough memory.");
		return false;
	}

	//	Create vertex buffer
	if (!(m_aVerts = new A3DVECTOR3[m_iNumSeg * 2]))
	{
		g_Log.Log("CStaff::Init: Not enough memory.");
		return false;
	}

	int i, iCount=0;

	//	Fill indices ...
	//	Top side
	for (i=2; i < m_iNumSeg; i++)
	{
		m_aIndices[iCount++] = 0;
		m_aIndices[iCount++] = i - 1;
		m_aIndices[iCount++] = i;
	}

	//	Bottom side
	for (i=2; i < m_iNumSeg; i++)
	{
		m_aIndices[iCount++] = m_iNumSeg;
		m_aIndices[iCount++] = m_iNumSeg + i;
		m_aIndices[iCount++] = m_iNumSeg + i - 1;
	}

	//	Sides
	for (i=0; i < m_iNumSeg; i++)
	{
		int j = (i + 1) % m_iNumSeg;

		m_aIndices[iCount++] = i;
		m_aIndices[iCount++] = m_iNumSeg + i;
		m_aIndices[iCount++] = m_iNumSeg + j;
		m_aIndices[iCount++] = i;
		m_aIndices[iCount++] = m_iNumSeg + j;
		m_aIndices[iCount++] = j;
	}

	return true;
}

//	Release sector
void CStaff::Release()
{
	if (m_aIndices)
	{
		delete [] m_aIndices;
		m_aIndices = NULL;
	}

	if (m_aVerts)
	{
		delete [] m_aVerts;
		m_aVerts = NULL;
	}

	m_pRender = NULL;
}

//	Render sector
bool CStaff::Render()
{
	ASSERT(m_pRender);

	A3DVECTOR3 p1, p2;
	
	p1 = m_vPos - m_vDir * (m_fLength * 0.5f);
	p2 = m_vPos + m_vDir * (m_fLength * 0.5f);

	float fDelta = 360.0f / m_iNumSeg;
	float fDegree = 360.0f;

	for (int i=0; i < m_iNumSeg; i++)
	{
		float fRad = DEG2RAD(fDegree);
		fDegree -= fDelta;

		A3DVECTOR3 v = m_vRight * (float)cos(fRad) + m_vUp * (float)sin(fRad);
		v *= m_fRadius;

		m_aVerts[i]	= (p1 + v) * m_matTM;
		m_aVerts[m_iNumSeg + i] = (p2 + v) * m_matTM;
	}

	//	Push to render buffer
	m_pRender->GetA3DEngine()->GetA3DFlatCollector()->AddRenderData_3D(m_aVerts, m_iNumSeg * 2, m_aIndices, m_iNumIdx, m_dwColor);

	return true;
}

void CStaff::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	m_vDir	= vDir;
	m_vUp	= vUp;

	m_vRight.CrossProduct(vUp, vDir);
}



