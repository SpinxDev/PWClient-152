// StaticModelObject.cpp: implementation of the CStaticModelObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "StaticModelObject.h"
#include "Box3D.h"
#include "Render.h"
#include "EL_Building.h"
#include "ModelContainer.h"
#include "A3D.h"
#include "SceneObjectManager.h"
#include "SceneBoxArea.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticModelObject::CStaticModelObject()
{
	m_pModel = NULL;
	m_bShawdow = false;
	m_bCircle = false;
	m_nObjectType = SO_TYPE_STATIC_MODEL; 
	m_vPos = A3DVECTOR3(0);
	m_bReflect = false;
	m_bRefraction = false;
	m_dwLastTime = 0;
	m_bIsTree = false;
	m_bCollideOnly = false;
	BuildProperty();
}

void CStaticModelObject::BuildProperty()
{
	
	bool bCon = false;
	AString strPathName;
	AString strHullPathName;
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_bShawdow),"是否产生阴影");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "是否反射");
		m_pProperty->DynAddProperty(AVariant(m_bShawdow),"是否折射");
		m_pProperty->DynAddProperty(AVariant(strPathName),"模型路径",NULL,NULL,WAY_FILENAME,"");
		m_pProperty->DynAddProperty(AVariant(strHullPathName),"凸包路径",NULL,NULL,WAY_READONLY);
		m_pProperty->DynAddProperty(AVariant(m_nObjectID),"对象ID",NULL,NULL,WAY_READONLY);
		m_pProperty->DynAddProperty(AVariant(bCon),"只碰撞");
		m_pProperty->DynAddProperty(AVariant(m_bIsTree),"只是树");
	}

}

CStaticModelObject::~CStaticModelObject()
{

}

void CStaticModelObject::Release()
{
	if(m_pModel )
	{
		if(m_pModel->pModel)
		{
			m_pModel->pModel->Release();
			delete m_pModel->pModel;
			m_pModel->pModel = NULL;
		}
		delete m_pModel;
		m_pModel = NULL;
	}
}

A3DVECTOR3 CStaticModelObject::GetMin()
{
	A3DVECTOR3 vMin(0.0f);
	if(m_pModel && m_pModel->pModel)
		vMin = m_pModel->pModel->GetModelAABB().Mins;
	return vMin;
}

A3DVECTOR3 CStaticModelObject::GetMax()
{
	A3DVECTOR3 vMax(0.0f);
	if(m_pModel && m_pModel->pModel)
		vMax = m_pModel->pModel->GetModelAABB().Maxs;
	return vMax;
}

// A3DVECTOR3 CStaticModelObject::GetPos()
// { 
// 	return m_pModel->pModel->GetPos();
// };
// 
// A3DMATRIX4 CStaticModelObject::GetTransMatrix()
// { 
// 	return m_pModel->pModel->GetAbsoluteTM(); 
// };
// 
// A3DMATRIX4 CStaticModelObject::GetRotateMatrix()
// { 
// 	A3DMATRIX4 mat;
// 	mat.Identity();
// 	mat = m_pModel->pModel->GetAbsoluteTM();
// 	mat.SetRow(3,A3DVECTOR3(0.0f));
// 	A3DVECTOR3 vecX = mat.GetRow(0);
// 	A3DVECTOR3 vecY = mat.GetRow(1);
// 	A3DVECTOR3 vecZ = mat.GetRow(2);
// 	vecX.Normalize();
// 	vecY.Normalize();
// 	vecZ.Normalize();
// 	mat.SetRow(0, vecX);
// 	mat.SetRow(1, vecY);
// 	mat.SetRow(2, vecZ);
// 	return mat;
// };
// 
// A3DMATRIX4 CStaticModelObject::GetScaleMatrix()
// { 
// 	A3DMATRIX4 mat;
// 	mat.Identity();
// 	A3DVECTOR3 v = m_pModel->pModel->GetScale();
// 	mat._11 = v.x;
// 	mat._22 = v.y;
// 	mat._33 = v.z;
// 	return mat; 
// };

