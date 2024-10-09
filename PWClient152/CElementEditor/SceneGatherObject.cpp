// SceneGatherObject.cpp: implementation of the CSceneGatherObject class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneGatherObject.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "Render.h"
#include "SceneAIGenerator.h"
#include "elementdataman.h"
#include "GatherDlg.h"
#include "EC_Model.h"

#include "NpcControllerManDlg.h"
#include "NpcControllerSelDlg.h"

#include "A3D.h"

//#define new A_DEBUG_NEW

extern float RandBetween(float fLow, float fHei);

void Init_Gather_Data(GATHER_DATA &data)
{
	data.fEdgeLen = 2;
    data.fEdgeWth = 2;
	data.bGenGatherInit = true;//怪物是否起始时生成
	data.bGatherRenascenceAuto = true;//区域怪物是否自动重生
	data.bValidAtOneTime = true;//区域是否同时生效一次
	data.dwGenID = 0;//区域制定编号

	data.nControllerId = 0;
	data.nGatherNum = 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneGatherObject::CSceneGatherObject()
{
	m_strGatherList = "...";
	m_nObjectType = SO_TYPE_GATHER;
	m_pIDSelFuncs = new GATHER_SEL_FUNCS;
	m_pControllerSelFuncs  = new CONTROLLER_GATHER_ID_FUNCS;
	m_nCopy = 0;
	BuildProperty();
}

CSceneGatherObject::~CSceneGatherObject()
{
}

void CSceneGatherObject::Translate(const A3DVECTOR3& vDelta)
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	m_vPos += vDelta;
	m_vPos.y = pMap->GetTerrain()->GetPosHeight(m_vPos) + 0.1f;

	m_matTrans.SetRow(3,m_vPos);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	
	POSITION pos = m_listModels.GetHeadPosition();
	while(pos)
	{
		CECModel *pModels = (CECModel *)m_listModels.GetNext(pos);
		A3DVECTOR3 old = pModels->GetA3DSkinModel()->GetPos();
		old = old + vDelta;
		old.y = pMap->GetTerrain()->GetPosHeight(old);
		pModels->GetA3DSkinModel()->SetPos(old);
	}
}

void CSceneGatherObject::Render(A3DViewport* pA3DViewport)
{
	if(m_nCopy != g_Configs.nShowCopyNum) return;
	DrawCenter();
	DrawRect();
	RenderModels(pA3DViewport);

	CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
	CRenderWnd *pRWnd = pFrame->GetRenderWnd();
	if(pRWnd->GetActiveViewport() == VIEW_XZ)
	{
		DrawMineName();
	}

	CSceneObject::Render(pA3DViewport);
}

void CSceneGatherObject::DrawMineName()
{
	A3DString str;
	HA3DFONT hA3dFont;
	hA3dFont = g_Render.GetA3DEngine()->GetA3DFontMan()->RegisterFontType("宋体",10,0,'w');
	float fCenterX = m_vPos.x;
	float fCenterY = m_vPos.z;
	float flRadius = m_Data.fEdgeLen/2.0f;
	float fwRadius = m_Data.fEdgeWth/2.0f;
	A3DVECTOR3 vLeftUp = A3DVECTOR3(fCenterX - flRadius,0,fCenterY + fwRadius);
	
	
	A3DVECTOR3 vScreenPos;
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->PointInFrustum(vLeftUp))
	{
		g_Render.GetA3DEngine()->GetActiveViewport()->Transform(vLeftUp, vScreenPos);
		int n = GetGatherNum();
		for(int i = 0; i < n; i++ )
		{
			GATHER* pro = GetGather(i);
			str.Init(g_Render.GetA3DDevice(),pro->strName,hA3dFont);	
			str.DrawText(vScreenPos.x, vScreenPos.y,A3DCOLORRGB(183,137,77));
			vScreenPos.y +=10;
			str.Release();
		}
	}
}

