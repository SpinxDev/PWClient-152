// SceneSkinModel.cpp: implementation of the CSceneSkinModel class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"

#include "elementeditor.h"
#include "ElementMap.h"

#include "SceneObjectManager.h"
#include "SceneSkinModel.h"
#include "Render.h"
#include "MainFrm.h"
#include "Box3D.h"
#include "EC_Model.h"
#include "a3d.h"

#include <A3DCombinedAction.h>




//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneSkinModel::CSceneSkinModel()
{
	m_pECModel = NULL;
	m_nObjectType = SO_TYPE_ECMODEL;
	m_bInitedEx = false;
	m_Property.fAlphaFlags = -1.0f;
	BuildPropery();
}

CSceneSkinModel::~CSceneSkinModel()
{

}

void CSceneSkinModel::Release()
{
	if(m_pECModel)
	{
		m_pECModel->Release();
		delete m_pECModel;
		m_pECModel = NULL;
	}
}

A3DVECTOR3 CSceneSkinModel::GetMin()
{
	A3DAABB aabb;

	if(m_pECModel) aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
	return aabb.Mins;
}

A3DVECTOR3 CSceneSkinModel::GetMax()
{
	A3DAABB aabb;
	if(m_pECModel) 
	aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
	return aabb.Maxs;
}

void CSceneSkinModel::Translate(const A3DVECTOR3& vDelta)
{
	if(m_pECModel == NULL) return;
	m_vPos = m_pECModel->GetA3DSkinModel()->GetPos();
	m_vPos +=vDelta;
	m_pECModel->GetA3DSkinModel()->SetPos(m_vPos);
}

void CSceneSkinModel::RotateX(float fRad)
{
	if(m_pECModel == NULL) return;

	m_pECModel->GetA3DSkinModel()->RotateX(fRad);
}

void CSceneSkinModel::RotateY(float fRad)
{
	if(m_pECModel == NULL) return;
	m_pECModel->GetA3DSkinModel()->RotateY(fRad);
}

void CSceneSkinModel::RotateZ(float fRad)
{
	if(m_pECModel == NULL) return;
	m_pECModel->GetA3DSkinModel()->RotateZ(fRad);
}

void CSceneSkinModel::Rotate(const A3DVECTOR3& axis,float fRad)
{
	if(m_pECModel == NULL) return;
	A3DMATRIX4 mat;
	mat.Identity();
	mat.RotateAxis(axis,fRad);
	
	A3DVECTOR3 vDir = m_pECModel->GetA3DSkinModel()->GetDir();
	A3DVECTOR3 vUp = m_pECModel->GetA3DSkinModel()->GetUp();
	vDir = mat * vDir;
	vUp = mat * vUp;
	m_pECModel->GetA3DSkinModel()->SetDirAndUp(vDir,vUp);
}

void CSceneSkinModel::ResetRotateMat()
{
	if(m_pECModel == NULL) return;
	m_pECModel->GetA3DSkinModel()->SetDirAndUp(A3DVECTOR3(0,0,1.0f),A3DVECTOR3(0,1.0f,0));

}

void CSceneSkinModel::Scale(float fScale)
{
	///A3DVECTOR3 vScale = m_pECModel->GetScale();
	//vScale *= fScale; 
	//m_pECModel->SetScale(vScale.x,vScale.y,vScale.z);
}

void CSceneSkinModel::SetPos(const A3DVECTOR3& vPos)
{
	if(m_pECModel == NULL) return;
	m_pECModel->SetPos(vPos);
	CSceneObject::SetPos(vPos);
}

void CSceneSkinModel::UpdateProperty(bool bGet)
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
				m_pProperty->SetPropVal(2,m_Property.fAlphaFlags);
				if(m_bInitedEx) m_pProperty->SetPropVal(3,m_Property.dwActionID);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName = temp;
		m_vPos	= m_pProperty->GetPropVal(1);
		m_Property.fAlphaFlags = m_pProperty->GetPropVal(2);
		if(m_bInitedEx)
		{
			m_Property.dwActionID = m_pProperty->GetPropVal(3);
			m_Property.strActionName = GetActNameByIndex(m_Property.dwActionID);
			if(m_pECModel != NULL)
			{
				m_pECModel->StopAllActions();
				m_pECModel->PlayActionByName(m_Property.strActionName,1.0f);
			}
		}
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_Property.fAlphaFlags);
		if(m_bInitedEx) m_pProperty->SetPropVal(3,m_Property.dwActionID);
	}
}

A3DVECTOR3 CSceneSkinModel::GetPos()
{
	if(m_pECModel == NULL) return A3DVECTOR3(0);
	return m_pECModel->GetA3DSkinModel()->GetPos();
}

A3DMATRIX4 CSceneSkinModel::GetTransMatrix()
{
	A3DMATRIX4 mat;
	if(m_pECModel == NULL) return mat;
	return m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
}

