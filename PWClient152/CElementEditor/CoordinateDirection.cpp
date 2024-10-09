//----------------------------------------------------------
// Filename	: CoordinateDirection.cpp
// Creator	: QingFeng Xin
// Date		: 2004.8.20
// Desc		: 坐标指示类。该类显示物体坐标信息，同时返回选中的
//			  坐标轴或是坐标平面
//-----------------------------------------------------------
#include "Global.h"
#include "elementeditor.h"
#include "CoordinateDirection.h"
#include "A3D.h"
#include "Render.h"
#include "RenderWnd.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordinateDirection::CCoordinateDirection()
{
	m_nSelectedAxis = 0;
	m_nSelectedTemp = 0;
	m_bLMouseDown = false;
	m_matRotate.Identity();
	m_matTranslate.Identity();;
	m_matABS.Identity();
	m_fDelta = 1.0f;
}

CCoordinateDirection::~CCoordinateDirection()
{

}

void CCoordinateDirection::SetPos(A3DVECTOR3 vPos)
{
	m_matTranslate.Translate(vPos.x,vPos.y,vPos.z);
	m_vPos = vPos;
}

void CCoordinateDirection::Update()
{
	CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
	CRenderWnd *pRWnd = pFrame->GetRenderWnd();
	if(pRWnd->GetActiveViewport() == VIEW_PERSPECTIVE)
	{
		A3DVECTOR3 camPos = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos();
		A3DVECTOR3 mAxisGizmoCenter = m_vPos;
		// assumes a 90deg FOV
		A3DVECTOR3 dir = mAxisGizmoCenter - camPos;
		A3DVIEWPORTPARAM* param = g_Render.GetA3DEngine()->GetActiveViewport()->GetParam();
		
		A3DMATRIX4 matScale;
		float mAxisGizmoProjLen = (60.0f/(float)param->Height) * dir.Magnitude();
		matScale.Identity();
		matScale.Scale(mAxisGizmoProjLen,mAxisGizmoProjLen,mAxisGizmoProjLen);
		m_matABS = matScale*m_matRotate*m_matTranslate;
	}else
	{
		A3DMATRIX4 matScale;
		A3DVECTOR3 vS1,vS2,vPosEx,vScreenPos,vScreenPosEx;
		float scaleX,scaleY,scaleZ,scale;
		
		//求投影比例
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(m_vPos, vScreenPos);
		vScreenPos.z = 0;
		//x
		vPosEx = m_vPos + A3DVECTOR3(1.0f,0,0);
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vPosEx, vScreenPosEx);
		vScreenPosEx.z = 0;
		scaleX =  (vPosEx - m_vPos).Magnitude()/(vScreenPosEx - vScreenPos).Magnitude();
		//y
		vPosEx = m_vPos + A3DVECTOR3(0,1.0f,0);
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vPosEx, vScreenPosEx);
		
		vScreenPosEx.z = 0;
		scaleY =  (vPosEx - m_vPos).Magnitude()/(vScreenPosEx - vScreenPos).Magnitude();
		//z
		vPosEx = m_vPos + A3DVECTOR3(0,0,1.0f);
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vPosEx, vScreenPosEx);
		
		vScreenPosEx.z = 0;
		scaleZ =  (vPosEx - m_vPos).Magnitude()/(vScreenPosEx - vScreenPos).Magnitude();
		
		//去掉变化最大的
		if(scaleX>scaleZ) scale = scaleZ;
		else scale = scaleX;
		
		if(scale>scaleY) scale = scaleY;
		
		//求屏幕60个象素的空间缩放因子
		scale = scale*60;
		
		matScale.Identity();
		matScale.Scale(scale,scale,scale);
		m_matABS = matScale*m_matRotate*m_matTranslate;
	}
}

void CCoordinateDirection::Draw()
{

	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	pWireCollector->Flush();
	//临时关闭Z_buffer
	g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(false);
	DrawArrow();
	DrawAxis();
	DrawPlane();
	
	pWireCollector->Flush();
	g_Render.GetA3DEngine()->GetA3DDevice()->SetZTestEnable(true);
}