void CStaticModelObject::Render(A3DViewport* pA3DViewport)
{
	if( m_pModel== NULL ||  m_pModel->pModel == NULL ) return;
	m_vMin = ((m_pModel->pModel)->GetModelAABB()).Mins;
	m_vMax = ((m_pModel->pModel)->GetModelAABB()).Maxs;
	if(m_bSelected)
	{
		DWORD timeCur = a_GetTime();
		DWORD timeDelta = timeCur - m_dwLastTime;
		A3DCOLOR curClr;
		if(timeDelta>1000)
		{
			m_dwLastTime = timeCur;
			m_bCircle = !m_bCircle;
			timeDelta = 0;
		}
		if(m_bCircle) curClr = A3DCOLORRGB((int)(0.2f*timeDelta),(int)(0.2f*(1000 - timeDelta)),100);
		else curClr = A3DCOLORRGB((int)(0.2f*(1000-timeDelta)),(int)(0.2f*timeDelta),100);
		
		CBox3D renderBox;
		renderBox.Init(&g_Render,1.0f,false);
		renderBox.SetSize((m_pModel->pModel)->GetModelAABB().Extents.x,(m_pModel->pModel)->GetModelAABB().Extents.y,(m_pModel->pModel)->GetModelAABB().Extents.z);
		renderBox.SetPos((m_pModel->pModel)->GetModelAABB().Center);
		renderBox.SetColor(A3DCOLORRGB(255,0,0));
		//A3DCOLOR oldAmbient = g_Render.GetA3DDevice()->GetAmbientColor();
		//g_Render.GetA3DDevice()->SetAmbient(curClr);
		A3DCOLORVALUE oldClr;
		//oldClr = (m_pModel->pModel)->GetLightScale();
		//A3DCOLORVALUE clrVaule(curClr);
		//(m_pModel->pModel)->SetLightScale(clrVaule);
		
		if(g_Configs.bShowModelHull) 
			(m_pModel->pModel)->RenderConvexHull(pA3DViewport);
		else
			(m_pModel->pModel)->Render(pA3DViewport, true);
		renderBox.Render();
		
		if(g_bShowModelPlane)
		{
			CBox3D plane;
			plane.Init(&g_Render,100.0f,true);
			plane.SetDirAndUp(A3DVECTOR3(0,0,1.0f),A3DVECTOR3(0,1.0f,0));
			plane.SetColor(A3DCOLORRGBA(255,255,255,255));
			plane.SetSize((m_pModel->pModel)->GetModelAABB().Extents.x + 5.0f,0.01f,(m_pModel->pModel)->GetModelAABB().Extents.z + 5.0f);
			A3DVECTOR3 vp = m_vPos;
			vp.y = (m_pModel->pModel)->GetModelAABB().Mins.y;
			plane.SetPos(vp);
			plane.Render();
		}
		
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		pWireCollector->Flush();
		//g_Render.GetA3DDevice()->SetAmbient(oldAmbient);
		//(m_pModel->pModel)->SetLightScale(oldClr);
	}else
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(g_Configs.bShowModelHull) 
			(m_pModel->pModel)->RenderConvexHull(pA3DViewport);
		else
			(m_pModel->pModel)->Render(pA3DViewport);

		m_bCircle = false;
	}
	
	CSceneObject::Render(pA3DViewport);
}

void CStaticModelObject::RenderForLight(A3DViewport* pA3DViewport, float fOffsetX, float fOffsetZ)
{
	if( m_pModel== NULL ||  m_pModel->pModel == NULL )
		return;

	A3DVECTOR3 vDelta(fOffsetX,0.0f,fOffsetZ);
	vDelta = m_vPos + vDelta;
	m_pModel->pModel->SetPos(vDelta);
	m_pModel->pModel->Render(pA3DViewport);
}

void CStaticModelObject::RenderForLightMapShadow(A3DViewport* pA3DViewport)
{
	if( m_pModel== NULL ||  m_pModel->pModel == NULL )
		return;

	if(!g_Configs.bShowTree && m_bIsTree) return;
	(m_pModel->pModel)->Render(pA3DViewport);
}

