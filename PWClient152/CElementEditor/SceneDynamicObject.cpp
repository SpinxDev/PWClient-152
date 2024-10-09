#include "global.h"
#include "elementeditor.h"
#include "SceneDynamicObject.h"
#include "SceneObjectManager.h"
#include "DynamicObjectSelDlg.h"
#include "MainFrm.h"
#include "DynamicObjectManDlg.h"
#include "ElementMap.h"
#include "SceneObjectManager.h"
#include "EC_Model.h"
#include "Box3D.h"
#include "NpcControllerSelDlg.h"
#include "NpcControllerManDlg.h"

#include "render.h"
#include <a3d.h>
#include <A3DGFXEx.h>
#include <A3DGFXExMan.h>

//	增加凸包需要的内容（拷贝自 GfxCommon 库）
static char* ECMPathFindExtensionA(const char* szPath)
{
	char* ret = NULL;
	int i = 0;
	char ch;
	
	while ((ch = szPath[i]) != 0)
	{
		if (::IsDBCSLeadByteEx(936, ch))
		{
			i++;
			
			if (szPath[i] == 0)
				break;
			
			i++;
			continue;
		}
		
		if (ch == '.')
			ret = (char*)szPath + i;
		
		i++;
	}
	
	if (ret)
		return ret;
	
	return (char*)szPath + i;
}
struct ECModelBrushProjHead_t
{
				unsigned int	nMagic;
				unsigned int	nVersion;
				char			szECModelFile[MAX_PATH];
				int				nFrame;
				unsigned int	nNumHull;
};
#define ECMODEL_PROJ_VERSION (unsigned int) (0x00000001)
//	结束凸包增加内容

CSceneDynamicObject::CSceneDynamicObject()
{
	m_uDynamicObjectID = 0;
	m_pIDSelFuncs = new DYNAMIC_SEL_FUNCS;
	m_pControllerSelFuncs = new CONTROLLER_DYNAMIC_ID_FUNCS;
	m_nObjectType = SO_TYPE_DYNAMIC;
	BuildProperty();
	m_pGfxEx = 0;
	m_pECModel = 0;
	m_uControllerID = 0;
}

CSceneDynamicObject::~CSceneDynamicObject()
{
}

void CSceneDynamicObject::Release()
{
	if(m_pIDSelFuncs) delete m_pIDSelFuncs;
	if(m_pControllerSelFuncs) delete m_pControllerSelFuncs;
	if(m_pGfxEx)
	{
		m_pGfxEx->Stop();
		m_pGfxEx->Release();
		delete m_pGfxEx;
		m_pGfxEx = NULL;
	}

	if(m_pECModel)
	{
		ReleaseHulls();
		m_pECModel->Release();
		delete m_pECModel;
		m_pECModel = NULL;
	}
	
	CSceneObject::Release();	
}


void CSceneDynamicObject::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(""), "动态对象", (ASet*)m_pIDSelFuncs, NULL, WAY_CUSTOM);
		m_pProperty->DynAddProperty(AVariant(""), "关联怪物控制器", (ASet*)m_pControllerSelFuncs, NULL, WAY_CUSTOM);
	}
}

void CSceneDynamicObject::Render(A3DViewport* pA3DViewport)
{
	DrawCenter();
	
	if(m_pECModel)
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		LIGHTINGPARAMS light = pMap->GetLightingParams();
		
		CBox3D renderBox;
		renderBox.Init(&g_Render,1.0f,false);
		A3DAABB aabb = m_pECModel->GetA3DSkinModel()->GetModelAABB();
		renderBox.SetSize(aabb.Extents.x,aabb.Extents.y,aabb.Extents.z);
		renderBox.SetPos(aabb.Center);
		renderBox.SetColor(A3DCOLORRGB(255,0,0));
		if(m_bSelected)	renderBox.Render();
		if (g_Configs.bShowModelHull2){
			if (!m_ECMHulls.empty()){
				A3DFlatCollector *pFlatCollector = g_Render.GetA3DEngine()->GetA3DFlatCollector();
				for(size_t i=0; i< m_ECMHulls.size(); i++)
				{
					if(m_ECMHulls[i]->GetFlags() & CHDATA_NPC_CANNOT_PASS)
						m_ECMHulls[i]->Render(pFlatCollector,FALSE, NULL,0x00000000, 0xa0ff0000);
					else
						m_ECMHulls[i]->Render(pFlatCollector);
				}
				pFlatCollector->Flush();
			}
		}
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
	
	CSceneObject::Render(pA3DViewport);
}