void CSceneGatherObject::RenderModels(A3DViewport *pView)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	LIGHTINGPARAMS light = pMap->GetLightingParams();
	A3DSkinModel::LIGHTINFO LightInfo;
	LightInfo.colAmbient	= A3DCOLORVALUE(light.dwAmbient);//	Directional light's specular color
	LightInfo.vLightDir		= light.vSunDir;//Direction of directional light
	LightInfo.colDirDiff	= A3DCOLORVALUE(light.dwSunCol);//Directional light's diffuse color
	LightInfo.colDirSpec	= A3DCOLORVALUE(0.8f,0.8f,0.8f,1.0f);//	Directional light's specular color
	LightInfo.bPtLight		= false;//	false, disable dynamic point light,
	
	POSITION pos = m_listModels.GetHeadPosition();
	while(pos)
	{
		CECModel *pModels = (CECModel *)m_listModels.GetNext(pos);
		pModels->GetA3DSkinModel()->SetLightInfo(LightInfo);
		pModels->Render(pView);
	}		
}

void CSceneGatherObject::Tick(DWORD timeDelta)
{
	if(m_nCopy != g_Configs.nShowCopyNum) return;
	POSITION pos = m_listModels.GetHeadPosition();
	while(pos)
	{
		CECModel *pModels = (CECModel *)m_listModels.GetNext(pos);
		pModels->Tick(timeDelta);
	}	
}

void CSceneGatherObject::UpdateProperty(bool bGet)
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
				m_pProperty->SetPropVal(2,m_nCopy);
				m_pProperty->SetPropVal(3,m_Data.fEdgeLen);
				m_pProperty->SetPropVal(4,m_Data.fEdgeWth);
				m_pProperty->SetPropVal(5,m_Data.fEdgeLen*m_Data.fEdgeWth);
				m_pProperty->SetPropVal(6,m_Data.bGatherRenascenceAuto);
				m_pProperty->SetPropVal(7,m_Data.nGatherNum);
				m_pControllerSelFuncs->OnSetValue(m_Data.nControllerId);
				m_pIDSelFuncs->OnSetValue(m_strGatherList);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName	        = temp;
		m_vPos	            = m_pProperty->GetPropVal(1);
		m_nCopy	            = m_pProperty->GetPropVal(2);
		m_Data.fEdgeLen     = m_pProperty->GetPropVal(3);
		m_Data.fEdgeWth     = m_pProperty->GetPropVal(4);
		m_pProperty->SetPropVal(5,m_Data.fEdgeLen*m_Data.fEdgeWth);
		m_Data.bGatherRenascenceAuto = m_pProperty->GetPropVal(6);
		m_Data.nGatherNum = m_pProperty->GetPropVal(7);
		
		if(m_Data.dwGenID!=0 || !m_Data.bGenGatherInit)
		{
			if(IDYES==AfxMessageBox("这个区域已经被你更新，由于是旧版数据，现需要完成版本转换，你确定要转换吗？",MB_YESNO|MB_ICONQUESTION))
			{
				m_Data.bGenGatherInit = true;
				m_Data.dwGenID = 0;
			}
		}
		
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReloadModels();
	
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_nCopy);
		m_pProperty->SetPropVal(3,m_Data.fEdgeLen);
		m_pProperty->SetPropVal(4,m_Data.fEdgeWth);
		m_pProperty->SetPropVal(5,m_Data.fEdgeLen*m_Data.fEdgeWth);
		//m_pProperty->SetPropVal(6,m_Data.bGenGatherInit);
		//m_pProperty->SetPropVal(7,m_Data.bGatherRenascenceAuto);
		//m_pProperty->SetPropVal(8,m_Data.bValidAtOneTime);
		//m_pProperty->SetPropVal(9,m_Data.dwGenID);
		m_pProperty->SetPropVal(6,m_Data.bGatherRenascenceAuto);
		m_pProperty->SetPropVal(7,m_Data.nGatherNum);
		m_pControllerSelFuncs->OnSetValue(m_Data.nControllerId);
		m_pIDSelFuncs->OnSetValue(m_strGatherList);
	}
}

void CSceneGatherObject::Release()
{
	POSITION pos = m_listGather.GetHeadPosition();
	while(pos)
	{
		GATHER *pro = (GATHER *)m_listGather.GetNext(pos);
		delete pro;
	}
	m_listGather.RemoveAll();

	ReleaseModels();
	if(m_pIDSelFuncs) delete m_pIDSelFuncs;
	if(m_pControllerSelFuncs) delete m_pControllerSelFuncs;
	CSceneObject::Release();
}

