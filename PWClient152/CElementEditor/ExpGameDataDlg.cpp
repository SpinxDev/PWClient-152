// ExpGameDataDlg.cpp : implementation file
//

#include "Global.h"
#include "Bitmap.h"
#include "TerrainExportor.h"
#include "SceneExportor.h"
#include "AIGenExportor.h"
#include "ExpGameDataDlg.h"
#include "LightMapMerger.h"
#include "DlgLighting.h"
#include "LightMapTransition.h"
#include "ExpPathSelDlg.h"
#include "ExportLitDlg.h"
#include "DlgCopySel.h"
#include "Global.h"
#include "AC.h"
#include "AF.h"
#include "VssOperation.h"
#include "DlgRandomMapInfoSetting.h"

#include <shlwapi.h>

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static void _GameExpLog(DWORD dwUserData, const char* szMsg)
{
	CExpGameDataDlg* pExpDlg = (CExpGameDataDlg*)dwUserData;
	ASSERT(pExpDlg);
	pExpDlg->AddLogMessage(szMsg);
}

static void _GameExpProgress(DWORD dwUserData, int iTotal, int iComplete)
{
	CExpGameDataDlg* pExpDlg = (CExpGameDataDlg*)dwUserData;
	ASSERT(pExpDlg);

	if (iTotal < 0)
		pExpDlg->ResetProgressBar(-iTotal);
	else
		pExpDlg->StepProgressBar(iComplete);
}

/////////////////////////////////////////////////////////////////////////////
// CExpGameDataDlg dialog


CExpGameDataDlg::CExpGameDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExpGameDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpGameDataDlg)
	m_bLighting = TRUE;
	m_bLightMapTrans = TRUE;
	m_bExpTerrain = TRUE;
	m_bExpObject = TRUE;
	m_strCopys = _T("");
	//}}AFX_DATA_INIT
	m_dwNumCol = 1;
}


void CExpGameDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpGameDataDlg)
	DDX_Control(pDX, IDC_COMBO_MASKTILE, m_MaskTileCombo);
	DDX_Control(pDX, IDC_EDIT_LOG, m_LogEdit);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Check(pDX, IDC_CHECK_LIGHTING, m_bLighting);
	DDX_Check(pDX, IDC_CHECK_LIGHTING_TRANS, m_bLightMapTrans);
	DDX_Check(pDX, IDC_CHECK_EXP_TERRAIN, m_bExpTerrain);
	DDX_Check(pDX, IDC_CHECK_EXP_OBJECT, m_bExpObject);
	DDX_Text(pDX, IDC_EDIT_COPYS, m_strCopys);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExpGameDataDlg, CDialog)
	//{{AFX_MSG_MAP(CExpGameDataDlg)
	ON_BN_CLICKED(IDC_CHECK_LIGHTING, OnCheckLighting)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_EXPAIGEN, OnBtnExpaigen)
	ON_BN_CLICKED(IDC_CHECK_LIGHTING_TRANS, OnCheckLightingTrans)
	ON_BN_CLICKED(IDC_CHECK_EXP_TERRAIN, OnCheckExpTerrain)
	ON_BN_CLICKED(IDC_CHECK_EXP_OBJECT, OnCheckExpObject)
	ON_BN_CLICKED(IDC_CHECK_EXPORT_MINI_MAP, OnCheckExportMiniMap)
	ON_BN_CLICKED(IDC_BUTTON_SEL_COPY, OnButtonSelCopy)
	ON_BN_CLICKED(IDC_BTN_SAVEMASK, OnBtnSavemask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpGameDataDlg message handlers

//	Export terrain data
bool CExpGameDataDlg::ExportTerrainData(const char* szPath, const char* szName, int iNumCol, int iNumRow)
{
	//	Create exporter
	CTerrainExportor Exportor;
	CTerrainExportor::INITDATA InitData;
	memset(&InitData, 0, sizeof (InitData));

	InitData.szName		= szName;
	InitData.szDestDir	= szPath;
	InitData.iBlockGrid	= g_Configs.iTExpBlkGrid;
	InitData.iMaskTile	= AUX_GetMaskTileSize();
	InitData.iNumCol	= iNumCol;
	InitData.iNumRow	= iNumRow;
	InitData.pLMGen		= NULL;
	InitData.bGameExp	= true;
	
	if (!Exportor.Init(InitData))
	{
		MessageBox("初始化地形导出对象失败！");
		return false;
	}

	//	Prepare exporting
	m_Progress.SetPos(0);

	//	Add project names...
	POSITION pos = m_listProj.GetHeadPosition();
	while(pos)
	{
		CString *pStrName = (CString *)m_listProj.GetNext(pos);
		Exportor.AddProject(*pStrName);
	}

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	bool bRet = Exportor.DoExport();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	
	if (!bRet)
		MessageBox("地形数据导出失败！");

	//	Save some data for later exporting
	m_iProjMaskCol	= Exportor.GetProjMaskCol();
	m_iProjMaskRow	= Exportor.GetProjMaskRow();
	m_iSubTrnGrid	= Exportor.GetSubTrnGrid();

	Exportor.Release();

	return bRet;
}

//	Export scene data
bool CExpGameDataDlg::ExportSceneData(const char* szPath, const char* szName, int iNumCol, int iNumRow)
{
	//	Create exporter
	CSceneExportor Exportor;
	CSceneExportor::INITDATA InitData;
	memset(&InitData, 0, sizeof (InitData));

	InitData.szName		= szName;
	InitData.szDestDir	= szPath;
	InitData.iBlockGrid	= g_Configs.iTExpBlkGrid;
	InitData.iNumCol	= iNumCol;
	InitData.iNumRow	= iNumRow;
	
	if (!Exportor.Init(InitData))
	{
		MessageBox("初始化场景导出对象失败！");
		return false;
	}

	//	Prepare exporting
	m_Progress.SetPos(0);

	//	Add project names...
	POSITION pos = m_listProj.GetHeadPosition();
	while(pos)
	{
		CString *pStrName = (CString *)m_listProj.GetNext(pos);
		Exportor.AddProject(*pStrName);
	}

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	bool bRet = Exportor.DoExport();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	
	if (!bRet)
		MessageBox("场景数据导出失败！");

	Exportor.Release();

	return bRet;
}

//	Export lightmap data
bool CExpGameDataDlg::ExportLightmapData(const char* szPath, const char* szName, int iNumCol, int iNumRow)
{
	//	Create exporter
	CLightMapMerger Exportor;
	CLightMapMerger::INITDATA InitData;
	memset(&InitData, 0, sizeof (InitData));

	InitData.szName			= szName;
	InitData.szDestDir		= szPath;
	InitData.iNumCol		= iNumCol;
	InitData.iNumRow		= iNumRow;
	InitData.iProjMaskCol	= m_iProjMaskCol;
	InitData.iProjMaskRow	= m_iProjMaskRow;
	InitData.iSubTrnGrid	= m_iSubTrnGrid;
	
	if (!Exportor.Init(InitData))
	{
		MessageBox("初始化光照图导出对象失败！");
		return false;
	}

	//	Prepare exporting
	m_Progress.SetPos(0);

	//	Add project names...
	POSITION pos = m_listProj.GetHeadPosition();
	while(pos)
	{
		CString *pStrName = (CString *)m_listProj.GetNext(pos);
		Exportor.AddProject(*pStrName);
	}

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	bool bRet = Exportor.DoExport();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	
	if (!bRet)
		MessageBox("光照图数据导出失败！");

	Exportor.Release();

	return bRet;
}

//	Export AI generator
bool CExpGameDataDlg::ExportAIGenerator(const char* szPath, const char* szWork, int iNumCol, int iNumRow)
{
	//	Create exporter
	CAIGenExportor Exportor;
	CAIGenExportor::INITDATA InitData;
	memset(&InitData, 0, sizeof (InitData));

	InitData.szExpDir	= szPath;
	InitData.szWorkingDir = szWork;
	InitData.iNumCol	= iNumCol;
	InitData.iNumRow	= iNumRow;
	
	if (!Exportor.Init(InitData))
	{
		MessageBox("初始化场景导出对象失败！");
		return false;
	}

	//	Prepare exporting
	m_Progress.SetPos(0);

	//	Add project names...
	POSITION pos = m_listProj.GetHeadPosition();
	while(pos)
	{
		CString *pStrName = (CString *)m_listProj.GetNext(pos);
		Exportor.AddProject(*pStrName);
	}

	SetCursor(LoadCursor(NULL, IDC_WAIT));
	bool bRet = Exportor.DoExport();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	
	if (!bRet)
		MessageBox("场景数据导出失败！");

	Exportor.Release();

	return bRet;
}

//	Add a message to log window
void CExpGameDataDlg::AddLogMessage(const char* szMsg, ...)
{
	char szBuf[1024];

	va_list vaList;
	va_start(vaList, szMsg);
	vsprintf(szBuf, szMsg, vaList);
	va_end(vaList);

	strcat(szBuf, "\n");

	//	Select all text at first then to cancel selection. This is the way to
	//	ensure caret is at the end of text when new text is added !!
	m_LogEdit.SetSel(0, -1);
	m_LogEdit.SetSel(-1, -1);
	m_LogEdit.ReplaceSel(szBuf);
}

//	Clear all messages
void CExpGameDataDlg::ClearLogMessages()
{
	m_LogEdit.SetSel(0, -1);
	m_LogEdit.ReplaceSel("");
}

//	Reset progress bar
void CExpGameDataDlg::ResetProgressBar(int iTotal)
{
	m_Progress.SetRange32(0, iTotal);
	m_Progress.SetPos(0);
	m_Progress.SetStep(1);
}

//	Step progress bar
void CExpGameDataDlg::StepProgressBar(int iCurStep)
{
	ASSERT(iCurStep > 0);
	while (iCurStep > 0)
	{
		m_Progress.StepIt();
		iCurStep--;
	}
}

//	Do export
void CExpGameDataDlg::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	
	int iNumRow, iNumCol, iNumProj = m_listProj.GetCount();
	if (iNumProj <= m_dwNumCol)
	{
		iNumRow = 1;
		iNumCol = iNumProj;
	}
	else
	{
		iNumRow = (iNumProj + m_dwNumCol - 1) / m_dwNumCol;
		iNumCol = iNumProj / iNumRow;
	}
	
	if (!iNumProj || (iNumRow != 1 && iNumCol != (int)m_dwNumCol))
	{
		MessageBox("无法导出不完整的地图。");
		return;
	}
	g_Log.Log("Exportting col = %d, row = %d", iNumCol, iNumRow);
	
	g_Configs.iTExpColNum	= (int)m_dwNumCol;

	if(!AUX_IsRandomMapWorkSpace())
		g_Configs.iTExpMaskTile	= m_aMaskTiles[m_MaskTileCombo.GetCurSel()];

	AUX_SetProgressFuncs(_GameExpLog, _GameExpProgress, (DWORD)this);
	ClearLogMessages();
	CExpPathSelDlg dlg;
	char szName[128];
	static char szPath[MAX_PATH] = "\0";
	if(!g_bOnlyCalMiniMap)
	{
		if(dlg.DoModal()==IDCANCEL) return;
		
		sprintf(szPath,"%s\\Maps",dlg.m_strPath);
		if(!PathFileExists(szPath))
		{
			if(!CreateDirectory(szPath, NULL))
			{
				MessageBox("创建输出路径失败！");
				return;
			}
		}
		
		sprintf(szName,"%s",szPath);
		sprintf(szPath,"%s\\%s",szName,g_Configs.szCurProjectName);
		if(!PathFileExists(szPath))
		{
			if(!CreateDirectory(szPath, NULL))
			{
				MessageBox("创建输出路径失败！");
				return;
			}
		}
	}
	
	if(m_bLightMapTrans && m_bExpTerrain)
	{
		CLightMapBatchProcess trans;
		if(!trans.Process())
		{
			AUX_SetProgressFuncs(NULL, NULL, 0);
			return;
		}
	}

	if(g_bOnlyCalMiniMap)
	{
		AUX_SetProgressFuncs(NULL, NULL, 0);
		OnCheckExportMiniMap();
		CButton *pTemp = (CButton *)GetDlgItem(IDC_CHECK_EXPORT_MINI_MAP);
		if(pTemp) pTemp->SetCheck(g_bOnlyCalMiniMap);
		MessageBox("卫星图光处理完成!");
		return;
	}
	/*
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
	*/

	sprintf(szName,"%s",g_Configs.szCurProjectName);
	
	if(m_bExpTerrain)
	{//	Export terrain data ...
		if (!ExportTerrainData(szPath, szName, iNumCol, iNumRow))
		{
			AUX_SetProgressFuncs(NULL, NULL, 0);
			return;
		}
	}

	if(m_bExpObject)
	{//	Export scene objects ...
		if (!ExportSceneData(dlg.m_strPath,szName, iNumCol, iNumRow))
		{
			AUX_SetProgressFuncs(NULL, NULL, 0);
			return;
		}
	}

	//	Merge light maps ...
	if(m_bExpTerrain)
	{
		if (m_bLighting)
		{
			if (!ExportLightmapData(szPath, szName, iNumCol, iNumRow))
			{
				AUX_SetProgressFuncs(NULL, NULL, 0);
				return;
			}
		}
	}
	
	// Export scene light list
	AUX_ProgressLog("-- Exporting all scene light data ...");
	CExportLitDlg dlge;
	dlge.m_strExpPath = szPath;
	dlge.m_strExpPath.Format("%s\\%s_light.dat",szPath,g_Configs.szCurProjectName);
	dlge.m_pProgress = &m_Progress;
	if(!dlge.ExpLitData()) MessageBox("场景灯光列表导出失败！");

	AUX_SetProgressFuncs(NULL, NULL, 0);
	MessageBox("数据导出成功！");

//	CDialog::OnOK();
}