void CCoordinateDirection::DrawArrow()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	
	A3DVECTOR3 vExt = A3DVECTOR3(m_fDelta,m_fDelta,m_fDelta);
	float fTemp = 0.05f;
	float fArrow = 0.1f;
	float fPlane = 0.25f;
	A3DVECTOR3 Vertices[6];
	WORD indices[10];

	indices[0] = 0; indices[1] = 1;
	indices[2] = 1; indices[3] = 2;
	indices[4] = 1; indices[5] = 3;
	indices[6] = 1; indices[7] = 4;
	indices[8] = 1; indices[9] = 5;

	//x 轴
	Vertices[0] =  A3DVECTOR3(m_fDelta*0.9f,0,0);
	Vertices[1] =  A3DVECTOR3(vExt.x,0,0);
	Vertices[2] =  A3DVECTOR3(vExt.x - fArrow,0 - fTemp,0 - fTemp);
	Vertices[3] =  A3DVECTOR3(vExt.x - fArrow,0 - fTemp,fTemp);
	Vertices[4] =  A3DVECTOR3(vExt.x - fArrow,fTemp,fTemp);
	Vertices[5] =  A3DVECTOR3(vExt.x - fArrow,fTemp,0 - fTemp);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	Vertices[2] = m_matABS*Vertices[2];
	Vertices[3] = m_matABS*Vertices[3];
	Vertices[4] = m_matABS*Vertices[4];
	Vertices[5] = m_matABS*Vertices[5];
	
	pWireCollector->AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(255,0,0));
	
	
	//y 轴
	Vertices[0] =  A3DVECTOR3(0,m_fDelta*0.9f,0);
	Vertices[1] =  A3DVECTOR3(0,vExt.y,0);
	Vertices[2] =  A3DVECTOR3(0 - fTemp,vExt.y - fArrow,0 - fTemp);
	Vertices[3] =  A3DVECTOR3(0 - fTemp,vExt.y - fArrow,fTemp);
	Vertices[4] =  A3DVECTOR3(fTemp,vExt.y - fArrow,fTemp);
	Vertices[5] =  A3DVECTOR3(fTemp,vExt.y - fArrow,0 - fTemp);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	Vertices[2] = m_matABS*Vertices[2];
	Vertices[3] = m_matABS*Vertices[3];
	Vertices[4] = m_matABS*Vertices[4];
	Vertices[5] = m_matABS*Vertices[5];
	pWireCollector->AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(0,255,0));

	//z 轴
	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*0.9f);
	Vertices[1] =  A3DVECTOR3(0,0,vExt.z);
	Vertices[2] =  A3DVECTOR3(0 - fTemp,0 - fTemp,vExt.z - fArrow);
	Vertices[3] =  A3DVECTOR3(0 - fTemp,fTemp,vExt.z - fArrow);
	Vertices[4] =  A3DVECTOR3(fTemp,fTemp,vExt.z - fArrow);
	Vertices[5] =  A3DVECTOR3(fTemp,0 - fTemp,vExt.z - fArrow);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	Vertices[2] = m_matABS*Vertices[2];
	Vertices[3] = m_matABS*Vertices[3];
	Vertices[4] = m_matABS*Vertices[4];
	Vertices[5] = m_matABS*Vertices[5];
	pWireCollector->AddRenderData_3D(Vertices,6,indices,10,A3DCOLORRGB(0,0,255));
	
}

