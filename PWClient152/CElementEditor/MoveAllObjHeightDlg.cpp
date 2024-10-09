// MoveAllObjHeightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "MoveAllObjHeightDlg.h"
#include "SceneObject.h"
#include "SceneObjectManager.h"
#include "EL_Building.h"
#include "SceneSpeciallyObject.h"
#include "A3DGFXEx.h"
#include "A3DGFXElement.h"
#include "EL_Forest.h"
#include "EL_Tree.h"
#include "EL_Grassland.h"
#include "EL_GrassType.h"
#include "TerrainLayer.h"
#include "EC_Model.h"
#include "SceneAIGenerator.h"
#include "SceneLightObject.h"
#include "SceneWaterObject.h"
#include "EditerBezier.h"
#include "SceneBoxArea.h"
#include "SceneDummyObject.h"
#include "ScenePrecinctObject.h"
#include "TerrainPlants.h"


#include "CritterGroup.h"
#include "A3DAutonomousGroup.h"
#include "SceneAudioObject.h"
#include "SceneSkinModel.h"
#include "StaticModelObject.h"
#include "AScriptFile.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CMoveAllObjHeightDlg dialog


CMoveAllObjHeightDlg::CMoveAllObjHeightDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMoveAllObjHeightDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMoveAllObjHeightDlg)
	m_fDelta = 0.0f;
	//}}AFX_DATA_INIT
}


void CMoveAllObjHeightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveAllObjHeightDlg)
	DDX_Control(pDX, IDC_PROGRESS_MOVE, m_Progress);
	DDX_Text(pDX, IDC_EDIT_DELTA, m_fDelta);
	DDV_MinMaxFloat(pDX, m_fDelta, -1000.f, 1000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveAllObjHeightDlg, CDialog)
	//{{AFX_MSG_MAP(CMoveAllObjHeightDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveAllObjHeightDlg message handlers

bool CMoveAllObjHeightDlg::ProcessMapData()
{
	
	if(IDNO==MessageBox("你真的要调整场景内所有对象的高度吗？","注意",MB_YESNO))
		return true;
	
	UpdateData(true);
	m_fMoveDelta = m_fDelta;
	
	CMainFrame *pFrame = AUX_GetMainFrame();
	if(pFrame)
	{
		if(pFrame->GetMap()) 
		{
			MessageBox("请先关闭所有打开地图文件!");
			return false;
		}
		
		CString mapName;
		CElementMap emap;
		
		// Load project file list
		AScriptFile File;
		DWORD dwCol;
		char szProj[256];
		sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
		if (!File.Open(szProj))
		{
			g_Log.Log("Open file %s failed!",szProj);
			return false;
		}
		
		if(File.GetNextToken(true))
		{
			CString strCol(File.m_szToken);
			if(strCol.Find("col=")!=-1)
			{
				strCol.Replace("col=","");
				dwCol = (int)atof(strCol);
			}else
			{
				File.ResetScriptFile();
				dwCol = 1;
			}
			
		}
		abase::vector<CString *> projectList;
		while (File.GetNextToken(true))
		{
			CString *pNew = new CString(File.m_szToken);
			projectList.push_back(pNew);
		}
		File.Close();
		
		BeginWaitCursor();
		int size = projectList.size();
		m_Progress.SetRange(0,size);
		m_Progress.SetStep(1);
		abase::vector<CString *>::iterator theIterator;
		for(theIterator = projectList.begin(); theIterator != projectList.end(); theIterator ++)
		{
			mapName.Format("%sEditMaps\\%s\\%s\\%s.elproj",g_szWorkDir,g_Configs.szCurProjectName,*(*theIterator),*(*theIterator));
			emap.Load(mapName);
			TransMap(&emap);
			if(!emap.Save(mapName))
				g_Log.Log("CMoveAllObjHeightDlg::ProcessMapData(), 保存地图 %s 失败!");
			emap.Release();
			delete (*theIterator);
			m_Progress.StepIt();
		}
		projectList.clear();
		g_Log.Release();
		EndWaitCursor();
	}
	return true;
}

