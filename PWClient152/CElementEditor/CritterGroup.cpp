// CritterGroup.cpp: implementation of the CCritterGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneObject.h"
#include "MainFrm.h"
#include "Render.h"
#include "A3D.h"
#include "A3DAutonomousGroup.h"
#include "A3DAutonomousBehaviour.h"
#include "A3DAutonomousAgent.h"
#include "A3DAutoFirefly.h"
#include "A3DAutoFish.h"
#include "A3DAutoButterfly.h"
#include "A3DAutoEagle.h"
#include "A3DAutoCrow.h"
#include "BezierSelDlg.h"
#include "EditerBezier.h"
#include "EC_Bezier.h"

#include "SceneObjectManager.h"
#include "TerrainWater.h"
#include "Box3D.h"

#include "CritterGroup.h"

//#define new A_DEBUG_NEW
#define MAX_NUM_SECT  24
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void InitCritterData(CRITTER_DATA &data)
{
	data.dwBehaviourType = 3;
	data.dwCritterType = 0;
	data.dwNumCritter = 10;
	data.fRadius = 10;
	data.nBezierID = -1;
	data.fSpeed = 0.01f;
}

CCritterGroup::CCritterGroup()
{
	m_nObjectType = SO_TYPE_CRITTER_GROUP;
	m_pGroup = new A3DAutonomousGroup;
	m_pIDSelFuncs = new BEZIER_SEL_ID_FUNCS;
	m_pBehaviour = NULL;
	InitCritterData(m_Data);
	BuildProperty();

	//behaviour_type.AddElement("漫游",BEHAVIOUR_WANDER);
	behaviour_type.AddElement("盘旋",BEHAVIOUR_HOVER);
	//behaviour_type.AddElement("到达",BEHAVIOUR_ARRIVAL);
	behaviour_type.AddElement("通用群体行为",BEHAVIOUR_BOID);
	//behaviour_type.AddElement("逃避",BEHAVIOUR_FLEE);
	behaviour_type.AddElement("受惊逃散",BEHAVIOUR_FLEE_AND_BACK);
	
	critter_type.AddElement("蝴蝶",CRITTER_BUTTERFLY);
	critter_type.AddElement("普通鱼",CRITTER_NORMAL_FISH);
	critter_type.AddElement("呆滞的鱼",CRITTER_DOLTISH_FISH);
	critter_type.AddElement("敏捷的鱼",CRITTER_BRISK_FISH);
	critter_type.AddElement("老鹰",CRITTER_EAGLE);
	critter_type.AddElement("萤火虫",CRITTER_FIREFLY);
	critter_type.AddElement("乌鸦",CRITTER_CROW);
	
	//Test for ec_bezier
	m_pECBezier = NULL;
	m_pBezierWalker = new CECBezierWalker;
}

CCritterGroup::~CCritterGroup()
{
}

void CCritterGroup::Release()
{
	if(m_pGroup) 
	{
		m_pGroup->Release();
		delete m_pGroup;
	}
	if(m_pBehaviour)
	{
		m_pBehaviour->Release();
		delete m_pBehaviour;
		m_pBehaviour = NULL;
	}

	if (m_pBezierWalker)
		delete m_pBezierWalker;

	if(m_pECBezier)
	{
		m_pECBezier->Release();
		delete m_pECBezier;
		m_pECBezier = NULL;
	}

	if(m_pIDSelFuncs) delete m_pIDSelFuncs;
}

void CCritterGroup::Render(A3DViewport* pA3DViewport)
{
	DrawCenter();
	if(m_pGroup)
	{
		DrawCircle();
		m_pGroup->Render(pA3DViewport);
	}
	CSceneObject::Render(pA3DViewport);
}

void CCritterGroup::Tick(DWORD timeDelta)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(m_pGroup)
			m_pGroup->Update((A3DTerrain2*)(pMap->GetTerrain())->GetRender(),pMap->GetTerrainWater()->GetA3DTerrainWater(),timeDelta);
	}

	//Test ec_bezier
	if (m_pECBezier)
	{
		if (m_Data.nBezierID < 0) 
		{
			m_pECBezier->Release();
			delete m_pECBezier;
			m_pECBezier = NULL;
			return;
		}

	//	m_pBezierWalker->Tick((int)timeDelta);
		A3DVECTOR3 vTemp1 = m_pBezierWalker->GetPos();
		m_pBezierWalker->Tick(100);
		A3DVECTOR3 vTemp2 = m_pBezierWalker->GetPos();
		SetPos(m_pBezierWalker->GetPos());
		
		//g_Render.GetA3DEngine()->GetActiveCamera()->SetPos(m_pBezierWalker->GetPos());
		//g_Render.GetA3DEngine()->GetActiveCamera()->SetDirAndUp(m_pBezierWalker->GetDir(),A3DVECTOR3(0.0f,1.0f,0.0f));
	}
	else if (m_Data.nBezierID >= 0)
	{
		//	Try to bind bezier object
		BindBezier();
	}
}

