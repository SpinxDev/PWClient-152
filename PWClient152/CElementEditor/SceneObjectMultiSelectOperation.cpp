// SceneObjectMultiSelectOperation.cpp: implementation of the SceneObjectMultiSelectOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "MainFrm.h"
#include "render.h"
#include "A3D.h"
#include "SceneObject.h"
#include "EditerBezier.h"

#include "SceneObjectMultiSelectOperation.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SceneObjectMultiSelectOperation::SceneObjectMultiSelectOperation()
{
	m_bMouseDown = false;
	m_nDirectFlag = 0;
	m_bDrawRect = true;
	m_pSelectedObject = 0;
	m_bRotate = false;
}

SceneObjectMultiSelectOperation::~SceneObjectMultiSelectOperation()
{

}

void SceneObjectMultiSelectOperation::Render(A3DViewport* pA3DViewport)
{
	//绘制选择区域
	if(m_bMouseDown && m_bDrawRect)
	{
		A3DPOINT2 start = A3DPOINT2(m_rcArea.left,m_rcArea.top);
		A3DPOINT2 end = A3DPOINT2(m_rcArea.right,m_rcArea.bottom);
		
		start = A3DPOINT2(m_rcArea.left,m_rcArea.top);
		end = A3DPOINT2(m_rcArea.left,m_rcArea.bottom);
		g_pA3DGDI->Draw2DLine(start,end,A3DCOLORRGB(0,0,255) );
		start = A3DPOINT2(m_rcArea.left,m_rcArea.bottom);
		end = A3DPOINT2(m_rcArea.right,m_rcArea.bottom);
		g_pA3DGDI->Draw2DLine(start,end,A3DCOLORRGB(0,0,255) );
		
		start = A3DPOINT2(m_rcArea.right,m_rcArea.bottom);
		end = A3DPOINT2(m_rcArea.right,m_rcArea.top);
		g_pA3DGDI->Draw2DLine(start,end,A3DCOLORRGB(0,0,255) );

		start = A3DPOINT2(m_rcArea.right,m_rcArea.top);
		end = A3DPOINT2(m_rcArea.left,m_rcArea.top);
		g_pA3DGDI->Draw2DLine(start,end,A3DCOLORRGB(0,0,255) );
	}
}

bool SceneObjectMultiSelectOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	m_rcArea.right = x;
	m_rcArea.bottom = y;
	bool bHasDirect = false;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL)	return true;
		if(m_nDirectFlag==0)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			if(pManager->RayTrace(vStart,vPos,false))
			{
				SetCursor(g_hSelectCursor);
			}
		}else
		{
			SetCursor(m_hCursor);
		}
		
		A3DMATRIX4 mat;
		mat.Identity();
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			PSCENEOBJECT ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			pManager->m_coordinateDirection.SetSelectedFlags(0);
			pManager->m_coordinateDirection.SetPos(ptemp->GetPos());
			if(m_bRotate)
				pManager->m_coordinateDirection.SetRotate(ptemp->GetRotateMatrix());
			else pManager->m_coordinateDirection.SetRotate(mat);
			pManager->m_coordinateDirection.Update();
			if(pManager->m_coordinateDirection.OnMouseMove(x,y,dwFlags))
			{
				if(pManager->m_coordinateDirection.GetSelectedFlags())
				{
					m_nDirectFlag = pManager->m_coordinateDirection.GetSelectedFlags();
					//m_pSelectedObject = ptemp;
					bHasDirect = true;
				}
			}
			else m_pSelectedObject = NULL;
			
		}
		
		if(!bHasDirect) m_nDirectFlag = 0;
	}
	
	return true;
}

bool SceneObjectMultiSelectOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_rcArea = ARectI(x,y,x,y);
	m_bMouseDown = true;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		A3DMATRIX4 mat;
		mat.Identity();
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			PSCENEOBJECT ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			
			pManager->m_coordinateDirection.SetPos(ptemp->GetPos());
			if(m_bRotate)
				pManager->m_coordinateDirection.SetRotate(ptemp->GetRotateMatrix());
			else pManager->m_coordinateDirection.SetRotate(mat);
			pManager->m_coordinateDirection.Update();
			if(pManager->m_coordinateDirection.OnLButtonDown(x,y,dwFlags))
			{
				m_nDirectFlag = pManager->m_coordinateDirection.GetSelectedFlags();
				if(m_nDirectFlag!=0)
				{
					pManager->m_coordinateDirection.SetCursor(m_hCursor);
					SetCursor(m_hCursor);
					m_bDrawRect = false;
					
					CPoint pt(x,y);
					ScreenTraceAxis(ptemp->GetPos(),pt,&m_vXY,&m_vXZ,&m_vYZ);
					m_vXY = m_vXY - ptemp->GetPos();
					m_vXZ = m_vXZ - ptemp->GetPos();
					m_vYZ = m_vYZ - ptemp->GetPos();
					m_pSelectedObject = ptemp;
					break;
				}else m_vYZ = m_vXZ = m_vXY = A3DVECTOR3(0,0,0);
			}
			
		}
		if( m_bDrawRect==true)
		{
			//如果CTRL按下，就不清除原列表
			SHORT s = GetKeyState(VK_LCONTROL);
			if(s >= 0) pManager->ClearSelectedList();//clear selected list
			pManager->RayTrace(vStart,vPos);
			AUX_GetMainFrame()->EnableBezierMerge(false);
			if(pManager->m_listSelectedObject.GetCount()==1)
			{
				PSCENEOBJECT pObj = pManager->m_listSelectedObject.GetHead();
				A3DVECTOR3 vpos = pObj->GetPos();
				if(!m_bRotate)
				{
					AUX_GetMainFrame()->SetXValue(vpos.x);
					AUX_GetMainFrame()->SetYValue(vpos.y);
					AUX_GetMainFrame()->SetZValue(vpos.z);
				}else
				{
			
					A3DMATRIX4 matr = pObj->GetRotateMatrix();
					float anglex, angley,anglez;
					float data[11] = { matr._11,matr._12,matr._13,matr._14,matr._21,matr._22,matr._23,matr._24,matr._31,matr._32,matr._33}; 
					AUX_ToEulerAnglesXYZ(data,anglex,angley,anglez);
					AUX_GetMainFrame()->SetXValue(anglex * 180/A3D_PI);
					AUX_GetMainFrame()->SetYValue(angley * 180/A3D_PI);
					AUX_GetMainFrame()->SetZValue(anglez * 180/A3D_PI);
				}
				
				pObj->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pObj->m_pProperty);
				AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
				AUX_GetMainFrame()->SetFocus();
				if(pObj->GetObjectType()==CSceneObject::SO_TYPE_BEZIER)
				{
					AUX_GetMainFrame()->EnableBezierOperation(true);
					if(((CEditerBezier*)pObj)->IsBezier())
					{
						AUX_GetMainFrame()->EnableBezierSmooth(true);
						AUX_GetMainFrame()->EnableBezierTest(true);
					}else
					{
						AUX_GetMainFrame()->EnableBezierSmooth(false);
						AUX_GetMainFrame()->EnableBezierTest(false);
					}
				}
			}else
			{
				AUX_GetMainFrame()->EnableBezierOperation(false);
				AUX_GetMainFrame()->EnableBezierSmooth(false);
				AUX_GetMainFrame()->EnableBezierTest(false);
				AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel();
				AUX_GetMainFrame()->SetFocus();
				
				if(pManager->m_listSelectedObject.GetCount()==2)
				{
					PSCENEOBJECT pObja = pManager->m_listSelectedObject.GetHead();
					PSCENEOBJECT pObjb = pManager->m_listSelectedObject.GetTail();
					if(pObja->GetObjectType() == CSceneObject::SO_TYPE_BEZIER &&
					   pObjb->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
					AUX_GetMainFrame()->EnableBezierMerge(true);
				}
			}
		}
		m_nOldX = x;
		m_nOldY = y;
	}
	//m_pSelectedObject = NULL;//其实该对象是在多选中的主要对象
	return false;
}