void CStaticModelObject::UpdateProperty(bool bGet)
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
				m_pProperty->SetPropVal(2,m_bShawdow);
				m_pProperty->SetPropVal(3,m_bReflect);
				m_pProperty->SetPropVal(4,m_bRefraction);
				if(m_pModel)
				{
					m_pProperty->SetPropVal(5,m_strPathName);
					m_pProperty->SetPropVal(6,m_strHullPathName);
				}
				m_pProperty->SetPropVal(7,m_nObjectID);
				m_pProperty->SetPropVal(8,m_bCollideOnly);
				m_pProperty->SetPropVal(9,m_bIsTree);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName = temp;
		m_vPos	= m_pProperty->GetPropVal(1);
		m_bShawdow = m_pProperty->GetPropVal(2);
		m_bReflect = m_pProperty->GetPropVal(3);
		m_bRefraction = m_pProperty->GetPropVal(4);
		AString strnew = AString(m_pProperty->GetPropVal(5));
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		
		if(m_pModel)
		{
			if(stricmp(strnew,m_strPathName)!=0)
			{
				m_strPathName = strnew;
				strnew.CutRight(4);
				strnew += ".chf";
				m_strHullPathName = strnew;
				ReloadModel();
			}
		}
		m_bCollideOnly = m_pProperty->GetPropVal(8);
		SetCollideOnly(m_bCollideOnly);
		SetPos(m_vPos);
		m_bIsTree = m_pProperty->GetPropVal(9);
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_bShawdow);
		m_pProperty->SetPropVal(3,m_bReflect);
		m_pProperty->SetPropVal(4,m_bRefraction);
		if(m_pModel)
		{
			m_pProperty->SetPropVal(5,m_strPathName);
			m_pProperty->SetPropVal(6,m_strHullPathName);
		}
		m_pProperty->SetPropVal(7,m_nObjectID);
		m_pProperty->SetPropVal(8,m_bCollideOnly);
		m_pProperty->SetPropVal(9,m_bIsTree);
	}
}

void CStaticModelObject::ReloadModel()
{
	if(m_pModel)
	{
		CELBuilding *pLitModel = m_pModel->pModel;
		if(pLitModel==0) 
			pLitModel = new CELBuilding;
		else 
			pLitModel->Release();
		
		//Reload object
		AFile sFile;
		if(!sFile.Open(m_strPathName,AFILE_OPENEXIST))
		{
			g_Log.Log("CStaticModelObject::ReloadModel(),Failed to open file %s",m_strPathName);
			return;
		}else
		{
			if(!pLitModel->Load(g_Render.GetA3DDevice(),&sFile))
			{
			
				g_Log.Log("CStaticModelObject::ReloadModel(),Failed to load file %s",m_strPathName);
				return;
			}
		}
		sFile.Close();

		m_pModel->pModel = pLitModel;
		UpdateModel();
	}
}

bool CStaticModelObject::RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd,A3DVECTOR3& inter)
{
	
	if(m_pModel)
	{
		if(m_pModel->pModel)
		{
			A3DVECTOR3 pos,nor;
			float fFraction = 0.1f;
			if(m_pModel->pModel->RayTraceMesh(vStart,vEnd - vStart,pos,&fFraction,nor))
			{
				inter = pos;
				return true;
			}
		}
	}
	
	return false;
}


void CStaticModelObject::Translate(const A3DVECTOR3& vDelta)
{
//	m_vPos = m_pModel->pModel->GetPos();
//	m_vPos +=vDelta;
//	m_pModel->pModel->SetPos(m_vPos);

	CSceneObject::Translate(vDelta);
	UpdateModel();	
}

void CStaticModelObject::RotateX(float fRad)
{
//	m_pModel->pModel->RotateX(fRad);

	CSceneObject::RotateX(fRad); 
	UpdateModel();
}

void CStaticModelObject::RotateY(float fRad)
{
//	m_pModel->pModel->RotateY(fRad);

	CSceneObject::RotateY(fRad); 
	UpdateModel();
}

void CStaticModelObject::RotateZ(float fRad)
{
//	m_pModel->pModel->RotateZ(fRad);

	CSceneObject::RotateZ(fRad); 
	UpdateModel();
}

void CStaticModelObject::Rotate(const A3DVECTOR3& axis,float fRad)
{
// 	A3DMATRIX4 mat;
// 	mat.Identity();
// 	mat.RotateAxis(axis,fRad);
// 	
// 	A3DVECTOR3 vDir = m_pModel->pModel->GetDir();
// 	A3DVECTOR3 vUp = m_pModel->pModel->GetUp();
// 	vDir = mat * vDir;
// 	vUp = mat * vUp;
// 	m_pModel->pModel->SetDirAndUp(vDir,vUp);

	CSceneObject::Rotate(axis,fRad); 
	UpdateModel();
}

void CStaticModelObject::Scale(float fScale)
{
// 	A3DVECTOR3 vScale = m_pModel->pModel->GetScale();
// 	vScale *= fScale; 
// 	m_pModel->pModel->SetScale(vScale.x);

	CSceneObject::Scale(fScale); 
	UpdateModel();
}