void CSceneGatherObject::EntryScene()
{

}

void CSceneGatherObject::LeaveScene()
{

}
//Load data
bool CSceneGatherObject::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	Init_Gather_Data(m_Data);
	if(dwVersion==0x19)
	{
		m_strName = ar.ReadString();
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.fEdgeWth,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeLen,sizeof(float)))
			return false;
		
		int num;
		if(!ar.Read(&num,sizeof(int)))
			return false;
		
		for(int i = 0; i<num; i++)
		{
			GATHER* pro = new GATHER;
			
			if(!ar.Read(&pro->dwId,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwType,sizeof(DWORD)))
				return false;
			pro->dwRefurbishTime = 1000;
			pro->dwNum = 1;
			pro->fOffset = 0.0f;
			pro->strName = ar.ReadString();
			m_listGather.AddTail(pro);
		}
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		//if(iLoadFlags!= LOAD_EXPLIGHTMAP) ReloadModels();	
	}else if(dwVersion == 0x1a)
	{
		m_strName = ar.ReadString();
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.fEdgeWth,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeLen,sizeof(float)))
			return false;
		
		int num;
		if(!ar.Read(&num,sizeof(int)))
			return false;
		
		for(int i = 0; i<num; i++)
		{
			GATHER* pro = new GATHER;
			
			if(!ar.Read(&pro->dwId,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwType,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwNum,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwRefurbishTime,sizeof(DWORD)))
				return false;
			pro->fOffset = 0.0f;
			pro->strName = ar.ReadString();
			m_listGather.AddTail(pro);
		}
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		//if(iLoadFlags!= LOAD_EXPLIGHTMAP) ReloadModels();	
	}else if(dwVersion > 0x1a && dwVersion <= 0x30)
	{
		m_strName = ar.ReadString();
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.fEdgeWth,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeLen,sizeof(float)))
			return false;
		
		int num;
		if(!ar.Read(&num,sizeof(int)))
			return false;
		
		for(int i = 0; i<num; i++)
		{
			GATHER* pro = new GATHER;
			
			if(!ar.Read(&pro->dwId,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwType,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwNum,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwRefurbishTime,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->fOffset,sizeof(float)))
			return false;
			pro->strName = ar.ReadString();
			m_listGather.AddTail(pro);
		}
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		//if(iLoadFlags!= LOAD_EXPLIGHTMAP) ReloadModels();	
	}else if(dwVersion > 0x30 && dwVersion <= 0x34)
	{
		m_strName = ar.ReadString();
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.fEdgeWth,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeLen,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.bGenGatherInit,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.bGatherRenascenceAuto,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.bValidAtOneTime,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.dwGenID,sizeof(DWORD)))
			return false;
		int num;
		if(!ar.Read(&num,sizeof(int)))
			return false;
		
		for(int i = 0; i<num; i++)
		{
			GATHER* pro = new GATHER;
			
			if(!ar.Read(&pro->dwId,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwType,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwNum,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwRefurbishTime,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->fOffset,sizeof(float)))
			return false;
			pro->strName = ar.ReadString();
			m_listGather.AddTail(pro);
		}
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		//if(iLoadFlags!= LOAD_EXPLIGHTMAP) ReloadModels();	
	}else if(dwVersion > 0x34 && dwVersion <= 0x38)
	{
		m_strName = ar.ReadString();
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_nCopy,sizeof(int)))
			return false;
		if(!ar.Read(&m_Data.fEdgeWth,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeLen,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.bGenGatherInit,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.bGatherRenascenceAuto,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.bValidAtOneTime,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.dwGenID,sizeof(DWORD)))
			return false;
		int num;
		if(!ar.Read(&num,sizeof(int)))
			return false;
		
		for(int i = 0; i<num; i++)
		{
			GATHER* pro = new GATHER;
			
			if(!ar.Read(&pro->dwId,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwType,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwNum,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwRefurbishTime,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->fOffset,sizeof(float)))
			return false;
			pro->strName = ar.ReadString();
			m_listGather.AddTail(pro);
		}
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		//if(iLoadFlags!= LOAD_EXPLIGHTMAP) ReloadModels();	
	}else if(dwVersion > 0x38)
	{
		m_strName = ar.ReadString();
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_nCopy,sizeof(int)))
			return false;
		if(!ar.Read(&m_Data.fEdgeWth,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fEdgeLen,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.bGenGatherInit,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.bGatherRenascenceAuto,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.bValidAtOneTime,sizeof(bool)))
			return false;
		if(!ar.Read(&m_Data.dwGenID,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.nControllerId,sizeof(int)))
			return false;
		//if(!ar.Read(&m_Data.nMounsterLife,sizeof(int)))
		//	return false;
		if(!ar.Read(&m_Data.nGatherNum,sizeof(int)))
			return false;

		int num;
		if(!ar.Read(&num,sizeof(int)))
			return false;
		
		for(int i = 0; i<num; i++)
		{
			GATHER* pro = new GATHER;
			
			if(!ar.Read(&pro->dwId,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwType,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwNum,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->dwRefurbishTime,sizeof(DWORD)))
				return false;
			if(!ar.Read(&pro->fOffset,sizeof(float)))
			return false;
			pro->strName = ar.ReadString();
			m_listGather.AddTail(pro);
		}
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		if(iLoadFlags!= LOAD_EXPLIGHTMAP && iLoadFlags != LOAD_EXPSCENE) ReloadModels();	
	}

	if(m_Data.dwGenID != 0 || !m_Data.bGenGatherInit)
	{
		CString msg;
		msg.Format("注意：需要手动更新区域完成版本转换：%s(区域编号: %d,初始时生成怪物: %d)",GetObjectName(),m_Data.dwGenID, m_Data.bGenGatherInit);
		g_LogDlg.Log(msg);
	}
	if(m_Data.nControllerId == -1) 
		m_Data.nControllerId = 0;
	
	return true;
}