void CCritterGroup::UpdateProperty(bool bGet)
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
				m_pProperty->SetPropVal(2,m_Data.fRadius);
				m_pProperty->SetPropVal(3,m_Data.dwCritterType);
				m_pProperty->SetPropVal(4,m_Data.dwNumCritter);
				m_pProperty->SetPropVal(5,m_Data.dwBehaviourType);
				m_pProperty->SetPropVal(6,m_Data.strModelPathName);
				m_pProperty->SetPropVal(7,m_Data.fSpeed);
				m_pIDSelFuncs->OnSetValue(m_strBezierName);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName = temp;
		m_vPos	= m_pProperty->GetPropVal(1);
		m_Data.fRadius = m_pProperty->GetPropVal(2);
		m_Data.dwCritterType = m_pProperty->GetPropVal(3);
		m_Data.dwNumCritter = m_pProperty->GetPropVal(4);
		m_Data.dwBehaviourType = m_pProperty->GetPropVal(5);
		m_Data.strModelPathName = AString(m_pProperty->GetPropVal(6));
		m_Data.fSpeed = m_pProperty->GetPropVal(7);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		BindBezier();
		UpdateGroup();
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_Data.fRadius);
		m_pProperty->SetPropVal(3,m_Data.dwCritterType);
		m_pProperty->SetPropVal(4,m_Data.dwNumCritter);
		m_pProperty->SetPropVal(5,m_Data.dwBehaviourType);
		m_pProperty->SetPropVal(6,m_Data.strModelPathName);
		m_pProperty->SetPropVal(7,m_Data.fSpeed);
		m_pIDSelFuncs->OnSetValue(m_strBezierName);
		
	}
}

