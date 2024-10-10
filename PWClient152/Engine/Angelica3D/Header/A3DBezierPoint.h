/*
 * FILE: A3DBezierPoint.h
 *
 * DESCRIPTION: class for bezier point
 *
 * CREATED BY: Hedi, 2002/9/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DBEZIERPOINT_H_
#define _A3DBEZIERPOINT_H_

#include "A3DVertex.h"
#include "A3DGeometry.h"

class A3DDevice;
class A3DEngine;
class A3DViewport;

class A3DBezierPoint
{
private:
	A3DDevice *			m_pA3DDevice;
	float				m_vRadius;				// radius
	A3DVECTOR3			m_vecPos;				// position of the point
	A3DAABB				m_aabb;					// bounding box for ray trace
	A3DCOLOR			m_color;				// color of the point

	A3DLVERTEX 			m_pVertices[4];			// using for render
	WORD				m_pIndices[12];			

	bool UpdatePoint();

public:
	A3DBezierPoint();
	~A3DBezierPoint();

	bool Init(A3DDevice * pA3DDevice, const A3DVECTOR3& vecPos, A3DCOLOR color=0xffff0000, float vRadius=2.0f);
	bool Release();

	bool SetPosition(const A3DVECTOR3& vecPos);
	bool SetColor(A3DCOLOR color);
	bool SetRadius(float vRadius);

	bool Render(A3DViewport * pCurrentViewport);
	bool Pick(A3DVECTOR3& vecStart, A3DVECTOR3& vecDir, A3DVECTOR3 * pPoint, float * pFraction);
};

#endif//_A3DBEZIERPOINT_H_