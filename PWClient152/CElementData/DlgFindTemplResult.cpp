// DlgFindTemplResult.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "DlgFindTemplResult.h"
#include <AString.h>
#include <AFile.h>
#include <ShellAPI.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFindTemplResult dialog


CDlgFindTemplResult::CDlgFindTemplResult(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFindTemplResult::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFindTemplResult)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgFindTemplResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFindTemplResult)
	DDX_Control(pDX, IDC_LIST1, m_Lst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFindTemplResult, CDialog)
	//{{AFX_MSG_MAP(CDlgFindTemplResult)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDBUTTON1, OnButton1)
	ON_BN_CLICKED(IDBUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFindTemplResult message handlers

void CDlgFindTemplResult::OnOK() 
{
	// TODO: Add extra validation here
	m_selection.clear();

	UINT nSel = m_Lst.GetSelectedCount();
	if (nSel <= 0)	return;
	int nIndex = -1;
	for (UINT i = 0; i < nSel; ++ i)
	{
		nIndex = m_Lst.GetNextItem(nIndex, LVNI_SELECTED);
		unsigned long id = m_Lst.GetItemData(nIndex);
		m_selection.push_back(id);
	}
	
	CDialog::OnOK();
}

void CDlgFindTemplResult::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_selection.clear();
	
	CDialog::OnCancel();
}

void CDlgFindTemplResult::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	UpdateView();
}

void CDlgFindTemplResult::UpdateView()
{
	m_Lst.DeleteAllItems();

	CString strText;
	for (size_t i = 0; i < m_result.size(); ++ i)
	{
		const FindItem &item = m_result[i];
		strText.Format(("%d"), item.uID);
		int index = m_Lst.InsertItem((int)i, strText);
		m_Lst.SetItemText(index, 1, item.strName);
		m_Lst.SetItemText(index, 2, item.strPath);
		m_Lst.SetItemData(index, item.uID);
	}

	//	在对话框标题中显示查找结果个数
	strText.Format("查找结果(%d)", m_result.size());
	SetWindowText(strText);
}

BOOL CDlgFindTemplResult::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Lst.InsertColumn(0, "ID", LVCFMT_LEFT, 60);
	m_Lst.InsertColumn(1, m_strColumn2Name, LVCFMT_LEFT, 180);
	m_Lst.InsertColumn(2, "路径", LVCFMT_LEFT, 60);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFindTemplResult::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();

	*pResult = 0;
}

AString MakeTempFilePath()
{
	//	获取临时文件名称
	AString strFullPath;

	while (true)
	{
		char szBuffer1[MAX_PATH];
		DWORD dwRetVal = GetTempPathA(MAX_PATH, szBuffer1);
		if (dwRetVal > MAX_PATH || dwRetVal==0)
			break;
		
		char szBuffer2[MAX_PATH];
		UINT bCreate = GetTempFileNameA(szBuffer1, NULL, 0, szBuffer2);
		if (!bCreate || bCreate==ERROR_BUFFER_OVERFLOW)
			break;

		strFullPath = szBuffer2;
		break;
	}

	return strFullPath;
}

void OpenInNotePad(const AString &strPath)
{
	SHELLEXECUTEINFOA si;
	ZeroMemory(&si, sizeof(si));

	si.cbSize = sizeof(si);
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.lpVerb = "open";
	si.lpFile = "notepad";
	si.lpParameters = strPath;
	si.nShow = SW_SHOW;
	ShellExecuteEx(&si);
	
	// 解压完成后才返回
	WaitForSingleObject(si.hProcess, INFINITE);
	CloseHandle(si.hProcess);
}

void CDlgFindTemplResult::OnButton1() 
{
	//	输出当前选中条目到文本文件供处理

	UINT nSel = m_Lst.GetSelectedCount();
	if (nSel <= 0)	return;

	AString strTempPath = MakeTempFilePath();
	if (strTempPath.IsEmpty())	return;	
	
	AFile file;
	if (!file.Open(strTempPath, AFILE_NOHEAD | AFILE_CREATENEW | AFILE_TEXT))
		return;

	AString strFormat = "%s	%s			%s";
	AString strNewLine;

	CString strID, strName, strPath;
	int nIndex = -1;
	for (UINT i = 0; i < nSel; ++ i)
	{
		nIndex = m_Lst.GetNextItem(nIndex, LVNI_SELECTED);

		strID = m_Lst.GetItemText(nIndex, 0);
		strName = m_Lst.GetItemText(nIndex, 1);
		strPath = m_Lst.GetItemText(nIndex, 2);

		strNewLine.Format(strFormat, AC2AS(strID), AC2AS(strName), AC2AS(strPath));
		file.WriteLine(strNewLine);
	}

	file.Flush();
	file.Close();
	
	OpenInNotePad(strTempPath);
}

void CDlgFindTemplResult::OnButton2() 
{
	//	输出当前所有条目到文本文件供处理

	UINT nCount = m_Lst.GetItemCount();
	if (nCount <= 0)	return;

	AString strTempPath = MakeTempFilePath();
	if (strTempPath.IsEmpty())	return;	
	
	AFile file;
	if (!file.Open(strTempPath, AFILE_NOHEAD | AFILE_CREATENEW | AFILE_TEXT))
		return;

	AString strFormat = "%s	%s			%s";
	AString strNewLine;

	CString strID, strName, strPath;
	for (UINT i = 0; i < nCount; ++ i)
	{
		strID = m_Lst.GetItemText(i, 0);
		strName = m_Lst.GetItemText(i, 1);
		strPath = m_Lst.GetItemText(i, 2);

		strNewLine.Format(strFormat, AC2AS(strID), AC2AS(strName), AC2AS(strPath));
		file.WriteLine(strNewLine);
	}

	file.Flush();
	file.Close();
	
	OpenInNotePad(strTempPath);
}
