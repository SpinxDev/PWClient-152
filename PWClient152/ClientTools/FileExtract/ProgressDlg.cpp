// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileExtract.h"
#include "ProgressDlg.h"
#include "FileAnalyse.h"
#include "EC_Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ProgressDlg dialog


CProgressDlg* CProgressDlg::g_pThis;

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	m_szCurFile = _T("");
	//}}AFX_DATA_INIT

	m_iOperation = 0;
	strcpy(m_szOutDir, "");
	m_nFileCount = 0;
	m_nFileIndex = 0;
	m_bRunning = false;
	m_iCurProgress = 0;

	m_hThreadProc = NULL;
	::InitializeCriticalSection(&m_csLock);
	g_pThis = this;
}

CProgressDlg::~CProgressDlg()
{
	if( m_hThreadProc )
	{
		::CloseHandle(m_hThreadProc);
		m_hThreadProc = NULL;
	}

	::DeleteCriticalSection(&m_csLock);
	g_pThis = NULL;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_CURPROGRESS, m_ctlProgress);
	DDX_Text(pDX, IDC_CUR_FILE, m_szCurFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProgressDlg message handlers

BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bRunning = true;
	m_ctlProgress.SetRange(0, 1000);
	SetTimer(0, 50, NULL);

	switch(m_iOperation)
	{
	case OP_EXPORT:
		SetWindowText(_T("导出"));
		break;
	case OP_SEARCHSCALEDNPC:
	case OP_SEARCHEQUIPGFXNOHANGER:
		SetWindowText(_T("分析"));
		break;
	}
	
	// 创建线程，开始导出
	m_hThreadProc = ::CreateThread(NULL, 0, ProcessProc, this, 0, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// 执行导出操作
int CProgressDlg::DoExport(const char* szOutDir)
{
	CProgressDlg dlg;
	dlg.m_iOperation = OP_EXPORT;
	strncpy(dlg.m_szOutDir, szOutDir, MAX_PATH);
	dlg.m_nFileCount = (UINT)FileAnalyse::GetInstance().GetResult().size();
	return dlg.DoModal();
}

// 搜索被缩放的NPC模型
int CProgressDlg::DoSearchScaledNPC()
{
	CProgressDlg dlg;
	dlg.m_iOperation = OP_SEARCHSCALEDNPC;
	dlg.m_nFileCount = GetNumAllNPCs();
	return dlg.DoModal();
}

// 执行搜索有特效无挂点的装备
int CProgressDlg::DoSearchEquipGfxNoHanger()
{
	CProgressDlg dlg;
	dlg.m_iOperation = OP_SEARCHEQUIPGFXNOHANGER;
	dlg.m_nFileCount = GetNumAllEquips();
	return dlg.DoModal();
}

// 线程函数
DWORD CProgressDlg::ProcessProc(LPVOID lpParameter)
{
	CProgressDlg* dlg = (CProgressDlg*)lpParameter;
	FileAnalyse::GetInstance().SetCBOperation(OPCallback);

	if( dlg->m_iOperation == OP_EXPORT )
		FileAnalyse::GetInstance().Export(dlg->m_szOutDir);
	else if( dlg->m_iOperation == OP_SEARCHSCALEDNPC )
		FileAnalyse::GetInstance().Analyse("Scaled NPC", FileAnalyse::TYPE_SCALEDNPC);
	else if( dlg->m_iOperation == OP_SEARCHEQUIPGFXNOHANGER )
		FileAnalyse::GetInstance().Analyse("Weapon FlySword Wing", FileAnalyse::TYPE_EQUIPGFXNOHANGER);

	dlg->m_bRunning = false;
	return 0;
}

// 处理文件时的回调函数
bool CProgressDlg::OPCallback(const char* filename)
{
	CProgressDlg* pDlg = CProgressDlg::g_pThis;
	::EnterCriticalSection(&pDlg->m_csLock);
	if( pDlg->m_bRunning && pDlg->m_nFileCount != 0 )
	{
		float fProgress = (float)pDlg->m_nFileIndex / pDlg->m_nFileCount;
		pDlg->m_szCurFile = filename;
		pDlg->m_iCurProgress = (int)(1000 * fProgress);
	}

	++pDlg->m_nFileIndex;
	::LeaveCriticalSection(&pDlg->m_csLock);
	return pDlg->m_bRunning;
}

void CProgressDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	::EnterCriticalSection(&m_csLock);
	m_bRunning = false;
	m_szCurFile = _T("正在取消");
	::LeaveCriticalSection(&m_csLock);

	::WaitForSingleObject(m_hThreadProc, INFINITE);
	CDialog::OnCancel();
}

void CProgressDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);

	::EnterCriticalSection(&m_csLock);
	m_ctlProgress.SetPos(m_iCurProgress);
	UpdateData(FALSE);
	::LeaveCriticalSection(&m_csLock);

	if( !m_bRunning ) CDialog::OnCancel();
}