A3DMATRIX4 CSceneSkinModel::GetRotateMatrix()
{
	A3DMATRIX4 mat;
	mat.Identity();
	if(m_pECModel==NULL) return mat;

	mat = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
	mat.SetRow(3,A3DVECTOR3(0.0f));
	A3DVECTOR3 vecX = mat.GetRow(0);
	A3DVECTOR3 vecY = mat.GetRow(1);
	A3DVECTOR3 vecZ = mat.GetRow(2);
	vecX.Normalize();
	vecY.Normalize();
	vecZ.Normalize();
	mat.SetRow(0, vecX);
	mat.SetRow(1, vecY);
	mat.SetRow(2, vecZ);
	return mat;
}

A3DMATRIX4 CSceneSkinModel::GetScaleMatrix()
{
	A3DMATRIX4 mat;
	mat.Identity();
	//A3DVECTOR3 v = m_pECModel->GetScale();
	//mat._11 = v.x;
	//mat._22 = v.y;
	//mat._33 = v.z;
	return mat; 
}

bool CSceneSkinModel::LoadSkinModel(AString strPathName)
{
	if(m_pECModel)
	{
		m_pECModel->Release();
	}else
	{
		m_pECModel = new CECModel;
	}
	bool bResult = m_pECModel->Load(strPathName,true,0,true,true,false);
	if(!bResult)
	{
		g_Log.Log("CSceneSkinModel::LoadSkinModel(), Failed load %s", strPathName);
		delete m_pECModel;
		m_pECModel = NULL;
		return false;
	}
	m_pECModel->GetA3DSkinModel()->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);
	
	return true;
}

void CSceneSkinModel::BuildPropery()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_Property.fAlphaFlags),"透明程度");
	}
}

void CSceneSkinModel::BuildPropertyEx()
{
	if(m_pECModel && !m_bInitedEx)
	{
		int n = m_pECModel->GetComActCount();
		m_pECModel->PlayActionByName(m_Property.strActionName,1.0f);
		for( int i = 0; i < n; i ++ )
		{
			A3DCombinedAction *pAct = m_pECModel->GetComActByIndex(i);
			AString strActName = pAct->GetName();
			act_type.AddElement(strActName,i);
			if(strcmp(strActName,m_Property.strActionName)==0)
				m_Property.dwActionID = i;
		}
		m_pProperty->DynAddProperty(AVariant(m_Property.dwActionID), "播放动作", &act_type);
		m_bInitedEx = true;
	}
}

AString CSceneSkinModel::GetActNameByIndex(int index)
{
	AString strActName;
	if(m_pECModel)
	{
		A3DCombinedAction *pAct = m_pECModel->GetComActByIndex(index);
		strActName = pAct->GetName();
	}
	return strActName;
}

void CSceneSkinModel::Render(A3DViewport* pA3DViewport)
{
	if(m_pECModel==NULL) return;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	LIGHTINGPARAMS light = pMap->GetLightingParams();
	
	CBox3D renderBox;
	renderBox.Init(&g_Render,1.0f,false);
	A3DAABB aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
	renderBox.SetSize(aabb.Extents.x,aabb.Extents.y,aabb.Extents.z);
	renderBox.SetPos(aabb.Center);
	renderBox.SetColor(A3DCOLORRGB(255,0,0));
	if(m_bSelected) renderBox.Render();
	A3DLIGHTPARAM lightParams = g_Render.GetDirLight()->GetLightparam();
	A3DSkinModel::LIGHTINFO LightInfo;
	LightInfo.colAmbient	= g_Render.GetA3DDevice()->GetAmbientColor();
	LightInfo.vLightDir		= lightParams.Direction;//Direction of directional light
	LightInfo.colDirDiff	= lightParams.Diffuse;//Directional light's diffuse color
	LightInfo.colDirSpec	= lightParams.Specular;//	Directional light's specular color
	LightInfo.bPtLight		= false;//	false, disable dynamic point light,
	m_pECModel->GetA3DSkinModel()->SetLightInfo(LightInfo);
	m_pECModel->Render(pA3DViewport);

	CSceneObject::Render(pA3DViewport);
}

void CSceneSkinModel::RenderForLight(A3DViewport* pA3DViewport, float fOffsetX, float fOffsetZ)
{
	if(m_pECModel==0) return;
	A3DLIGHTPARAM lightParams = g_Render.GetDirLight()->GetLightparam();
	A3DSkinModel::LIGHTINFO LightInfo;
	LightInfo.colAmbient	= g_Render.GetA3DDevice()->GetAmbientColor();
	LightInfo.vLightDir		= lightParams.Direction;//Direction of directional light
	LightInfo.colDirDiff	= lightParams.Diffuse;//Directional light's diffuse color
	LightInfo.colDirSpec	= lightParams.Specular;//	Directional light's specular color
	LightInfo.bPtLight		= false;//	false, disable dynamic point light,
	m_pECModel->GetA3DSkinModel()->SetLightInfo(LightInfo);
	m_pECModel->SetPos(m_vPos + A3DVECTOR3(fOffsetX,0.0f,fOffsetZ));
	m_pECModel->Render(pA3DViewport);
	m_pECModel->SetPos(m_vPos);
}

