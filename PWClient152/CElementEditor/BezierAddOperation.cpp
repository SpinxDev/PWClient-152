// BezierAddOperation.cpp: implementation of the CBezierAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "BezierAddOperation.h"
#include "SceneObjectManager.h"
#include "SceneLightObject.h"
#include "Render.h"
#include "a3d.h"
#include "BezierUndoAction.h"
#include "UndoMan.h"
#include "UndoLightAddAction.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBezierAddOperation::CBezierAddOperation()
{
	m_nPointID = 0;
	m_nBezierID = 0;
	m_pBezier = NULL; 
}

CBezierAddOperation::~CBezierAddOperation()
{
	m_tempBezier.Release();
}

bool CBezierAddOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		pMap->SetModifiedFlag(true);
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		LIGHTDATA lightData;
		
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		if(pSManager->GetTracePos(vStart,vPos,TraceRt.vPoint))
		{	
			if(m_pBezier==NULL)
			{
				
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(true);//读属性数据
				m_tempBezier.UpdateProperty(true);
				m_pBezier = pSManager->CreateBezier(m_tempBezier.GetObjectName());
				if(m_pBezier)
				{
					DWORD dwColor;
					m_tempBezier.GetProperty(dwColor);
					m_pBezier->SetProperty(dwColor);
					m_pBezier->SetBezier(m_tempBezier.IsBezier());
					m_pBezier->SetObjectID(pSManager->GenerateObjectID());
					m_pBezier->CreatePoint(PointAlign(TraceRt.vPoint),m_nBezierID,m_nPointID++);
					AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_pBezier->m_pProperty);//把灯光的属性给对象属性表
					m_pBezier->UpdateProperty(false);
					AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
					AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
					//for undo
					CUndoSceneObjectAddAction *pUndo = new CUndoSceneObjectAddAction(pMap->GetSceneObjectMan());
					pUndo->GetSeletedListData();
					g_UndoMan.AddUndoAction(pUndo);
					ReleaseCapture();
				}else g_Log.Log("CBezierAddOperation::OnLButtonDown, Failed to create bezier");
			}else
			{
				CEditerBezierPoint *pNewPt = m_pBezier->CreatePoint(PointAlign(TraceRt.vPoint),m_nBezierID,m_nPointID++);
				if(m_pBezier->IsBezier()) m_pBezier->OptimizeSmooth();
				m_pBezier->ReCalculateLength();
				if(m_pBezier->m_list_points.GetCount()>1)
					AUX_GetMainFrame()->EnableBezierOperation(true);
			}
		}
		
	}
	return true;
}

bool CBezierAddOperation::OnRButtonUp( int x, int y, DWORD dwFlags)
{
	return true;
}

bool CBezierAddOperation::OnMouseMove(int x,int y,DWORD dwFlags)
{
	SetCursor(g_hAddObjectCursor);
	return true;
}

void CBezierAddOperation::CreateBezier()
{
	InitBezierCreateState();
	int m_nBezierID = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Bezier_%d",m_nBezierID);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				m_nBezierID++;
			}else break; 
			
		}
		DWORD color;
		int r = (int)(255*(float)rand()/(float)RAND_MAX);
		int g = (int)(255*(float)rand()/(float)RAND_MAX);
		int b = (int)(255*(float)rand()/(float)RAND_MAX);
		color = A3DCOLORRGB(r,g,b);
		m_tempBezier.SetProperty(color);
		m_tempBezier.SetObjectName(name);
		m_tempBezier.UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(m_tempBezier.m_pProperty);//把灯光的属性给对象属性表
	}
}