void CStaticModelObject::SetPos(const A3DVECTOR3& vPos)
{
// 	CSceneObject::SetPos(vPos);
// 	if(m_pModel)
// 	{
// 		if(m_pModel->pModel)
// 			m_pModel->pModel->SetPos(m_vPos);
// 	}

	CSceneObject::SetPos(vPos); 
	UpdateModel();
}

void CStaticModelObject::ResetRotateMat()
{
//	m_pModel->pModel->SetDirAndUp(A3DVECTOR3(0,0,1.0f),A3DVECTOR3(0,1.0f,0));

	CSceneObject::ResetRotateMat(); 
	UpdateModel();
}

void CStaticModelObject::SetDirAndUp(const A3DVECTOR3& dir, const A3DVECTOR3& up)
{
// 	if(m_pModel)
// 	{
// 		if(m_pModel->pModel)
// 		{
// 			m_pModel->pModel->SetDirAndUp(dir,up);
// 		}
// 	}
// 	CSceneObject::SetDirAndUp(dir,up);

	CSceneObject::SetDirAndUp(dir,up);
	UpdateModel();
}

void CStaticModelObject::SetScale(const A3DVECTOR3& vs)
{
// 	if(m_pModel)
// 		m_pModel->pModel->SetScale(vs.x);
// 	CSceneObject::SetScale(vs);

	CSceneObject::SetScale(vs); 
	UpdateModel();
}

CSceneObject* CStaticModelObject::CopyObject()
{
// 	CStaticModelObject *pNewObject = new CStaticModelObject(*this);
// 	ASSERT(pNewObject);
// 	pNewObject->m_pProperty = new ADynPropertyObject();	
// 	pNewObject->BuildProperty();
// 	PESTATICMODEL pEM = pNewObject->GetA3DModel();
// 	
// 	AString strPath = pEM->m_strPathName; 
// 	AString strHullPath = pEM->m_strHullPathName;
// 	A3DVECTOR3 vPos = pEM->pModel->GetPos();
// 	A3DVECTOR3 vScale = pEM->pModel->GetScale();
// 	A3DMATRIX4 matRotate = GetRotateMatrix();
// 	
// 	
// 	pEM = new ESTATICMODEL();
// 	pEM->m_strPathName = strPath;
// 	pEM->m_strHullPathName = strHullPath;
// 	ASSERT(pEM);
// 	pEM->pModel = new CELBuilding();
// 	ASSERT(pEM->pModel);
// 	AFile sFile;
// 	if(!sFile.Open(pEM->m_strPathName,AFILE_OPENEXIST))
// 	{
// 		g_Log.Log("CStaticModelObject::CopyObject(),Open file %s failed!",pEM->m_strPathName);
// 		return NULL;
// 	}else
// 	{
// 		if(!pEM->pModel->Load(g_Render.GetA3DDevice(),&sFile))
// 		{
// 			g_Log.Log("CStaticModelObject::CopyObject(),Load file %s failed!",pEM->m_strPathName);
// 			return NULL;
// 		}
// 	}
// 	sFile.Close();
// 	pEM->pModel->SetPos(vPos);
// 	pEM->pModel->SetDirAndUp(matRotate.GetRow(2),matRotate.GetRow(1));
// 	pEM->pModel->SetScale(vScale.x);
// 	pNewObject->SetA3dModel(pEM);
// 	
// 	int StaticModelNum = 0;
// 	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
// 	if(pMap)
// 	{
// 		AString name;
// 		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
// 		pSManager->ClearSelectedList();
// 		while(1)
// 		{
// 			name.Format("StaticModel_%d",StaticModelNum);
// 			if(pSManager->FindSceneObject(name)!=NULL)
// 			{
// 				StaticModelNum++;
// 			}else 
// 			{
// 				StaticModelNum++;
// 				break;
// 			}
// 		}
// 		pNewObject->m_nObjectID = pSManager->GenerateObjectID();
// 		pSManager->InsertSceneObject(pNewObject);
// 		pSManager->AddObjectPtrToSelected(pNewObject);
// 		pNewObject->SetObjectName(name);
// 		pNewObject->UpdateProperty(false);
// 		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
// 		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
// 	}
// 	return pNewObject;

	CStaticModelObject *pNewObject = new CStaticModelObject(*this);
	ASSERT(pNewObject);
//	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildProperty();
	PESTATICMODEL pEM = new ESTATICMODEL();
	ASSERT(pEM);
	pEM->pModel = new CELBuilding();
	ASSERT(pEM->pModel);
	AFile sFile;
	if(!sFile.Open(m_strPathName,AFILE_OPENEXIST))
	{
		g_Log.Log("CStaticModelObject::CopyObject(),Open file %s failed!",m_strPathName);
		delete pEM->pModel;
		delete pEM;
		return NULL;
	}else
	{
		if(!pEM->pModel->Load(g_Render.GetA3DDevice(),&sFile))
		{
			g_Log.Log("CStaticModelObject::CopyObject(),Load file %s failed!",m_strPathName);
			delete pEM->pModel;
			delete pEM;
			return NULL;
		}
	}
	sFile.Close();
	pNewObject->SetA3dModel(pEM);
	
	int StaticModelNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("StaticModel_%d",StaticModelNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				StaticModelNum++;
			}else 
			{
				StaticModelNum++;
				break;
			}
		}
		pNewObject->m_nObjectID = pSManager->GenerateObjectID();
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->UpdateProperty(false);

		pNewObject->UpdateModel();
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	return pNewObject;
}

