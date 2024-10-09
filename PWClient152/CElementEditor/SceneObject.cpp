// SceneObject.cpp: implementation of the CSceneObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "SceneObject.h"
#include "A3DTypes.h"
#include "A3D.h"
#include "Render.h"
#include "Box3D.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneObject::CSceneObject()
{
	m_nObjectType = 0;
	m_bDeleted = false;
	m_dwExportID = 0;
	m_bSelected = false;
	m_pProperty = new ADynPropertyObject();
	m_vPos = A3DVECTOR3(0.0f);
	m_fScaleUnit = 1.0f;
	m_matRotate.Identity();
	m_matTrans.Identity();
	m_matScale.Identity();
	m_matAbs.Identity();
}

CSceneObject::~CSceneObject()
{
	if(m_pProperty) delete m_pProperty;
}

A3DVECTOR3 CSceneObject::GetMin()
{
	A3DVECTOR3 minEdge;
	minEdge = m_matTrans.GetRow(3)+A3DVECTOR3(-m_fScaleUnit,-m_fScaleUnit,-m_fScaleUnit);
	return minEdge;
}
A3DVECTOR3 CSceneObject::GetMax()
{
	A3DVECTOR3 maxEdge;
	maxEdge = m_matTrans.GetRow(3)+A3DVECTOR3(m_fScaleUnit,m_fScaleUnit,m_fScaleUnit);
	return maxEdge;
}

void CSceneObject::Translate(const A3DVECTOR3& vDelta)
{
	m_vPos += vDelta;
	m_matTrans.SetRow(3,m_vPos);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
};

void CSceneObject::SetPos(const A3DVECTOR3& vPos)
{
	m_matTrans.Translate(vPos.x,vPos.y,vPos.z);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	m_vPos = vPos;
}

void CSceneObject::RotateX(float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateX(fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = m_matRotate.GetRow(2);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
};

void CSceneObject::RotateY(float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateY(fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = m_matRotate.GetRow(2);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
};
void CSceneObject::RotateZ(float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateZ(fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = m_matRotate.GetRow(2);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
};

void CSceneObject::Rotate(const A3DVECTOR3& axis,float fRad)
{
	A3DMATRIX4 mat;
	mat.RotateAxis(axis,fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = m_matRotate.GetRow(2);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
};

void CSceneObject::Scale(float fScale)
{
	A3DMATRIX4 mat;
	mat.Scale(fScale,fScale,fScale);
	m_matScale= m_matScale*mat;
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
};

void CSceneObject::Render(A3DViewport* pA3DViewport) 
{
	if(g_Configs.bShowObjectName)
	{
		
		A3DVECTOR3 vScreenPos;
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(m_vPos, vScreenPos);
		if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(m_vPos))
			g_Render.TextOut(vScreenPos.x,vScreenPos.y,m_strName,A3DCOLORRGB(255,255,0));
	}
}

void CSceneObject::DrawCenter()
{
	if(!g_bShowPosDot) return;
	
	A3DVECTOR3 vPos = m_matTrans.GetRow(3);
	A3DVECTOR3 vScreenPos,vPosEx,vScreenPosEx;
	
	//求投影比例
	A3DVECTOR3 camPos = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos();
	A3DVECTOR3 mAxisGizmoCenter = m_vPos;
	// assumes a 90deg FOV
	A3DVECTOR3 dir = mAxisGizmoCenter - camPos;
	A3DVIEWPORTPARAM* param = g_Render.GetA3DEngine()->GetActiveViewport()->GetParam();

	A3DMATRIX4 matScale;
	float mAxisGizmoProjLen = (4.0f/(float)param->Width) * dir.Magnitude() * tan(A3D_PI/180.0f * 45.0f);
	m_fScaleUnit = mAxisGizmoProjLen;

	if(!g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(m_vPos)) return;
	
	/*
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(m_bSelected)
	{
		pWireCollector->AddSphere(m_vPos,m_fScaleUnit,A3DCOLORRGB(255,0,0));
	}else
	{
		pWireCollector->AddSphere(m_vPos,m_fScaleUnit,A3DCOLORRGB(255,255,0));
	}
	pWireCollector->Flush();*/
	CBox3D renderBox;
	renderBox.Init(&g_Render,m_fScaleUnit,true);
	renderBox.SetDirAndUp(A3DVECTOR3(0,0,1.0f),A3DVECTOR3(0,1.0f,0));
	if(m_bSelected)
		renderBox.SetColor(A3DCOLORRGB(255,0,0));
	else renderBox.SetColor(A3DCOLORRGB(255,255,0));
	renderBox.SetPos(m_vPos);
	renderBox.Render();
}


float _getAngle(float X,float Y)
{
	if(Y==0) return 0;
	
	if(X==0)
	{
		if(Y>0) return 90.0f;
		if(Y<0) return 270.0f;
	}
	
	float temp = RAD2DEG(atan(Y/X));
	if(X>0 && Y>0) return temp;
	else if( X>0 && Y<0) return 360 + temp;
	else if( X<0 && Y>0) return 180 + temp;
	else if( X<0 && Y<0) return 180 + temp;
	else return 0;
}

/*
float _getAngle(float X,float Y)
{
if (Y == 0.0)
return X > 0.0 ? 90.0 : 180.0;

  float tmp = Y / sqrt(X*X + Y*Y);
  tmp = RAD2DEG(atan(sqrt(1 - tmp*tmp) / tmp));
  
	if (X>0.0 && Y>0.0)
	return tmp + 270;
	else
	if (X>0.0 && Y<0.0)
			return tmp + 90;
		else
			if (X<0.0 && Y<0.0)
				return 90 - tmp;
			else
				if (X<0.0 && Y>0.0)
					return 270 - tmp;
				
				return tmp;
}
*/