A3DVECTOR3 CBezierAddOperation::PointAlign(A3DVECTOR3 v)
{
	if(!g_bBezierPtSnap) return v;
	float grip = 5.0f;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain *pTerrain = pMap->GetTerrain();
		ARectF rc = pTerrain->GetTerrainArea();
		
		if(fabs(rc.left - v.x) < grip)
			v.x = rc.left;
		if(fabs(rc.right - v.x) < grip)
			v.x = rc.right;
		if(fabs(rc.top - v.z) < grip)
			v.z = rc.top;
		if(fabs(rc.bottom - v.z) < grip)
			v.z = rc.bottom;
	}
	
	float mindis = 99999.0f;
	A3DVECTOR3 vs;	
	APtrList<CSceneObject *>* bList = pMap->GetSceneObjectMan()->GetSortObjectList(CSceneObject::SO_TYPE_BEZIER);
	ALISTPOSITION pos = bList->GetHeadPosition();
	while(pos)
	{
		CEditerBezier* pBezier = (CEditerBezier*)bList->GetNext(pos);																																																										
		if(pBezier->IsDeleted() && m_pBezier == pBezier) continue;
		POSITION pos2 = pBezier->m_list_points.GetHeadPosition();
		while( pos2 )
		{
 			CEditerBezierPoint *pt = (CEditerBezierPoint *)pBezier->m_list_points.GetNext(pos2);
			A3DVECTOR3 vPos = pt->GetPos();
			float dis = (v - vPos).Magnitude();
			if(dis < mindis)
			{
				mindis = dis;
				vs = vPos;
			}
		}
	}

	if(mindis < grip) v = vs;
	
	return v;
}

void CBezierAddOperation::InitBezierCreateState()
{
	m_nBezierID++;
	m_nPointID = 0;
	m_pBezier = NULL;
}


CBezierDragOperation::CBezierDragOperation()
{
	m_bHasHit = false;
	m_nDirectFlag = 0;
	m_pUndo = NULL;
	m_vDelta = A3DVECTOR3(0.0f);
}

CBezierDragOperation::~CBezierDragOperation()
{

}

bool CBezierDragOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_nOldX = x;
	m_nOldY = y;
	m_bMouseDown = true;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pObj = pSManager->m_listSelectedObject.GetTail();
			if(pObj)
			{
				if(pObj->GetObjectType()==CSceneObject::SO_TYPE_BEZIER)
				{
					if(m_bHasHit)
					{
						CEditerBezierSegment *pSegHead = NULL,*pSegTail = NULL;
						CEditerBezierPoint *pt = NULL;
						bool bStart = false;
						
						int nFlag = 0;
						A3DVECTOR3 vDirPos;
						
						//先看是有有显示的坐标可以被点中
						nFlag = ((CEditerBezier*)pObj)->GetHitFlag();
						switch(nFlag)
						{
						case 0:
							m_bHasHit = false;
							break;
						case 1://命中点
							pt = ((CEditerBezier*)pObj)->GetCurPoint(&pSegHead,&pSegTail);
							if(pt)
							{
								vDirPos = pt->GetPos();
							}
							break;
						case 2://命中段
							pSegHead = ((CEditerBezier*)pObj)->GetCurSegment(vDirPos);
							break;
						case 3://命中控制点
							pSegHead = ((CEditerBezier*)pObj)->GetCurCtrl(bStart);
							if(pSegHead)
							{
								if(bStart) vDirPos = pSegHead->GetAnchor1();
								else vDirPos = pSegHead->GetAnchor2();
								
							}
							break;
						}
						A3DMATRIX4 matR; matR.Identity();
						pSManager->m_coordinateDirection.SetSelectedFlags(0);
						pSManager->m_coordinateDirection.SetRotate(matR);
						pSManager->m_coordinateDirection.SetPos(vDirPos);
						pSManager->m_coordinateDirection.Update();
						pSManager->m_coordinateDirection.OnLButtonDown(x,y,dwFlags);
						m_nDirectFlag = pSManager->m_coordinateDirection.GetSelectedFlags();
						if(m_nDirectFlag==0) m_bHasHit = false;
						else
						{
							m_pUndo = new CBezierUndoAction();
							m_pUndo->StartRecord();
							return true;
						}
						
					}
					A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
					A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
					A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
					A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
					pViewport->InvTransform(vStart,vStart);
					pViewport->InvTransform(vPos,vPos);
					int nFlag;
					m_bHasHit = ((CEditerBezier*)pObj)->PickControl(vStart, vPos,nFlag);
				}
			}
		}
	}
	return true;
}

bool CBezierDragOperation::OnLButtonUp(int x,int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(m_vDelta!=A3DVECTOR3(0.0f))
		{	
			if(m_pUndo)
			{
				g_UndoMan.AddUndoAction(m_pUndo);
				m_pUndo = NULL;
			}
		}else
		{
			if(m_pUndo)
			{
				m_pUndo->Release();
				delete m_pUndo;
				m_pUndo = NULL;
			}
		}
		pSManager->m_coordinateDirection.OnLButtonUp(x,y,dwFlags);
	}
	m_bMouseDown = false;
	return true;
}