//Save data
bool CSceneGatherObject::Save(CELArchive& ar,DWORD dwVersion)
{
	ar.WriteString(m_strName);
	
	if(!ar.Write(&m_nObjectID,sizeof(int)))
		return false;
	if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
		return false;
	if(!ar.Write(&m_nCopy,sizeof(int)))
		return false;
	if(!ar.Write(&m_Data.fEdgeWth,sizeof(float)))
		return false;
	if(!ar.Write(&m_Data.fEdgeLen,sizeof(float)))
		return false;
	if(!ar.Write(&m_Data.bGenGatherInit,sizeof(bool)))
		return false;
	if(!ar.Write(&m_Data.bGatherRenascenceAuto,sizeof(bool)))
		return false;
	if(!ar.Write(&m_Data.bValidAtOneTime,sizeof(bool)))
		return false;
	if(!ar.Write(&m_Data.dwGenID,sizeof(DWORD)))
		return false;
	
	if(!ar.Write(&m_Data.nControllerId,sizeof(int)))
		return false;
	//if(!ar.Write(&m_Data.nMounsterLife,sizeof(int)))
	//	return false;
	if(!ar.Write(&m_Data.nGatherNum,sizeof(int)))
		return false;
	
	int num = GetGatherNum();
	if(!ar.Write(&num,sizeof(int)))
		return false;
	
	for(int i = 0; i<num; i++)
	{
		GATHER* pro = (GATHER*)GetGather(i);
		
		if(!ar.Write(&pro->dwId,sizeof(DWORD)))
			return false;
		if(!ar.Write(&pro->dwType,sizeof(DWORD)))
			return false;
		if(!ar.Write(&pro->dwNum,sizeof(DWORD)))
			return false;
		if(!ar.Write(&pro->dwRefurbishTime,sizeof(DWORD)))
			return false;
		if(!ar.Write(&pro->fOffset,sizeof(float)))
			return false;
		ar.WriteString(pro->strName);
	}
	
	return true;
}

//Exp data
bool CSceneGatherObject::ExpData(AFile *pFile,DWORD dwVersion)
{
	return true;
}

bool CSceneGatherObject::ImpData(AFile *pFile,DWORD dwVersion)
{
	return true;
}

