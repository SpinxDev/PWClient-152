/*
 * FILE: A3DHorizonCull.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPI.h"
#include "A3DCamera.h"
#include "A3DViewport.h"
#include "A3DMacros.h"
#include "A3DHorizonCull.h"
#include "ALog.h"
#include "AAssist.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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

//	+ operator
A3DVECTOR4 operator + (const A3DVECTOR4& v1, const A3DVECTOR4& v2) { return A3DVECTOR4(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w); }
//	- operator
A3DVECTOR4 operator - (const A3DVECTOR4& v1, const A3DVECTOR4& v2) { return A3DVECTOR4(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w); }
//	* operator
A3DVECTOR4 operator * (const A3DVECTOR4& v, float f) { return A3DVECTOR4(v.x*f, v.y*f, v.z*f, v.w*f); }
A3DVECTOR4 operator * (float f, const A3DVECTOR4& v) { return A3DVECTOR4(v.x*f, v.y*f, v.z*f, v.w*f); }

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DHorizonCull
//	
///////////////////////////////////////////////////////////////////////////

A3DHorizonCull::A3DHorizonCull()
{
	m_iWidth		= 0;
	m_iHeight		= 0;
	m_aHeights		= NULL;

	m_pHoriCam		= NULL;
	m_pVertCam		= NULL;
	m_bUseHoriCam	= false;
	m_bUseVertCam	= false;

	m_iNumTest		= 0;
	m_iNumInsert	= 0;
	m_iNumDraw		= 0;
}

A3DHorizonCull::~A3DHorizonCull()
{
}

//	Initialize object
bool A3DHorizonCull::Init(int iViewWid, int iViewHei)
{
	if (!Resize(iViewWid, iViewHei))
		return false;

	//	Initialize cameras with default parameters
	m_pHoriCam	= new A3DCamera;
	m_pVertCam	= new A3DCamera;

	m_pHoriCam->Init(NULL);
	m_pVertCam->Init(NULL);

	return true;
}

//	Release resource
void A3DHorizonCull::Release()
{
	if (m_aHeights)
	{
		delete [] m_aHeights;
		m_aHeights = NULL;
	}

	A3DRELEASE(m_pHoriCam);
	A3DRELEASE(m_pVertCam);
}

//	Prepare to do occlusion calculation
bool A3DHorizonCull::PrepareToWork(A3DViewport* pViewport)
{
	//	Resize at first
	const A3DVIEWPORTPARAM& vp = *pViewport->GetParam();
	if (!Resize((int)vp.Width, (int)vp.Height))
		return false;

	//	Reset buffer data
	memset(m_aHeights, 0, m_iWidth * sizeof (float));

	//	Reset camera
	//	Note: Ortho-projection camera or rolling camera couldn't use 
	//	horizon culling algorithm.
	A3DCameraBase* pSrcCam = pViewport->GetCamera();
	float fDotProduct = (float)fabs(DotProduct(pSrcCam->GetRight(), A3D::g_vAxisY));
	if (!pSrcCam || pSrcCam->GetClassID() != A3D_CID_CAMERA || fDotProduct > 0.001f)
		return false;

	if (!ResetCamera((A3DCamera*)pSrcCam))
		return false;

	m_iNumTest		= 0;
	m_iNumInsert	= 0;
	m_iNumDraw		= 0;

	return true;
}

//	Resize
bool A3DHorizonCull::Resize(int iViewWid, int iViewHei)
{
	if (m_iWidth != iViewWid)
	{
		if (m_aHeights)
		{
			delete [] m_aHeights;
			m_aHeights = NULL;
		}

		if (!(m_aHeights = new float [iViewWid]))
		{
			a_LogOutput(1, "A3DHorizonCull::Resize, Not enough memory");
			return false;
		}
	}

	m_iWidth	= iViewWid;
	m_iHeight	= iViewHei;

	return true;
}

//	Set horizonal camera
void A3DHorizonCull::SetHoriCamera(A3DCamera* pSrcCam, float fNewHalfFOV, float fHalfY, 
								float fHalfX)
{
	A3DVECTOR3 v1, v2;
	const A3DVECTOR3& vDir = pSrcCam->GetDir();
	float fFar = pSrcCam->GetZBack();

	if (vDir.y >= 0.0f)
	{
		v1 = vDir + pSrcCam->GetUp() * fHalfY;
		v2 = fFar * (vDir - pSrcCam->GetUp() * fHalfY);
	}
	else
	{
		v1 = vDir - pSrcCam->GetUp() * fHalfY;
		v2 = fFar * (vDir + pSrcCam->GetUp() * fHalfY);
	}

	float fNewHalfX = fHalfX / DotProduct(v1, pSrcCam->GetDirH());
	float fNewRatio = fNewHalfX / (float)tan(fNewHalfFOV);

	//	Calculate the new z far distance. z near usually is very small, 
	//	so we ignore it's change
	float fNewFar = DotProduct(v2, pSrcCam->GetDirH());

	m_pHoriCam->SetProjectionParam(fNewHalfFOV * 2.0f, pSrcCam->GetZFront(), fNewFar, fNewRatio);
	m_pHoriCam->SetPos(pSrcCam->GetPos());
	m_pHoriCam->SetDirAndUp(pSrcCam->GetDirH(), A3D::g_vAxisY);
}

//	Reset camera
bool A3DHorizonCull::ResetCamera(A3DCamera* pSrcCam)
{
	float fHalfFOV = pSrcCam->GetFOV() * 0.5f;
	float fHalfY = (float)tan(fHalfFOV);
	float fHalfX = fHalfY * pSrcCam->GetRatio();

	const A3DVECTOR3& vDir = pSrcCam->GetDir();
	const A3DVECTOR3& vEyePos = pSrcCam->GetPos();
	float fRadian;	//	Radian between dir and y axis

	if (vDir.y >= 0.0f)
		fRadian = (float)acos(DotProduct(pSrcCam->GetDir(), A3D::g_vAxisY));
	else
		fRadian = (float)acos(DotProduct(pSrcCam->GetDir(), -A3D::g_vAxisY));
	
	float fNear = pSrcCam->GetZFront();
	float fFar = pSrcCam->GetZBack();
	const float fPIDiv2 = A3D_PI * 0.5f;
	const float fError = 0.1745f;	//	0.1745 radian is about 20 degree

	if (fRadian >= fHalfFOV + fError)
	{
		m_bUseHoriCam = true;
		m_bUseVertCam = false;

		float fNewHalfFOV = fPIDiv2 - (fRadian - fHalfFOV);
		SetHoriCamera(pSrcCam, fNewHalfFOV, fHalfY, fHalfX);
	}
	else if (fRadian >= fHalfFOV)
	{
		m_bUseHoriCam = true;
		m_bUseVertCam = true;

		//	Horizonal camera
		float fNewHalfFOV = fPIDiv2 - fError;
		float d = (float)tan(fRadian - fError);
		SetHoriCamera(pSrcCam, fNewHalfFOV, d, fHalfX);

		//	Vertical camera
		float fNewRatio = fHalfX / (float)tan(fError);
		m_pVertCam->SetProjectionParam(fError * 2.0f, fNear, fFar, fNewRatio);
		m_pVertCam->SetPos(pSrcCam->GetPos());

		if (vDir.y >= 0.0f)
			m_pVertCam->SetDirAndUp(A3D::g_vAxisY, -pSrcCam->GetDirH());
		else
			m_pVertCam->SetDirAndUp(-A3D::g_vAxisY, -pSrcCam->GetDirH());
	}
	else if (fRadian < 0.052f)	//	0.052 radian is about 3 degree
	{
		m_bUseHoriCam = false;
		m_bUseVertCam = true;

		m_pVertCam->SetProjectionParam(pSrcCam->GetFOV(), fNear, fFar, pSrcCam->GetRatio());
		m_pVertCam->SetPos(pSrcCam->GetPos());
		m_pVertCam->SetDirAndUp(pSrcCam->GetDir(), pSrcCam->GetUp());
	}
	else
	{
		m_bUseHoriCam = true;
		m_bUseVertCam = true;

		//	Vertical camera
		float fNewHalfFOV = a_Max(fRadian, fHalfFOV - fRadian, fError);
		float fNewRatio = fHalfX / (float)tan(fNewHalfFOV);
		m_pVertCam->SetProjectionParam(fNewHalfFOV * 2.0f, fNear, fFar, fNewRatio);
		m_pVertCam->SetPos(pSrcCam->GetPos());

		if (vDir.y >= 0.0f)
			m_pVertCam->SetDirAndUp(A3D::g_vAxisY, -pSrcCam->GetDirH());
		else
			m_pVertCam->SetDirAndUp(-A3D::g_vAxisY, -pSrcCam->GetDirH());

		//	Horizonal camera
		float d = (float)tan(fRadian - fNewHalfFOV);
		fNewHalfFOV = fPIDiv2 - fNewHalfFOV;
		SetHoriCamera(pSrcCam, fNewHalfFOV, d, fHalfX);
	}

	return true;
}

//	Determine block visibility
bool A3DHorizonCull::BlockVisibility(const A3DAABB& aabb, const OCCEDGES& OccEdges, 
								const TESTQUAD& TestQuad)
{
	bool bInsert = false;

	//	Use vertical camera to check at first
	if (m_bUseVertCam)
		bInsert = m_pVertCam->AABBInViewFrustum(aabb.Mins, aabb.Maxs);
	
	if (!m_bUseHoriCam)
		return bInsert;

	int i;
	const A3DMATRIX4& matVP = m_pHoriCam->GetVPTM();

	if (!bInsert)
	{
		A3DVECTOR4 a = CameraTransform(TestQuad.v1, matVP);
		A3DVECTOR4 b = CameraTransform(TestQuad.v2, matVP);
		A3DVECTOR4 c = CameraTransform(TestQuad.v3, matVP);
		A3DVECTOR4 d = CameraTransform(TestQuad.v4, matVP);

		A3DVECTOR4* aEdges[8] = 
		{
			&a, &b, &b, &c, &c, &d, &d, &a, 
		};

		//	ClipAndTestLine 使用水平线对 block 进行剔除判定，大部分情况下是可靠的。
		//	但有一个特殊情况，就是当摄像机在 block 中或者离得很近的时候，block 的
		//	TestQuad 的四条测试边有可能都没有在屏幕内，这种情况需要特殊处理。
		//	经过观察发现这四条不在屏幕内的测试边经过 CameraTransform 和
		//	ViewportTransform （注意其中的 y）变换以后得到的
		//	顶点坐标 (x1, y1) - (x2, y2) 只可能出现三种情况:
		//		1: x1 < 0 && x2 < 0
		//		2: x1 >= m_iWidth && x2 >= m_iWidth
		//		3: 使用 x=0 和 x=m_iWidth-1 裁剪后得到的 y1' <= 0.0f && y2' <= 0.0f
		//
		//	bTotalClip 标志就是用来表示这个特殊情况的，如果 block 的所有测试边都完
		//	全被 clip 掉，也就是说出现上面说的三种情况。那么这个 block 不能随便丢弃。

		bool bTotalClip=true;

		for (i=0; i < 8; i+=2)
		{
			bool bClip;
			bool bBelow = ClipAndTestLine(*aEdges[i], *aEdges[i+1], bClip);
			bTotalClip &= bClip;

			m_iNumTest++;

			if (!bBelow)
				break;		//	Break if one test edge is visible
		}

		if (i >= 8)	//	All test edges are below horizon line
		{
			//	Special case, if bTotalClip == true means the block may 
			//	project huge size on screen, so don't return, go continue
			if (!bTotalClip)
				return false;
		}
	}

	//	Block is visible, insert it's occlusion edges
	for (i=0; i < OccEdges.iNumEdge; i++)
	{
		A3DVECTOR4 a = CameraTransform(OccEdges.v1[i], matVP);
		A3DVECTOR4 b = CameraTransform(OccEdges.v2[i], matVP);
		ClipAndInsertLine(a, b);

		m_iNumInsert++;
	}

	return true;	
}

//	Camera transform
A3DVECTOR4 A3DHorizonCull::CameraTransform(const A3DVECTOR3& v, const A3DMATRIX4& mat)
{
	return A3DVECTOR4(v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43,
					  v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + mat._44);
}

//	Viewport transform
void A3DHorizonCull::ViewportTransform(A3DVECTOR4& v)
{
	const float oow = 1.0f / v.w;

	v.x *= oow;
	v.y *= oow;

	v.x = (v.x + 1.0f) * m_iWidth * 0.5f;
	//	Note: the y in horizon culling is inverse with screen coordinates
	v.y = (v.y + 1.0f) * m_iHeight * 0.5f;
}

//	Clip and insert line.
void A3DHorizonCull::ClipAndInsertLine(A3DVECTOR4 a, A3DVECTOR4 b)
{
	if (ClipLine(a, b))
	{
		ViewportTransform(a);
		ViewportTransform(b);
		InsertLine(a.x, a.y, b.x, b.y);
	}
}

//	Clip and test line.
//	Returns true if the entire line is below the horizon or outside frustum, false if any part is visible.
bool A3DHorizonCull::ClipAndTestLine(A3DVECTOR4 a, A3DVECTOR4 b, bool& bTotalClip)
{
	if (ClipLine(a, b))
	{
		ViewportTransform(a);
		ViewportTransform(b);
		return TestLine(a.x, a.y, b.x, b.y, bTotalClip) == TEST_BELOW;
	}

	bTotalClip = true;

    return true;
}

//	Clip line with camera's near clip plane
bool A3DHorizonCull::ClipLine(A3DVECTOR4& a, A3DVECTOR4& b)
{
	bool bNearPlaneClip1 = a.z <= 0.0f ? true : false;
	bool bNearPlaneClip2 = b.z <= 0.0f ? true : false;

	if (!bNearPlaneClip1 && !bNearPlaneClip2)
	{
		//	Trivially accept
		return true;
	}
	else if (bNearPlaneClip1 && bNearPlaneClip2)
	{
		//	Trivially reject
		return false;
	}

	if (bNearPlaneClip1)
	{
		A3DVECTOR4 d = b - a;
		float t = a.z / (a.z - b.z);
		a = a + d * t;
	}
	else if (bNearPlaneClip2)
	{
		A3DVECTOR4 d = a - b;
		float t = b.z / (b.z - a.z);
		b = b + d * t;
	}

    return true;
}

//	Test if a line is above or below the horizon
int A3DHorizonCull::TestLine(float x1, float y1, float x2, float y2, bool& bTotalClip)
{
	if (x1 > x2)
	{
		a_Swap(x1, x2);
		a_Swap(y1, y2);
	}

	bTotalClip = false;

	if (x2 < 0.0f || x1 > m_iWidth-1.0f)
	{
		bTotalClip = true;
		return TEST_BELOW;
	}

	if (y1 <= 0.0f && y2 <= 0.0f)
	{
		bTotalClip = true;
		return TEST_BELOW;
	}

	float dx = x2 - x1;
	float dy = y2 - y1;

	float dydx = dy / dx;

	if (x1 < 0.0f)
	{
		//	Clip left
		y1 += -x1 * dydx;
		x1 = 0.0f;
	}

	float right = m_iWidth - 1.0f;

	if (x2 > right)
	{
		//	Clip right
		y2 -= (x2 - right) * dydx;
		x2 = right;
	}

	if (y1 <= 0.0f && y2 <= 0.0f)
	{
		bTotalClip = true;
		return TEST_BELOW;
	}

	int ix1 = (int)ceil(x1 - 0.5f);
	int ix2 = (int)(x2 - 0.5f);
	float y = y1;

	for (int x=ix1; x <= ix2; x++)
	{
		ASSERT(x >= 0);
		ASSERT(x < m_iWidth);

		if (m_aHeights[x] < y)
		{
			m_iNumDraw += x - ix1;
			return TEST_ABOVE;
		}

		y += dydx;
	}

	m_iNumDraw += ix2 - ix1;

	return TEST_BELOW;
}

//	Insert a line into the horizon
void A3DHorizonCull::InsertLine(float x1, float y1, float x2, float y2)
{
	if (x1 > x2)
	{
		a_Swap(x1, x2);
		a_Swap(y1, y2);
	}

	if (x2 < 0.0f || x1 >= m_iWidth-1.0f)
		return;

	float dx = x2 - x1;
	float dy = y2 - y1;

	float dydx = dy / dx;

	if (x1 < 0.0f)
	{
		//	Clip left
		y1 += -x1 * dydx;
		x1 = 0.0f;
	}

	float right = m_iWidth - 1.0f;

	if (x2 > right)
	{
		//	Clip right
		y2 -= (x2 - right) * dydx;
		x2 = right;
	}

	int ix1 = (int)ceil(x1 - 0.5f);
	int ix2 = (int)(x2 - 0.5f);
	float y = y1;

	for (int x=ix1; x <= ix2; x++)
	{
		ASSERT(x >= 0);
		ASSERT(x < m_iWidth);

		if (m_aHeights[x] < y)
			m_aHeights[x] = y;

		y += dydx;
	}

	m_iNumDraw += ix2 - ix1;
}

