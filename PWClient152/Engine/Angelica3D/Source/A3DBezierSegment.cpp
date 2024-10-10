/*
 * FILE: A3DBezierSegment.cpp
 *
 * DESCRIPTION: class for bezier segment
 *
 * CREATED BY: Hedi, 2002/9/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DBezierSegment.h"
#include "A3DGDI.h"
#include "A3DFuncs.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "A3DCollision.h"

A3DBezierSegment::A3DBezierSegment()
{
	m_pA3DDevice		= NULL;
}

A3DBezierSegment::~A3DBezierSegment()
{
}

bool A3DBezierSegment::Init(A3DDevice * pA3DDevice, A3DVECTOR3& vecStart, A3DVECTOR3& vecEnd, 
		A3DVECTOR3& vecStartCtrl, A3DVECTOR3& vecEndCtrl, float vSegmentRadius, float vControlRadius, A3DCOLOR colorSegment, A3DCOLOR colorControl)
{
	m_pA3DDevice	= pA3DDevice;

	m_vecStart		= vecStart;
	m_vecEnd		= vecEnd;
	m_vecStartCtrl	= vecStartCtrl;
	m_vecEndCtrl	= vecEndCtrl;

	m_vSegmentRadius= vSegmentRadius;
	m_vControlRadius= vControlRadius;

	m_colorSegment	= colorSegment;
	m_colorControl	= colorControl;

	// we use a billboard to represent the segment line
	// now fill segments indices at before hand
	//
	//		  TOP View
	//		0--------1
	//		|        |				 Left View
	//		|		 |		   4-------------------5
	//		|		 |		   |				   |
	//		|		 |		   |				   |
	//		|		 |		   |				   |
	//		|		 |		   7-------------------6
	//		|		 |		   
	//	    3--------2
	//
	//
	
	int i;
	for(i=0; i<SUB_DIVIDE; i++)
	{
		m_pIndices[i * 12 + 0] = i * 8;
		m_pIndices[i * 12 + 1] = i * 8 + 1;
		m_pIndices[i * 12 + 2] = i * 8 + 2;

		m_pIndices[i * 12 + 3] = i * 8;
		m_pIndices[i * 12 + 4] = i * 8 + 2;
		m_pIndices[i * 12 + 5] = i * 8 + 3;

		m_pIndices[i * 12 + 6] = i * 8 + 4;
		m_pIndices[i * 12 + 7] = i * 8 + 5;
		m_pIndices[i * 12 + 8] = i * 8 + 6;

		m_pIndices[i * 12 + 9]  = i * 8 + 4;
		m_pIndices[i * 12 + 10] = i * 8 + 6;
		m_pIndices[i * 12 + 11] = i * 8 + 7;
	}

	static WORD aBOXINDICES[] = 
		{0, 1, 3, 3, 1, 2, 
		 6, 2, 1, 6, 1, 5, 
		 7, 3, 2, 7, 2, 6, 
		 4, 0, 3, 4, 3, 7, 
		 4, 7, 6, 4, 6, 5,
		 5, 1, 4, 4, 1, 0};
	for(i=0; i<12 * 3 * 2; i++)
		m_pBoxIndices[i] = (i / 36) * 8 + aBOXINDICES[i % 36];

	return UpdateSegment();
}

bool A3DBezierSegment::SetSegmentColor(A3DCOLOR color)
{
	m_colorSegment = color;

	return UpdateSegment();
}

bool A3DBezierSegment::SetControlColor(A3DCOLOR color)
{
	m_colorControl = color;

	return UpdateSegment();
}

bool A3DBezierSegment::SetSegmentRadius(float vRadius)
{
	m_vSegmentRadius = vRadius;

	return UpdateSegment();
}

bool A3DBezierSegment::SetControlRadius(float vRadius)
{
	m_vControlRadius = vRadius;

	return UpdateSegment();
}

bool A3DBezierSegment::SetStartPos(A3DVECTOR3& vecPos)
{
	m_vecStart		= vecPos;

	return UpdateSegment();
}

bool A3DBezierSegment::SetEndPos(A3DVECTOR3& vecPos)
{
	m_vecEnd		= vecPos;

	return UpdateSegment();
}

bool A3DBezierSegment::SetStartCtrl(A3DVECTOR3& vecPos)
{
	m_vecStartCtrl	= vecPos;

	return UpdateSegment();
}

bool A3DBezierSegment::SetEndCtrl(A3DVECTOR3& vecPos)
{
	m_vecEndCtrl	= vecPos;

	return UpdateSegment();
}

bool A3DBezierSegment::Release()
{
	return true;
}

bool A3DBezierSegment::Render(A3DViewport * pCurrentViewport, bool bShowControl)
{
	//if( m_pA3DDevice->GetD3DDevice() )
	{
		m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);

		m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());
		m_pA3DDevice->SetLightingEnable(false);
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->ClearTexture(0);

		m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
			SUB_DIVIDE * 8, SUB_DIVIDE * 4, m_pIndices, 
			A3DFMT_INDEX16, m_pVertices, sizeof(A3DLVERTEX));

		if( bShowControl )
		{
			m_pA3DDevice->DrawPrimitiveUP(A3DPT_LINELIST, 2, m_pLineVerts, sizeof(A3DLVERTEX));

			m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, 
				2 * 8, 12 * 2, m_pBoxIndices, 
				A3DFMT_INDEX16, m_pBoxVerts, sizeof(A3DLVERTEX));
		}

		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
		m_pA3DDevice->SetLightingEnable(true);
	}

	return true;
}

bool A3DBezierSegment::UpdateSegment()
{
	int		i, s;

	// Now interpolate the segments for rendering;
	A3DVECTOR3	vecPoints[SUB_DIVIDE + 1];
	A3DVECTOR3	vecStart, vecStartCtrl, vecEndCtrl, vecEnd;

	vecStart		= m_vecStart;			// x0
	vecStartCtrl	= m_vecStartCtrl;		// x1
	vecEndCtrl		= m_vecEndCtrl;			// x2
	vecEnd			= m_vecEnd;				// x3

	vecPoints[0]			= vecStart;
	vecPoints[SUB_DIVIDE]   = vecEnd;
	// calucate the coefficients here;
	float a[3], b[3], c[3];
	for(i=0; i<3; i++)
	{
		c[i] = 3 * (vecStartCtrl.m[i] - vecStart.m[i]);			// 3(x1 - x0);
		b[i] = 3 * (vecEndCtrl.m[i] - vecStartCtrl.m[i]) - c[i]; // 3(x2 - x1) - cx
		a[i] = vecEnd.m[i] - vecStart.m[i] - c[i] - b[i];	//x3 - x0 - cx - bx
	}

	for(i=1; i<SUB_DIVIDE; i++)
	{
		float	t = i * 1.0f / SUB_DIVIDE;
		// Use Bezier Interpolate function to calculate the inner points on the curve;
		for(s=0; s<3; s++)
		{
			vecPoints[i].m[s] = a[s] * t * t * t + b[s] * t * t + c[s] * t + vecStart.m[s];
		}
	}
	
	float		vRadius = m_vSegmentRadius;
	A3DAABB	aabb;
	aabb.Clear();

	A3DCOLOR	color;
	color = m_colorSegment;
	
	// Now construct the mesh
	int nVertIndex = 0;
	for(i=0; i<SUB_DIVIDE; i++)
	{
		A3DVECTOR3	vecDir;
		A3DVECTOR3	vecLeft, vecLastLeft;
		A3DVECTOR3	vecUp, vecLastUp;
		float		vDis;
		float		vRadiusStart, vRadiusEnd;
		
		// from vecPoints[i + 1], point to vecPoint[i];
		vecDir = Normalize(vecPoints[i] - vecPoints[i + 1]);
		vDis   = a3d_Magnitude(vecPoints[i] - vecPoints[i + 1]);
		
		if( fabs(vecDir.y) < 0.9f )
		{
			A3DVECTOR3 vecPerpendicular = A3DVECTOR3(0.0f, 1.0f, 0.0f);
			vecLeft = Normalize(CrossProduct(vecDir, vecPerpendicular));
		}
		else
		{
			A3DVECTOR3 vecPerpendicular = A3DVECTOR3(1.0f, 0.0f, 0.0f);
			vecLeft = Normalize(CrossProduct(vecDir, vecPerpendicular));
		}
		vecUp = Normalize(CrossProduct(vecLeft, vecDir));

		A3DOBB		obb;
		obb.Center	= 0.5f * (vecPoints[i] + vecPoints[i + 1]);
		obb.ZAxis	= vecDir;
		obb.XAxis	= -vecLeft;
		obb.YAxis	= vecUp;
		obb.Extents = A3DVECTOR3(vRadius, vRadius, vDis / 2.0f);
		a3d_CompleteOBB(&obb);
		m_obbs[i] = obb;

		a3d_ExpandAABB(aabb.Mins, aabb.Maxs, obb);
		
		if( i == 0 )
		{
			vecLastLeft = vecLeft;
			vecLastUp = vecUp;
		}

		vRadiusStart = vRadius;
		vRadiusEnd = vRadius;

		if( i == SUB_DIVIDE - 1 )
		{
			vRadiusStart *= 2.5f;
			vRadiusEnd *= 0.1f;
		}

		m_pVertices[nVertIndex + 0] = A3DLVERTEX(vecPoints[i] - vecLastLeft * vRadiusStart,
			color, 0xff000000, 0.0f, 0.0f);
		m_pVertices[nVertIndex + 1] = A3DLVERTEX(vecPoints[i] + vecLastLeft * vRadiusStart,
			color, 0xff000000, 0.0f, 0.0f);
		m_pVertices[nVertIndex + 2] = A3DLVERTEX(vecPoints[i + 1] + vecLeft * vRadiusEnd,
			color, 0xff000000, 0.0f, 0.0f);
		m_pVertices[nVertIndex + 3] = A3DLVERTEX(vecPoints[i + 1] - vecLeft * vRadiusEnd,
			color, 0xff000000, 0.0f, 0.0f);

		m_pVertices[nVertIndex + 4] = A3DLVERTEX(vecPoints[i] + vecLastUp * vRadiusStart,
			color, 0xff000000, 0.0f, 0.0f);
		m_pVertices[nVertIndex + 5] = A3DLVERTEX(vecPoints[i + 1] + vecUp * vRadiusEnd,
			color, 0xff000000, 0.0f, 0.0f);
		m_pVertices[nVertIndex + 6] = A3DLVERTEX(vecPoints[i + 1] - vecUp * vRadiusEnd,
			color, 0xff000000, 0.0f, 0.0f);
		m_pVertices[nVertIndex + 7] = A3DLVERTEX(vecPoints[i] - vecLastUp * vRadiusStart,
			color, 0xff000000, 0.0f, 0.0f);

		nVertIndex += 8;

		vecLastLeft = vecLeft;
		vecLastUp = vecUp;
	}

	aabb.CompleteCenterExts();
	m_aabb = aabb;
	
	aabb.Mins = m_vecStartCtrl - A3DVECTOR3(m_vControlRadius);
	aabb.Maxs = aabb.Mins + 2.0f * A3DVECTOR3(m_vControlRadius);
	a3d_CompleteAABB(&aabb);
	m_aabbStartCtrl = aabb;
	m_pBoxVerts[0] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[1] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[2] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[3] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);

	m_pBoxVerts[4] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, -aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[5] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, -aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[6] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, -aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[7] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, -aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);

	aabb.Mins = m_vecEndCtrl - A3DVECTOR3(m_vControlRadius);
	aabb.Maxs = aabb.Mins + 2.0f * A3DVECTOR3(m_vControlRadius);
	a3d_CompleteAABB(&aabb);
	m_aabbEndCtrl = aabb;
	m_pBoxVerts[8] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[9] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[10] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[11] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
																										   
	m_pBoxVerts[12] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, -aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[13] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, -aabb.Extents.y, aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[14] = A3DLVERTEX(aabb.Center + A3DVECTOR3(aabb.Extents.x, -aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);
	m_pBoxVerts[15] = A3DLVERTEX(aabb.Center + A3DVECTOR3(-aabb.Extents.x, -aabb.Extents.y, -aabb.Extents.z), m_colorControl,
		0xff000000, 0.0f, 0.0f);

	m_pLineVerts[0] = A3DLVERTEX(m_vecStart, m_colorControl, 0xff000000, 0.0f, 0.0f);
	m_pLineVerts[1] = A3DLVERTEX(m_vecStartCtrl, m_colorControl, 0xff000000, 0.0f, 0.0f);
	m_pLineVerts[2] = A3DLVERTEX(m_vecEnd, m_colorControl, 0xff000000, 0.0f, 0.0f);
	m_pLineVerts[3] = A3DLVERTEX(m_vecEndCtrl, m_colorControl, 0xff000000, 0.0f, 0.0f);
	return true;
}

bool A3DBezierSegment::PickSegment(A3DVECTOR3& vecStart, A3DVECTOR3& vecDir, A3DVECTOR3 * pPoint, float * pFraction)
{
	bool		bPicked = false;
	float		vMinFraction = 99999.9f;

	float		vFraction;
	A3DVECTOR3	vecPoint, vecNormal;

	A3DAABB  aabb = m_aabb;

	if( CLS_RayToAABB3(vecStart, vecDir, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
	{
		for(int j=0; j<SUB_DIVIDE; j++)
		{
			A3DOBB	obb = m_obbs[j];
			if( CLS_RayToOBB3(vecStart, vecDir, obb, vecPoint, &vFraction, vecNormal) )
			{
				if( vFraction < vMinFraction )
				{
					vMinFraction = vFraction;
					*pPoint = vecPoint;
					*pFraction = vFraction;
					bPicked = true;
				}
			}
		}
	}

	return bPicked;
}

bool A3DBezierSegment::PickControl(A3DVECTOR3& vecStart, A3DVECTOR3& vecDir, bool * pbStart, A3DVECTOR3 * pPoint)
{
	float		vMinFraction = 99999.9f;
	float		vFraction;
	bool		bPicked = false;
	
	A3DVECTOR3	vecPoint, vecNormal;

	A3DAABB	aabb = m_aabbStartCtrl;
	// First check start control point aabb;
	if( CLS_RayToAABB3(vecStart, vecDir, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
	{
		if( vFraction < vMinFraction )
		{
			vMinFraction = vFraction;
			*pPoint = vecPoint;
			*pbStart = true;
			bPicked = true;
		}
	}

	aabb = m_aabbEndCtrl;
	// Then check end control point aabb;
	if( CLS_RayToAABB3(vecStart, vecDir, aabb.Mins, aabb.Maxs, vecPoint, &vFraction, vecNormal) )
	{
		if( vFraction < vMinFraction )
		{
			vMinFraction = vFraction;
			*pPoint = vecPoint;
			*pbStart = false;
			bPicked = true;
		}
	}

	return bPicked;
}

