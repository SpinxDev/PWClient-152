/*
 * FILE: A3DGDI.h
 *
 * DESCRIPTION: An interface of drawing some 3D GDI element, such as triangle, rectange, and etc.
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DGDI_H_
#define _A3DGDI_H_

#include "A3DVertex.h"
#include "A3DControl.h"
#include "A3DGeometry.h"

#define A3DGDI_MAX_FACES		32

class A3DStream;
class A3DDevice;
class A3DTexture;

// The static 
class A3DGDI : public A3DControl
{
private:
	A3DDevice			* m_pA3DDevice;
	A3DStream			* m_pA3DStream;
	A3DStream			* m_pA3DTLStream;

	int					m_nVertCount;
	int					m_nIndexCount;
	A3DTLVERTEX			m_pTLVertexBuffer[A3DGDI_MAX_FACES * 3];
	A3DLVERTEX			m_pVertexBuffer[A3DGDI_MAX_FACES * 3];
	WORD				m_pIndexBuffer[A3DGDI_MAX_FACES * 3];

	A3DTexture			* m_pA3DTexture;

public:
	A3DGDI();
	~A3DGDI();

	bool Init(A3DDevice * pA3DDevice, char * pszTextureMap=NULL);
	bool ChangeTexture(char * pszTextureMap);
	bool Release();

	bool DrawTriangle(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, FLOAT vSize1, FLOAT vSize2, A3DCOLOR color);
	bool Draw2DTriangle(const A3DPOINT2& v0, const A3DPOINT2& v1, const A3DPOINT2& v2, A3DCOLOR color);

	bool DrawRectangle(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp, FLOAT vSize1, FLOAT vSize2, A3DCOLOR color, bool b2Sided=true);
	bool Draw2DRectangle(const A3DRECT& rect, A3DCOLOR color);
	bool Draw2DRectWithEdge(const A3DRECT& rect, A3DCOLOR colorEdge, A3DCOLOR colorRect);
	bool Draw2DZRectWithEdge(const A3DRECT& rect, float z, A3DCOLOR colorEdge, A3DCOLOR colorRect);

	bool Draw2DLine(const A3DPOINT2& ptStart, const A3DPOINT2& ptEnd, A3DCOLOR color);
	bool Draw3DLine(const A3DVECTOR3 &vecStart, const A3DVECTOR3 &vecEnd, A3DCOLOR color);

	bool DrawBox(const A3DAABB& aabb, A3DCOLOR color);
	bool DrawBox(const A3DOBB& obb, A3DCOLOR color);

	bool Draw2DPoint(const A3DPOINT2& vecPos, A3DCOLOR color, FLOAT vSize);
	bool Draw3DPoint(const A3DVECTOR3& vecPos, A3DCOLOR color, FLOAT vSize);
};

typedef A3DGDI * PA3DGDI;

extern A3DGDI * g_pA3DGDI;
#endif//_A3DGDI_H_