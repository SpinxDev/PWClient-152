// ModelChangedStatusDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "ModelChangedStatusDlg.h"
#include "ElementMap.h"
#include "ElementResMan.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "StaticModelObject.h"
#include "A3DVector.h"
#include "VssOperation.h"

#include "AF.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CModelChangedStatusDlg dialog


CModelChangedStatusDlg::CModelChangedStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelChangedStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelChangedStatusDlg)
	m_nDate = 8;
	m_nMouth = 11;
	m_nYear = 5;
	//}}AFX_DATA_INIT
}


void CModelChangedStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelChangedStatusDlg)
	DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_Progress);
	DDX_Text(pDX, IDC_EDIT_DATE, m_nDate);
	DDV_MinMaxInt(pDX, m_nDate, 1, 31);
	DDX_Text(pDX, IDC_EDIT_MOUTH, m_nMouth);
	DDV_MinMaxInt(pDX, m_nMouth, 1, 12);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_nYear);
	DDV_MinMaxInt(pDX, m_nYear, 1, 9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelChangedStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CModelChangedStatusDlg)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelChangedStatusDlg message handlers

void CModelChangedStatusDlg::OnOK()
{
	UpdateData(true);
	abase::vector<Res_Item *> listChanged;
	//找出已经变化过的资源表
	ElementResMan resNew;
	if(!resNew.Load()) 
	{
		MessageBox("本地资源表打开失败!");
		return;
	}
	
	ALog  m_Log;
	char szLogPath[MAX_PATH];
	char szTemp[MAX_PATH];
	sprintf(szLogPath,"%s.log",g_Configs.szCurProjectName);
	m_Log.Init(szLogPath, "下面是项目中资源变动过的模型...");

	for( int i = 0; i < resNew.GetResCount(ElementResMan::RES_TYPE_BUILDING); ++i)
	{
		Res_Item *pNewItem = resNew.GetRes(ElementResMan::RES_TYPE_BUILDING,i);
		bool bIsExist = false;
		
		CString rc;
		g_VSS.Property(pNewItem->szID);
		rc = g_VSS.GetLastCommandResult();
		int pos = rc.Find("Date:");
		rc = rc.Right(rc.GetLength() - pos);
		int nYear,nMouth,nDate;
		sscanf(rc,"Date:  %d-%d-%d", &nYear,&nMouth,&nDate);
		
		if(nYear > m_nYear) bIsExist = true;
		else 
		{
			if(nYear == m_nYear)
			{
				if(nMouth > m_nMouth) bIsExist = true;
				else if( nMouth == m_nMouth)
				{
					if(nDate >= m_nDate) bIsExist = true;
				}
			}
		}
		
		if(bIsExist) 
		{
			m_Log.Log("%s   %s",pNewItem->szID,pNewItem->szOrgName);
			listChanged.push_back(pNewItem);
		}
	}
	
	//逐一检测地图，看地图中是否有模型已经存在于资源变动表中，如果存在该表中打印出模型的ID
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
		return;
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
	abase::vector<CString> projectList;
	while (File.GetNextToken(true))
	{
		CString strNew(File.m_szToken);
		projectList.push_back(strNew);
	}
	File.Close();
	m_Progress.SetRange(0,projectList.size());
	m_Progress.SetStep(1);
	m_Log.Log("下面是地图中用到改动过的模型的场景对象ID...");
	// Proessing map
	abase::vector<CString>::iterator theIterator;
	for(theIterator = projectList.begin(); theIterator != projectList.end(); theIterator ++)
	{
		
		mapName.Format("%s%s\\%s\\%s.elproj",g_szWorkDir,g_szEditMapDir,(*theIterator),(*theIterator));
		if(!emap.Load(mapName))
		{
			CString error_msg;
			error_msg.Format("不能打开地图文件 :%s",mapName);
			MessageBox(error_msg);
			return;
		}
		m_Log.Log("**地图: %s",emap.GetMapName());
		CSceneObjectManager *pMan =emap.GetSceneObjectMan();
		ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject* ptemp = pMan->m_listSceneObject.GetPrev(pos);
			int type = ptemp->GetObjectType();
			if(type==CSceneObject::SO_TYPE_STATIC_MODEL)
			{
				CStaticModelObject *pStatic = (CStaticModelObject *)ptemp;
				int num = listChanged.size();
				for(int i = 0; i < num; ++i)
				{
					sprintf(szTemp,"%s",listChanged[i]->szID);
					if(NULL==pStatic->GetA3DModel() || NULL == pStatic->GetA3DModel()->pModel) continue;
					if(stricmp(szTemp,pStatic->GetModelPath())==0)
					{//记录模型的ID
						m_Log.Log("%d",pStatic->GetObjectID());
						break;
					}
				}
				
			}
		}
		emap.Release();
		m_Progress.StepIt();
	}
	m_Log.Release();
	CDialog::OnCancel();
}