void CExpGameDataDlg::OnBtnExpaigen() 
{
	if (!UpdateData(TRUE))
		return;
	
	int iNumRow, iNumCol, iNumProj = m_listProj.GetCount();
	if (iNumProj <= m_dwNumCol)
	{
		iNumRow = 1;
		iNumCol = iNumProj;
	}
	else
	{
		iNumRow = (iNumProj + m_dwNumCol - 1) / m_dwNumCol;
		iNumCol = iNumProj / iNumRow;
	}
	
	if (!iNumProj || (iNumRow != 1 && iNumCol != (int)m_dwNumCol))
	{
		MessageBox("无法导出不完整的地图。");
		return;
	}

	/*
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
	Info.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_DONTGOBELOWDOMAIN ;
	
	LPITEMIDLIST pPIDL;
	if (!(pPIDL = SHBrowseForFolder(&Info)))
		return;

	SHGetPathFromIDList(pPIDL, szPath);
	*/
	CExpPathSelDlg dlg;
	if(dlg.DoModal()!=IDOK) return;
	static char szPath[MAX_PATH] = "\0";
	sprintf(szPath,"%s",dlg.m_strPath);
	char szName[128];
	sprintf(szName,"%s",szPath);
	
	//create client folder maps
	sprintf(szPath,"%s\\Maps",szName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建输出路径失败！");
			return;
		}
	}
	sprintf(szPath,"%s\\Maps\\%s",szName,g_Configs.szCurProjectName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建输出路径失败！");
			return;
		}
	}	
	//world 
	sprintf(szPath,"%s\\%s",szName,g_Configs.szCurProjectName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建输出路径失败！");
			return;
		}
	}
	
	//world - map
	sprintf(szPath,"%s\\%s\\Map",szName,g_Configs.szCurProjectName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建输出路径失败！");
			return;
		}
	}

	//world - MoveMap
	sprintf(szPath,"%s\\%s\\MoveMap",szName,g_Configs.szCurProjectName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建输出路径失败！");
			return;
		}
	}
	//world - WaterMap
	sprintf(szPath,"%s\\%s\\WaterMap",szName,g_Configs.szCurProjectName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建输出路径失败！");
			return;
		}
	}

	//world - SpMap
	sprintf(szPath,"%s\\%s\\airmap",szName,g_Configs.szCurProjectName);
	if(!PathFileExists(szPath))
	{
		if(!CreateDirectory(szPath, NULL))
		{
			MessageBox("创建输出路径失败！");
			return;
		}
	}
	
	sprintf(szPath,"%s\\%s",szName,g_Configs.szCurProjectName);
	g_Configs.iTExpColNum	= (int)m_dwNumCol;
	
	if(!AUX_IsRandomMapWorkSpace())
		g_Configs.iTExpMaskTile	= m_aMaskTiles[m_MaskTileCombo.GetCurSel()];

	AUX_SetProgressFuncs(_GameExpLog, _GameExpProgress, (DWORD)this);
	ClearLogMessages();

	//	Export scene objects ...
	if (!ExportAIGenerator(szPath,szName,iNumCol, iNumRow))
	{
		AUX_SetProgressFuncs(NULL, NULL, 0);
		return;
	}

	AUX_SetProgressFuncs(NULL, NULL, 0);

	MessageBox("数据导出成功！");
}