void CCritterGroup::UpdateGroup()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(m_Data.strModelPathName.IsEmpty()) return;
	
	if(m_pGroup)
	{
		m_pGroup->Release();
	}else m_pGroup = new A3DAutonomousGroup;

	if(m_pBehaviour)
	{
		m_pBehaviour->Release();
		delete m_pBehaviour;
		m_pBehaviour = NULL;
	}

	switch(m_Data.dwBehaviourType)
	{
	case BEHAVIOUR_WANDER:
		m_pBehaviour = new A3DAutoBehaviourWander;
		break;
	case BEHAVIOUR_HOVER://1
		m_pBehaviour = new A3DAutoBehaviourHover;
		((A3DAutoBehaviourHover*)m_pBehaviour)->SetTargetPosition(m_vPos);
		break;
	case BEHAVIOUR_FLEE_AND_BACK://1
		m_pBehaviour = new A3DAutoBehaviourFleeAndBack;
		((A3DAutoBehaviourHover*)m_pBehaviour)->SetTargetPosition(m_vPos);
		break;//Arrival	
	case BEHAVIOUR_ARRIVAL://1
		m_pBehaviour = new A3DAutoBehaviourArrival;
		((A3DAutoBehaviourHover*)m_pBehaviour)->SetTargetPosition(m_vPos);
		break;
	case BEHAVIOUR_BOID:
		m_pBehaviour = new A3DAutoBehaviourBoid;
		break;
	}

	if(m_pBehaviour)
	{
		m_pGroup->SetActiveBehaviour(m_pBehaviour);
		m_pGroup->SetCenter(m_vPos);
		m_pGroup->SetBoundaryRadius(m_Data.fRadius);
		
		
		for(unsigned int i = 0; i< m_Data.dwNumCritter; i++)
		{
			A3DAutonomousAgent* agent = NULL;
			switch(m_Data.dwCritterType)
			{
			case CRITTER_BUTTERFLY:
				 agent = new A3DAutoButterfly;
				 agent->SetPosition(m_vPos);
				break;
			case CRITTER_EAGLE:
				agent = new A3DAutoEagle;
				agent->SetPosition(m_vPos);
				break;
			case CRITTER_DOLTISH_FISH:
				agent = new A3DAutoDoltishFish;
				agent->SetPosition(m_vPos);
				break;
			case CRITTER_NORMAL_FISH:
				agent = new A3DAutoFish;
				agent->SetPosition(m_vPos);
				break;
			case CRITTER_BRISK_FISH:
				agent = new A3DAutoBriskFish;
				agent->SetPosition(m_vPos);
				break;
			case CRITTER_FIREFLY:
				agent = new A3DAutoFirefly;
				agent->SetPosition(m_vPos);
				break;
			case CRITTER_CROW:
				agent = new A3DAutoCrow;
				agent->SetPosition(m_vPos);
				break;
			}
			if (!agent->Init(g_Render.GetA3DEngine()))
			{
				delete agent;
				return;
			}
			agent->SetPosition(m_vPos);
			AString path = "Models\\Autoca\\" + m_Data.strModelPathName;
			if (!agent->Load(path))
			{
				CString msg;
				msg.Format("CCritterGroup::UpdateGroup(),Failed to load autonomous agent model %s !", path);
				g_Log.Log(msg);
				g_LogDlg.Log(msg);
				agent->Release();
				delete agent;
				return;
			}
			
			if(pMap!=NULL)
			{
				LIGHTINGPARAMS light = pMap->GetLightingParams();
				A3DSkinModel::LIGHTINFO LightInfo;
				LightInfo.colAmbient	= A3DCOLORVALUE(light.dwAmbient);//	Directional light's specular color
				LightInfo.vLightDir		= light.vSunDir;//Direction of directional light
				LightInfo.colDirDiff	= A3DCOLORVALUE(light.dwSunCol);//Directional light's diffuse color
				LightInfo.colDirSpec	= A3DCOLORVALUE(0.8f,0.8f,0.8f,1.0f);//	Directional light's specular color
				LightInfo.bPtLight		= false;//	false, disable dynamic point light,
				agent->SetLightInfo(LightInfo);
//				agent->GetSkinModel()->GetSkeleton()->GetBone(0)->
//					SetScaleFactor(A3DVECTOR3((1.5+0.5*frandom01()), (1.5+0.5*frandom01()), (1.5+0.5*frandom01())));
//				agent->GetSkinModel()->GetSkeleton()->GetBone(0)->SetScaleType(A3DBone::SCALE_WHOLE);
			}
			m_pGroup->AddMember(agent);			
		}
		m_pGroup->Init();
	}
}

void CCritterGroup::Repair_Behaviour(DWORD& dwBehaviour)
{
	switch(dwBehaviour)
	{
	case	BEHAVIOUR_WANDER:	dwBehaviour = BEHAVIOUR_BOID; break;
	case	BEHAVIOUR_HOVER:	break;
	case	BEHAVIOUR_ARRIVAL:	dwBehaviour = BEHAVIOUR_BOID; break;
	case	BEHAVIOUR_BOID:	    break;
	case	BEHAVIOUR_FLEE:		dwBehaviour = BEHAVIOUR_BOID; break;
	}
}

//	Load data
bool CCritterGroup::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{ 
	if(dwVersion>6 && dwVersion<=0xd)
	{
		m_strName = AString(ar.ReadString());
		m_Data.strModelPathName = AString(ar.ReadString());
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.dwBehaviourType,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.dwCritterType,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.dwNumCritter,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.fRadius,sizeof(float)))
			return false;
		
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		Repair_Behaviour(m_Data.dwBehaviourType);
		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGroup();
	}else if(dwVersion == 0xe)
	{
		m_strName = AString(ar.ReadString());
		m_Data.strModelPathName = AString(ar.ReadString());
		
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.dwBehaviourType,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.dwCritterType,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.dwNumCritter,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.fRadius,sizeof(float)))
			return false;
		
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		Repair_Behaviour(m_Data.dwBehaviourType);
		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGroup();
	}else if( dwVersion > 0xe)
	{
		m_strName = AString(ar.ReadString());
		m_Data.strModelPathName = AString(ar.ReadString());
		m_Data.strBezierName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.dwBehaviourType,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.dwCritterType,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.dwNumCritter,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.fRadius,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.fSpeed,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.nBezierID,sizeof(int)))
			return false;
		
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		Repair_Behaviour(m_Data.dwBehaviourType);
		if(iLoadFlags!= LOAD_EXPLIGHTMAP) UpdateGroup();
	}
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	return true;
};

