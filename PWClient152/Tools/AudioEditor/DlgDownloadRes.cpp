// DlgDownloadRes.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgDownloadRes.h"
#include "DownloadResources.h"
#include "Global.h"


// CDlgDownloadRes dialog

IMPLEMENT_DYNAMIC(CDlgDownloadRes, CBCGPDialog)

CDlgDownloadRes::CDlgDownloadRes(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgDownloadRes::IDD, pParent)
{

}

CDlgDownloadRes::~CDlgDownloadRes()
{
}

void CDlgDownloadRes::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_FOLDERS, m_wndListBox);
}


BEGIN_MESSAGE_MAP(CDlgDownloadRes, CBCGPDialog)
	ON_BN_CLICKED(ID_BTN_DOWNLOAD, &CDlgDownloadRes::OnBnClickedBtnDownload)
END_MESSAGE_MAP()


// CDlgDownloadRes message handlers

BOOL CDlgDownloadRes::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();

	m_wndListBox.SetCheckStyle(BS_AUTOCHECKBOX);
	int nCnt = m_wndListBox.GetCount();
	for (int i=0; i<nCnt; ++i)
	{
		m_wndListBox.DeleteString(0);
	}

	char szBuf[MAX_PATH];
	_snprintf_s(szBuf, MAX_PATH, "%s\\Audio_res\\*.*", g_Configs.szResServerPath);

	CFileFind finder;
	BOOL bFind = finder.FindFile(szBuf);
	while (bFind)
	{
		bFind = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if(finder.IsDirectory())
		{
			m_wndListBox.AddString(finder.GetFileName());
		}		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDownloadRes::OnBnClickedBtnDownload()
{
	CDownloadResources downloadRes;	
	int nCnt = m_wndListBox.GetCount();
	for (int i=0;i<nCnt; ++i)
	{
		if(m_wndListBox.GetCheck(i))
		{
			CString csText;
			m_wndListBox.GetText(i,csText);
			char szBuf[MAX_PATH];
			_snprintf_s(szBuf, MAX_PATH, "%s\\Audio_res\\%s", g_Configs.szResServerPath, csText.GetBuffer(csText.GetLength()));
			downloadRes.AddPath(szBuf, csText.GetBuffer(csText.GetLength()));
		}
	}
	downloadRes.Download();
	CBCGPDialog::OnOK();
}