BOOL CExpGameDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//	Initialize mask size list
	m_aMaskTiles[MASKTILE_64]	= 64;
	m_aMaskTiles[MASKTILE_128]	= 128;
	m_aMaskTiles[MASKTILE_256]	= 256;
	m_aMaskTiles[MASKTILE_512]	= 512;

	m_MaskTileCombo.InsertString(-1, "64 x 64");
	m_MaskTileCombo.InsertString(-1, "128 x 128");
	m_MaskTileCombo.InsertString(-1, "256 x 256");
	m_MaskTileCombo.InsertString(-1, "512 x 512");

	m_MaskTileCombo.SetCurSel(MASKTILE_128);

	for (int i=0; i < MASKTILE_NUM; i++)
	{
		if (AUX_GetMaskTileSize() == m_aMaskTiles[i])
		{
			m_MaskTileCombo.SetCurSel(i);
			break;
		}
	}

	// Load data
	AScriptFile File;
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
			m_dwNumCol = (int)atof(strCol);
		}
		else File.ResetScriptFile();
		
	}
	
	while (File.GetNextToken(true))
	{
		CString *pStr = new CString(File.m_szToken);
		m_listProj.AddTail(pStr);
	}
	File.Close();
	
	UpdateString();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CExpGameDataDlg::OnCheckLighting() 
{
	// TODO: Add your control notification handler code here
	
}

void CExpGameDataDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	POSITION pos = m_listProj.GetHeadPosition();
	while(pos)
	{
		CString *pStrName = (CString *)m_listProj.GetNext(pos);
		delete pStrName;
	}
	m_listProj.RemoveAll();
}


void CExpGameDataDlg::OnCheckLightingTrans() 
{
	// TODO: Add your control notification handler code here
	
}

void CExpGameDataDlg::OnCheckExpTerrain() 
{
	// TODO: Add your control notification handler code here
	
}

void CExpGameDataDlg::OnCheckExpObject() 
{
	// TODO: Add your control notification handler code here
	
}

void CExpGameDataDlg::OnCheckExportMiniMap() 
{
	// TODO: Add your control notification handler code here
	g_bOnlyCalMiniMap = !g_bOnlyCalMiniMap;
	
	CWnd *pTemp = GetDlgItem(IDC_CHECK_LIGHTING);
	if(pTemp) pTemp->EnableWindow( !g_bOnlyCalMiniMap );
	pTemp = GetDlgItem(IDC_CHECK_LIGHTING_TRANS);
	if(pTemp) pTemp->EnableWindow( !g_bOnlyCalMiniMap );
	pTemp = GetDlgItem(IDC_CHECK_EXP_TERRAIN);
	if(pTemp) pTemp->EnableWindow( !g_bOnlyCalMiniMap );
	pTemp = GetDlgItem(IDC_CHECK_EXP_OBJECT);
	if(pTemp) pTemp->EnableWindow( !g_bOnlyCalMiniMap );
	if(g_bOnlyCalMiniMap) 
	{
		m_bLightMapTrans = true;
		UpdateData(false);
	}
}

void CExpGameDataDlg::OnButtonSelCopy() 
{
	// TODO: Add your control notification handler code here
	CDlgCopySel dlg;
	dlg.DoModal();
	UpdateString();
}

void CExpGameDataDlg::UpdateString()
{
	CString str;
	for( int i = 0; i < 32; ++i)
	{
		if(g_dwExpCopy & (1 << i))
		{
			CString msg;
			msg.Format("%d,",i);
			str += msg;
		}
	}
	m_strCopys = str; 
	UpdateData(FALSE);
}

void CExpGameDataDlg::OnBtnSavemask() 
{
}