void CCoordinateDirection::DrawPlane()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	
	float m_fDelta = 1.0f;
	A3DVECTOR3 vExt = A3DVECTOR3(m_fDelta,m_fDelta,m_fDelta);
	float fTemp = 0.1f;
	float fArrow = 0.25f;
	float fPlane = 0.25f;
	A3DVECTOR3 Vertices[2];
	WORD indices[2];

	indices[0] = 0; indices[1] = 1;

	//xy plane
	Vertices[0] =  A3DVECTOR3(m_fDelta*fPlane,0,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,m_fDelta*fPlane,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XY)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));

	Vertices[0] =  A3DVECTOR3(0,m_fDelta*fPlane,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,m_fDelta*fPlane,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==PLANE_XY)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));
	
	//xz plane
	Vertices[0] =  A3DVECTOR3(m_fDelta*fPlane,0,0);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,0,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));
	
	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*fPlane);;
	Vertices[1] =  A3DVECTOR3(m_fDelta*fPlane,0,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
	
	//yz plane
	Vertices[0] =  A3DVECTOR3(0,m_fDelta*fPlane,0);;
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*fPlane,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==PLANE_YZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));

	Vertices[0] =  A3DVECTOR3(0,0,m_fDelta*fPlane);;
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*fPlane,m_fDelta*fPlane);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==PLANE_YZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
}

void CCoordinateDirection::DrawAxis()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	
	A3DVECTOR3 vExt = A3DVECTOR3(m_fDelta,m_fDelta,m_fDelta);
	float fTemp = 0.1f;
	float fArrow = 0.25f;
	float fPlane = 0.25f;
	A3DVECTOR3 Vertices[2];
	A3DVECTOR3 vScreenPos;
	WORD indices[2];

	indices[0] = 0; indices[1] = 1;

	//x 轴
	Vertices[0] =  A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(m_fDelta*0.9f,0,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	if(m_nSelectedAxis==AXIS_X || m_nSelectedAxis==PLANE_XY || m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
		g_Render.TextOut(vScreenPos.x,vScreenPos.y,"X",A3DCOLORRGB(255,255,255));
	
	//y 轴
	Vertices[0] = A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(0,m_fDelta*0.9f,0);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==AXIS_Y || m_nSelectedAxis==PLANE_XY || m_nSelectedAxis==PLANE_YZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
	g_Render.TextOut(vScreenPos.x,vScreenPos.y,"Y",A3DCOLORRGB(255,255,255));
	//z 轴
	Vertices[0] =  A3DVECTOR3(0,0,0);
	Vertices[1] =  A3DVECTOR3(0,0,m_fDelta*0.9f);
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	if(m_nSelectedAxis==AXIS_Z || m_nSelectedAxis==PLANE_YZ || m_nSelectedAxis==PLANE_XZ)
		pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
	else pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));
	g_Render.GetA3DEngine()->GetActiveViewport()->Transform(Vertices[1], vScreenPos);
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(Vertices[1]))
	g_Render.TextOut(vScreenPos.x,vScreenPos.y,"Z",A3DCOLORRGB(255,255,255));
}

int CCoordinateDirection::RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd)
{
	
	float fDis,ftemp;
	fDis = 999999.0f;
	m_nSelectedAxis = AXIS_PLANE_NO;

	if(TraceAxis(A3DVECTOR3(m_fDelta*0.9f,0,0),vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = AXIS_X;
		}
	}
	
	if(TraceAxis(A3DVECTOR3(0,m_fDelta*0.9f,0),vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = AXIS_Y;
		}
	}
	
	if(TraceAxis(A3DVECTOR3(0,0,m_fDelta*0.9f),vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = AXIS_Z;
		}
	}

	A3DVECTOR3 Vertices[4];
	Vertices[0] = m_matABS*A3DVECTOR3(0,0,0);
	Vertices[1] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,0,0);
	Vertices[2] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,m_fDelta*0.25f,0);
	Vertices[3] = m_matABS*A3DVECTOR3(0,m_fDelta*0.25f,0);
	
	if(TracePlane(Vertices,vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = PLANE_XY;
		}
	}
	
	
	Vertices[0] = m_matABS*A3DVECTOR3(0,0,0);
	Vertices[1] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,0,0);
	Vertices[2] = m_matABS*A3DVECTOR3(m_fDelta*0.25f,0,m_fDelta*0.25f);
	Vertices[3] = m_matABS*A3DVECTOR3(0,0,m_fDelta*0.25f);
	
	if(TracePlane(Vertices,vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = PLANE_XZ;
		}
	}
	
	Vertices[0] = m_matABS*A3DVECTOR3(0,0,0);
	Vertices[1] = m_matABS*A3DVECTOR3(0,m_fDelta*0.25f,0);
	Vertices[2] = m_matABS*A3DVECTOR3(0,m_fDelta*0.25f,m_fDelta*0.25f);
	Vertices[3] = m_matABS*A3DVECTOR3(0,0,m_fDelta*0.25f);
	
	if(TracePlane(Vertices,vStart,vEnd,ftemp))
	{
		if(ftemp < fDis) 
		{
			fDis = ftemp;
			m_nSelectedAxis = PLANE_YZ;
		}
	}
	
	return m_nSelectedAxis;
}