void CSceneDynamicObject::Tick(DWORD timeDelta)
{
	if(m_pECModel) m_pECModel->Tick(timeDelta);
	if(m_pGfxEx)
	{	
		float scale = m_matScale._11;
		m_pGfxEx->SetScale(scale);
		m_pGfxEx->SetParentTM(m_matRotate * m_matTrans);
		m_pGfxEx->TickAnimation(timeDelta);
		AfxGetGFXExMan()->RegisterGfx(m_pGfxEx);
	}
}

void CSceneDynamicObject::UpdateProperty(bool bGet)
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
				m_pProperty->SetPropVal(2,m_uDynamicObjectID);
				m_pIDSelFuncs->OnSetValue("");
				m_pControllerSelFuncs->OnSetValue("");
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName	        = temp;
		m_vPos	            = m_pProperty->GetPropVal(1);
		//m_uDynamicObjectID	= m_pProperty->GetPropVal(2);
		//m_uControllerID = m_pProperty->GetPropVal(3);
		ReloadModels();
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_uDynamicObjectID);
		m_pIDSelFuncs->OnSetValue("");
		m_pControllerSelFuncs->OnSetValue("");
	}
}

void CSceneDynamicObject::ReleaseHulls()
{
	if (!m_ECMHulls.empty()){
		for (size_t i = 0; i < m_ECMHulls.size(); i++)
			delete m_ECMHulls[i];
		m_ECMHulls.clear();
	}
}

void CSceneDynamicObject::LoadHulls(const char *szECMPath)
{
	const char* szPath = strstr(szECMPath, "\\");
	if (!szPath) return;
	
	char szProjFile[MAX_PATH];
	strcpy(szProjFile, "grasses\\ECModelHull");
	strcat(szProjFile, szPath);
	
	char* ext = ECMPathFindExtensionA(szProjFile);
	if (*ext==0) return;
	strcpy(ext, ".ecp");
	
	AFileImage file;
	if (!file.Open(szProjFile, AFILE_OPENEXIST | AFILE_BINARY))
		return;
	
	ECModelBrushProjHead_t ProjHead;
	DWORD dwLen;			
	file.Read(&ProjHead, sizeof(ProjHead), &dwLen);	
	if (ProjHead.nVersion == ECMODEL_PROJ_VERSION && ProjHead.nNumHull > 0)
	{
		for (unsigned i = 0; i < ProjHead.nNumHull; i++)
		{
			CHBasedCD::CConvexHullData* pHull = new CHBasedCD::CConvexHullData;
			pHull->LoadBinaryData(&file);
			// currently, the ECModelHullEditor has turned the model 180 degree which is a mistake,
			// so we have to turn it back before we use it.
			pHull->Transform(TransformMatrix(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0), A3DVECTOR3(0)));
			m_ECMHulls.push_back(pHull);
		}
	}			
	file.Close();
}

void CSceneDynamicObject::TransformHulls(const A3DMATRIX4 &mat)
{
	if (!m_ECMHulls.empty() && m_pECModel && m_pECModel->GetA3DSkinModel()){
		for (size_t i = 0; i < m_ECMHulls.size(); i++)
		{
			CHBasedCD::CConvexHullData* pHull = m_ECMHulls[i];
			pHull->Transform(mat);
		}
	}
}

void  CSceneDynamicObject::ReloadModels()
{
	//Realse all old objects
	if(m_pGfxEx)
	{
		m_pGfxEx->Stop();
		m_pGfxEx->Release();
		delete m_pGfxEx;
		m_pGfxEx = NULL;
	}

	if(m_pECModel)
	{
		ReleaseHulls();
		m_pECModel->Release();
		delete m_pECModel;
		m_pECModel = NULL;
	}
	
	
	DYNAMIC_OBJECT obj;
	InitDynamicObj(&obj);
	g_DynamicObjectMan.GetObjByID(m_uDynamicObjectID,obj);
	if(obj.id == 0) return;
	CString strPath = obj.szObjectPath;
	int pos = strPath.ReverseFind('.');
	if(pos == -1) return;
	strPath = strPath.Right(strPath.GetLength() - pos);
	strPath.MakeLower();
	if(strPath == ".gfx")
	{
		m_pGfxEx = AfxGetGFXExMan()->LoadGfx(g_Render.GetA3DDevice(),obj.szObjectPath);
		if(m_pGfxEx==NULL)
		{
			AString msg;
			msg.Format("CSceneDynamicObject::ReloadModels(), Load %s file failed",obj.szObjectPath);
			g_Log.Log(msg);
			return;
		}
		m_pGfxEx->Start(true);
	}else if(strPath == ".ecm")
	{
		m_pECModel = new CECModel;
		bool bResult = m_pECModel->Load(obj.szObjectPath);
		if(!bResult)
		{
			g_Log.Log("CSceneDynamicObject::ReloadModels(), Failed load %s", obj.szObjectPath);
			delete m_pECModel;
			m_pECModel = NULL;
			return;
		}
		m_pECModel->GetA3DSkinModel()->SetPos(m_vPos);
		m_pECModel->GetA3DSkinModel()->SetDirAndUp(m_matRotate.GetRow(2),m_matRotate.GetRow(1));
		m_pECModel->GetA3DSkinModel()->SetAutoAABBType(A3DSkinModel::AUTOAABB_INITMESH);

		LoadHulls(obj.szObjectPath);
		TransformHulls(m_pECModel->GetA3DSkinModel()->GetAbsoluteTM());
	}
}

