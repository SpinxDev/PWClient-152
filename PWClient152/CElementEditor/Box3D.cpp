/*
 * FILE: Box3D.cpp
 *
 * DESCRIPTION: 3D box class
 *
 * CREATED BY: Duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "Box3D.h"
#include "Render.h"
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
//	Implement CBox3D
//
///////////////////////////////////////////////////////////////////////////

CBox3D::CBox3D()
{
	m_pRender	= NULL;
	m_dwColor	= 0;
	m_vPos		= g_vOrigin;
	m_vDir		= g_vAxisZ;
	m_vUp		= g_vAxisY;
	m_vRight	= g_vAxisX;
	m_bFlat		= true;
	
	m_matTM.Identity();
}

//	Initialize sector
bool CBox3D::Init(CRender* pRender, float fSize, bool bFlat/* true */)
{
	m_pRender	= pRender;
	m_bFlat		= bFlat;

	SetSize(fSize);

	return true;
}

//	Release sector
void CBox3D::Release()
{
	m_pRender = NULL;
}

//	Render sector
bool CBox3D::Render()
{
	ASSERT(m_pRender);

	//	Push to render buffer
	A3DVECTOR3 vExts(m_aSizes[0], m_aSizes[1], m_aSizes[2]);

	if (m_bFlat)
		m_pRender->GetA3DEngine()->GetA3DFlatCollector()->AddBox_3D(m_vPos, m_vDir, m_vUp, m_vRight, vExts, m_dwColor, &m_matTM);
	else
		m_pRender->GetA3DEngine()->GetA3DWireCollector()->Add3DBox(m_vPos, m_vDir, m_vUp, m_vRight, vExts, m_dwColor, &m_matTM);

	return true;
}

void CBox3D::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	m_vDir	= vDir;
	m_vUp	= vUp;

	m_vRight.CrossProduct(vUp, vDir);
}