//  Copy object
CSceneObject* CSceneGatherObject::CopyObject()
{
	CSceneGatherObject *pNewObject = new CSceneGatherObject();
	if(pNewObject==NULL) return NULL;
	int Num = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("GatherObject_%d",Num);
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
		pNewObject->SetPos(m_vPos +	A3DVECTOR3(0.1f,0.0f,0.1f));
		pNewObject->SetProperty(GetProperty());
		pNewObject->SetCopyFlags(GetCopyFlags());
		
		POSITION pos = m_listGather.GetHeadPosition();
		while(pos)
		{
			GATHER *pOldGather = (GATHER *)m_listGather.GetNext(pos);
			GATHER *pNewGather = new GATHER;
			*pNewGather = *pOldGather;
		    pNewObject->AddGather(pNewGather);
		}

		pNewObject->ReloadModels();
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	return pNewObject;
}

DWORD CSceneGatherObject::GetGatherNum()
{
	return m_listGather.GetCount();
}

AString CSceneGatherObject::GetGatherModelPath(int index)
{
	GATHER *pGather = GetGather(index);
	DWORD sType = pGather->dwType;
	DWORD sID = pGather->dwId;
	return GetModelPath(sID,sType);	
}

//CECModel* CSceneGatherObject::GetGatherModel(int index)
//{
//}

GATHER* CSceneGatherObject::GetGather(int index)
{
	return (GATHER*)m_listGather.GetAt(m_listGather.FindIndex(index));
}

GATHER* CSceneGatherObject::GetGather(AString name)
{
	POSITION pos = m_listGather.GetHeadPosition();
	while(pos)
	{
		GATHER *pro = (GATHER *)m_listGather.GetNext(pos);
		if(strcmp(pro->strName,name)==0)
			return pro;
	}	
	return NULL;
}

void  CSceneGatherObject::AddGather(GATHER *gather)
{
	m_listGather.AddTail(gather);
}

void  CSceneGatherObject::DeleteGather(int index)
{
	m_listGather.RemoveAt(m_listGather.FindIndex(index));
}

AString  CSceneGatherObject::GetModelPath(DWORD dwId ,DWORD stype)
{
	AString name;
	DWORD dwNum = g_dataMan.get_data_num(ID_SPACE_ESSENCE);
	
	DATA_TYPE type = DT_MINE_ESSENCE;
	MINE_ESSENCE* pData;
	pData = (MINE_ESSENCE*)g_dataMan.get_data_ptr(dwId,ID_SPACE_ESSENCE,type);
	if(pData) name =pData->file_model;
	return name;
}

void CSceneGatherObject::DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr)
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	
	WORD* pIndices = new WORD[(dwNum - 1)*2];
	int n = 0;
	for(int i=0; i<(int)dwNum - 1; i++)
	{
		pIndices[n] = i;
		pIndices[n+1] = i+1;
		n += 2;
	}
	
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,dwNum,pIndices,(dwNum-1)*2,clr);
	}
	delete []pIndices;
}

