// CSceneObjectRotateOperation.cpp: implementation of the CSceneObjectRotateOperation class.
//
//////////////////////////////////////////////////////////////////////
#include "Global.h"
#include "elementeditor.h"
#include "SceneObjectRotateOperation.h"
#include "MainFrm.h"
#include "A3D.h"
#include "render.h"
#include "SceneObjectManager.h"
#include "UndoRotateAction.h"
#include "UndoMan.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneObjectRotateOperation::CSceneObjectRotateOperation()
{
	m_bRotate = true; 
	m_pUndo = 0;
}

CSceneObjectRotateOperation::~CSceneObjectRotateOperation()
{
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
	}
}

bool CSceneObjectRotateOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return true;
		m_hCursor = g_hRotateCursor;
		SceneObjectMultiSelectOperation::OnMouseMove(x,y,dwFlags);
		if(m_bMouseDown==false) return true;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		float deltaX = (x - m_nOldX)*0.01f;
		float deltaY = (m_nOldY - y)*0.01f;
		m_nOldX = x;
		m_nOldY = y;
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			A3DVECTOR3 vTrans = A3DVECTOR3(0,0,0);
			A3DVECTOR3 axis;
			switch(m_nDirectFlag)
			{
			case  CCoordinateDirection::AXIS_PLANE_NO:
				break;
			case  CCoordinateDirection::AXIS_X: 
				axis = A3DVECTOR3(1.0f,0,0);
				axis = ptemp->GetRotateMatrix()*axis;
				m_fRotateRad += deltaY;
				m_vAxis = axis;
				ptemp->Rotate(axis,deltaY);
				break;
			case  CCoordinateDirection::AXIS_Y: 
				axis = A3DVECTOR3(0,1.0f,0);
				axis = ptemp->GetRotateMatrix()*axis;
				m_fRotateRad += deltaY;
				m_vAxis = axis;
				ptemp->Rotate(axis,deltaY);
				break;
			case  CCoordinateDirection::AXIS_Z: 
				axis = A3DVECTOR3(0,0,1.0f);
				axis = ptemp->GetRotateMatrix()*axis;
				m_fRotateRad += deltaY;
				m_vAxis = axis;
				ptemp->Rotate(axis,deltaY);
				break;
			case  CCoordinateDirection::PLANE_XY: break;
			case  CCoordinateDirection::PLANE_XZ: break;
			case  CCoordinateDirection::PLANE_YZ: break;
			}
			ptemp->UpdateProperty(false);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
		}
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *test = pManager->m_listSelectedObject.GetHead();
			A3DMATRIX4 matr = test->GetRotateMatrix();
			float anglex, angley,anglez;
			float data[11] = { matr._11,matr._12,matr._13,matr._14,matr._21,matr._22,matr._23,matr._24,matr._31,matr._32,matr._33}; 
			AUX_ToEulerAnglesXYZ(data,anglex,angley,anglez);
			AUX_GetMainFrame()->SetXValue(anglex * 180/A3D_PI);
			AUX_GetMainFrame()->SetYValue(angley * 180/A3D_PI);
			AUX_GetMainFrame()->SetZValue(anglez * 180/A3D_PI);
		}
	}
	return true;
}

void CSceneObjectRotateOperation::Render(A3DViewport* pA3DViewport)
{
	SceneObjectMultiSelectOperation::Render(pA3DViewport);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{

		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			pManager->m_coordinateDirection.SetSelectedFlags(m_nDirectFlag);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			A3DMATRIX4 mat = ptemp->GetRotateMatrix();
			pManager->m_coordinateDirection.SetPos(vCoorPos);
			pManager->m_coordinateDirection.SetRotate(mat);
			pManager->m_coordinateDirection.Update();
			pManager->m_coordinateDirection.Draw();
		}
	}
}

bool CSceneObjectRotateOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_vAxis = A3DVECTOR3(0,0,0);
	m_fRotateRad = 0;
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
		m_pUndo = 0;
	}
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		m_pUndo = new CUndoRotateAction(pMap->GetSceneObjectMan());
		m_pUndo->SetData();
	}
	return SceneObjectMultiSelectOperation::OnLButtonDown(x,y,dwFlags);
}

bool CSceneObjectRotateOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	SceneObjectMultiSelectOperation::OnLButtonUp(x,y,dwFlags);
	if(m_pUndo==0) return true;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(m_fRotateRad)
		{
			g_UndoMan.AddUndoAction(m_pUndo);
			m_pUndo = 0;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// Ex ..
////////////////////////////////////////////////////////////////////////////////////

CSceneObjectRotateOperationEx::CSceneObjectRotateOperationEx()
{
	m_bRotate = true; 
	m_pUndo = 0;
	m_nRotateFlag = ROTATE_X;
}

CSceneObjectRotateOperationEx::~CSceneObjectRotateOperationEx()
{
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
	}
}