void CSceneSkinModel::RenderForLightMapShadow(A3DViewport* pA3DViewport)
{
	if(m_pECModel==0) return;
	A3DLIGHTPARAM lightParams = g_Render.GetDirLight()->GetLightparam();
	A3DSkinModel::LIGHTINFO LightInfo;
	LightInfo.colAmbient	= g_Render.GetA3DDevice()->GetAmbientColor();
	LightInfo.vLightDir		= lightParams.Direction;//Direction of directional light
	LightInfo.colDirDiff	= lightParams.Diffuse;//Directional light's diffuse color
	LightInfo.colDirSpec	= lightParams.Specular;//	Directional light's specular color
	LightInfo.bPtLight		= false;//	false, disable dynamic point light,
	m_pECModel->GetA3DSkinModel()->SetLightInfo(LightInfo);
	m_pECModel->Render(pA3DViewport);
}

void CSceneSkinModel::Tick(DWORD timeDelta)
{
	if(m_pECModel)
	{
		m_pECModel->Tick(timeDelta);
	}
}

//	Load data
bool CSceneSkinModel::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	if(dwVersion>6 && dwVersion<=0xd)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_Property.strPathName = AString(ar.ReadString());
		m_Property.strActionName = AString(ar.ReadString());
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
		//if(iLoadFlags!= LOAD_EXPLIGHTMAP)
		{
			if(!LoadSkinModel(m_Property.strPathName))
			{
				CString msg;
				g_Log.Log(msg);
				msg.Format("CSceneSkinModel::Load(),Failed to Load skin model %s !",m_Property.strPathName);
				AfxMessageBox(msg);
				return false;
			}
			BuildPropertyEx();
			m_pECModel->GetA3DSkinModel()->SetPos(m_vPos);
			m_pECModel->GetA3DSkinModel()->SetDirAndUp(vr,vu);
			if(!m_Property.strActionName.IsEmpty())
				m_pECModel->PlayActionByName(m_Property.strActionName,1.0f);
		}
	}else if(dwVersion>0xd && dwVersion <= 0x12)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_Property.strPathName = AString(ar.ReadString());
		m_Property.strActionName = AString(ar.ReadString());
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
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		//if(iLoadFlags!= LOAD_EXPLIGHTMAP)
		{
			
			if(!LoadSkinModel(m_Property.strPathName))
			{
				CString msg;
				g_Log.Log(msg);
				msg.Format("CSceneSkinModel::Load(),Failed to Load skin model %s !",m_Property.strPathName);
				AfxMessageBox(msg);
				return false;
			}
			BuildPropertyEx();
			m_pECModel->GetA3DSkinModel()->SetPos(m_vPos);
			m_pECModel->GetA3DSkinModel()->SetDirAndUp(vr,vu);
			if(!m_Property.strActionName.IsEmpty())
				m_pECModel->PlayActionByName(m_Property.strActionName,1.0f);
		}
	}else if(dwVersion > 0x12)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
		m_Property.strPathName = AString(ar.ReadString());
		m_Property.strActionName = AString(ar.ReadString());
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
		if(!ar.Read(&m_Property.fAlphaFlags,sizeof(float)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		
		if(iLoadFlags!= LOAD_EXPLIGHTMAP)
		{
			
			if(!LoadSkinModel(m_Property.strPathName))
			{
				CString msg;
				
				msg.Format("CSceneSkinModel::Load(),Failed to Load skin model %s !",m_Property.strPathName);
				g_Log.Log(msg);
				AfxMessageBox(msg);
				return false;
			}
			if(m_pECModel)
			{
				BuildPropertyEx();
				m_pECModel->GetA3DSkinModel()->SetPos(m_vPos);
				m_pECModel->GetA3DSkinModel()->SetDirAndUp(vr,vu);
				if(!m_Property.strActionName.IsEmpty())
					m_pECModel->PlayActionByName(m_Property.strActionName,1.0f);
			}
		}
	}
	return true;
}
//	Save data
bool CSceneSkinModel::Save(CELArchive& ar,DWORD dwVersion)
{
	if(dwVersion>6)
	{
		m_matRotate = GetRotateMatrix();
		A3DVECTOR3 vr,vu,vl,vs;
		vr = m_matRotate.GetRow(2);
		vu = m_matRotate.GetRow(1);
		vl = m_matRotate.GetRow(0);
		vs = A3DVECTOR3(m_matScale._11,m_matScale._22,m_matScale._33);
		ar.WriteString(m_strName);
			
		ar.WriteString(m_Property.strPathName);
		
		ar.WriteString(m_Property.strActionName);
		
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
		if(!ar.Write(&m_Property.fAlphaFlags,sizeof(float)))
			return false;
	}
	
	return true;
}

void CSceneSkinModel::SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up)
{
	if(m_pECModel) m_pECModel->SetDirAndUp(dir,up);
	CSceneObject::SetDirAndUp(dir,up);
}