void CMoveAllObjHeightDlg::TransMap(CElementMap *pMap)
{
	ElementResMan mResMan;
	CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
	ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = pMan->m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		switch(type)
		{
		case CSceneObject::SO_TYPE_LIGHT:
				ProcessLight(ptemp);
				break;
			case CSceneObject::SO_TYPE_STATIC_MODEL:
				ProcessStaticModel(ptemp);
				break;
			case CSceneObject::SO_TYPE_WATER:
				ProcessWater(ptemp);
				break;
			case CSceneObject::SO_TYPE_BEZIER:
				ProcessBezier(ptemp);
				break;
			case CSceneObject::SO_TYPE_AREA:
				ProcessArea(ptemp);
				break;
			case CSceneObject::SO_TYPE_AUDIO:
				ProcessAudio(ptemp);
				break;
			case CSceneObject::SO_TYPE_DUMMY:
				ProcessDummy(ptemp);
				break;
			case CSceneObject::SO_TYPE_AIAREA:
				ProcessAiArea(ptemp);
				break;
			case CSceneObject::SO_TYPE_SPECIALLY:
				ProcessSpecially(ptemp);
				break;
			case CSceneObject::SO_TYPE_ECMODEL:
				ProcessECMdoel(ptemp);
				break;
			case CSceneObject::SO_TYPE_CRITTER_GROUP:
				ProcessCritterGroup(ptemp);
				break;
			case CSceneObject::SO_TYPE_FIXED_NPC:
				ProcessFixedNpc(ptemp);
				break;
			case CSceneObject::SO_TYPE_PRECINCT:
				ProcessPrecinct(ptemp);
				break;
		}
	}
	
	ProcessTree(pMap);
	ProcessGrass(pMap);
	ProcessTerrain(pMap);
}

void CMoveAllObjHeightDlg::ProcessLight(CSceneObject*pObj)
{
	CSceneLightObject *pLight = ((CSceneLightObject *)pObj);
	if(pLight==NULL) return;
	A3DVECTOR3 vpos = pLight->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pLight->SetPos(vpos);
}
	
void CMoveAllObjHeightDlg::ProcessWater(CSceneObject*pObj)
{
	CSceneWaterObject *pWater = ((CSceneWaterObject *)pObj);
	if(pWater==NULL) return;
	A3DVECTOR3 vpos = pWater->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pWater->SetPos(vpos);
}

void CMoveAllObjHeightDlg::ProcessBezier(CSceneObject*pObj)
{
	CEditerBezier * pBezier = ((CEditerBezier*)pObj);
	if(pBezier == NULL) return;
	A3DVECTOR3 vpos = pBezier->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pBezier->SetPos(vpos);
}

void CMoveAllObjHeightDlg::ProcessArea(CSceneObject*pObj)
{
	CSceneBoxArea *pBox = (CSceneBoxArea*)pObj;
	if(pBox==NULL) return;
	A3DVECTOR3 vpos = pBox->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pBox->SetPos(vpos);
}
	
void CMoveAllObjHeightDlg::ProcessDummy(CSceneObject*pObj)
{
	CSceneDummyObject *pDummy = (CSceneDummyObject *)pObj;
	if(pDummy==NULL) return;
	A3DVECTOR3 vpos = pDummy->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
}
	
void CMoveAllObjHeightDlg::ProcessPrecinct(CSceneObject*pObj)
{
	CScenePrecinctObject *pPrecinct = (CScenePrecinctObject*) pObj;
	if(pPrecinct==NULL) return;
	A3DVECTOR3 vpos =pPrecinct->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pPrecinct->SetPos(vpos);
}


void CMoveAllObjHeightDlg::ProcessStaticModel(CSceneObject *pObj)
{
	/*
	if(((CStaticModelObject*)pObj)->GetA3DModel()== NULL) return;
	CELBuilding *pBuilding = ((CStaticModelObject*)pObj)->GetA3DModel()->pModel;
	if(pBuilding==NULL) return;
	A3DVECTOR3 vpos = pBuilding->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pBuilding->SetPos(vpos);*/
	A3DVECTOR3 vpos = pObj->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pObj->SetPos(vpos);
}
void CMoveAllObjHeightDlg::ProcessAudio(CSceneObject *pObj )
{
	CSceneAudioObject *pAudio = (CSceneAudioObject*)pObj;
	if(pAudio==NULL) return;
	A3DVECTOR3 vpos = pAudio->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pAudio->SetPos(vpos);
}

