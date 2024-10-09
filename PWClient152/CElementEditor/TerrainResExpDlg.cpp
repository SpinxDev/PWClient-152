// TerrainResExpDlg.cpp : implementation file
//

#include "global.h"

#include "elementeditor.h"
#include "TerrainResExpDlg.h"
#include "MainFrm.h"
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

#include "CritterGroup.h"
#include "A3DAutonomousGroup.h"
#include "SceneAudioObject.h"
#include "SceneSkinModel.h"
#include "StaticModelObject.h"
#include "Bitmap.h"

#include "AF.h"
#include "A3D.h"

#include <Shlwapi.h>

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTerrainResExpDlg dialog


CTerrainResExpDlg::CTerrainResExpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTerrainResExpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTerrainResExpDlg)
	m_strPath = _T("");
	m_bIsExpTexture = FALSE;
	//}}AFX_DATA_INIT
	nExpType = exp_res;
	m_nTopViewWidth = 256;
}


void CTerrainResExpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerrainResExpDlg)
	DDX_Control(pDX, IDC_PROGRESS_RES_EXP, m_Progress);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDX_Check(pDX, IDC_CHECK_TEXTURE, m_bIsExpTexture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerrainResExpDlg, CDialog)
	//{{AFX_MSG_MAP(CTerrainResExpDlg)
	ON_BN_CLICKED(IDC_PATH_SELECT, OnPathSelect)
	ON_BN_CLICKED(IDC_RADIO_SIZE512, OnRadioSize512)
	ON_BN_CLICKED(IDC_RADIO_SIZE1024, OnRadioSize1024)
	ON_BN_CLICKED(IDC_RADIO_SIZE256, OnRadioSize256)
	ON_BN_CLICKED(IDC_CHECK_TEXTURE, OnCheckTexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainResExpDlg message handlers

void CTerrainResExpDlg::OnOK()
{
	UpdateData(true);
	ExpResData();
	CDialog::OnOK();
}

void CTerrainResExpDlg::OnCancel()
{
	CDialog::OnCancel();
}

bool CTerrainResExpDlg::ExpResData()
{
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
		
		
		char szPath[256];
		if(nExpType==exp_topview)
		{
			sprintf(szPath,"%s\\Day",m_strPath);
			if(!PathFileExists(szPath))
			{
				if(!CreateDirectory(szPath, NULL))
				{
					MessageBox("创建输出路径失败！");
					return false;
				}
			}
			sprintf(szPath,"%s\\Night",m_strPath);
			if(!PathFileExists(szPath))
			{
				if(!CreateDirectory(szPath, NULL))
				{
					MessageBox("创建输出路径失败！");
					return false;
				}
			}	
		}
		
		BeginWaitCursor();
		m_Log.Init("LostFile.log", "下面是地图中丢失文件信息,请把这些文件拷贝到相应的位置或是在地图中删除引用该文件的场景对象，然后重新输出资源 ...");
		int size = projectList.size();
		m_Progress.SetRange(0,size);
		m_Progress.SetStep(1);
		abase::vector<CString *>::iterator theIterator;
		int count_a = 0;
		int iProjColCnt = dwCol;
		int iProjRowCnt = projectList.size()/dwCol;

		bool bRandomMap = AUX_IsRandomMapWorkSpace();
		float fMapSize = bRandomMap ? 0.0f : 1024.0f;

		float fWorldWid		= fMapSize * iProjColCnt;
		float fWorldLen		= fMapSize * iProjRowCnt;

		for(theIterator = projectList.begin(); theIterator != projectList.end(); theIterator ++)
		{
			if(nExpType==exp_topview)
			{
				
				iProjColCnt = count_a%dwCol;
				iProjRowCnt = count_a/dwCol;

				mapName.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,*(*theIterator),*(*theIterator));

				if (bRandomMap && fMapSize < 1.0f) // 随机地图做特殊处理。 地图尺寸应该有个全局配置文件
				{
					emap.Load(mapName);
					if(emap.GetTerrain())
						fMapSize = emap.GetTerrain()->GetTerrainSize();
					else fMapSize = 1024.f;

					fWorldWid		= fMapSize * iProjColCnt;
					fWorldLen		= fMapSize * iProjRowCnt;
					emap.Release();
				}
				float fSceneOffX = -fWorldWid * 0.5f + iProjColCnt * fMapSize + fMapSize * 0.5f;
				float fSceneOffZ = fWorldLen * 0.5f - iProjRowCnt * fMapSize - fMapSize * 0.5f;

				emap.m_fUoff = fSceneOffX;
				emap.m_fVoff = fSceneOffZ;
				emap.Load(mapName);
				bool bShow[3];
				bShow[0] = g_Configs.bShowStaticModel;
				bShow[1] = g_Configs.bShowTree;
				bShow[2] = g_Configs.bShowWater;
				if(m_bIsExpTexture)
				{
					g_Configs.bShowStaticModel = false;
					g_Configs.bShowTree = false;
					g_Configs.bShowWater = false;
				}
				emap.GetSceneObjectMan()->ClearSelectedList();
				sprintf(szPath,"%s\\Day",m_strPath);
				emap.ExpTopView(szPath,m_nTopViewWidth,false);
				sprintf(szPath,"%s\\Night",m_strPath);
				emap.ExpTopView(szPath,m_nTopViewWidth,true);
				g_Configs.bShowStaticModel = bShow[0];
				g_Configs.bShowTree = bShow[1];
				g_Configs.bShowWater = bShow[2];
			}else if(nExpType == exp_test_precinct)
			{
				mapName.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,*(*theIterator),*(*theIterator));
				if(!emap.Load(mapName)) 
				{
					CString error_msg; 
					error_msg.Format("不能打开地图 %s",mapName);
					MessageBox(error_msg);
					continue;
				}
				CELBitmap *pPrecinctMap = emap.ExpPrecinctMap();
				bool error = false;
				if(pPrecinctMap==NULL) continue;
				
				BYTE *pData = pPrecinctMap->GetBitmapData();
				for( int w  = 0; w < pPrecinctMap->GetWidth(); w++)
				{	
					for( int h = 0; h < pPrecinctMap->GetHeight(); h++)
					{	
						if(pData[h*pPrecinctMap->GetWidth() + w]!=0)
						{
							error = true;
							break;
						}
					}
					if(error) break;
				}
				if(error)
				{
				   CString dst;
				   dst.Format("%s\\%s_error.bmp",m_strPath,emap.GetMapName());
				   pPrecinctMap->SaveToFile(dst);
				}
				pPrecinctMap->Release();
				delete pPrecinctMap;
			}else
			{
				mapName.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,*(*theIterator),*(*theIterator));
				if(!emap.Load(mapName)) 
				{
					CString error_msg; 
					error_msg.Format("不能打开地图 %s",mapName);
					MessageBox(error_msg);
					continue;
				}
				TransMap(&emap);
			}
			
			emap.Release();
			delete (*theIterator);
			m_Progress.StepIt();
			count_a++;
		}
		projectList.clear();
		m_Log.Release();
		EndWaitCursor();
	}
	return true;
}

