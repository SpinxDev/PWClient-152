/*
 * FILE: A3DBezierSegment.h
 *
 * DESCRIPTION: class for bezier segment
 *
 * CREATED BY: Hedi, 2002/9/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DBEZIERSEGMENT_H_
#define _A3DBEZIERSEGMENT_H_

#include "A3DVertex.h"
#include "A3DGeometry.h"

#define SUB_DIVIDE		5

class A3DDevice;
class A3DViewport;

class A3DBezierSegment
{
private:
	A3DDevice *			m_pA3DDevice;
	
	A3DVECTOR3			m_vecStart;		
	A3DVECTOR3			m_vecEnd;		
	A3DVECTOR3			m_vecStartCtrl;
	A3DVECTOR3			m_vecEndCtrl;

	float				m_vControlRadius;
	float				m_vSegmentRadius;

	A3DCOLOR			m_colorSegment;
	A3DCOLOR			m_colorControl;

	A3DAABB				m_aabb;							// bounding box for ray trace
	A3DOBB				m_obbs[SUB_DIVIDE];				// bb for each sub segment

	A3DAABB				m_aabbStartCtrl;
	A3DAABB				m_aabbEndCtrl;
	
	A3DLVERTEX 			m_pVertices[8 * SUB_DIVIDE];	// using for render
	WORD				m_pIndices[12 * SUB_DIVIDE];			

	A3DLVERTEX			m_pBoxVerts[8 * 2];
	WORD				m_pBoxIndices[12 * 3 * 2];

	A3DLVERTEX			m_pLineVerts[2 * 2];
	
	bool UpdateSegment();

public:
	A3DBezierSegment();
	virtual ~A3DBezierSegment();

	bool Init(A3DDevice * pA3DDevice, A3DVECTOR3& vecStart, A3DVECTOR3& vecEnd, 
		A3DVECTOR3& vecStartCtrl, A3DVECTOR3& vecEndCtrl, float vSegmentRadius=1.0f, float vControlRadius=2.0f,
		A3DCOLOR colorSegment=0xffffffff, A3DCOLOR colorControl=0xffffffff);
	bool Release();

	bool SetStartPos(A3DVECTOR3& vecPos);
	bool SetEndPos(A3DVECTOR3& vecPos);
	bool SetStartCtrl(A3DVECTOR3& vecPos);
	bool SetEndCtrl(A3DVECTOR3& vecPos);

	bool SetSegmentRadius(float vRadius);
	bool SetControlRadius(float vRadius);

	bool SetSegmentColor(A3DCOLOR color);
	bool SetControlColor(A3DCOLOR color);

	bool Render(A3DViewport * pCurrentViewport, bool bShowControl);

	// return true if picked, otherwise return false
	bool PickSegment(A3DVECTOR3& vecStart, A3DVECTOR3& vecDir, A3DVECTOR3 * pPoint, float * pFraction);
	bool PickControl(A3DVECTOR3& vecStart, A3DVECTOR3& vecDir, bool * pbStart, A3DVECTOR3 * pPoint);
};

#endif//_A3DBEZIERSEGMENT_H_