bool CBezierDragOperation::OnMouseMove(int x,int y,DWORD dwFlags)
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		if(pSManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pObj = pSManager->m_listSelectedObject.GetTail();
			if(m_bMouseDown && m_nDirectFlag!=0)
			{
				SetCursor(g_hMoveCursor);
				CEditerBezierSegment *pSeg = NULL;
				CEditerBezierSegment *pSegHead = NULL,*pSegTail = NULL;
				CEditerBezierPoint *pPoint = NULL;
				A3DVECTOR3 vTarget;
				bool bStartCtrl;
				int nFlag = ((CEditerBezier*)pObj)->GetHitFlag();
				switch(nFlag) 
				{
				case 1:
					pPoint = ((CEditerBezier*)pObj)->GetCurPoint(&pSegTail,&pSegHead);
					if(pPoint)
					{//对点的处理
						A3DVECTOR3 vBezierTrans = GetDeltaVector(pPoint->GetPos(),x,y);
						A3DVECTOR3 vPos = pPoint->GetPos();
						vPos = vPos + vBezierTrans;
						if(!((CEditerBezier*)pObj)->IsBezier())
						{	
							vPos.y = pMap->GetTerrain()->GetPosHeight(vPos) + 0.1f;
						}
						pPoint->SetPos(vPos);
						m_vDelta +=vBezierTrans;
						if(pSegTail)
						{
							if(((CEditerBezier*)pObj)->IsBezier())
							{	
								vPos = pSegTail->GetAnchor2();
								vPos = vPos + vBezierTrans;
							}
							A3DVECTOR3 a2 = pSegTail->GetAnchor2();
							A3DVECTOR3 tail = pSegTail->GetTail();
							
							pSegTail->SetAnchor2(vPos);
							pSegTail->SetTail(pPoint->GetPos());
						}
						if(pSegHead)
						{
							if(((CEditerBezier*)pObj)->IsBezier())
							{
								vPos = pSegHead->GetAnchor1();
								vPos = vPos + vBezierTrans;
							}
							
							A3DVECTOR3 a1 = pSegHead->GetAnchor1();
							A3DVECTOR3 head = pSegHead->GetHead();
							pSegHead->SetAnchor1(vPos);
							pSegHead->SetHead(pPoint->GetPos());
						}
					}
					
					break;
				case 2:
					pSeg = ((CEditerBezier*)pObj)->GetCurSegment(vTarget);
					if(pSeg && ((CEditerBezier*)pObj)->IsBezier())
					{
						A3DVECTOR3 vBezierTrans = GetDeltaVector(vTarget,x,y);
						A3DVECTOR3 v1 = pSeg->GetAnchor1();
						A3DVECTOR3 v2 = pSeg->GetAnchor2();
						m_vDelta +=vBezierTrans;
						v1 = v1 + vBezierTrans;
						v2 = v2 + vBezierTrans;
						pSeg->SetAnchor1(v1);
						pSeg->SetAnchor2(v2);
					}
					break;
				case 3:
					pSeg = ((CEditerBezier*)pObj)->GetCurCtrl(bStartCtrl);
					if(pSeg && ((CEditerBezier*)pObj)->IsBezier())
					{
						if(bStartCtrl)
						{
							A3DVECTOR3 v1 = pSeg->GetAnchor1();
							A3DVECTOR3 vBezierTrans = GetDeltaVector(v1,x,y);
							v1 = v1 + vBezierTrans;
							m_vDelta +=vBezierTrans;
							pSeg->SetAnchor1(v1);
						}else
						{
							A3DVECTOR3 v2 = pSeg->GetAnchor2();
							A3DVECTOR3 vBezierTrans = GetDeltaVector(v2,x,y);
							v2 = v2 + vBezierTrans;
							m_vDelta +=vBezierTrans;
							pSeg->SetAnchor2(v2);
						}
					}
					break;
				}
			}else
			{
				CEditerBezierSegment *pSegHead = NULL,*pSegTail = NULL;
				CEditerBezierPoint *pt = NULL;
				bool bStart = false;
				
				int nFlag = 0;
				A3DVECTOR3 vDirPos;
				
				//先看是否有显示的坐标可以被点中
				nFlag = ((CEditerBezier*)pObj)->GetHitFlag();
				switch(nFlag)
				{
				case 1://命中点
					pt = ((CEditerBezier*)pObj)->GetCurPoint(&pSegHead,&pSegTail);
					if(pt)
					{
						vDirPos = pt->GetPos();
					}
					break;
				case 2://命中段
					pSegHead = ((CEditerBezier*)pObj)->GetCurSegment(vDirPos);
					
					break;
				case 3://命中控制点
					pSegHead = ((CEditerBezier*)pObj)->GetCurCtrl(bStart);
					if(pSegHead)
					{
						if(bStart) vDirPos = pSegHead->GetAnchor1();
						else vDirPos = pSegHead->GetAnchor2();
						
					}
					break;
				}
				A3DMATRIX4 mat;
				mat.Identity();
				pSManager->m_coordinateDirection.SetSelectedFlags(0);
				pSManager->m_coordinateDirection.SetRotate(mat);
				pSManager->m_coordinateDirection.SetPos(vDirPos);
				pSManager->m_coordinateDirection.Update();
				pSManager->m_coordinateDirection.OnMouseMove(x,y,dwFlags);
				m_nDirectFlag = pSManager->m_coordinateDirection.GetSelectedFlags();
				//if(m_nDirectFlag!=0) SetCursor(g_hMoveCursor);
			}
		}
	}
	m_nOldX = x;
	m_nOldY = y;
	return true;
}