//	Save data
bool CCritterGroup::Save(CELArchive& ar,DWORD dwVersion)
{ 
	if(dwVersion>6)
	{
		ar.WriteString(m_strName);
		ar.WriteString(m_Data.strModelPathName);
		ar.WriteString(m_Data.strBezierName);
		
		if(!ar.Write(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&m_Data.dwBehaviourType,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_Data.dwCritterType,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_Data.dwNumCritter,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_Data.fRadius,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.fSpeed,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.nBezierID,sizeof(int)))
			return false;
	}
	
	return true;
};

void CCritterGroup::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置");
		m_pProperty->DynAddProperty(AVariant(m_Data.fRadius), "范围半径");
		m_pProperty->DynAddProperty(AVariant(m_Data.dwCritterType), "生物类型",&critter_type);
		m_pProperty->DynAddProperty(AVariant(m_Data.dwNumCritter), "生物群的生物数");
		m_pProperty->DynAddProperty(AVariant(m_Data.dwBehaviourType), "生物群的行为类型",&behaviour_type);
		m_pProperty->DynAddProperty(AVariant(m_Data.strModelPathName), "模型路径",NULL,NULL,WAY_FILENAME,"Models\\Autoca");
		m_pProperty->DynAddProperty(AVariant(m_Data.fSpeed), "沿路径移动速度(m/s)");
		m_pProperty->DynAddProperty(AVariant(m_strBezierName), "路径曲线", (ASet*)m_pIDSelFuncs, NULL, WAY_CUSTOM);
	}
}

void CCritterGroup::DrawCircle()
{
	const float pi_2 = A3D_2PI;
	const float pi_delta = pi_2/MAX_NUM_SECT;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	float fRadius = m_Data.fRadius;
	A3DVECTOR3 vertices[MAX_NUM_SECT];

	A3DVECTOR3 vPos = A3DVECTOR3(0,0,0);
	A3DMATRIX4 matTrans;
	//Calculate vertices array
	if(pMap && pMap->GetTerrain()->GetRender())
	{
		
		float angle = 0;
		//0 - pi2
		for(int i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].x = vPos.x + fRadius*cos(angle);
			vertices[i].y = vPos.y + fRadius*sin(angle);
			vertices[i].z = vPos.z;
			
			angle += pi_delta;
		}
		
		matTrans = m_matRotate*m_matTrans;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i] = vertices[i]*matTrans;
		}
		//Render wire
		DrawLine(vertices,MAX_NUM_SECT,A3DCOLORRGB(255,255,0));

		//0 - pi2
		angle = 0;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].x = vPos.x + fRadius*cos(angle);
			vertices[i].z = vPos.z + fRadius*sin(angle);
			vertices[i].y = vPos.y;
			
			angle += pi_delta;
		}
		matTrans = m_matRotate*m_matTrans;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i] = vertices[i]*matTrans;
		}
		//Render wire
		DrawLine(vertices,MAX_NUM_SECT,A3DCOLORRGB(255,255,0));

		angle = 0;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].y = vPos.y + fRadius*cos(angle);
			vertices[i].z = vPos.z + fRadius*sin(angle);
			vertices[i].x = vPos.x;
			
			angle += pi_delta;
		}
		matTrans = m_matRotate*m_matTrans;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i] = vertices[i]*matTrans;
		}
		//Render wire
		DrawLine(vertices,MAX_NUM_SECT,A3DCOLORRGB(255,255,0));
	}
}

void CCritterGroup::DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr)
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	
	WORD pIndices[MAX_NUM_SECT*2];
	int n = 0;
	for(int i=0; i<MAX_NUM_SECT; i++)
	{
		pIndices[n] = i;
		pIndices[n+1] = (i+1)%(MAX_NUM_SECT);
		n += 2;
	}
	
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,dwNum,pIndices,MAX_NUM_SECT*2,clr);
	}
}

void CCritterGroup::Translate(A3DVECTOR3 vDelta)
{
	m_vPos += vDelta;
	m_matTrans.SetRow(3,m_vPos);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	if(m_pGroup)
		m_pGroup->SetCenter(m_vPos);
}