//判断区域是否跟该区域相交
bool CStaticModelObject::IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax)
{
	if(m_pModel == NULL) return false;
	if(m_pModel->pModel == NULL) return false;

	A3DVECTOR3 min = m_pModel->pModel->GetModelAABB().Mins;
	A3DVECTOR3 max = m_pModel->pModel->GetModelAABB().Maxs;
	
	if(min.x>vMax.x) return false;
	if(min.z>vMax.z) return false;
	if(max.x<vMin.x) return false;
	if(max.z<vMin.z) return false;
	return true;
}

//	Load data
bool CStaticModelObject::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	/*
	if(dwVersion<2)
	{
		m_strName = AString(ar.ReadString());//模型的名字，这个名字编辑器用，唯一
		AString strPath;
		strPath   = AString(ar.ReadString());//写入模型的路径名便于加载
		strPath.CutRight(4);
		strPath = strPath + ".mox";
		if( -1 == strPath.Find("Models",0))
		{//这儿是兼容一个错误，少保存了"Models\\"
			strPath = "Models\\" + strPath;
		}
		m_pModel  = g_ModelContainer.GetStaticModel(strPath);
		if(m_pModel==NULL)
		{
			CString msg;
			msg.Format("模型(%s)没找到，编辑器将用默认模型代替该模型！",strPath);
			AfxMessageBox(msg);
			m_pModel = g_ModelContainer.GetStaticModel("static\\default\\default.mod");
		}
		if(m_pModel==NULL) return false;
		//本来不该存储这么多数据
		//老版本只能暂时将就用
		//新文件版本将改正这个问题
		if(!ar.Read(&m_matTrans,sizeof(A3DMATRIX4)))//位置
			return false;
		if(!ar.Read(&m_matRotate,sizeof(A3DMATRIX4)))//旋转
			return false;
		if(!ar.Read(&m_matScale,sizeof(A3DMATRIX4)))//宿放
			return false;	
		m_vPos = m_matTrans.GetRow(3);
		m_vDirection = m_matRotate.GetRow(2);
		m_pModel->pModel->SetPos(m_vPos);
		m_pModel->pModel->SetDirAndUp(m_vDirection,A3DVECTOR3(0.0f,1.0f,0.0f));
		return true;
	}
	if(dwVersion>1  && dwVersion<=0xc)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());//模型的名字，这个名字编辑器用，唯一
		AString strPath;
		strPath   = AString(ar.ReadString());//写入模型的路径名便于加载
		strPath.CutRight(4);
		strPath = strPath + ".mox";
		if( -1 == strPath.Find("Models",0))
		{//这儿是兼容一个错误，少保存了"Models\\"
			strPath = "Models\\" + strPath;
		}
		m_pModel  = g_ModelContainer.GetStaticModel(strPath);
		if(m_pModel==NULL)
		{
			CString msg;
			msg.Format("模型(%s)没找到，编辑器将用默认模型代替该模型！",strPath);
			AfxMessageBox(msg);
			m_pModel = g_ModelContainer.GetStaticModel("static\\default\\default.mod");
		}
		if(m_pModel==NULL) return false;
		
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
		if(!ar.Read(&m_bShawdow,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bReflect,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bRefraction,sizeof(bool)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;

		m_pModel->pModel->SetPos(m_vPos);
		m_pModel->pModel->SetDirAndUp(vr,vu);
		m_pModel->pModel->SetScale(vs.x,vs.y,vs.z);
		return true;
	}

	if(dwVersion > 0xc)
	{
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName = AString(ar.ReadString());//模型的名字，这个名字编辑器用，唯一
		AString strPath,strHullPath;
		strPath   = AString(ar.ReadString());//写入模型的路径名便于加载
		strHullPath = AString(ar.ReadString());
		strPath.CutRight(4);
		strPath = strPath + ".mox";
		if( -1 == strPath.Find("Models",0))
		{//这儿是兼容一个错误，少保存了"Models\\"
			strPath = "Models\\" + strPath;
		}
		m_pModel  = g_ModelContainer.GetStaticModel(strPath);
		if(m_pModel==NULL)
		{
			CString msg;
			msg.Format("模型(%s)没找到，编辑器将用默认模型代替该模型！",strPath);
			AfxMessageBox(msg);
			m_pModel = g_ModelContainer.GetStaticModel("static\\default\\default.mod");
		}
		if(m_pModel==NULL) return false;
		
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
		if(!ar.Read(&m_bShawdow,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bReflect,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bRefraction,sizeof(bool)))
			return false;
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;

		m_pModel->pModel->SetPos(m_vPos);
		m_pModel->pModel->SetDirAndUp(vr,vu);
		m_pModel->pModel->SetScale(vs.x,vs.y,vs.z);
		return true;
	}
	*/
	
	//>0x3d时.scene中恢复存储静态模型信息
	if(dwVersion > 0x3d && dwVersion < 0x41)
	{
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName			= AString(ar.ReadString());	//模型的名字
		AString strPath		= AString(ar.ReadString());	//模型的路径
		AString strHullPath = AString(ar.ReadString());	//模型凸包路径
	
		m_pModel = Aux_GetStaticModelFromTempMap(m_nObjectID);
		if(m_pModel==0)
		{
			AString msg;
			msg.Format("CStaticModelObject::Load(), can't get object ptr from object manager(id = %d)",m_nObjectID);
			g_Log.Log(msg);
			g_LogDlg.Log(CString(msg));
		}

		if(m_pModel)
			m_bCollideOnly = m_pModel->pModel->GetCollideOnly();
		
		m_strPathName = strPath;
		m_strHullPathName = strHullPath;
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

		if(!ar.Read(&m_bShawdow,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bReflect,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bRefraction,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bIsTree,sizeof(bool)))
			return false;

		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		UpdateModel();
	}else if(dwVersion > 0x40)
	{
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		A3DVECTOR3 vr,vu,vl,vs;
		m_strName			= AString(ar.ReadString());	//模型的名字
		AString strPath		= AString(ar.ReadString());	//模型的路径
		AString strHullPath = AString(ar.ReadString());	//模型凸包路径
	
		m_pModel = Aux_GetStaticModelFromTempMap(m_nObjectID);
		if(m_pModel==0)
		{
			AString msg;
			msg.Format("CStaticModelObject::Load(), can't get object ptr from object manager(id = %d)",m_nObjectID);
			g_Log.Log(msg);
			g_LogDlg.Log(CString(msg));
		}
		
		m_strPathName = strPath;
		m_strHullPathName = strHullPath;
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

		if(!ar.Read(&m_bShawdow,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bReflect,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bRefraction,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bIsTree,sizeof(bool)))
			return false;
		if(!ar.Read(&m_bCollideOnly,sizeof(bool)))
			return false;

		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		UpdateModel();
	}

	return true;
}

