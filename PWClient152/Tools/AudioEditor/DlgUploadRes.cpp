// DlgUploadRes.cpp : implementation file
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgUploadRes.h"
#include "Render.h"
#include "Global.h"
#include <AFileDialogWrapper.h>


// CDlgUploadRes dialog

IMPLEMENT_DYNAMIC(CDlgUploadRes, CBCGPDialog)

CDlgUploadRes::CDlgUploadRes(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgUploadRes::IDD, pParent)
{

}

CDlgUploadRes::~CDlgUploadRes()
{
}

void CDlgUploadRes::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_UPLOAD, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgUploadRes, CBCGPDialog)
	ON_BN_CLICKED(IDC_BTN_ADD_FILE, &CDlgUploadRes::OnBnClickedBtnAddFile)
	ON_BN_CLICKED(IDOK, &CDlgUploadRes::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgUploadRes message handlers

void CDlgUploadRes::OnBnClickedBtnAddFile()
{
	char szWorkDir[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, szWorkDir);
	CString csWorkDir(szWorkDir);
	if(csWorkDir.Right(1) != "\\")
		csWorkDir += "\\";
	csWorkDir.MakeLower();
	AFileDialogWrapper dlg(g_Render.GetA3DDevice(), TRUE, "", "选择要上传的资源", "mp3 wav ogg", NULL);
	if (dlg.DoModal() != IDOK)
		return;

	int iSelCnt = dlg.GetSelectedCount();
	for (int iCnt=0; iCnt<iSelCnt; ++iCnt)
	{
		CString csFullPath(dlg.GetSelectedFileByIdx(iCnt));
		csFullPath.MakeLower();
		csFullPath.Replace(csWorkDir, "");
		if(m_setResPath.find(csFullPath.GetBuffer(csFullPath.GetLength())) != m_setResPath.end())
			continue;
		m_setResPath.insert(csFullPath.GetBuffer(csFullPath.GetLength()));

		int iItem = m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), csFullPath);
		CFileStatus status;
		if(!CFile::GetStatus(csFullPath, status))
			return;
		CString csSize;
		float fSize = status.m_size/1024.0f/1024.0f;
		csSize.Format("%.2fMB", fSize);
		if(fSize < 1.0f)
			csSize.Format("%.0fKB", fSize*1024.0f);
		m_wndListCtrl.SetItemText(iItem, 1, csSize);
		CString csModifyTime;
		csModifyTime.Format("%4d-%02d-%02d %02d:%02d", status.m_mtime.GetYear(), status.m_mtime.GetMonth(), status.m_mtime.GetDay(),
			status.m_mtime.GetHour(), status.m_mtime.GetMinute());
		m_wndListCtrl.SetItemText(iItem, 2, csModifyTime);
	}
}

void CDlgUploadRes::OnBnClickedOk()
{
	int iFileNum = (int)m_setResPath.size();
	int iCnt = 0;
	ResPathSet::const_iterator it = m_setResPath.begin();
	for (; it != m_setResPath.end(); ++it)
	{
		CString csLog("正在复制：");
		csLog += (*it).c_str();		
		GF_Log(LOG_NORMAL, csLog);

		CString csDestPath(g_Configs.szResServerPath);
		csDestPath += "\\Audio_res";
		if(!createDirs((*it).c_str(), csDestPath))
			return;

		csDestPath += '\\';
		csDestPath += (*it).c_str();
		SetFileAttributes((*it).c_str(), GetFileAttributes((*it).c_str()) & !FILE_ATTRIBUTE_READONLY);
		if(!CopyFile((*it).c_str(), csDestPath, FALSE))
		{
			GF_Log(LOG_ERROR, "%s, 复制文件%s失败",__FUNCTION__, (*it).c_str());
			return;
		}
		++iCnt;		
	}

	MessageBox("上传文件成功", "完成");
	OnOK();
}

BOOL CDlgUploadRes::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();

	SetDlgItemText(IDC_EDIT_UPLOAD_PATH, g_Configs.szResServerPath);

	m_wndListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_wndListCtrl.InsertColumn(0, "文件路径", LVCFMT_LEFT, 200);
	m_wndListCtrl.InsertColumn(1, "文件大小", LVCFMT_RIGHT, 200);
	m_wndListCtrl.InsertColumn(2, "修改时间", LVCFMT_LEFT, 200);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgUploadRes::createDirs(const char* szSrcFilePath, const char* szDestRootPath)
{
	CString csSrc(szSrcFilePath);
	int iDirLen = csSrc.Find('\\');
	if(-1 == iDirLen)
		return true;
	CString csDir = csSrc.Left(iDirLen);
	CString csDestFullDir(szDestRootPath);
	csDestFullDir += "\\";
	csDestFullDir += csDir;
	if(!PathIsDirectory(csDestFullDir) && !CreateDirectory(csDestFullDir, NULL))
	{
		GF_Log(LOG_ERROR, "%s, 创建目录%s失败",__FUNCTION__, csDestFullDir);
		return false;
	}
	CString csRightSrcPath = csSrc.Right(csSrc.GetLength() - iDirLen - 1);
	if(!createDirs(csRightSrcPath, csDestFullDir))
		return false;
	return true;
}