void CTerrainResExpDlg::OnPathSelect() 
{
	// TODO: Add your control notification handler code here
	//	Let user select a place to store exporting data
	BROWSEINFO Info;
	memset(&Info, 0, sizeof (Info));

	static char szPath[MAX_PATH] = "\0";
	if (!szPath[0])
		strcpy(szPath, g_szWorkDir);

	Info.hwndOwner		= m_hWnd;
	Info.pidlRoot		= NULL;
	Info.pszDisplayName	= szPath;
	Info.lpszTitle		= "请选择导出数据的存放路径：";
	Info.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_DONTGOBELOWDOMAIN;

	LPITEMIDLIST pPIDL;
	if (!(pPIDL = SHBrowseForFolder(&Info)))
		return;

	SHGetPathFromIDList(pPIDL, szPath);
	char* szName = strrchr(szPath, '\\');
	if (!szName || *(szName+1) == '\0')
	{
		MessageBox("无法导出数据到此路径。");
		return;
	}

	m_strPath = szPath;
	UpdateData(false);
}

bool CTerrainResExpDlg::CopyResFile( const char *szDst, const char *szSrc, CSceneObject* pObj, CElementMap *pMap)
{
	if(PathFileExists(szDst))
		return TRUE;
	
	CFile sFile,dFile;
	CString path,name;
	
	if(!sFile.Open(szSrc,CFile::modeRead | CFile::typeBinary))
	{
		if(pObj)
			m_Log.Log("--文件不存在: %s! MapName is %s, ObjectName is %s!",szSrc,pMap->GetMapName(),pObj->GetObjectName());
		else m_Log.Log("--文件不存在: %s! MapName is %s",szSrc,pMap->GetMapName());
		return false;
	}
	if(!dFile.Open(szDst,CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		sFile.Close();
		g_Log.Log(" CTerrainResExpDlg::CopyResFile(),创建文件%s失败!",szDst);
		return false;
	}
	DWORD dwFileLen = sFile.GetLength();
	DWORD dwLoaded = 0;
	BYTE* pBuffer = new BYTE[dwFileLen];
	ASSERT(pBuffer);
	if(0==sFile.Read(pBuffer,dwFileLen))
		goto failed;
	dFile.Write(pBuffer,dwFileLen);
	
	sFile.Close();
	dFile.Close();
	delete pBuffer;
	return true;

failed:
	sFile.Close();
	dFile.Close();
	delete pBuffer;
	return false;
}

bool CTerrainResExpDlg::CreateFileSystem(const char *szDst, const char *szSrc, CSceneObject* pObj, CElementMap *pMap)
{
	CString srcpath = szSrc;
	if(srcpath.IsEmpty()) return false;
	CString path = szDst;

	int pos = path.ReverseFind('\\');
	if(pos == -1)
	{
		pos = 0;
	}
	path = path.Mid(0, pos);

	if(!CreateFolder(path)) return false;
	if(!CopyResFile(szDst,szSrc,pObj,pMap)) return false;
	return true;
}

CString CTerrainResExpDlg::GetParentPath(CString  path)
{
	if(path[path.GetLength() - 1] == '\\')
	{
		path = path.Mid(0, path.GetLength() - 1);
	}

	CString parent;
	int pos = path.ReverseFind('\\');
	if(pos == -1)
	{
		pos = 0;
	}

	parent = path.Mid(0, pos);
	return parent;
}

BOOL CTerrainResExpDlg::CreateFolder(CString path)
{
	if(PathFileExists(path))
		return TRUE;
	
	if(!CreateDirectory(path, NULL))
	{
		if(path.GetLength() == 2)
		{
			return FALSE;
		}

		if(!CreateFolder(GetParentPath(path)))
		{
			return FALSE;
		}

		if(!CreateDirectory(path, NULL))
		{
			return FALSE;
		}
	}
	return TRUE;
}


void CTerrainResExpDlg::TransMap(CElementMap *pMap)
{
	ElementResMan mResMan;
	CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
	if(pMan==NULL) return;
	ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = pMan->m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		switch(type)
		{
		case CSceneObject::SO_TYPE_LIGHT:
				break;
			case CSceneObject::SO_TYPE_STATIC_MODEL:
				ProcessStaticModel(ptemp ,pMap);
				break;
			case CSceneObject::SO_TYPE_WATER:
				break;
			case CSceneObject::SO_TYPE_BEZIER:
				break;
			case CSceneObject::SO_TYPE_AREA:
				break;
			case CSceneObject::SO_TYPE_AUDIO:
				ProcessAudio(ptemp ,pMap);
				break;
			case CSceneObject::SO_TYPE_DUMMY:
				break;
			case CSceneObject::SO_TYPE_AIAREA:
				ProcessAiArea(ptemp ,pMap);
				break;
			case CSceneObject::SO_TYPE_SPECIALLY:
				ProcessSpecially(ptemp ,pMap);
				break;
			case CSceneObject::SO_TYPE_ECMODEL:
				ProcessECMdoel(ptemp ,pMap);
				break;
			case CSceneObject::SO_TYPE_CRITTER_GROUP:
				ProcessCritterGroup(ptemp ,pMap);
				break;
			case CSceneObject::SO_TYPE_FIXED_NPC:
				ProcessFixedNpc(ptemp ,pMap);
				break;
			case CSceneObject::SO_TYPE_PRECINCT:
				break;
		}
	}
	
	ProcessTree(pMap);
	ProcessGrass(pMap);
	ProcessTerrain(pMap);
}

