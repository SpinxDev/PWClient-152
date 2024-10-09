// SceneSpeciallyObject.cpp: implementation of the CSceneSpeciallyObject class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"


#include "elementeditor.h"
#include "SceneObjectManager.h"
#include "SceneLightObject.h"
#include "Box3D.h"
#include "Render.h"
#include "A3D.h"
#include "MainFrm.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "SceneSpeciallyObject.h"
#include "LogDlg.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneSpeciallyObject::CSceneSpeciallyObject()
{
	m_nObjectType = CSceneObject::SO_TYPE_SPECIALLY;
	m_fPlaySpeed = 1.0f;
	m_nGfxType = GFX_TYPE_BOTH;
	m_bAttenuation = true;
	m_fTransparency = 1.0f;
	m_fOldAlpha = 1.0f;
	m_pGfxEx = NULL;
	m_strGfxPathName = "level_up.gfx";
	//m_matRotate.RotateX(A3D_PI/2.0f);
	gfx_type.AddElement("白天", 0);
	gfx_type.AddElement("黑夜", 1);
	gfx_type.AddElement("白天黑夜", 2);
	BuildProperty();
	m_bLoading = true;
}

CSceneSpeciallyObject::~CSceneSpeciallyObject()
{
}

void CSceneSpeciallyObject::Render(A3DViewport* pA3DViewport)
{
	DrawCenter();
	CSceneObject::Render(pA3DViewport);
}

void CSceneSpeciallyObject::Tick(DWORD timeDelta)
{
	if(!g_Configs.bShowSpecially) return;
	if(m_pGfxEx)
	{	
		m_pGfxEx->GetAlpha();
		if(m_nGfxType==GFX_TYPE_DAY)
		{
			m_pGfxEx->SetAlpha(m_fOldAlpha*(1.0f - g_fNightFactor)*m_fTransparency);
		}
		else if(m_nGfxType==GFX_TYPE_NIGHT)
		{
			m_pGfxEx->SetAlpha(m_fOldAlpha*g_fNightFactor*m_fTransparency);
		}else m_pGfxEx->SetAlpha(m_fOldAlpha*m_fTransparency);
		float scale = m_matScale._11;
		m_pGfxEx->SetScale(scale);
		m_pGfxEx->SetParentTM(m_matRotate * m_matTrans);
		
		m_pGfxEx->TickAnimation(timeDelta);
		AfxGetGFXExMan()->RegisterGfx(m_pGfxEx);
	}
}

void CSceneSpeciallyObject::UpdateProperty(bool bGet)
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
				AfxMessageBox("该对象的名字已经存在，编辑器将恢复原对象名字！",NULL,MB_OK);
				m_pProperty->SetPropVal(0,m_strName);
				m_pProperty->SetPropVal(1,m_vPos);
				m_pProperty->SetPropVal(2,m_strGfxPathName);
				m_pProperty->SetPropVal(3,m_fPlaySpeed);
				m_pProperty->SetPropVal(4,m_nGfxType);
				m_pProperty->SetPropVal(5,m_fTransparency);
				m_pProperty->SetPropVal(6,m_bAttenuation);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName	        = temp;
		m_vPos	            = m_pProperty->GetPropVal(1);
		m_strGfxPathName	= AString(m_pProperty->GetPropVal(2));
		m_fPlaySpeed        = m_pProperty->GetPropVal(3);
		m_nGfxType          = m_pProperty->GetPropVal(4);
		m_fTransparency     = m_pProperty->GetPropVal(5);
		m_bAttenuation      = m_pProperty->GetPropVal(6);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		UpdateGfx();
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_strGfxPathName);
		m_pProperty->SetPropVal(3,m_fPlaySpeed);
		m_pProperty->SetPropVal(4,m_nGfxType);
		m_pProperty->SetPropVal(5,m_fTransparency);
		m_pProperty->SetPropVal(6,m_bAttenuation);
	}
}

void CSceneSpeciallyObject::Release()
{
	if(m_pGfxEx)
		m_pGfxEx->Stop();
	
	if(m_pGfxEx) 
	{
		m_pGfxEx->Release();
		delete m_pGfxEx;
	}
	
	CSceneObject::Release();
}

void CSceneSpeciallyObject::EntryScene()
{
	if(m_pGfxEx)
		m_pGfxEx->Start();
}

void CSceneSpeciallyObject::LeaveScene()
{
	if(m_pGfxEx)
		m_pGfxEx->Stop();
}

