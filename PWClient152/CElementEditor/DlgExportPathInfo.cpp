// DlgExportPathInfo.cpp : implementation file
//

#include "Global.h"
#include "DlgExportPathInfo.h"
#include "ExpElementMap.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "EditerBezier.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportPathInfo dialog


CDlgExportPathInfo::CDlgExportPathInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportPathInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportPathInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgExportPathInfo::~CDlgExportPathInfo()
{
	delete m_pMapGridSelectPolicy;
	m_pMapGridSelectPolicy = NULL;
}

void CDlgExportPathInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportPathInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportPathInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgExportPathInfo)
	ON_BN_CLICKED(IDC_UNSELECT_ALL, OnUnselectAll)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportPathInfo message handlers

BOOL CDlgExportPathInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_projList.Init()){
		g_Log.Log("CDlgExportPathInfo::OnInitDialog, Failed to init ProjList.dat.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	RECT rcWnd;
	GetDlgItem(IDC_STATIC_GRID)->GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);
	
	m_pMapGridSelectPolicy = new CSeperateMapGridSelectPolicy;
	m_mapGrid.SetSelectPolicy(m_pMapGridSelectPolicy);
	if (!m_mapGrid.Create(this, rcWnd, 40))
	{
		g_Log.Log("CDlgExportPathInfo::OnInitDialog, Failed to create map grid window.");
		EndDialog(IDCANCEL);
		return FALSE;
	}
	m_mapGrid.SetTileAndRow(m_projList.GetList(), m_projList.GetNumCol(), true);	
	OnSelectAll();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportPathInfo::OnUnselectAll() 
{
	m_pMapGridSelectPolicy->SelectAll(false);
	m_mapGrid.InvalidateRect(NULL);
}

void CDlgExportPathInfo::OnSelectAll() 
{
	m_pMapGridSelectPolicy->SelectAll(true);
	m_mapGrid.InvalidateRect(NULL);
}

void CDlgExportPathInfo::OnExport() 
{	
	int iTile(0);

	//	检查是否选择内容为空
	for (iTile = 0; iTile < m_pMapGridSelectPolicy->GetTileNum(); ++ iTile)
	{
		if (m_pMapGridSelectPolicy->IsSelected(iTile)){
			break;
		}
	}
	if (iTile >= m_pMapGridSelectPolicy->GetTileNum()){
		return;
	}
	
	//	生成导出文件路径（临时文件）
	char szPath[MAX_PATH];
	::GetTempPathA(MAX_PATH, szPath);
	::GetTempFileNameA(szPath, NULL, 0, szPath);

	//	打开文件写入
	FILE *fp = fopen(szPath, "w");
	if (!fp){
		return;
	}
	fprintf(fp, "%s号图路径信息\n", g_Configs.szCurProjectName);
	fprintf(fp, "%8s	%20s	%6s\n", "曲线ID", "路径名称", "所在地图块");
	fprintf(fp, "------------------------------------------\n");

	//	逐地图导出
	int pathCount(0);
	for (iTile = 0; iTile < m_pMapGridSelectPolicy->GetTileNum(); ++ iTile)
	{
		if (!m_pMapGridSelectPolicy->IsSelected(iTile)){
			continue;
		}
		const CString &strProjName = m_projList.Get(iTile);
		CString strFile;
		strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProjName, strProjName);
		
		CExpElementMap Map;
		if (!Map.Load(strFile, LOAD_EXPSCENE))
		{
			g_Log.Log("CDlgExportPathInfo::OnExport, Failed to call CExpElementMap::Load(%s)", strProjName);
			CString strMsg;
			strMsg.Format(_T("加载 %s 失败，忽略导出！"), strProjName);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			continue;
		}
		
		CSceneObjectManager* pManager = Map.GetSceneObjectMan();		
		ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
		while (pos)
		{
			CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
			if (pSceneObject->GetObjectType() == CSceneObject::SO_TYPE_BEZIER){
				++ pathCount;
				CEditerBezier* pBezier = (CEditerBezier*)pSceneObject;
				fprintf(fp, "%8d	%20s	%6s\n", pBezier->GetGlobalID(), pBezier->GetObjectName(), strProjName);
			}
		}
		fprintf(fp, "------------------------------------------\n");

		Map.Release();
	}

	//	写入路径总数
	fprintf(fp, "导出总数：%d\n", pathCount);

	//	关闭文件写入
	fflush(fp);
	fclose(fp);

	//	打开导出内容
	ShellExecuteA(NULL, "open", "notepad.exe", szPath, NULL, SW_SHOW);

	//	清空当前选中
	OnUnselectAll();
}