void CTerrainResExpDlg::ProcessStaticModel(CSceneObject *pObj ,CElementMap *pMap)
{
	if((((CStaticModelObject*)pObj)->GetA3DModel() == NULL) || ((CStaticModelObject*)pObj)->GetA3DModel()->pModel==NULL) return;
	CELBuilding *pBuilding = ((CStaticModelObject*)pObj)->GetA3DModel()->pModel;
	if(pBuilding==NULL) return;
	CString strDstPath;

	int nMesh = pBuilding->GetA3DLitModel()->GetNumMeshes();
	for( int i = 0; i < nMesh; i++)
	{
		A3DLitMesh *pMesh = pBuilding->GetA3DLitModel()->GetMesh(i);
		if(pMesh)
		{
			CString strTexture = pMesh->GetTextureMap();
			if(strTexture.IsEmpty()) return;
			strDstPath = m_strPath + "\\" + strTexture;
			CreateFileSystem(strDstPath,strTexture,pObj,pMap);
		}
	}
	
	strDstPath = m_strPath + "\\";
	strDstPath = strDstPath + CString(((CStaticModelObject*)pObj)->GetModelPath());
	CreateFileSystem(strDstPath,((CStaticModelObject*)pObj)->GetModelPath() ,pObj,pMap);
}