bool CCoordinateDirection::TraceAxis(A3DVECTOR3 vAxis,A3DVECTOR3 vStart,A3DVECTOR3 vEnd, float& dis)
{
	A3DVECTOR3 Vertices[2];
	Vertices[0] =  A3DVECTOR3(0,0,0);
	Vertices[1] =  vAxis;
	Vertices[0] = m_matABS*Vertices[0];
	Vertices[1] = m_matABS*Vertices[1];
	
	A3DVECTOR3 vPlaneDir = Vertices[1] - Vertices[0];
	A3DVECTOR3 vPlaneUp  = CrossProduct(vPlaneDir,g_Render.GetA3DEngine()->GetActiveCamera()->GetDir());
	A3DVECTOR3 vPlaneNormal = CrossProduct(vPlaneDir,vPlaneUp);
	vPlaneNormal.Normalize();

	D3DXPLANE plane;
	D3DXVECTOR3 vOut,vV,vN;
	vV = D3DXVECTOR3(Vertices[0].x, Vertices[0].y, Vertices[0].z);
	vN = D3DXVECTOR3(vPlaneNormal.x,vPlaneNormal.y,vPlaneNormal.z);
	D3DXPlaneFromPointNormal(&plane,&vV,&vN);
	
	vV = D3DXVECTOR3(vStart.x, vStart.y, vStart.z);
	vN = D3DXVECTOR3(vEnd.x, vEnd.y, vEnd.z);
	if(NULL!=D3DXPlaneIntersectLine(&vOut,&plane,&vV,&vN))
	{
		A3DVECTOR3 vPlane[4], vIntersection;
		vIntersection = A3DVECTOR3(vOut.x, vOut.y, vOut.z);
		vPlane[0] = Vertices[0] + vPlaneUp*m_fDelta*0.1f;
		vPlane[1] = Vertices[0] - vPlaneUp*m_fDelta*0.1f;
		vPlane[2] = Vertices[1] - vPlaneUp*m_fDelta*0.1f;
		vPlane[3] = Vertices[1] + vPlaneUp*m_fDelta*0.1f;
		
		bool inside = true;
		for(int j = 0; inside && (j < 4); j++)
		{
            int k = (j+1) % 4;
            A3DVECTOR3 vec1 = vPlane[k] - vPlane[j];
            A3DVECTOR3 vec2 = vIntersection - vPlane[k];
			
            if(DotProduct(vec1, vec2) > 0.f)
				inside = false;   
		}
		
		if(inside)
		{
			vIntersection = vIntersection - vStart;
			dis = vIntersection.Magnitude();
			return true;
		}
	}
	return false;
}