void  CSceneSpeciallyObject::UpdateGfx()
{
	
	if(m_pGfxEx)
	{
		m_pGfxEx->Stop();
		m_pGfxEx->Release();
		delete m_pGfxEx;
		m_pGfxEx = NULL;
	}
	if(!m_bLoading) return;
	AString strPath = m_strGfxPathName;
	strPath.MakeLower();
	if(strPath.Find(".gfx")==-1) return;
	m_pGfxEx = AfxGetGFXExMan()->LoadGfx(g_Render.GetA3DDevice(),strPath);
	if(m_pGfxEx==NULL)
	{
		AString msg;
		msg.Format("CSceneSpeciallyObject::UpdateGfx(), Load %s file failed!(%s)",m_strGfxPathName, GetObjectName());
		g_Log.Log(msg);
		g_LogDlg.Log(CString(msg));
		return;
	}
	m_fOldAlpha = m_pGfxEx->GetAlpha();
	m_pGfxEx->SetPlaySpeed(m_fPlaySpeed);
	m_pGfxEx->Start(true);
	
}

void CSceneSpeciallyObject::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_strGfxPathName), "GFX文件路径",NULL,NULL,WAY_FILENAME,"Gfx");
		m_pProperty->DynAddProperty(AVariant(m_fPlaySpeed),"播放速度");
		m_pProperty->DynAddProperty(AVariant(m_nGfxType), "播放时间", &gfx_type);
		m_pProperty->DynAddProperty(AVariant(m_fTransparency),"透明程度(0-1)");
		m_pProperty->DynAddProperty(AVariant(m_bAttenuation),"随距离衰减");
	}
}

CSceneObject* CSceneSpeciallyObject::CopyObject()
{
	CSceneSpeciallyObject *pNewObject = new CSceneSpeciallyObject(*this);
	ASSERT(pNewObject);
	pNewObject->SetGfxNull();
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildProperty();
	pNewObject->UpdateGfx();
	int SpeciallyNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("GFX_%d",SpeciallyNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				SpeciallyNum++;
			}else 
			{
				SpeciallyNum++;
				break;
			}
		}
		pNewObject->m_nObjectID = pSManager->GenerateObjectID();
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->SetPos(m_vPos);
		
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	UpdateGfx();
	return pNewObject;
}

//Load data
bool CSceneSpeciallyObject::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	if(dwVersion>7 && dwVersion<=0xc)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_strGfxPathName = AString(ar.ReadString());
		
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_fPlaySpeed = 1.0f;
		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGfx();	
	}else if( dwVersion > 0xc && dwVersion <= 0xd)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_strGfxPathName = AString(ar.ReadString());
		
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_fPlaySpeed,sizeof(float)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;

		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGfx();	
	}else if(dwVersion == 0xe)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_strGfxPathName = AString(ar.ReadString());
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_fPlaySpeed,sizeof(float)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;

		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGfx();	
	}else if(dwVersion > 0xe && dwVersion <=0x2d)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_strGfxPathName = AString(ar.ReadString());
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_fPlaySpeed,sizeof(float)))
			return false;
		if(!ar.Read(&m_nGfxType,sizeof(int)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;

		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGfx();	
	}else if(dwVersion > 0x2d)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_strGfxPathName = AString(ar.ReadString());
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_fPlaySpeed,sizeof(float)))
			return false;
		if(!ar.Read(&m_nGfxType,sizeof(int)))
			return false;
		if(!ar.Read(&m_fTransparency,sizeof(float)))
			return false;
		if(!ar.Read(&m_bAttenuation,sizeof(bool)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;

		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGfx();	
	}
	
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	return true;	
}

//Save data
bool CSceneSpeciallyObject::Save(CELArchive& ar,DWORD dwVersion)
{
	if(dwVersion>7)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		vr = m_matRotate.GetRow(2);
		vu = m_matRotate.GetRow(1);
		vl = m_matRotate.GetRow(0);
		vs = A3DVECTOR3(m_matScale._11,m_matScale._22,m_matScale._33);
		ar.WriteString(m_strName);
			
		ar.WriteString(m_strGfxPathName);
		if(!ar.Write(&m_nObjectID,sizeof(int)))
			return false;	
		if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vs,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&m_fPlaySpeed,sizeof(float)))
			return false;
		if(!ar.Write(&m_nGfxType,sizeof(int)))
			return false;
		if(!ar.Write(&m_fTransparency,sizeof(float)))
			return false;
		if(!ar.Write(&m_bAttenuation,sizeof(bool)))
			return false;
	}
	return true;
}