//	Save data
bool CStaticModelObject::Save(CELArchive& ar,DWORD dwVersion)
{
	///////////////////////////////临时///////////////////////////////
	//转换时使用,老版本静态对象信息没有存储在.scene中,直接存储在.smod中,
	//位置等这些信息静态对象没有保存,直接从模型中获取的
	//转换后这些信息将存储在.scene文件中,因此转换时需要从老版本的模型获取这些信息
	//转换后所有对静态对象的操作后的位置等信息直接存储在对象的pos,m_matRotate等中
	//保存就不需要和模型相关了
// 	ASSERT(m_pModel && m_pModel->pModel && "CStaticModelObject::Save");
// 	m_vPos = m_pModel->pModel->GetPos();
// 
// 	m_matRotate.Identity();
// 	m_matRotate = m_pModel->pModel->GetAbsoluteTM();
// 	m_matRotate.SetRow(3,A3DVECTOR3(0.0f));
// 	A3DVECTOR3 vecX = m_matRotate.GetRow(0);
// 	A3DVECTOR3 vecY = m_matRotate.GetRow(1);
// 	A3DVECTOR3 vecZ = m_matRotate.GetRow(2);
// 	vecX.Normalize();
// 	vecY.Normalize();
// 	vecZ.Normalize();
// 	m_matRotate.SetRow(0, vecX);
// 	m_matRotate.SetRow(1, vecY);
// 	m_matRotate.SetRow(2, vecZ);
// 
// 	m_matScale.Identity();
// 	A3DVECTOR3 v = m_pModel->pModel->GetScale();
// 	m_matScale._11 = v.x;
// 	m_matScale._22 = v.y;
// 	m_matScale._33 = v.z;
// 
// 	m_strPathName	  = m_pModel->m_strPathName;
// 	m_strHullPathName = m_pModel->m_strHullPathName;
	/////////////////////////////临时///////////////////////////////

	//>0x3d时.scene中恢复存储静态模型信息
	if(dwVersion > 0x3d)
	{
		ar.Write(&m_nObjectID,sizeof(int));
		ar.WriteString(m_strName);
		ar.WriteString(m_strPathName);
		ar.WriteString(m_strHullPathName);
	
		A3DVECTOR3 vr,vu,vl,vs;
		vr = m_matRotate.GetRow(2);
		vu = m_matRotate.GetRow(1);
		vl = m_matRotate.GetRow(0);
		vs = A3DVECTOR3(m_matScale._11,m_matScale._22,m_matScale._33);
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

		if(!ar.Write(&m_bShawdow,sizeof(bool)))
			return false;
		if(!ar.Write(&m_bReflect,sizeof(bool)))
			return false;
		if(!ar.Write(&m_bRefraction,sizeof(bool)))
			return false;
		if(!ar.Write(&m_bIsTree,sizeof(bool)))
			return false;
		if(!ar.Write(&m_bCollideOnly,sizeof(bool)))
			return false;
		return true;	
	}

	return true;
}