void CSceneGatherObject::DrawRect()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;	
	
	float fCenterX = m_vPos.x;
	float fCenterY = m_vPos.z;

	int lseg = (int)(m_Data.fEdgeLen/STEP_LEN);
	int wseg = (int)(m_Data.fEdgeWth/STEP_LEN);

	lseg += 2;
	wseg += 2;

	float flRadius = m_Data.fEdgeLen/2.0f;
	float fwRadius = m_Data.fEdgeWth/2.0f;
	float offset_y = 0.1f;

	A3DVECTOR3 vLeftBottom = A3DVECTOR3(fCenterX - flRadius ,0, fCenterY - fwRadius);
	A3DVECTOR3 vRightBottom = A3DVECTOR3(fCenterX + flRadius ,0, fCenterY -  fwRadius);
	A3DVECTOR3 vRightUp = A3DVECTOR3(fCenterX + flRadius ,0,fCenterY + fwRadius);
	A3DVECTOR3 vLeftUp = A3DVECTOR3(fCenterX - flRadius,0,fCenterY + fwRadius);
	
	A3DVECTOR3* lvertices = new A3DVECTOR3[lseg];
	A3DVECTOR3* wvertices = new A3DVECTOR3[wseg];
	DWORD clr; 
	if(m_bSelected)	clr = A3DCOLORRGB(255,255,255);
	else clr = A3DCOLORRGB(0,255,255);
	int n;
	float temp;
	for(n = 0 ; n<lseg; n++)
	{
		temp = (float)n/(float)(lseg-1);
		lvertices[n] = vLeftBottom + (vRightBottom - vLeftBottom)*temp;
		lvertices[n].y = pMap->GetTerrain()->GetPosHeight(lvertices[n]) + offset_y;//0.1f
	}
	DrawLine(lvertices,lseg,clr);

	for(n = 0 ; n<lseg; n++)
	{
		temp = (float)n/(float)(lseg-1);
		lvertices[n] = vLeftUp + (vRightUp - vLeftUp)*temp;
		lvertices[n].y = pMap->GetTerrain()->GetPosHeight(lvertices[n]) + offset_y;//0.1f
	}
	DrawLine(lvertices,lseg,clr);

	for(n = 0 ; n < wseg; n++)
	{
		temp = (float)n/(float)(wseg-1);
		wvertices[n] = vRightUp + (vRightBottom - vRightUp)*temp;
		wvertices[n].y = pMap->GetTerrain()->GetPosHeight(wvertices[n]) + offset_y;//0.1f
	}
	DrawLine(wvertices,wseg,clr);

	for(n = 0 ; n < wseg; n++)
	{
		temp = (float)n/(float)(wseg-1);
		wvertices[n] = vLeftUp + (vLeftBottom - vLeftUp)*temp;
		wvertices[n].y = pMap->GetTerrain()->GetPosHeight(wvertices[n]) + offset_y;//0.1f
	}
	DrawLine(wvertices,wseg,clr);

	delete []lvertices;
	delete []wvertices;
}

void CSceneGatherObject::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_nCopy), "第几套数据");
	
		m_pProperty->DynAddProperty(AVariant(m_Data.fEdgeLen), "长(x)");
		m_pProperty->DynAddProperty(AVariant(m_Data.fEdgeWth), "宽(y)");
		m_pProperty->DynAddProperty(AVariant(m_fArea), "面积(x,z平面)",NULL,NULL,WAY_READONLY);
		//m_pProperty->DynAddProperty(AVariant(m_Data.bGenGatherInit), "是否起始时生成");
		//m_pProperty->DynAddProperty(AVariant(m_Data.bGatherRenascenceAuto), "是否自动重生");
		//m_pProperty->DynAddProperty(AVariant(m_Data.bValidAtOneTime), "是否同时生效一次");
		//m_pProperty->DynAddProperty(AVariant(m_Data.dwGenID), "编号");
		m_pProperty->DynAddProperty(AVariant(m_Data.bGatherRenascenceAuto), "是否自动重生");
		m_pProperty->DynAddProperty(AVariant(m_Data.nGatherNum), "产生最大数");
		m_pProperty->DynAddProperty(AVariant(m_Data.nControllerId), "关联怪物控制器", (ASet*)m_pControllerSelFuncs, NULL, WAY_CUSTOM);
		m_pProperty->DynAddProperty(AVariant(m_strGatherList), "可采集对象列表", (ASet*)m_pIDSelFuncs, NULL, WAY_CUSTOM);
	}
}

void CSceneGatherObject::ReloadModels()
{
	ReleaseModels();
	A3DVECTOR3 vDelta;
	POSITION pos = m_listGather.GetHeadPosition();
	while(pos)
	{
		GATHER *pro = (GATHER *)m_listGather.GetNext(pos);
		AString pathName = GetModelPath(pro->dwId,pro->dwType);
		if(pathName.IsEmpty()) 
		{
			g_Log.Log("Warning: CSceneAIGenerator::ReloadModels(),Model's path is empty!");
			continue;
		}
		CECModel* pNew = new CECModel;
		ASSERT(pNew);
		if(!pNew->Load(pathName))
		{
			g_Log.Log("CSceneAIGenerator::ReloadModels() failed! Can't load file %s",pathName);
			pNew->Release();
			if(!pNew->Load("Models\\error\\error.ecm"))
			{	
				pNew->Release();
				delete pNew;
				continue;
			}
		}
		float fRandX = RandBetween(-1.0f,1.0f);
		float fRandZ = RandBetween(-1.0f,1.0f);
		float fEdgeLen,fEdgeWth;
		if(m_Data.fEdgeLen > 10.0f) fEdgeLen = 10.0f;
		else fEdgeLen = m_Data.fEdgeLen;
		if(m_Data.fEdgeWth > 10.0f) fEdgeWth = 10.0f;
		else fEdgeWth = m_Data.fEdgeWth;
		vDelta = A3DVECTOR3(fRandX*fEdgeLen/2.0f,0,fRandZ*fEdgeWth/2.0f);
		
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap==NULL) return;
		vDelta = vDelta + m_vPos;
		vDelta.y = pMap->GetTerrain()->GetPosHeight(vDelta) + pro->fOffset;
		pNew->GetA3DSkinModel()->SetPos(vDelta);
		pNew->GetA3DSkinModel()->SetDirAndUp(m_matRotate.GetRow(2),m_matRotate.GetRow(1));
		pNew->Tick(10);
		m_listModels.AddTail(pNew);
	}	
}