void CTerrainResExpDlg::ProcessAudio(CSceneObject *pObj ,CElementMap *pMap)
{
	CSceneAudioObject *pAudio = (CSceneAudioObject*)pObj;
	GFX_AUDIO_DATA pro = pAudio->GetProperty();
	CString strDstPath;
	CString strAudioPath = "Sfx\\" + pro.m_strPathName;
	strDstPath = m_strPath + "\\";
	strDstPath = strDstPath + CString(strAudioPath);
	CreateFileSystem(strDstPath,strAudioPath,pObj,pMap);
}

void CTerrainResExpDlg::ProcessAiArea(CSceneObject *pObj ,CElementMap *pMap)
{
	CSceneAIGenerator *pAi = (CSceneAIGenerator*)pObj;
	if(pAi == NULL) return;
	CString strDstPath;

	int num = pAi->GetNPCNum();
	for( int k = 0; k < num; k ++)
	{
		CECModel *pModel = pAi->GetNpcModel(k);
		
		if(pModel==NULL) continue;
		if(pModel->GetA3DSkinModel()==NULL) continue;
		
		int n = pModel->GetA3DSkinModel()->GetSkinNum();
		for( int i = 0; i < n; i++)
		{
			A3DSkin *pSkin = pModel->GetA3DSkinModel()->GetA3DSkin(i);
			int m = pSkin->GetTextureNum();
			for( int j = 0; j < m; j++)
			{
				A3DTexture *pTexture = pSkin->GetTexture(j);
				CString strTex = pTexture->GetMapFile();
				strDstPath = m_strPath + "\\" + strTex;
				CreateFileSystem(strDstPath,strTex,pObj,pMap);
			}
		}
		CString modelPath = pAi->GetNPCModelPath(k);
		strDstPath = m_strPath + "\\" + modelPath;
		CreateFileSystem(strDstPath,modelPath,pObj,pMap);
	}
}

