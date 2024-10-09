// TerrainObstruct.cpp: implementation of the CTerrainObstruct class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "TerrainObstruct.h"
#include "Render.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerrainObstruct::CTerrainObstruct()
{

}

CTerrainObstruct::~CTerrainObstruct()
{

}

void CTerrainObstruct::Release()
{
	/*
	POSITION pos = m_BezierList.GetHeadPosition();
	while(pos)
	{
		CEditerBezier * ptemp = (CEditerBezier *)m_BezierList.GetNext(pos);
		ptemp->Release();
		delete ptemp;
	}
	*/
	m_BezierList.RemoveAll();
}

void CTerrainObstruct::AddBezier(CEditerBezier *pBezier,bool bNew)
{
	ASSERT(pBezier);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		int nObstructID = 0;
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Obstuct_%d",nObstructID);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				nObstructID++;
			}else break;
		}
		pBezier->SetObjectName(name);
		pBezier->SetBezier(false);
		m_BezierList.AddTail(pBezier);
		if(bNew) pSManager->InsertSceneObject(pBezier);
	}
}

void CTerrainObstruct::Tick(DWORD timeDelta)
{

}

void CTerrainObstruct::Render(A3DViewport* pA3DViewport)
{
	POSITION pos = m_BezierList.GetHeadPosition();
	while(pos)
	{
		CEditerBezier * ptemp = (CEditerBezier *)m_BezierList.GetNext(pos);
		ptemp->Render(pA3DViewport);
	}

#ifdef _DEBUG
	//debug
	int test = m_BezierList.GetCount();
	AString str;
	str.Format("Obsruct profile num = %d ",test);
	g_Render.TextOut(5,120,str,A3DCOLORRGB(0,255,0));
#endif 
}