void CSceneGatherObject::ReleaseModels()
{
	POSITION pos = m_listModels.GetHeadPosition();
	while(pos)
	{
		CECModel *pModels = (CECModel *)m_listModels.GetNext(pos);
		pModels->Release();
		delete pModels;
	}	
	m_listModels.RemoveAll();
}

void RenderModels(A3DViewport *pView)
{

}

BOOL CALLBACK GATHER_SEL_FUNCS::OnActivate(void)
{
	m_strText = "";
	CSceneGatherObject* pObj = NULL;
	CString strID;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			pObj = (CSceneGatherObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_GATHER)
			{
				CGatherDlg dlg;
				dlg.Init(pObj);
				dlg.DoModal();
				pMap->SetModifiedFlag(true);

				int n = pObj->GetGatherNum();
				for( int i = 0; i < n ; i++)
				{
					GATHER *pro = pObj->GetGather(i);
					m_strText += pro->strName;
					if( i+1 < n) m_strText += ";";	
				}
			}
		}
	}
	
	return TRUE;
}

LPCTSTR CALLBACK GATHER_SEL_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK GATHER_SEL_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK GATHER_SEL_FUNCS::OnSetValue(const AVariant& var)
{
	m_strText = "";
	CString strID;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneGatherObject* pObj = (CSceneGatherObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_GATHER)
			{
				int n = pObj->GetGatherNum();
				for( int i = 0; i < n ; i++)
				{
					GATHER *pro = pObj->GetGather(i);
					m_strText += pro->strName;
					if( i+1 < n) m_strText += ";";	
				}
			}
		}
	}
}

//Controller Gather select function
BOOL CALLBACK CONTROLLER_GATHER_ID_FUNCS::OnActivate(void)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pObj = (CSceneObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_GATHER)
			{
				CNpcContollerSelDlg dlg;
				GATHER_DATA data = ((CSceneGatherObject*)pObj)->GetProperty();
				dlg.m_nDyObjID = data.nControllerId;
				if(dlg.DoModal()==IDOK)
				{
					data.nControllerId = dlg.m_nDyObjID;
					((CSceneGatherObject*)pObj)->SetProperty(data);
					CONTROLLER_OBJECT obj;
					InitControllerObj(&obj);
					g_ControllerObjectMan.GetObjByID(data.nControllerId,obj);
					if(obj.id==0) m_strText = "";
					else m_strText.Format("[%d]%s",obj.id,obj.szName);
				}
			}
		}
	}
	
	return TRUE;
}

LPCTSTR CALLBACK CONTROLLER_GATHER_ID_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK CONTROLLER_GATHER_ID_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK CONTROLLER_GATHER_ID_FUNCS::OnSetValue(const AVariant& var)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pObj = (CSceneObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_GATHER)
			{
				CNpcContollerSelDlg dlg;
				GATHER_DATA data = ((CSceneGatherObject*)pObj)->GetProperty();
				CONTROLLER_OBJECT obj;
				InitControllerObj(&obj);
				g_ControllerObjectMan.GetObjByID(data.nControllerId,obj);
				if(obj.id!=0) m_strText.Format("[%d]%s",obj.id,obj.szName);
			}
		}
	}
}