//Load data
bool CSceneDynamicObject::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	
	if(dwVersion > 0x37 && dwVersion <= 0x3a)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
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
		if(!ar.Read(&m_uDynamicObjectID,sizeof(unsigned int)))
		return false;
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;	

		if(iLoadFlags!= LOAD_EXPLIGHTMAP) ReloadModels();
	}else if(dwVersion > 0x3a)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());
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
		if(!ar.Read(&m_uDynamicObjectID,sizeof(unsigned int)))
			return false;
		if(!ar.Read(&m_uControllerID,sizeof(unsigned int)))
			return false;
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;	

		if(iLoadFlags!= LOAD_EXPLIGHTMAP && iLoadFlags != LOAD_EXPSCENE) ReloadModels();
	}
	return true;
}
	//Save data
bool CSceneDynamicObject::Save(CELArchive& ar,DWORD dwVersion)
{

	m_matRotate = GetRotateMatrix();
	A3DVECTOR3 vr,vu,vl,vs;
	vr = m_matRotate.GetRow(2);
	vu = m_matRotate.GetRow(1);
	vl = m_matRotate.GetRow(0);
	vs = A3DVECTOR3(m_matScale._11,m_matScale._22,m_matScale._33);
	
	ar.WriteString(m_strName);
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
	if(!ar.Write(&m_uDynamicObjectID,sizeof(unsigned int)))
		return false;
	if(!ar.Write(&m_uControllerID,sizeof(unsigned int)))
		return false;

	return true;
}

