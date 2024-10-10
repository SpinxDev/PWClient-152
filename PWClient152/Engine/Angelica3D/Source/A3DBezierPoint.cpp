/*
 * FILE: A3DBezierPoint.cpp
 *
 * DESCRIPTION: class for bezier point
 *
 * CREATED BY: Hedi, 2002/9/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DFuncs.h"
#include "A3DBezierPoint.h"
#include "A3DCollision.h"
#include "A3DMacros.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DViewport.h"

A3DBezierPoint::A3DBezierPoint()
{
	m_pA3DDevice		= NULL;
}

A3DBezierPoint::~A3DBezierPoint()
{
}

bool A3DBezierPoint::Init(A3DDevice * pA3DDevice, const A3DVECTOR3& vecPos, A3DCOLOR color, float vRadius)
{
	m_pA3DDevice	= pA3DDevice;
	m_vecPos		= vecPos;
	m_color			= color;
	m_vRadius		= vRadius;

	m_pIndices[0] = 0;
	m_pIndices[1] = 1;
	m_pIndices[2] = 2;

	m_pIndices[3] = 0;
	m_pIndices[4] = 2;
	m_pIndices[5] = 3;

	m_pIndices[6] = 0;
	m_pIndices[7] = 3;
	m_pIndices[8] = 1;

	m_pIndices[9] = 1;
	m_pIndices[10] = 3;
	m_pIndices[11] = 2;

	UpdatePoint();

	return true;	
}

bool A3DBezierPoint::Release()
{
	return true;
}

bool A3DBezierPoint::Render(A3DViewport * pCurrentViewport)
{
	//if( m_pA3DDevice->GetD3DDevice() )
	{
		m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);
		m_pA3DDevice->ClearTexture(0);

		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
			4, 4, m_pIndices, A3DFMT_INDEX16, m_pVertices, sizeof(A3DLVERTEX));
		m_pA3DDevice->SetLightingEnable(true);
	}
	return true;
}

bool A3DBezierPoint::Pick(A3DVECTOR3& vecStart, A3DVECTOR3& vecDir, A3DVECTOR3 * pPoint, float * pFraction)
{
	A3DVECTOR3	vecPoint, vecNormal;
	float		vFraction;

	if( CLS_RayToAABB3(vecStart, vecDir, m_aabb.Mins, m_aabb.Maxs, vecPoint, &vFraction, vecNormal) )
	{
		*pPoint = vecPoint;
		*pFraction = vFraction;
		return true;
	}

	return false;
}

bool A3DBezierPoint::SetColor(A3DCOLOR color)
{
	m_color = color;

	return UpdatePoint();
}

bool A3DBezierPoint::SetPosition(const A3DVECTOR3& vecPos)
{
	m_vecPos = vecPos;

	return UpdatePoint();
}

bool A3DBezierPoint::SetRadius(float vRadius)
{
	m_vRadius = vRadius;

	return UpdatePoint();
}

bool A3DBezierPoint::UpdatePoint()
{
	float		vRadius = m_vRadius;
	float		vRadiusLow = float(vRadius * cos(DEG2RAD(30.0f)));
	float		vHLow = -float(vRadius * sin(DEG2RAD(30.0f)));

	m_pVertices[0] = A3DLVERTEX(m_vecPos + A3DVECTOR3(0.0f, vRadius, 0.0f),
		m_color, 0xff000000, 0.0f, 0.0f);
	m_pVertices[1] = A3DLVERTEX(m_vecPos + A3DVECTOR3(0.0f, vHLow, vRadiusLow),
		m_color, 0xff000000, 0.0f, 0.0f);
	m_pVertices[2] = A3DLVERTEX(m_vecPos + A3DVECTOR3(float(vRadiusLow * cos(DEG2RAD(30.0f))), vHLow, -float(vRadiusLow * sin(DEG2RAD(30.0f)))),
		m_color, 0xff000000, 0.0f, 0.0f);
	m_pVertices[3] = A3DLVERTEX(m_vecPos + A3DVECTOR3(-float(vRadiusLow * cos(DEG2RAD(30.0f))), vHLow, -float(vRadiusLow * sin(DEG2RAD(30.0f)))),
		m_color, 0xff000000, 0.0f, 0.0f);

	m_aabb.Mins = m_vecPos - A3DVECTOR3(m_vRadius * 0.68f);
	m_aabb.Maxs = m_vecPos + A3DVECTOR3(m_vRadius * 0.68f);
	a3d_CompleteAABB(&m_aabb);

	return true;
}
