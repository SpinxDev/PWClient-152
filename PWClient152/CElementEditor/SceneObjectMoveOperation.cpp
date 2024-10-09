// SceneObjectMoveOperation.cpp: implementation of the CSceneObjectMoveOperation class.
//
//////////////////////////////////////////////////////////////////////


#include "Global.h"
#include "elementeditor.h"
#include "SceneObjectMoveOperation.h"
#include "SceneWaterObject.h"
#include "MainFrm.h"
#include "A3D.h"
#include "render.h"
#include "SceneObjectManager.h"
#include "UndoObjectMoveAction.h"
#include "BezierUndoAction.h"
#include "UndoMan.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneObjectMoveOperation::CSceneObjectMoveOperation()
{
	m_bRotate = false;
	m_vDelta = A3DVECTOR3(0.0f,0.0f,0.0f);
	m_pUndo = NULL;
}

CSceneObjectMoveOperation::~CSceneObjectMoveOperation()
{

}

bool CSceneObjectMoveOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	A3DVECTOR3 vTrans = A3DVECTOR3(0.0f,0.0f,0.0f);
	SceneObjectMultiSelectOperation::OnMouseMove(x,y,dwFlags);
	if(pMap && (!m_bDrawRect))
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return true;
		pManager->m_coordinateDirection.SetCursor(g_hMoveCursor);
		if(m_bMouseDown==false) return true;
		
		if(m_pSelectedObject == NULL) return true;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		vTrans = GetDeltaVector(m_pSelectedObject->GetPos(),x,y);
		m_pSelectedObject->Translate(vTrans);
		m_pSelectedObject->UpdateProperty(false);
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			if(m_pSelectedObject==ptemp) continue;
			ptemp->Translate(vTrans);
			ptemp->UpdateProperty(false);
		}
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject* pobj = pManager->m_listSelectedObject.GetHead();
			A3DVECTOR3 vpos = pobj->GetPos();
			AUX_GetMainFrame()->SetXValue(vpos.x);
			AUX_GetMainFrame()->SetYValue(vpos.y);
			AUX_GetMainFrame()->SetZValue(vpos.z);
		}
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
		m_vDelta += vTrans;
		m_nOldX = x;
		m_nOldY = y;
	}
	return true;
}

A3DVECTOR3 CSceneObjectMoveOperation::GetDeltaVector(A3DVECTOR3 pos,int x,int y)
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	A3DVECTOR3 vDelta = A3DVECTOR3(0.0f);
	CPoint pt(x,y);
	if(pMap)
	{
		A3DVECTOR3 xy(pos),xz(pos),yz(pos);
		ScreenTraceAxis(pos,pt,&xy,&xz,&yz);
		switch(m_nDirectFlag)
		{
		case  CCoordinateDirection::AXIS_X: 
			vDelta.x = xz.x - pos.x - m_vXZ.x;
			break;
		
		case  CCoordinateDirection::AXIS_Y: 
			vDelta.y = xy.y - pos.y - m_vXY.y;
			break;
		
		case  CCoordinateDirection::AXIS_Z: 
			vDelta.z = xz.z - pos.z - m_vXZ.z;
			break;
		
		case  CCoordinateDirection::PLANE_XY: 
			vDelta.x = xy.x - pos.x - m_vXY.x;
			vDelta.y = xy.y - pos.y - m_vXY.y;
			break;
		
		case  CCoordinateDirection::PLANE_XZ: 
			vDelta.x = xz.x - pos.x - m_vXZ.x;
			vDelta.z = xz.z - pos.z - m_vXZ.z;
			break;
		
		case  CCoordinateDirection::PLANE_YZ: 
			vDelta.y = yz.y - pos.y - m_vYZ.y;
			vDelta.z = yz.z - pos.z - m_vYZ.z;
			break; 
		}
	}
	return vDelta;
	
	/*
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
	*/
	
}
void CSceneObjectMoveOperation::Render(A3DViewport* pA3DViewport)
{
	SceneObjectMultiSelectOperation::Render(pA3DViewport);
	//绘制每个对象的坐标指示
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		A3DMATRIX4 mat;
		mat.Identity();
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			pManager->m_coordinateDirection.SetSelectedFlags(m_nDirectFlag);
			pManager->m_coordinateDirection.SetPos(vCoorPos);
			pManager->m_coordinateDirection.Update();
			pManager->m_coordinateDirection.SetRotate(mat);
			pManager->m_coordinateDirection.Draw();
		}
	}
}

bool CSceneObjectMoveOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_vDelta = A3DVECTOR3(0.0f,0.0f,0.0f);
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		//if(!m_bDrawRect)
		//{	
			pMap->SetModifiedFlag(true);
			m_pUndo = new CUndoObjectMoveAction(pMap->GetSceneObjectMan());
			m_pUndo->GetSeletedListData();
		//}
	}
	return SceneObjectMultiSelectOperation::OnLButtonDown(x,y,dwFlags);
}

bool CSceneObjectMoveOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	SceneObjectMultiSelectOperation::OnLButtonUp(x,y,dwFlags);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(m_vDelta != A3DVECTOR3(0.0f,0.0f,0.0f))
		{
			if(m_pUndo)
			{
				g_UndoMan.AddUndoAction(m_pUndo);
				m_pUndo = NULL;
			}
			//对水对象的特殊处理，鼠标弹起才更新数据
			ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
			while( pos )
			{
				CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
				if(ptemp->GetObjectType()== CSceneObject::SO_TYPE_WATER)
					((CSceneWaterObject*)ptemp)->BuildRenderWater();
			}
		}else
		{
			if(m_pUndo)
			{//
				m_pUndo->Release();
				delete m_pUndo;
				m_pUndo = NULL;
			}
		}
	}

	m_vDelta = A3DVECTOR3(0.0f,0.0f,0.0f);
	return true;
}