void CStaticModelObject::QueryRef(ElementResMan *pResMan)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CString strRefName = pMap->GetMapName();
	strRefName = strRefName + "..";
	strRefName = strRefName + CString(m_strName);
	if(m_pModel) pResMan->AddReference(ElementResMan::RES_TYPE_BUILDING,m_strPathName,strRefName);
}

//把静态模型设置成只有碰撞没有显示的特殊模型
void CStaticModelObject::SetCollideOnly(bool bCollide)
{
	if(m_pModel)
	{
		if(m_pModel->pModel)
			m_pModel->pModel->SetCollideOnly(bCollide);
	}
}

bool CStaticModelObject::GetCollideFlags()
{
	if(m_pModel)
	{
		if(m_pModel->pModel)
			return m_pModel->pModel->GetCollideOnly();
	}
	return false;
}

int	CStaticModelObject::GetTriangleNum()
{
	if(m_pModel)
	{
		if(m_pModel->pModel)
		{
			if(m_pModel->pModel->GetCollideOnly()) return 0;
			A3DLitModel * pLitModel = m_pModel->pModel->GetA3DLitModel();
			if(pLitModel)
			{
				int nNumMeshes = pLitModel->GetNumMeshes();
				int nFaces = 0;
				for(int idMesh=0; idMesh<nNumMeshes; idMesh++)
				{
					A3DLitMesh* pMesh = pLitModel->GetMesh(idMesh);
					nFaces += pMesh->GetNumFaces();
				}
				return nFaces;
			}
		}
	}
	return 0;
}

void CStaticModelObject::UpdateModel()
{
	// m_Obb.Build(A3DAABB(0, 0));
	if(m_pModel)
	{
		if(m_pModel->pModel)
		{
			(m_pModel->pModel)->SetPos(m_vPos);
			(m_pModel->pModel)->SetScale(m_matScale._11);
			(m_pModel->pModel)->SetCollideOnly(m_bCollideOnly);
			
			A3DVECTOR3 up = g_vAxisY * m_matRotate;
			A3DVECTOR3 dir = g_vAxisZ * m_matRotate;
			(m_pModel->pModel)->SetDirAndUp(dir,up);
			
        }
	}
}