void CBezierDragOperation::Render(A3DViewport* pA3DViewport)
{
	//绘制每个对象的坐标指示
	A3DMATRIX4 mat;
	mat.Identity();
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			
			CSceneObject *ptemp = pManager->m_listSelectedObject.GetTail();
			if(ptemp->GetObjectType()!=CSceneObject::SO_TYPE_BEZIER) return;
			CEditerBezierSegment *pSegHead = NULL,*pSegTail = NULL;
			CEditerBezierPoint *pt = NULL;
			bool bStart = false;
			
			int nFlag = 0;
			A3DVECTOR3 vDirPos = A3DVECTOR3(0.0f,0.0f,0.0f);
			nFlag = ((CEditerBezier*)ptemp)->GetHitFlag();
			if(nFlag>0)
			{
				switch(nFlag)
				{
				case 1://命中点
					pt = ((CEditerBezier*)ptemp)->GetCurPoint(&pSegHead,&pSegTail);
					if(pt)
					{
						vDirPos = pt->GetPos();
					}
					break;
				case 2://命中段
					if(!((CEditerBezier*)ptemp)->IsBezier()) return;
					pSegHead = ((CEditerBezier*)ptemp)->GetCurSegment(vDirPos);
					break;
				case 3://命中控制点
					if(!((CEditerBezier*)ptemp)->IsBezier()) return;
					pSegHead = ((CEditerBezier*)ptemp)->GetCurCtrl(bStart);
					if(pSegHead)
					{
						if(bStart) vDirPos = pSegHead->GetAnchor1();
						else vDirPos = pSegHead->GetAnchor2();
					}
					break;
				}
				pManager->m_coordinateDirection.SetSelectedFlags(m_nDirectFlag);
				pManager->m_coordinateDirection.SetPos(vDirPos);
				pManager->m_coordinateDirection.Update();
				pManager->m_coordinateDirection.SetRotate(mat);
				pManager->m_coordinateDirection.Draw();
			}
		}
	}
}

