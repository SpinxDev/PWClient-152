// CloudBox.cpp: implementation of the CCloudBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "CloudBox.h"
#include "A3D.h"
#include "box3d.h"
#include "Render.h"

//#define new A_DEBUG_NEW

void init_cloud_data(CLOUD_BOX_DATA &data)
{
	data.fEdgeHei = 20;
	data.fEdgeLen = 20;
	data.fEdgeWth = 20;
	data.nGroup   = 1;
	data.nType = 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCloudBox::CCloudBox()
{

	m_nObjectType = SO_TYPE_CLOUD_BOX;
	init_cloud_data(m_Data);
	cloud_type.AddElement("base",0);
	cloud_type.AddElement("stratus",1);
	cloud_type.AddElement("solid",2);
	cloud_type.AddElement("wispy",3);
	pRandSeedFunc = new CLOUD_RAND_SEED_FUNCS;
	BuildProperty();
}

CCloudBox::~CCloudBox()
{
	if(pRandSeedFunc) delete pRandSeedFunc;
}

void CCloudBox::Render(A3DViewport* pA3DViewport)
{
	CSceneObject::DrawCenter();
	
	if(m_bSelected) 
	{
		CBox3D renderBox;
		CBox3D renderWire;
		renderBox.Init(&g_Render,1.0f,true);
		renderWire.Init(&g_Render,1.0f,false);
		renderBox.SetSize(m_Data.fEdgeWth/2.0f,m_Data.fEdgeHei/2.0f,m_Data.fEdgeLen/2.0f);
		renderWire.SetSize(m_Data.fEdgeWth/2.0f,m_Data.fEdgeHei/2.0f,m_Data.fEdgeLen/2.0f);
		renderBox.SetTM(m_matTrans);
		renderWire.SetTM(m_matTrans);
		renderWire.SetColor(A3DCOLORRGBA(0,0,255,255));
		renderBox.SetColor(A3DCOLORRGBA(255,0,0,128));
		renderBox.Render();
		renderWire.Render();
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		pWireCollector->Flush();
	}
	CSceneObject::Render(pA3DViewport);
}

void CCloudBox::GetAABB(A3DVECTOR3 &vMin, A3DVECTOR3 &vMax)
{
	vMin = A3DVECTOR3(m_vPos.x - m_Data.fEdgeWth/2.0f, m_vPos.y - m_Data.fEdgeHei/2.0f, m_vPos.z - m_Data.fEdgeLen/2.0f);
	vMax = A3DVECTOR3(m_vPos.x + m_Data.fEdgeWth/2.0f, m_vPos.y + m_Data.fEdgeHei/2.0f, m_vPos.z + m_Data.fEdgeLen/2.0f);	
}

void CCloudBox::Tick(DWORD timeDelta)
{

}

void CCloudBox::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_Data.fEdgeHei), "盒子高(y)");
		m_pProperty->DynAddProperty(AVariant(m_Data.fEdgeLen),"盒子长(z)");
		m_pProperty->DynAddProperty(AVariant(m_Data.fEdgeWth), "盒子宽(x)");
		m_pProperty->DynAddProperty(AVariant(m_Data.nGroup), "所属组");
		m_pProperty->DynAddProperty(AVariant(m_Data.nType),"类型",&cloud_type);
		m_pProperty->DynAddProperty(AVariant(strSeed), "随机种子", (ASet*)pRandSeedFunc, NULL, WAY_CUSTOM);
	}
}

void CCloudBox::UpdateProperty(bool bGet)
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
				m_pProperty->SetPropVal(2,m_Data.fEdgeHei);
				m_pProperty->SetPropVal(3,m_Data.fEdgeLen);
				m_pProperty->SetPropVal(4,m_Data.fEdgeWth);
				m_pProperty->SetPropVal(5,m_Data.nGroup);
				m_pProperty->SetPropVal(6,m_Data.nType);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName				= temp;
		m_vPos					= m_pProperty->GetPropVal(1);
		m_Data.fEdgeHei		= m_pProperty->GetPropVal(2);
		m_Data.fEdgeLen		= m_pProperty->GetPropVal(3);
		m_Data.fEdgeWth		= m_pProperty->GetPropVal(4);
		m_Data.nGroup	    = m_pProperty->GetPropVal(5);
		m_Data.nType        = m_pProperty->GetPropVal(6);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
	}else
	{
	
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_Data.fEdgeHei);
		m_pProperty->SetPropVal(3,m_Data.fEdgeLen);
		m_pProperty->SetPropVal(4,m_Data.fEdgeWth);
		m_pProperty->SetPropVal(5,m_Data.nGroup);
		m_pProperty->SetPropVal(6,m_Data.nType);
		pRandSeedFunc->OnSetValue("");
	}
}

void CCloudBox::Release()
{

}

void CCloudBox::RotateX(float fRad)
{

}
void CCloudBox::RotateY(float fRad)
{

}
void CCloudBox::RotateZ(float fRad)
{
	
}
void CCloudBox::Rotate(A3DVECTOR3 axis,float fRad)
{

}

void CCloudBox::EntryScene()
{

}

void CCloudBox::LeaveScene()
{

}
//  Copy object
CSceneObject* CCloudBox::CopyObject()
{
	CCloudBox *pNewObject = new CCloudBox(*this);
	ASSERT(pNewObject);
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildProperty();
	int CloudNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("CloudBox_%d",CloudNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				CloudNum++;
			}else 
			{
				CloudNum++;
				break;
			}
		}
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->SetPos(m_vPos);
		
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	return pNewObject;
}

//	Load data
bool CCloudBox::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	if(dwVersion>25)
	{
		m_strName = ar.ReadString();
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.dwRandSeed,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.fEdgeHei,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeLen,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeWth,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.nGroup,sizeof(int)))
			return false;
		if(!ar.Read(&m_Data.nType,sizeof(int)))
			return false;
	}
	m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
	return true;
}

//	Save data
bool CCloudBox::Save(CELArchive& ar,DWORD dwVersion)
{
	ar.WriteString(m_strName);
	if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
		return false;
	if(!ar.Write(&m_Data.dwRandSeed,sizeof(DWORD)))
		return false;
	if(!ar.Write(&m_Data.fEdgeHei,sizeof(float)))
		return false;
	if(!ar.Write(&m_Data.fEdgeLen,sizeof(float)))
		return false;
	if(!ar.Write(&m_Data.fEdgeWth,sizeof(float)))
		return false;
	if(!ar.Write(&m_Data.nGroup,sizeof(int)))
		return false;
	if(!ar.Write(&m_Data.nType,sizeof(int)))
		return false;
	return true;
}

BOOL CALLBACK CLOUD_RAND_SEED_FUNCS::OnActivate(void)
{
	m_strText = "...";
	CString strID;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CCloudBox* pObj = (CCloudBox*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_CLOUD_BOX)
			{
				if(IDYES==AfxMessageBox("你要重新生成随机种子吗？",MB_YESNO))
				{
					DWORD seed = 9999999 * rand()/RAND_MAX;
					CLOUD_BOX_DATA dat = pObj->GetProperty();
					dat.dwRandSeed = seed;
					pObj->SetProperty(dat);
					pMap->RecreateCloudGroup(dat.nGroup);
				}
			}
		}
	}
	return TRUE;
}

LPCTSTR CALLBACK CLOUD_RAND_SEED_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK CLOUD_RAND_SEED_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK CLOUD_RAND_SEED_FUNCS::OnSetValue(const AVariant& var)
{
	m_strText = "...";
}