// 计算屏幕上一点在世界中的连线，与某点开始的座标系的三个平面的交点
// 修改：更正为垂直于射线的一个平面与射线求交，这样就不会存在射线与平面趋于平行的问题(2005-01-03)
void SceneObjectMultiSelectOperation::ScreenTraceAxis(A3DVECTOR3 &pos, CPoint pt, A3DVECTOR3 *pXY, A3DVECTOR3 *pXZ, A3DVECTOR3 *pYZ)
{
	A3DVECTOR3 s_start,s_end,w_start,w_end;
	s_start.x = (float)pt.x;
	s_start.y = (float)pt.y;
	s_start.z = 0.0f;

	s_end.x = (float)pt.x;
	s_end.y = (float)pt.y;
	s_end.z = 1.0f;

	g_Render.GetA3DEngine()->GetActiveViewport()->InvTransform(s_start,w_start);
	g_Render.GetA3DEngine()->GetActiveViewport()->InvTransform(s_end,w_end);
	
	
	D3DXPLANE plane;
	D3DXVECTOR3 iXY;
	D3DXVECTOR3 start(w_start.x, w_start.y, w_start.z), end(w_end.x, w_end.y, w_end.z);
	D3DXVECTOR3 center(pos.x,pos.y,pos.z);
	D3DXVECTOR3 normal((w_end - w_start).x,(w_end - w_start).y,(w_end - w_start).z);
	D3DXPlaneFromPointNormal(&plane,&center,&normal);
	D3DXPlaneIntersectLine(&iXY, &plane, &start, &end);

	if(pXY) *pXY = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pXZ) *pXZ = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pYZ) *pYZ = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	
	/*
	D3DXPLANE xy,xz,yz;
	D3DXVECTOR3 normal[3];
	D3DXVECTOR3 vDelta1,vDelta2;
	float fDotProduct[3];
	{ // 计算XY平面
		D3DXVECTOR3 pt1(pos.x - 10.0f, pos.y - 10.0f, pos.z),
					pt2(pos.x - 10.0f, pos.y, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&xy, &pt1, &pt2, &pt3);
		
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[0],&vDelta1, &vDelta2);
	}

	{ // 计算XZ平面
		D3DXVECTOR3 pt1(pos.x - 10.0f, pos.y, pos.z - 10.0f),
					pt2(pos.x - 10.0f, pos.y, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&xz, &pt1, &pt2, &pt3);
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[1],&vDelta1, &vDelta2);
	}

	{ // 计算YZ平面
		D3DXVECTOR3 pt1(pos.x, pos.y - 10.0f, pos.z - 10.0f),
					pt2(pos.x, pos.y - 10.0f, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&yz, &pt1, &pt2, &pt3);
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[2],&vDelta1, &vDelta2);
	}

	D3DXVECTOR3 vecLine = D3DXVECTOR3(w_end.x,w_end.y,w_end.z) -D3DXVECTOR3(w_start.x,w_start.y,w_start.z);
	for( int i =0; i<3; i++)
		fDotProduct[i] = (float)abs(D3DXVec3Dot(&normal[i],&vecLine)) - 0.01f;

	D3DXVECTOR3 start(w_start.x, w_start.y, w_start.z), end(w_end.x, w_end.y, w_end.z);
	D3DXVECTOR3 iXY(pos.x,pos.y,pos.z),iXZ(pos.x,pos.y,pos.z),iYZ(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iXY, &xy, &start, &end) || fDotProduct[0] < 0.0f)
		iXY = D3DXVECTOR3(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iXZ, &xz, &start, &end) || fDotProduct[1] < 0.0f)
		iXZ = D3DXVECTOR3(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iYZ, &yz, &start, &end) || fDotProduct[2] < 0.0f)
		iYZ = D3DXVECTOR3(pos.x,pos.y,pos.z);
		
	if(pXY) *pXY = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pXZ) *pXZ = A3DVECTOR3(iXZ.x, iXZ.y, iXZ.z);
	if(pYZ) *pYZ = A3DVECTOR3(iYZ.x, iYZ.y, iYZ.z);
	*/
}

bool SceneObjectMultiSelectOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(m_bMouseDown)
		{
			m_bMouseDown = false;
			m_rcArea.right = x;
			m_rcArea.bottom = y;
			if(m_bDrawRect) SelectObjectToList();
			m_nDirectFlag = 0;
		}
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			if(m_bDrawRect)
			{
				PSCENEOBJECT pObj = pManager->m_listSelectedObject.GetHead();
				if(!m_bRotate)
				{
					A3DVECTOR3 vpos = pObj->GetPos();
					AUX_GetMainFrame()->SetXValue(vpos.x);
					AUX_GetMainFrame()->SetYValue(vpos.y);
					AUX_GetMainFrame()->SetZValue(vpos.z);
				}else
				{
					A3DMATRIX4 matr = pObj->GetRotateMatrix();
					float anglex, angley,anglez;
					float data[11] = { matr._11,matr._12,matr._13,matr._14,matr._21,matr._22,matr._23,matr._24,matr._31,matr._32,matr._33}; 
					AUX_ToEulerAnglesXYZ(data,anglex,angley,anglez);
					AUX_GetMainFrame()->SetXValue(anglex * 180/A3D_PI);
					AUX_GetMainFrame()->SetYValue(angley * 180/A3D_PI);
					AUX_GetMainFrame()->SetZValue(anglez * 180/A3D_PI);
				}
				
				pObj->UpdateProperty(false);
				AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pObj->m_pProperty);
				AUX_GetMainFrame()->GetToolTabWnd()->HideObjectProperyPanel(false);
				AUX_GetMainFrame()->SetFocus();
			}
		}
		pManager->m_coordinateDirection.OnLButtonUp(x,y,dwFlags);
	}
	m_bMouseDown = false;
	m_bDrawRect = true;
	m_pSelectedObject = 0;
	return false;
}

void SceneObjectMultiSelectOperation::SelectObjectToList()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		pManager->SelectArea(m_rcArea);
	}
}