A3DVECTOR3 CBezierDragOperation::GetDeltaVector(A3DVECTOR3 pos,int x,int y)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	A3DVECTOR3 vTrans = A3DVECTOR3(0,0,0);
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		
		//求投影比例
		A3DVECTOR3 vS1,vS2,vPosEx,vCenter,vScreenPosX,vScreenPosY,vScreenPosZ;
		float scaleX,scaleY,scaleZ,scale;
		float deltaX,deltaY;
		
		A3DVECTOR3 vCoorPos = pos;
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vCoorPos, vCenter);
		vCenter.z = 0;
		//x
		vPosEx = vCoorPos + A3DVECTOR3(1.0f,0,0);
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vPosEx, vScreenPosX);
		vScreenPosX.z = 0;
		scaleX =  (vPosEx - vCoorPos).Magnitude()/(vScreenPosX - vCenter).Magnitude();
		//y
		vPosEx = vCoorPos + A3DVECTOR3(0,1.0f,0);
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vPosEx, vScreenPosY);
		
		vScreenPosY.z = 0;
		scaleY =  (vPosEx - vCoorPos).Magnitude()/(vScreenPosY - vCenter).Magnitude();
		//z
		vPosEx = vCoorPos + A3DVECTOR3(0,0,1.0f);
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vPosEx, vScreenPosZ);
		
		vScreenPosZ.z = 0;
		scaleZ =  (vPosEx - vCoorPos).Magnitude()/(vScreenPosZ - vCenter).Magnitude();
		
		//去掉变化最大的
		if(scaleX>scaleZ) scale = scaleZ;
		else scale = scaleX;
		
		if(scale>scaleY) scale = scaleY;
		
		deltaX = (x - m_nOldX)*scale;
		deltaY = (y - m_nOldY)*scale;

		
		A3DVECTOR3 tempV;
		switch(m_nDirectFlag)
		{
		case  CCoordinateDirection::AXIS_PLANE_NO:
			break;
		case  CCoordinateDirection::AXIS_X: 
			vScreenPosX = vScreenPosX - vCenter;
			//这而计算移动的方向
			if(vScreenPosX.y !=0 )
			{//除数不能为0
				if((float)abs(vScreenPosX.x/vScreenPosX.y)>1.0f)
				{
					if(vScreenPosX.x>0)
						vTrans = A3DVECTOR3(deltaX,0,0);
					else vTrans = A3DVECTOR3(-deltaX,0,0);
				}else 
				{
					if(vScreenPosX.y>0)
						vTrans = A3DVECTOR3(deltaY,0,0);
					else vTrans = A3DVECTOR3(-deltaY,0,0);
				}
			}else
			{
				if(vScreenPosX.x>0)
					vTrans = A3DVECTOR3(deltaX,0,0);
				else vTrans = A3DVECTOR3(-deltaX,0,0);
			}
			break;
		case  CCoordinateDirection::AXIS_Y: 
			vScreenPosY = vScreenPosY - vCenter;
			//这而计算移动的方向
			if(vScreenPosY.y !=0 )
			{//除数不能为0
				if((float)abs(vScreenPosY.x/vScreenPosY.y)>1.0f)
				{
					if(vScreenPosY.x>0)
						vTrans = A3DVECTOR3(0,deltaX,0);
					else vTrans = A3DVECTOR3(0,-deltaX,0);
				}else 
				{
					if(vScreenPosY.y>0)
						vTrans = A3DVECTOR3(0,deltaY,0);
					else vTrans = A3DVECTOR3(0,-deltaY,0);
				}
			}else
			{
				if(vScreenPosY.x>0)
					vTrans = A3DVECTOR3(0,deltaX,0);
				else vTrans = A3DVECTOR3(0,-deltaX,0);
			}
			break;
		case  CCoordinateDirection::AXIS_Z: 
			vScreenPosZ = vScreenPosZ - vCenter;
			//这而计算移动的方向
			if(vScreenPosZ.y !=0 )
			{//除数不能为0
				if((float)abs(vScreenPosZ.x/vScreenPosZ.y)>1.0f)
				{
					if(vScreenPosZ.x>0)
						vTrans = A3DVECTOR3(0,0,deltaX);
					else vTrans = A3DVECTOR3(0,0,-deltaX);
				}else 
				{
					if(vScreenPosZ.y>0)
						vTrans = A3DVECTOR3(0,0,deltaY);
					else vTrans = A3DVECTOR3(0,0,-deltaY);
				}
			}else
			{
				if(vScreenPosZ.x>0)
					vTrans = A3DVECTOR3(0,0,deltaX);
				else vTrans = A3DVECTOR3(0,0,-deltaX);
			}
			break;
		case  CCoordinateDirection::PLANE_XY: 
			vScreenPosX = vScreenPosX - vCenter;
			vScreenPosY = vScreenPosY - vCenter;
			//这而计算移动的方向
			if(vScreenPosX.y !=0 )
			{//除数不能为0
				if((float)abs(vScreenPosX.x/vScreenPosX.y)>1.0f)
				{
					if(vScreenPosX.x>0)
					{
						if(vScreenPosY.y>0)
							vTrans = A3DVECTOR3(deltaX,deltaY,0);
						else vTrans = A3DVECTOR3(deltaX,-deltaY,0);
					}else
					{
						if(vScreenPosY.y>0)
							vTrans = A3DVECTOR3(-deltaX,deltaY,0);
						else vTrans = A3DVECTOR3(-deltaX,-deltaY,0);
					}
				}else 
				{
					if(vScreenPosX.y>0)
					{
						if(vScreenPosY.x>0)
							vTrans = A3DVECTOR3(deltaY,deltaX,0);
						else vTrans = A3DVECTOR3(deltaY,-deltaX,0);
					}else
					{
						if(vScreenPosY.x>0)
							vTrans = A3DVECTOR3(-deltaY,deltaX,0);
						else vTrans = A3DVECTOR3(-deltaY,-deltaX,0);
					} 
				}
			}else
			{
				if(vScreenPosX.x>0)
				{
					if(vScreenPosY.y>0)
						vTrans = A3DVECTOR3(deltaX,deltaY,0);
					else vTrans = A3DVECTOR3(deltaX,-deltaY,0);
				}else
				{
					if(vScreenPosY.y>0)
						vTrans = A3DVECTOR3(-deltaX,deltaY,0);
					else vTrans = A3DVECTOR3(-deltaX,-deltaY,0);
				}
			}
			break;
		case  CCoordinateDirection::PLANE_XZ: 
			vScreenPosX = vScreenPosX - vCenter;
			vScreenPosZ = vScreenPosZ - vCenter;
			//这而计算移动的方向
			if(vScreenPosX.y !=0 )
			{//除数不能为0
				if((float)abs(vScreenPosX.x/vScreenPosX.y)>1.0f)
				{
					if(vScreenPosX.x>0)
					{
						if(vScreenPosZ.y>0)
							vTrans = A3DVECTOR3(deltaX,0,deltaY);
						else vTrans = A3DVECTOR3(deltaX,0,-deltaY);
					}else
					{
						if(vScreenPosZ.y>0)
							vTrans = A3DVECTOR3(-deltaX,0,deltaY);
						else vTrans = A3DVECTOR3(-deltaX,0,-deltaY);
					}
				}else 
				{
					if(vScreenPosX.y>0)
					{
						if(vScreenPosZ.x>0)
							vTrans = A3DVECTOR3(deltaY,0,deltaX);
						else vTrans = A3DVECTOR3(deltaY,0,-deltaX);
					}else
					{
						if(vScreenPosZ.x>0)
							vTrans = A3DVECTOR3(-deltaY,0,deltaX);
						else vTrans = A3DVECTOR3(-deltaY,0,-deltaX);
					} 
				}
			}else
			{
				if(vScreenPosX.x>0)
				{
					if(vScreenPosZ.y>0)
						vTrans = A3DVECTOR3(deltaX,0,deltaY);
					else vTrans = A3DVECTOR3(deltaX,0,-deltaY);
				}else
				{
					if(vScreenPosZ.y>0)
						vTrans = A3DVECTOR3(-deltaX,0,deltaY);
					else vTrans = A3DVECTOR3(-deltaX,0,-deltaY);
				}
			}
			break;
		case  CCoordinateDirection::PLANE_YZ: 
			vScreenPosY = vScreenPosY - vCenter;
			vScreenPosZ = vScreenPosZ - vCenter;
			//这而计算移动的方向
			if(vScreenPosY.y !=0 )
			{//除数不能为0
				if((float)abs(vScreenPosY.x/vScreenPosY.y)>1.0f)
				{
					if(vScreenPosY.x>0)
					{
						if(vScreenPosZ.y>0)
							vTrans = A3DVECTOR3(0,deltaX,deltaY);
						else vTrans = A3DVECTOR3(0,deltaX,-deltaY);
					}else
					{
						if(vScreenPosZ.y>0)
							vTrans = A3DVECTOR3(0,-deltaX,deltaY);
						else vTrans = A3DVECTOR3(0,-deltaX,-deltaY);
					}
				}else 
				{
					if(vScreenPosY.y>0)
					{
						if(vScreenPosZ.x>0)
							vTrans = A3DVECTOR3(0,deltaY,deltaX);
						else vTrans = A3DVECTOR3(0,deltaY,-deltaX);
					}else
					{
						if(vScreenPosZ.x>0)
							vTrans = A3DVECTOR3(0,-deltaY,deltaX);
						else vTrans = A3DVECTOR3(0,-deltaY,-deltaX);
					} 
				}
			}else
			{
				if(vScreenPosY.x>0)
				{
					if(vScreenPosZ.y>0)
						vTrans = A3DVECTOR3(0,deltaX,deltaY);
					else vTrans = A3DVECTOR3(0,deltaX,-deltaY);
				}else
				{
					if(vScreenPosZ.y>0)
						vTrans = A3DVECTOR3(0,-deltaX,deltaY);
					else vTrans = A3DVECTOR3(0,-deltaX,-deltaY);
				}
			}
			break;
		}
	}
	return vTrans;
}

