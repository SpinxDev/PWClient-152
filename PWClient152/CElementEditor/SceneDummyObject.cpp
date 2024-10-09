// SceneDummyObject.cpp: implementation of the CSceneDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneDummyObject.h"
#include "Box3D.h"
#include "A3D.h"
#include "Render.h"
#include "SceneObjectManager.h"
#include "SceneLightObject.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneDummyObject::CSceneDummyObject()
{
	m_fScale = 1.0f;
	m_fWidth = 1.0f;
	m_nObjectType = SO_TYPE_DUMMY;
	m_vPos = A3DVECTOR3(0.0f);
	BuildProperty();
}

CSceneDummyObject::~CSceneDummyObject()
{

}

void CSceneDummyObject::Render(A3DViewport* pA3DViewport)
{
	CBox3D renderBox;
	A3DVECTOR3 vPos = m_matTrans.GetRow(3);
	A3DVECTOR3 vScreenPos,vPosEx,vScreenPosEx;
	m_fScale = m_fScaleUnit;
	
	DrawCenter();
	
	renderBox.Init(&g_Render,m_fWidth,true);
	renderBox.SetDirAndUp(A3DVECTOR3(0,0,1.0f),A3DVECTOR3(0,1.0f,0));
	if(m_bSelected)
		renderBox.SetColor(A3DCOLORRGBA(255,0,0,64));
	else renderBox.SetColor(A3DCOLORRGBA(255,255,255,64));
	renderBox.SetPos(m_vPos);
	renderBox.SetSize(m_fWidth);
	renderBox.Render();
	renderBox.Release();
	
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector) pWireCollector->Flush();
	
	CSceneObject::Render(pA3DViewport);
}

void CSceneDummyObject::UpdateProperty(bool bGet)
{
	if(bGet)
	{
		AString temp = AString(m_pProperty->GetPropVal(0));
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			CSceneObject*pObj = pSManager->FindSceneObject(temp);
			if(pObj != NULL && pObj != this)
			{
				MessageBox(NULL,"该对象的名字已经存在，编辑器将恢复原对象名字！",NULL,MB_OK);
				m_pProperty->SetPropVal(0,m_strName);
				m_pProperty->SetPropVal(1,m_vPos);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName	= temp;
		m_vPos	= m_pProperty->GetPropVal(1);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
	}
}

void CSceneDummyObject::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置");
	}
}

CSceneObject* CSceneDummyObject::CopyObject()
{
	CSceneDummyObject *pNewObject = new CSceneDummyObject(*this);
	ASSERT(pNewObject);
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildProperty();
	int DummyNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Dummy_%d",DummyNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				DummyNum++;
			}else 
			{
				DummyNum++;
				break;
			}
		}
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->SetPos(m_vPos +	A3DVECTOR3(0.1f,0.0f,0.1f));
		
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	return pNewObject;
}

//	Load data
bool CSceneDummyObject::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	if(dwVersion>2 && dwVersion<=0xd)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}else if(dwVersion>0xd)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}
	return true;
}
//	Save data
bool CSceneDummyObject::Save(CELArchive& ar,DWORD dwVersion)
{
	if(dwVersion>2)
	{
		ar.WriteString(m_strName);
		if(!ar.Write(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
	}
	return true;
}