void CCritterGroup::SetPos(A3DVECTOR3 vPos)
{
	m_matTrans.Translate(vPos.x,vPos.y,vPos.z);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	m_vPos = vPos;
	if(m_pGroup)
		m_pGroup->SetCenter(m_vPos);
}

//判断区域是否跟该区域相交
bool CCritterGroup::IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax)
{
	A3DVECTOR3 max = m_vPos + A3DVECTOR3(m_Data.fRadius,m_Data.fRadius,m_Data.fRadius);
	A3DVECTOR3 min = m_vPos - A3DVECTOR3(m_Data.fRadius,m_Data.fRadius,m_Data.fRadius);
	
	if(min.x>=vMax.x) return false;
	if(min.z>=vMax.z) return false;
	if(max.x<vMin.x) return false;
	if(max.z<vMin.z) return false;
	return true;
}

CSceneObject* CCritterGroup::CopyObject()
{
	CCritterGroup *pNewObject = new CCritterGroup(*this);
	ASSERT(pNewObject);
	//pNewObject->setoSO_TYPE_CRITTER_GROUP
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildProperty();
	pNewObject->ResetGroup();
	int CritterNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("CritterGroup_%d",CritterNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				CritterNum++;
			}else 
			{
				CritterNum++;
				break;
			}
		}
		pNewObject->m_nObjectID = pSManager->GenerateObjectID();
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
		pNewObject->UpdateGroup();
	}
	return pNewObject;
}

void CCritterGroup::BindBezier()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if (!pMap)
		return;

	if (m_Data.nBezierID < 0)
	{
		if (m_pECBezier)
		{
			m_pECBezier->Release();
			delete m_pECBezier;
			m_pECBezier = NULL;
		}

		return;
	}
	
	CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
	CEditerBezier* pEditBezier = pSManager->FindBezier(m_Data.nBezierID);
	if (!pEditBezier) return;
	if (!pEditBezier->IsBezier()) return;

	if (m_pECBezier)
	{
		m_pECBezier->Release();
		delete m_pECBezier;
	}
	
	m_pECBezier = new CECBezier;

	pEditBezier->GeneralECBezier(m_pECBezier);
	
	m_pBezierWalker->BindBezier(m_pECBezier);
	m_pBezierWalker->SetSpeed(m_Data.fSpeed);
	m_pBezierWalker->StartWalk(true, true);
			
	/*For test
	FILE* File = NULL;
	File = fopen("d:\\test_ec_bezier.dat","w");
	if (File==NULL)
	{
		return;
	}
	
	//Create file archive
	if(!pEBezier->ExportBezier(File,A3DVECTOR3(0,0,0))) 
	{
		fclose(File);
		return;
	}
	fclose(File);
	
	if(m_pECBezier) m_pECBezier->Release();
	else m_pECBezier = new CECBezier();
	
	File = fopen("d:\\test_ec_bezier.dat","r");
	if (File==NULL)
	{	
		return;
	}
	
	if(!m_pECBezier->Load(File)) 
	{
		fclose(File);
		return;
	}

	m_pECBezier->SetSpeed(m_Data.fSpeed);
	m_pECBezier->SetForward(false);
	fclose(File);
	*/
}

BOOL CALLBACK BEZIER_SEL_ID_FUNCS::OnActivate(void)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject *pObj = (CSceneObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_CRITTER_GROUP)
			{
				CBezierSelDlg dlg;
				CRITTER_DATA data = ((CCritterGroup*)pObj)->GetProperty();
				dlg.m_nBezierID = data.nBezierID;
				ASSERT(pObj);
				if(dlg.DoModal()==IDOK)
				{
					m_strText = dlg.m_strBezierName;
					
					data.nBezierID = dlg.m_nBezierID;
					data.strBezierName = dlg.m_strBezierName;
					((CCritterGroup*)pObj)->SetProperty(data);
				}
			}
		}
	}
	
	return TRUE;
}

LPCTSTR CALLBACK BEZIER_SEL_ID_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK BEZIER_SEL_ID_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK BEZIER_SEL_ID_FUNCS::OnSetValue(const AVariant& var)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			CSceneObject* pObj = (CSceneObject*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_CRITTER_GROUP)
			{
				CRITTER_DATA data = ((CCritterGroup*)pObj)->GetProperty();
				m_strText = data.strBezierName;
			}
		}
	}
}