//////////////////////////////////////////////////////////////////////////////
//class BezierTestOperation
//////////////////////////////////////////////////////////////////////////////

CBezierTestOperation::CBezierTestOperation()
{
	m_bStartPlay = false;
}

CBezierTestOperation::~CBezierTestOperation()
{
}

void CBezierTestOperation::Tick(DWORD dwTimeDelta)
{
	if(m_bStartPlay)
	{
		UpdateCamra(dwTimeDelta);
	}
}

void CBezierTestOperation::UpdateCamra(DWORD dwTimeDelta)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pBezier = (CSceneObject *)pManager->m_listSelectedObject.GetHead();
			if((pBezier!=m_pBezier))
			{
				Stop();
			}
		}else
			Stop();
	}
	
	if(m_dwEscapeTime>=m_dwSegTime)
	{
		m_pSeg = GetNextSeg();
		if(m_pSeg==NULL)
		{
			m_SegPos = m_pBezier->m_list_segments.GetHeadPosition();
			m_pSeg = GetNextSeg();
			if(m_pSeg==NULL) return;
		}
		m_pSeg->ReCalculateLength();//放这儿不好，浪费
		float length = m_pSeg->GetLenght();
		m_dwSegTime = length * 100;  
		m_dwEscapeTime = 0;
	}else
	{
		float u = (float)m_dwEscapeTime/(float)m_dwSegTime;
		A3DVECTOR3 pos = m_pSeg->Bezier(u);
		A3DVECTOR3 dir = GetSpot(m_pSeg,u);
		g_Render.GetA3DEngine()->GetActiveCamera()->SetPos(pos);
		g_Render.GetA3DEngine()->GetActiveCamera()->SetDirAndUp(dir,A3DVECTOR3(0.0f,1.0f,0.0f));
		m_dwEscapeTime += dwTimeDelta;
	}
}

