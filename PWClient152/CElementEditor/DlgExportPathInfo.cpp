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

	//	����Ƿ�ѡ������Ϊ��
	for (iTile = 0; iTile < m_pMapGridSelectPolicy->GetTileNum(); ++ iTile)
	{
		if (m_pMapGridSelectPolicy->IsSelected(iTile)){
			break;
		}
	}
	if (iTile >= m_pMapGridSelectPolicy->GetTileNum()){
		return;
	}
	
	//	���ɵ����ļ�·������ʱ�ļ���
	char szPath[MAX_PATH];
	::GetTempPathA(MAX_PATH, szPath);
	::GetTempFileNameA(szPath, NULL, 0, szPath);

	//	���ļ�д��
	FILE *fp = fopen(szPath, "w");
	if (!fp){
		return;
	}
	fprintf(fp, "%s��ͼ·����Ϣ\n", g_Configs.szCurProjectName);
	fprintf(fp, "%8s	%20s	%6s\n", "����ID", "·������", "���ڵ�ͼ��");
	fprintf(fp, "------------------------------------------\n");

	//	���ͼ����
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
			strMsg.Format(_T("���� %s ʧ�ܣ����Ե�����"), strProjName);
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

	//	д��·������
	fprintf(fp, "����������%d\n", pathCount);

	//	�ر��ļ�д��
	fflush(fp);
	fclose(fp);

	//	�򿪵�������
	ShellExecuteA(NULL, "open", "notepad.exe", szPath, NULL, SW_SHOW);

	//	��յ�ǰѡ��
	OnUnselectAll();
}