void CMoveAllObjHeightDlg::ProcessAiArea(CSceneObject *pObj)
{
	CSceneAIGenerator *pAi = (CSceneAIGenerator*)pObj;
	if(pAi == NULL) return;
	A3DVECTOR3 vpos = pAi->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pAi->SetPos(vpos);
}

void CMoveAllObjHeightDlg::ProcessSpecially(CSceneObject *pObj)
{
	CSceneSpeciallyObject *pEfx = (CSceneSpeciallyObject*)pObj;
	if(pEfx==NULL) return;
	A3DVECTOR3 vpos = pEfx->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pEfx->SetPos(vpos);
}

void CMoveAllObjHeightDlg::ProcessECMdoel(CSceneObject *pObj)
{
	CSceneSkinModel *pModel = ((CSceneSkinModel*)pObj);
	if(pModel==NULL) return;

	A3DVECTOR3 vpos = pModel->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pModel->SetPos(vpos);
}

void CMoveAllObjHeightDlg::ProcessCritterGroup(CSceneObject *pObj)
{
	CCritterGroup* pGroup = ((CCritterGroup*)pObj);
	if(pGroup==NULL) return;
	A3DVECTOR3 vpos = pGroup->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pGroup->Translate(vpos);
}

void CMoveAllObjHeightDlg::ProcessFixedNpc(CSceneObject *pObj)
{
	CSceneFixedNpcGenerator *pModel = ((CSceneFixedNpcGenerator *)pObj);
	if(pModel==NULL) return;
	A3DVECTOR3 vpos = pModel->GetPos();
	vpos.y = vpos.y - m_fMoveDelta;
	pModel->SetPos(vpos);
}

void CMoveAllObjHeightDlg::ProcessTree(CElementMap *pMap)
{
	CTerrainPlants *pPlants = pMap->GetSceneObjectMan()->GetPlants();	
	if(pPlants==NULL) return;
	int n = pPlants->GetPlantNum();
	for(int i = 0; i < n; i++)
	{
		PPLANT pp = pPlants->GetPlantByIndex(i);
		if(pp->GetPlantType()==PLANT_TREE)
		{
			CELForest *pForest = pMap->GetSceneObjectMan()->GetElForest();
			CELTree *pTree = pForest->GetTreeTypeByID(pp->nID);
			
			ALISTPOSITION pos = pp->m_listPlantsPos.GetTailPosition();
			while(pos)
			{
				PPLANTPOS ptemp = pp->m_listPlantsPos.GetPrev(pos);
				ptemp->y = ptemp->y - m_fMoveDelta;
				pTree->AdjustTreePosition(ptemp->id,A3DVECTOR3(ptemp->x,ptemp->y,ptemp->z));
			}
		}
	}
}

void CMoveAllObjHeightDlg::ProcessGrass(CElementMap *pMap)
{
}

void CMoveAllObjHeightDlg::ProcessTerrain(CElementMap *pMap)
{
	/*
	CTerrain *pTerrain = pMap->GetTerrain();
	if(pTerrain==NULL) return;
	CString strDstPath;

	int n = pTerrain->GetLayerNum();
	for(int i = 0; i < n; i ++)
	{
		CTerrainLayer *pLayer = pTerrain->GetLayer(i);
		CString texFile = 	pLayer->GetTextureFile();
	    CString texSpec = pLayer->GetSpecularMapFile();
		strDstPath = m_strPath + "\\" + texFile;
		CreateFileSystem(strDstPath,texFile,NULL,pMap);
		strDstPath = m_strPath + "\\" + texSpec;
		CreateFileSystem(strDstPath,texSpec,NULL,pMap);	
	}
	*/
}

void CMoveAllObjHeightDlg::OnOK()
{
	if(!ProcessMapData()) return;
	CDialog::OnOK();
}