// 根据长度返回视点
A3DVECTOR3 CBezierTestOperation::GetSpot(CEditerBezierSegment *pseg,float u)
{

	A3DVECTOR3 headspot = pseg->GetHeadSpot(),
			   tailspot = pseg->GetTailSpot();

	if(u <= 0.0f) return headspot;
	if(u >= 1.0f) return tailspot;

	float m1 = Magnitude(headspot),
		  m2 = Magnitude(tailspot);

	float dot = DotProduct(Normalize(headspot), Normalize(tailspot));

	if(dot > 1.0f)
	{
		return headspot;
	}

	if(dot < -1.0f) dot = -1.0f;

	float r = (float)acos(dot);
	bool isinv = _isnan(r) ? true : false;
	float deg = RAD2DEG(r);
	r = r * u;

	A3DVECTOR3 dir = CrossProduct(headspot, tailspot);
	A3DMATRIX4 matrix = RotateAxis(dir, r);
	headspot = matrix * headspot;
	return Normalize(headspot) * (m1 + (m2 - m1) * u);
}

CEditerBezierSegment* CBezierTestOperation::GetNextSeg()
{
	if(m_SegPos)
		return (CEditerBezierSegment*)m_pBezier->m_list_segments.GetNext(m_SegPos);
	else return NULL;
}

void CBezierTestOperation::Play()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CEditerBezier *pBezier = (CEditerBezier *)pManager->m_listSelectedObject.GetHead();
			if(pBezier==NULL) return;
			if(pBezier->GetObjectType()!=CSceneObject::SO_TYPE_BEZIER) return;
			m_pBezier = pBezier;
			m_pBezier->CalculateDirection();
			//m_pBezier->EanbleTest(true);
			m_SegPos = m_pBezier->m_list_segments.GetHeadPosition();
			m_dwSegTime = 0;
			m_dwEscapeTime = 0;
			m_bStartPlay = true;
		}
	}
	
}

void CBezierTestOperation::Stop()
{
	m_bStartPlay = false;
}

bool CBezierTestOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	//if(m_bStartPlay) Stop();
	//else Play();
	return false;
}
