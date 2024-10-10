/*
 * FILE: A3DHorizonCull.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DHORIZONCULL_H_
#define _A3DHORIZONCULL_H_

#include "A3DVector.h"
#include "A3DMatrix.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DViewport;
class A3DCamera;
class A3DAABB;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DHorizonCull
//	
///////////////////////////////////////////////////////////////////////////

class A3DHorizonCull
{
public:		//	Types

	//	Test result
	enum
	{
		TEST_ABOVE,		//	Above the horizon at some point
		TEST_BELOW,		//	Below the horizon at all points
	};

	//	Test quad info
	struct TESTQUAD
	{
		A3DVECTOR3	v1;
		A3DVECTOR3	v2;
		A3DVECTOR3	v3;
		A3DVECTOR3	v4;
	};

	//	Occlusion edge info
	struct OCCEDGES
	{
		int			iNumEdge;
		A3DVECTOR3	v1[8];
		A3DVECTOR3	v2[8];
	};

public:		//	Constructor and Destructor

	A3DHorizonCull();
	virtual ~A3DHorizonCull();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int iViewWid, int iViewHei);
	//	Release resource
	void Release();

	//	Prepare to do occlusion calculation
	bool PrepareToWork(A3DViewport* pViewport);
	//	Determine block visibility
	bool BlockVisibility(const A3DAABB& aabb, const OCCEDGES& OccEdges, const TESTQUAD& TestQuad);

	//	Get width
	int GetWidth() { return m_iWidth; }
	//	Get height
	int GetHeight() { return m_iHeight; }
	//	Get buffer data
	float* GetBufferData() { return m_aHeights; }

	//	Get statistics
	int GetTestLineNum() { return m_iNumTest; }
	int GetInsertLineNum() { return m_iNumInsert; }
	int GetDrawLineNum() { return m_iNumDraw; }

protected:	//	Attributes

	int			m_iWidth;		//	Viewport size
	int			m_iHeight;
	float*		m_aHeights;		//	Heights

	A3DCamera*	m_pHoriCam;		//	Horizonal camera
	A3DCamera*	m_pVertCam;		//	Vertical camera
	bool		m_bUseHoriCam;	//	Use horizonal camera flag
	bool		m_bUseVertCam;	//	Use vertical camera flag

	//	Statistics
	int			m_iNumTest;		//	Number of test line
	int			m_iNumInsert;	//	Number of insert line
	int			m_iNumDraw;		//	Number of DDA drawed point

protected:	//	Operations

	//	Resize
	bool Resize(int iViewWid, int iViewHei);
	//	Reset camera
	bool ResetCamera(A3DCamera* pSrcCam);
	//	Set horizonal camera
	void SetHoriCamera(A3DCamera* pSrcCam, float fNewHalfFOV, float fHalfY, float fHalfX);

	//	Camera transform
	A3DVECTOR4 CameraTransform(const A3DVECTOR3& v, const A3DMATRIX4& mat);
	//	Viewport transform
	void ViewportTransform(A3DVECTOR4& v);
	//	Clip and insert line.
	void ClipAndInsertLine(A3DVECTOR4 a, A3DVECTOR4 b);
	//	Clip and test line.
	bool ClipAndTestLine(A3DVECTOR4 a, A3DVECTOR4 b, bool& bTotalClip);
	//	Clip line with camera's near clip plane
	bool ClipLine(A3DVECTOR4& a, A3DVECTOR4& b);

	//	Test if a line is above or below the horizon
	int TestLine(float x1, float y1, float x2, float y2, bool& bTotalClip);
	//	Insert a line into the horizon
	void InsertLine(float x1, float y1, float x2, float y2);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DHORIZONCULL_H_