void CSceneDynamicObject::SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up)
{
	if(m_pECModel){
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		m_pECModel->SetDirAndUp(dir,up);
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
	CSceneObject::SetDirAndUp(dir,up);
}

void CSceneDynamicObject::Translate(const A3DVECTOR3& vDelta)
{
	CSceneObject::Translate(vDelta);
	if(m_pECModel)
	{
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		m_pECModel->GetA3DSkinModel()->SetPos(m_vPos);
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
}

void CSceneDynamicObject::RotateX(float fRad)
{
	if(m_pECModel){
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		m_pECModel->GetA3DSkinModel()->RotateX(fRad);
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
	CSceneObject::RotateX(fRad);
}

void CSceneDynamicObject::RotateY(float fRad)
{
	if(m_pECModel){
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		m_pECModel->GetA3DSkinModel()->RotateY(fRad);
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
	CSceneObject::RotateY(fRad);
}

void CSceneDynamicObject::RotateZ(float fRad)
{
	if(m_pECModel){
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		m_pECModel->GetA3DSkinModel()->RotateZ(fRad);
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
	CSceneObject::RotateZ(fRad);
}

void CSceneDynamicObject::Rotate(const A3DVECTOR3& axis,float fRad)
{
	if(m_pECModel)
	{
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();

		A3DMATRIX4 mat;
		mat.Identity();
		mat.RotateAxis(axis,fRad);
		
		A3DVECTOR3 vDir = m_pECModel->GetA3DSkinModel()->GetDir();
		A3DVECTOR3 vUp = m_pECModel->GetA3DSkinModel()->GetUp();
		vDir = mat * vDir;
		vUp = mat * vUp;
		m_pECModel->GetA3DSkinModel()->SetDirAndUp(vDir,vUp);
		
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
	
	CSceneObject::Rotate(axis,fRad);
}

void CSceneDynamicObject::ResetRotateMat()
{
	if(m_pECModel){
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		m_pECModel->GetA3DSkinModel()->SetDirAndUp(A3DVECTOR3(0,0,1.0f),A3DVECTOR3(0,1.0f,0));
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
	m_matRotate.Identity();
}

void CSceneDynamicObject::Scale(float fScale)
{
	///A3DVECTOR3 vScale = m_pECModel->GetScale();
	//vScale *= fScale; 
	//m_pECModel->SetScale(vScale.x,vScale.y,vScale.z);
	CSceneObject::Scale(fScale);
}

void CSceneDynamicObject::SetPos(const A3DVECTOR3& vPos)
{
	if(m_pECModel){		
		A3DMATRIX4 tmOld = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		m_pECModel->SetPos(vPos);
		A3DMATRIX4 tmNew = m_pECModel->GetA3DSkinModel()->GetAbsoluteTM();
		A3DMATRIX4 tm = InverseTM(tmOld) * tmNew;
		TransformHulls(tm);
	}
	CSceneObject::SetPos(vPos);
}

CSceneObject* CSceneDynamicObject::CopyObject()
{
	CSceneDynamicObject *pNewObject = new CSceneDynamicObject(*this);
	ASSERT(pNewObject);
	pNewObject->SetAllPrtToNull();
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->SetPropertyPtr(new DYNAMIC_SEL_FUNCS);
	pNewObject->SetControllerPtr(new CONTROLLER_DYNAMIC_ID_FUNCS);
	pNewObject->BuildProperty();
	pNewObject->ReloadModels();
	int Num = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Dynamic_%d",Num);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				Num++;
			}else 
			{
				Num++;
				break;
			}
		}
		pNewObject->m_nObjectID = pSManager->GenerateObjectID();
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->SetPos(m_vPos);
		
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
	}
	return pNewObject;
}

BOOL CALLBACK DYNAMIC_SEL_FUNCS::OnActivate(void)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
	CSceneObject *pObj = 0;
	if(pManager->m_listSelectedObject.GetCount()==1)
	{
		pObj = pManager->m_listSelectedObject.GetHead();
		if(pObj->GetObjectType()==CSceneObject::SO_TYPE_DYNAMIC)
		{
			CSceneDynamicObject *pObj1 = (CSceneDynamicObject*)pObj;
			CDynamicObjectSelDlg dlg;
			dlg.m_nDyObjID = pObj1->GetDynamicObjectID();
			if(IDOK==dlg.DoModal())
			{
				pObj1->SetDynamicObjectID(dlg.m_nDyObjID);
				m_var = dlg.m_nDyObjID;
				DYNAMIC_OBJECT obj;
				InitDynamicObj(&obj);
				g_DynamicObjectMan.GetObjByID(dlg.m_nDyObjID,obj);
				if(obj.id==0) m_strText = "";
				else m_strText.Format("[%d]%s",obj.id,obj.szName);
			}
		}
	}
	return TRUE;
}

LPCTSTR CALLBACK DYNAMIC_SEL_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK DYNAMIC_SEL_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK DYNAMIC_SEL_FUNCS::OnSetValue(const AVariant& var)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject* pObj = (CSceneObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_DYNAMIC)
			{
				unsigned int id = ((CSceneDynamicObject*)pObj)->GetDynamicObjectID();
				DYNAMIC_OBJECT obj;
				InitDynamicObj(&obj);
				g_DynamicObjectMan.GetObjByID(id,obj);
				if(obj.id!=0) m_strText.Format("[%d]%s",obj.id,obj.szName);
			}
		}
	}
}


//Controller select function
BOOL CALLBACK CONTROLLER_DYNAMIC_ID_FUNCS::OnActivate(void)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pObj = (CSceneObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_DYNAMIC)
			{
				CNpcContollerSelDlg dlg;
				int id = ((CSceneDynamicObject*)pObj)->GetControllerID();
				dlg.m_nDyObjID = id;
				if(dlg.DoModal()==IDOK)
				{
					id = dlg.m_nDyObjID;
					((CSceneDynamicObject*)pObj)->SetControllerID(id);
					CONTROLLER_OBJECT obj;
					InitControllerObj(&obj);
					g_ControllerObjectMan.GetObjByID(id,obj);
					if(obj.id==0) m_strText = "";
					else m_strText.Format("[%d]%s",obj.id,obj.szName);
				}
			}
		}
	}
	
	return TRUE;
}

LPCTSTR CALLBACK CONTROLLER_DYNAMIC_ID_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK CONTROLLER_DYNAMIC_ID_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK CONTROLLER_DYNAMIC_ID_FUNCS::OnSetValue(const AVariant& var)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pObj = (CSceneObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_DYNAMIC)
			{
				CNpcContollerSelDlg dlg;
				int id = ((CSceneDynamicObject*)pObj)->GetControllerID();
				CONTROLLER_OBJECT obj;
				InitControllerObj(&obj);
				g_ControllerObjectMan.GetObjByID(id,obj);
				if(obj.id!=0) m_strText.Format("[%d]%s",obj.id,obj.szName);
			}
		}
	}
}