void CTerrainResExpDlg::ProcessSpecially(CSceneObject *pObj ,CElementMap *pMap)
{
	A3DGFXEx *pGfxEx = ((CSceneSpeciallyObject*)pObj)->GetGfxExObject();
	if(pGfxEx==NULL) return;
	CString strDstPath;

	int n = pGfxEx->GetElementCount();
	for( int i = 0; i < n ; i++)
	{
		A3DGFXElement *pEL = pGfxEx->GetElement(i);
		if(pEL==NULL) continue;
		GFX_PROPERTY pro = pEL->GetProperty(ID_GFXOP_TEX_PATH);
		CString strTexture,strSound;
		strTexture = pro.m_string;
		strTexture = "GFX\\Textures\\" + strTexture;
		pro = pEL->GetProperty(ID_GFXOP_SOUND_FILE);
		strSound = pro.m_string;
		strDstPath = m_strPath + "\\" + strTexture;
		CreateFileSystem(strDstPath,strTexture,pObj,pMap);

		strDstPath = m_strPath + "\\" + strSound;
		CreateFileSystem(strDstPath,strSound,pObj,pMap);
	}
	CString gfxPath = "Gfx\\";
	gfxPath += ((CSceneSpeciallyObject*)pObj)->GetGfxExFileName();
	strDstPath = m_strPath + "\\";
	strDstPath = strDstPath + gfxPath;
	CreateFileSystem(strDstPath,gfxPath,pObj,pMap);
}

void CTerrainResExpDlg::ProcessECMdoel(CSceneObject *pObj ,CElementMap *pMap)
{
	CECModel *pModel = ((CSceneSkinModel*)pObj)->GetECModel();
	if(pModel==NULL) return;
	CString strDstPath;
	
	int n = pModel->GetA3DSkinModel()->GetSkinNum();
	for( int i = 0; i < n; i++)
	{
		A3DSkin *pSkin = pModel->GetA3DSkinModel()->GetA3DSkin(i);
		int m = pSkin->GetTextureNum();
		for( int j = 0; j < m; j++)
		{
			A3DTexture *pTexture = pSkin->GetTexture(j);
			CString strTex = pTexture->GetMapFile();
			strDstPath = m_strPath + "\\" + strTex;
			CreateFileSystem(strDstPath,strTex,pObj,pMap);
		}
	}
	CString srcPath = ((CSceneSkinModel *)pObj)->GetECModelPath();
	strDstPath = m_strPath + "\\" + srcPath;
	CreateFileSystem(strDstPath,srcPath,pObj,pMap);
}

void CTerrainResExpDlg::ProcessCritterGroup(CSceneObject *pObj ,CElementMap *pMap)
{
	CRITTER_DATA data = ((CCritterGroup*)pObj)->GetProperty();
	if(data.strModelPathName.IsEmpty()) return;
	CString strDstPath;
	CString path = "Models\\Autoca\\" + data.strModelPathName;
	
	strDstPath = m_strPath + "\\" + path;
	CreateFileSystem(strDstPath,path,pObj,pMap);

	A3DAutonomousGroup* pGroup = ((CCritterGroup*)pObj)->GetA3DAutonomousGroup();
	if(pGroup==NULL) return;
	const A3DAutonomousAgentVector& lst = pGroup->GetAllMembers();
	for( int i = 0; i < (int)lst.size(); i++)
	{
		A3DAutonomousAgent* pAgent = lst[i];
		A3DSkinModel *pSkinModel = pAgent->GetSkinModel();
		if(pSkinModel==NULL) continue;
		for( int h = 0; h < pSkinModel->GetSkinNum(); h++)
		{
			A3DSkin *pSkin = pSkinModel->GetA3DSkin(h);
			int m = pSkin->GetTextureNum();
			for( int j = 0; j < m; j++)
			{
				A3DTexture *pTexture = pSkin->GetTexture(j);
				CString strTex = pTexture->GetMapFile();
				strDstPath = m_strPath + "\\" + strTex;
				CreateFileSystem(strDstPath,strTex,pObj,pMap);
			}
		}
		
	}
}