bool CCoordinateDirection::TracePlane(A3DVECTOR3* pVertices, A3DVECTOR3 vStart,A3DVECTOR3 vEnd, float& dis)
{
	A3DVECTOR3 vPlaneNormal = CrossProduct(pVertices[1] - pVertices[0],pVertices[3] - pVertices[0]);
	vPlaneNormal.Normalize();

	D3DXPLANE plane;
	D3DXVECTOR3 vOut,vV,vN;
	vV = D3DXVECTOR3(pVertices[0].x, pVertices[0].y, pVertices[0].z);
	vN = D3DXVECTOR3(vPlaneNormal.x,vPlaneNormal.y,vPlaneNormal.z);
	D3DXPlaneFromPointNormal(&plane,&vV,&vN);
	
	vV = D3DXVECTOR3(vStart.x, vStart.y, vStart.z);
	vN = D3DXVECTOR3(vEnd.x, vEnd.y, vEnd.z);
	if(NULL!=D3DXPlaneIntersectLine(&vOut,&plane,&vV,&vN))
	{
		A3DVECTOR3 vIntersection;
		vIntersection = A3DVECTOR3(vOut.x, vOut.y, vOut.z);
		
		bool inside = true;
		for(int j = 0; inside && (j < 4); j++)
		{
            int k = (j+1) % 4;
            A3DVECTOR3 vec1 = pVertices[k] - pVertices[j];
            A3DVECTOR3 vec2 = vIntersection - pVertices[k];
			
            if(DotProduct(vec1, vec2) > 0.f)
				inside = false;   
		}
		
		if(inside)
		{
			vIntersection = vIntersection - vStart;
			dis = vIntersection.Magnitude();
			return true;
		}
	}
	return false;
}



void CCoordinateDirection::ReEdge(A3DVECTOR3 &vMaxEdge,A3DVECTOR3 &vMinEdge)
{
	float temp;
	if(vMaxEdge.x<vMinEdge.x) 
	{
		temp = vMaxEdge.x;
		vMaxEdge.x = vMinEdge.x;
		vMinEdge.x = temp;
	}
	if(vMaxEdge.y<vMinEdge.y) 
	{
		temp = vMaxEdge.y;
		vMaxEdge.y = vMinEdge.y;
		vMinEdge.y = temp;
	}
	if(vMaxEdge.z<vMinEdge.z) 
	{
		temp = vMaxEdge.z;
		vMaxEdge.z = vMinEdge.z;
		vMinEdge.z = temp;
	}
}

// 判断光线与包围盒子是否相交  
// 相交 return true \xqf
bool intersectsBoxWithLine(const A3DVECTOR3& linemiddle, 
						   const A3DVECTOR3& linevect,
						   float halflength,const A3DVECTOR3 &MaxEdge,const A3DVECTOR3 &MinEdge)
{
	const A3DVECTOR3 e = (MaxEdge - MinEdge) * 0.5f;
	const A3DVECTOR3 t = (MinEdge + e) - linemiddle;
	float r;
	
	if ((fabs(t.x) > e.x + halflength * fabs(linevect.x)) || 
		(fabs(t.y) > e.y + halflength * fabs(linevect.y)) ||
		(fabs(t.z) > e.z + halflength * fabs(linevect.z)) )
		return false;
	
	r = e.y * (float)fabs(linevect.z) + e.z * (float)fabs(linevect.y);
	if (fabs(t.y*linevect.z - t.z*linevect.y) > r )
		return false;
	
	r = e.x * (float)fabs(linevect.z) + e.z * (float)fabs(linevect.x);
	if (fabs(t.z*linevect.x - t.x*linevect.z) > r )
		return false;
	
	r = e.x * (float)fabs(linevect.y) + e.y * (float)fabs(linevect.x);
	if (fabs(t.x*linevect.y - t.y*linevect.x) > r)
		return false;
	
	return true;
}

bool CCoordinateDirection::OnMouseMove(int x, int y, DWORD dwFlags)
{
	if(m_bLMouseDown)
	{
		 m_nSelectedAxis = m_nSelectedTemp;
	}else
	{
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		if(RayTrace(vStart,vPos)!=AXIS_PLANE_NO)
		{
			SetCursor(m_hCursor);
		}else return false;
	}
	return true;
}

bool CCoordinateDirection::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
	A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
	A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
	A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
	pViewport->InvTransform(vStart,vStart);
	pViewport->InvTransform(vPos,vPos);
	m_nSelectedTemp = RayTrace(vStart,vPos);
	if(m_nSelectedTemp!=AXIS_PLANE_NO)
		m_bLMouseDown = true;
	else return false;
	return true;
}

bool CCoordinateDirection::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	m_bLMouseDown = false;
	return true;
}



