// SceneVernier.cpp: implementation of the CSceneVernier class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"

#include "SceneVernier.h"
#include "elementeditor.h"
#include "Box3D.h"
#include "Render.h"
#include "A3D.h"
//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneVernier::CSceneVernier()
{
	m_vPos = A3DVECTOR3(0.0f);
	m_fAreaSize = 50.0f;
	m_nShowPlane = 0;
}

CSceneVernier::~CSceneVernier()
{
	
}

void CSceneVernier::Render(A3DViewport* pA3DViewport)
{
	
	DrawZxis();
	
	CBox3D renderBox;
	renderBox.Init(&g_Render,m_fAreaSize,true);
	renderBox.SetDirAndUp(A3DVECTOR3(0,0,1.0f),A3DVECTOR3(0,1.0f,0));
	renderBox.SetColor(A3DCOLORRGBA(255,255,255,60));
	renderBox.SetPos(m_vPos);
	switch(m_nShowPlane)
	
	{
	case 0:
		renderBox.SetSize(m_fAreaSize,0.01f,m_fAreaSize);break;
	case 1:
		renderBox.SetSize(m_fAreaSize,m_fAreaSize,0.01f);break;
	case 2:
		renderBox.SetSize(0.01f,m_fAreaSize,m_fAreaSize);break;
	}
		renderBox.Render();
	renderBox.Release();

	//显示中心坐标
	if(g_Configs.bShowObjectName)
	{
		A3DVECTOR3 vScreenPos;
		m_strName.Format("%.2f,%.2f,%.2f",m_vPos.x,m_vPos.y,m_vPos.z);
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(m_vPos, vScreenPos);
		if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(m_vPos))
			g_Render.TextOut(vScreenPos.x,vScreenPos.y,m_strName,A3DCOLORRGB(0,0,255));
	}
}

void CSceneVernier::DrawZxis()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	A3DVECTOR3 Vertices[2];
	WORD indices[2];
	indices[0] = 0;
	indices[1] = 1;
	Vertices[0] = m_vPos + A3DVECTOR3(-m_fAreaSize,0.1f,0.1f);
	Vertices[1] = m_vPos + A3DVECTOR3(m_fAreaSize,0.1f,0.1f);
	pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,0,0));

	Vertices[0] = m_vPos + A3DVECTOR3(0.1f,-m_fAreaSize,0.1f);
	Vertices[1] = m_vPos + A3DVECTOR3(0.1f,m_fAreaSize,0.1f);
	pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,255,0));

	Vertices[0] = m_vPos + A3DVECTOR3(0.1f,0.1f,-m_fAreaSize);
	Vertices[1] = m_vPos + A3DVECTOR3(0.1f,0.1f,m_fAreaSize);
	pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(0,0,255));

	pWireCollector->Flush();
}

void CSceneVernier::UpdateProperty(bool bGet)
{
	if(bGet)
	{
		m_vPos	= m_pProperty->GetPropVal(0);
		m_fAreaSize	= m_pProperty->GetPropVal(1);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}else
	{
		m_pProperty->SetPropVal(0,m_vPos);
		m_pProperty->SetPropVal(1,m_fAreaSize);
	}
}

void CSceneVernier::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_fAreaSize), "可见区域尺寸");
	}
}