void CTerrainResExpDlg::ProcessFixedNpc(CSceneObject *pObj ,CElementMap *pMap)
{
	CECModel *pModel = ((CSceneFixedNpcGenerator *)pObj)->GetECModel();
	if(pModel==NULL) return;
	CString strDstPath;
	
	int n = pModel->GetA3DSkinModel()->GetSkinNum();
	for( int i = 0; i < n; i++)
	{
		A3DSkin *pSkin = pModel->GetA3DSkinModel()->GetA3DSkin(i);
		int m = pSkin->GetTextureNum();
		for( int j = 0; j < m; j++)
		{
			A3DTexture *pTexture = pSkin->GetTexture(j);
			CString strTex = pTexture->GetMapFile();
			strDstPath = m_strPath + "\\" + strTex;
			CreateFileSystem(strDstPath,strTex,pObj,pMap);
		}
	}
	CString srcPath = ((CSceneFixedNpcGenerator *)pObj)->GetECModelPath();
	strDstPath = m_strPath + "\\" + srcPath;
	CreateFileSystem(strDstPath,srcPath,pObj,pMap);
}

void CTerrainResExpDlg::ProcessTree(CElementMap *pMap)
{
	CELForest *pForest = pMap->GetSceneObjectMan()->GetElForest();	
	if(pForest==NULL) return;
	CString strDstPath;
	int n = pForest->GetNumTreeType();
	for( int i = 0; i < n; i++)
	{
		CELTree* pTree = pForest->GetTreeType(i);
		CString compoTexture = pTree->GetCompositeMap();
		CString BranchTexture = pTree->GetBranchMap();
		CString treeName = pTree->GetTreeFile();
		strDstPath = m_strPath + "\\" + compoTexture;
		CreateFileSystem(strDstPath,compoTexture,NULL,pMap);
		strDstPath = m_strPath + "\\" + BranchTexture;
		CreateFileSystem(strDstPath,BranchTexture,NULL,pMap);
		strDstPath = m_strPath + "\\" + treeName;
		CreateFileSystem(strDstPath,treeName,NULL,pMap);
		treeName.MakeLower();
		//Only for temp debug
		if(treeName.Find("trees\\新树")!=-1)
		{
			g_Log.Log("*****************地图 %s 中树 %s 的路径依然是旧的!!", pMap->GetMapName(),treeName);
		}
	}
}

void CTerrainResExpDlg::ProcessGrass(CElementMap *pMap)
{
	CELGrassLand *pGrassLand = pMap->GetSceneObjectMan()->GetElGrassLand();		
	if(pGrassLand==NULL) return;
	CString strDstPath;
	
	int n = pGrassLand->GetNumGrassType();
	for( int i = 0; i < n; i++)
	{
		CELGrassType *pType = pGrassLand->GetGrassType(i);
		if(pType==NULL) return;
		CString grassMap = pType->GetGrassMap();
		CString grass = pType->GetGrassFile();
		strDstPath = m_strPath + "\\" + grassMap;
		CreateFileSystem(strDstPath,grassMap,NULL,pMap);
		strDstPath = m_strPath + "\\" + grass;
		CreateFileSystem(strDstPath,grass,NULL,pMap);
	}
}

void CTerrainResExpDlg::ProcessTerrain(CElementMap *pMap)
{
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
}

BOOL CTerrainResExpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd *pWnd = NULL;
	// TODO: Add extra initialization here
	if(nExpType!=exp_topview)
	{
		pWnd = GetDlgItem(IDC_CHECK_TEXTURE);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_RADIO_SIZE256);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_RADIO_SIZE512);
		pWnd->EnableWindow(false);
		pWnd = GetDlgItem(IDC_RADIO_SIZE1024);
		pWnd->EnableWindow(false);
	}else
	{
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_SIZE256);
		pButton->SetCheck(1);
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerrainResExpDlg::OnRadioSize512() 
{
	// TODO: Add your control notification handler code here
	m_nTopViewWidth = 512;
}

void CTerrainResExpDlg::OnRadioSize1024() 
{
	// TODO: Add your control notification handler code here
	m_nTopViewWidth = 1024;
}

void CTerrainResExpDlg::OnRadioSize256() 
{
	// TODO: Add your control notification handler code here
	m_nTopViewWidth = 256;
}

void CTerrainResExpDlg::OnCheckTexture() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
}