bool CSceneObjectRotateOperationEx::OnMouseMove(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return true;
		m_hCursor = g_hRotateCursor;
		SceneObjectMultiSelectOperation::OnMouseMove(x,y,dwFlags);
		if(m_bMouseDown==false) return true;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		float deltaX = (x - m_nOldX)*0.01f;
		float deltaY = (m_nOldY - y)*0.01f;
		m_nOldX = x;
		m_nOldY = y;
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			A3DVECTOR3 vTrans = A3DVECTOR3(0,0,0);
			A3DVECTOR3 axis;
			switch(m_nRotateFlag)
			{
			case  ROTATE_X: 
				axis = A3DVECTOR3(1.0f,0,0);
				axis = ptemp->GetRotateMatrix()*axis;
				m_fRotateRad += deltaY;
				m_vAxis = axis;
				ptemp->Rotate(axis,deltaY);
				break;
			case  ROTATE_Y: 
				axis = A3DVECTOR3(0,1.0f,0);
				axis = ptemp->GetRotateMatrix()*axis;
				m_fRotateRad += deltaY;
				m_vAxis = axis;
				ptemp->Rotate(axis,deltaY);
				break;
			case  ROTATE_Z: 
				axis = A3DVECTOR3(0,0,1.0f);
				axis = ptemp->GetRotateMatrix()*axis;
				m_fRotateRad += deltaY;
				m_vAxis = axis;
				ptemp->Rotate(axis,deltaY);
				break;
			}
			ptemp->UpdateProperty(false);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
		}
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *test = pManager->m_listSelectedObject.GetHead();
			A3DMATRIX4 matr = test->GetRotateMatrix();
			float anglex, angley,anglez;
			float data[11] = { matr._11,matr._12,matr._13,matr._14,matr._21,matr._22,matr._23,matr._24,matr._31,matr._32,matr._33}; 
			AUX_ToEulerAnglesXYZ(data,anglex,angley,anglez);
			AUX_GetMainFrame()->SetXValue(anglex * 180/A3D_PI);
			AUX_GetMainFrame()->SetYValue(angley * 180/A3D_PI);
			AUX_GetMainFrame()->SetZValue(anglez * 180/A3D_PI);
		}
	}
	return true;
}

void CSceneObjectRotateOperationEx::Render(A3DViewport* pA3DViewport)
{
	SceneObjectMultiSelectOperation::Render(pA3DViewport);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{

		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			pManager->m_coordinateDirection.SetSelectedFlags(m_nDirectFlag);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			A3DMATRIX4 mat = ptemp->GetRotateMatrix();
			pManager->m_coordinateDirection.SetPos(vCoorPos);
			pManager->m_coordinateDirection.SetRotate(mat);
			pManager->m_coordinateDirection.Update();
			pManager->m_coordinateDirection.Draw();
		}
	}
}

bool CSceneObjectRotateOperationEx::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_vAxis = A3DVECTOR3(0,0,0);
	m_fRotateRad = 0;
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
		m_pUndo = 0;
	}
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		m_pUndo = new CUndoRotateAction(pMap->GetSceneObjectMan());
		m_pUndo->SetData();
	}
	return SceneObjectMultiSelectOperation::OnLButtonDown(x,y,dwFlags);
}

bool CSceneObjectRotateOperationEx::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	SceneObjectMultiSelectOperation::OnLButtonUp(x,y,dwFlags);
	if(m_pUndo==0) return true;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(m_fRotateRad)
		{
			g_UndoMan.AddUndoAction(m_pUndo);
			m_pUndo = 0;
		}
	}
	return true;
}



///////////////////////////////////////////////////////////////////////////////////////
//2
////////////////////////////////////////////////////////////////////////////////////

CSceneObjectRotateOperation2::CSceneObjectRotateOperation2()
{
	m_bRotate = false; 
	m_pUndo = 0;
	m_nDirectFlag = 0;
}

CSceneObjectRotateOperation2::~CSceneObjectRotateOperation2()
{
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
	}
}

bool CSceneObjectRotateOperation2::OnMouseMove(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return true;
		m_hCursor = g_hRotateCursor;
		SceneObjectMultiSelectOperation::OnMouseMove(x,y,dwFlags);
		if(m_bMouseDown==false) return true;
		
		float deltaX = (x - m_nOldX)*0.01f;
		float deltaY = (m_nOldY - y)*0.01f;
		m_nOldX = x;
		m_nOldY = y;
		m_fRotateRad +=deltaY;
		
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		A3DVECTOR3 vCenter(0,0,0);
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			vCenter +=ptemp->GetPos();
		}
		vCenter = vCenter/pManager->m_listSelectedObject.GetCount();

		pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			A3DVECTOR3 vTrans = A3DVECTOR3(0,0,0);
			A3DVECTOR3 axis;
			switch(m_nDirectFlag)
			{
			case  CCoordinateDirection::AXIS_PLANE_NO:
				break;
			case  CCoordinateDirection::AXIS_X: 
				Delta(ptemp,deltaY,vCenter,ROTATE_X);
				break;
			case  CCoordinateDirection::AXIS_Y: 
				Delta(ptemp,deltaY,vCenter,ROTATE_Y);
				break;
			case  CCoordinateDirection::AXIS_Z: 
				Delta(ptemp,deltaY,vCenter,ROTATE_Z);
				break;
			case  CCoordinateDirection::PLANE_XY: break;
			case  CCoordinateDirection::PLANE_XZ: break;
			case  CCoordinateDirection::PLANE_YZ: break;
			}
			ptemp->UpdateProperty(false);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
		}
		
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *test = pManager->m_listSelectedObject.GetHead();
			A3DMATRIX4 matr = test->GetRotateMatrix();
			float anglex, angley,anglez;
			float data[11] = { matr._11,matr._12,matr._13,matr._14,matr._21,matr._22,matr._23,matr._24,matr._31,matr._32,matr._33}; 
			AUX_ToEulerAnglesXYZ(data,anglex,angley,anglez);
			AUX_GetMainFrame()->SetXValue(anglex * 180/A3D_PI);
			AUX_GetMainFrame()->SetYValue(angley * 180/A3D_PI);
			AUX_GetMainFrame()->SetZValue(anglez * 180/A3D_PI);
		}
	}
	return true;
}

void CSceneObjectRotateOperation2::Delta(CSceneObject* pTemp, float fDelta, A3DVECTOR3 vCenter, int nAxis)
{
	A3DMATRIX4 matAbs,matTrans,matRotate,matTemp,matDelta;
	matTrans.Identity();
	matRotate.Identity();
	matDelta.Identity();
	matTrans.Translate(pTemp->GetPos().x,pTemp->GetPos().y,pTemp->GetPos().z);
	matRotate = pTemp->GetRotateMatrix();
	
	//Translate to (0,0,0) point
	matDelta.SetRow(3,-vCenter);
	matTrans = matTrans * matDelta;
	
	if(nAxis==ROTATE_X)
	{
		matTemp.RotateX(fDelta);
	}else if(nAxis==ROTATE_Y)
	{
		matTemp.RotateY(fDelta);
	}else if(nAxis==ROTATE_Z)
	{
		matTemp.RotateZ(fDelta);
	}

	matTrans = matTrans * matTemp;
	matTrans = matRotate * matTrans;
	
	matDelta.SetRow(3,vCenter);
	matTrans = matTrans * matDelta;

	pTemp->SetPos(matTrans.GetRow(3));
	pTemp->SetDirAndUp(matTrans.GetRow(2),matTrans.GetRow(1));
}

void CSceneObjectRotateOperation2::Render(A3DViewport* pA3DViewport)
{
	SceneObjectMultiSelectOperation::Render(pA3DViewport);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{

		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager==NULL) return;
		ALISTPOSITION pos = pManager->m_listSelectedObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pManager->m_listSelectedObject.GetPrev(pos);
			pManager->m_coordinateDirection.SetSelectedFlags(m_nDirectFlag);
			A3DVECTOR3 vCoorPos = ptemp->GetPos();
			A3DMATRIX4 mat; mat.Identity();
			pManager->m_coordinateDirection.SetPos(vCoorPos);
			pManager->m_coordinateDirection.SetRotate(mat);
			pManager->m_coordinateDirection.Update();
			pManager->m_coordinateDirection.Draw();
		}
	}
}

bool CSceneObjectRotateOperation2::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_fRotateRad = 0;
	if(m_pUndo)
	{
		m_pUndo->Release();
		delete m_pUndo;
		m_pUndo = 0;
	}
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		m_pUndo = new CUndoRotateAction2(pMap->GetSceneObjectMan());
		m_pUndo->SetData();
	}
	return SceneObjectMultiSelectOperation::OnLButtonDown(x,y,dwFlags);
}

bool CSceneObjectRotateOperation2::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	SceneObjectMultiSelectOperation::OnLButtonUp(x,y,dwFlags);
	if(m_pUndo==0) return true;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(m_fRotateRad)
		{
			g_UndoMan.AddUndoAction(m_pUndo);
			m_pUndo = 0;
		}
	}
	return true;
}

