// CommonFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "CommonFileDlg.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CCommonFileDlg dialog

CCommonFileDlg::CCommonFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommonFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCommonFileDlg)
	m_fileName = _T("");
	//}}AFX_DATA_INIT

	m_curDir = "";
	m_curRoot = "";
	m_relRoot = "";
	m_ext = "*.*";
	m_workRoot = "";

	m_pCurNode = NULL;
	m_pRootNode = NULL;
	
	m_pPackage = NULL;
	m_pSoundBufMan = NULL;
	m_pSoundBuf = NULL;

	bChanged = true;
	flags = AFILE_ALLOWMULFILES;
}

CCommonFileDlg::CCommonFileDlg(A3DEngine* pA3DEngine, CString sRelPath, CString sExt, CWnd* pParent /* = NULL */)
	: CDialog(CCommonFileDlg::IDD, pParent)
{
	m_pA3DEngine = pA3DEngine;

	m_curDir = "";
	m_curRoot = "";
	m_relRoot = "";
	m_ext = sExt;
	m_workRoot = sRelPath;

	m_pCurNode = NULL;
	m_pRootNode = NULL;
	
	m_pPackage = NULL;
	m_pSoundBufMan = NULL;
	m_pSoundBuf = NULL;

	bChanged = true;
	flags = AFILE_ALLOWMULFILES;
}

void CCommonFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommonFileDlg)
	DDX_Control(pDX, IDC_STATIC_PREV, m_sPrev);
	DDX_Control(pDX, IDC_UP, m_upBut);
	DDX_Control(pDX, IDC_STATIC_IMAGEINFO, m_strImageInfo);
	DDX_Control(pDX, IDC_STATIC_FILEINFO, m_strFileInfo);
	DDX_Control(pDX, IDC_FILELIST, m_lstFile);
	DDX_Control(pDX, IDC_DIRLIST, m_lstDir);
	DDX_Control(pDX, IDC_COMBOBOXEX_LOOKIN, m_cboLookIn);
	DDX_Control(pDX, IDC_COMBO_FILETYPE, m_cboFileType);
	DDX_Control(pDX, IDC_CHECK, m_preview);
	DDX_Control(pDX, IDC_BACKWARD, m_prevBut);
	DDX_Text(pDX, IDC_FILENAME, m_fileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommonFileDlg, CDialog)
	//{{AFX_MSG_MAP(CCommonFileDlg)
	ON_EN_CHANGE(IDC_FILENAME, OnChangeFilename)
	ON_NOTIFY(NM_CLICK, IDC_DIRLIST, OnClickDirlist)
	ON_NOTIFY(NM_CLICK, IDC_FILELIST, OnClickFilelist)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, OnDblclkFilelist)
	ON_BN_CLICKED(IDC_CHECK, OnPreview)
	ON_CBN_SELCHANGE(IDC_COMBO_FILETYPE, OnSelchangeComboFiletype)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LOOKIN, OnSelchangeComboboxexLookin)
	ON_BN_CLICKED(IDC_UP, OnbtnUp)
	ON_BN_CLICKED(IDC_BACKWARD, OnbtnBack)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommonFileDlg message handlers

void CCommonFileDlg::OnChangeFilename() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_fileName = "";
	UpdatePrevRegion();
	m_strFileInfo.SetWindowText("");
	m_strImageInfo.SetWindowText("");
	UpdateData(FALSE);
}

void CCommonFileDlg::OnClickDirlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int hItem =((NMLISTVIEW*)pNMHDR)->iItem;
	if(hItem < 0)
		return;

	CString strName = m_lstDir.GetItemText(hItem, 0);

	m_curDir = m_workRoot;
	m_curDir += "\\";
	m_curDir += strName;

	UpdateLookinCombo();

	if(OpenMode == 1)
		UpdateDiskListCtrl(m_curDir);
	
	if(OpenMode == 2)
	{
		if(m_pRootNode != NULL)
		{
			CPckTreeNode* pChild = m_pRootNode->FindChild(strName);
			
			if(pChild != NULL)
			{
				UpdatePckListCtrl(pChild);
			}
		}
	}

	m_backDir.Add(m_curDir);
	bChanged = true;

	*pResult = 0;
}

void CCommonFileDlg::OnClickFilelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int hItem = ((NMLISTVIEW*)pNMHDR)->iItem;

	if(hItem < 0)
	{
		m_fileName = "";
		if(m_pSoundBuf != NULL)
			m_pSoundBuf->Stop();
		UpdatePrevRegion();
		m_strFileInfo.SetWindowText("");
		m_strImageInfo.SetWindowText("");
		UpdateData(FALSE);

		return;
	}

	GetRelCurRoot();
	m_fileName = m_lstFile.GetItemText(hItem, 0);

	if(hItem < m_arrDir.GetSize())
	{
		m_fileName = "";
		if(m_pSoundBuf != NULL)
			m_pSoundBuf->Stop();
		UpdatePrevRegion();
		m_strFileInfo.SetWindowText("");
		m_strImageInfo.SetWindowText("");
		UpdateData(FALSE);
	}
	else
	{
		if(m_preview.GetCheck() == TRUE)
		{
			if(m_fileName.Find("bmp") != -1 || 
					m_fileName.Find("tga") != -1 ||
					m_fileName.Find("jpg") != -1 ||
					m_fileName.Find("dds") != -1)
			{
				LoadPrev(m_relRoot+"\\"+m_fileName);
			}
			else if(m_fileName.Find("mp3") != -1 || m_fileName.Find("wav") != -1)
			{
				/*
				if(m_pSoundBuf != NULL)
					m_pSoundBufMan->Release2DSound(&m_pSoundBuf);
				m_pSoundBuf = m_pSoundBufMan->Load2DSound(m_relRoot+"\\"+m_fileName);
				if(m_pSoundBuf != NULL)
				{
					m_pSoundBuf->Stop();
					m_pSoundBuf->Play(0);
				}*/
			}
			else
				UpdatePrevRegion();
		}
		else
		{
			UpdatePrevRegion();
			m_strFileInfo.SetWindowText("");
			m_strImageInfo.SetWindowText("");
		}
		UpdateData(FALSE);
	}

	if(flags == AFILE_ALLOWMULFILES)
		GetMulFiles();
	
	*pResult = 0;
}

void CCommonFileDlg::OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int hItem = ((NMLISTVIEW*)pNMHDR)->iItem;
	if(hItem < 0)
		return;

	CString strName = m_lstFile.GetItemText(hItem, 0);

	if(OpenMode == 1)
	{
		if(hItem < m_arrDir.GetSize())
		{
			m_curDir += "\\";
			m_curDir += strName;

			UpdateLookinCombo();

			UpdateDiskListCtrl(m_curDir);
		}
	}
	
	if(OpenMode == 2)
	{
		if(m_pCurNode != NULL)
		{
			CPckTreeNode* pChild = m_pCurNode->FindChild(strName);
			if(pChild != NULL)
			{
				if(pChild->m_bIsDirectory)
				{
					m_curDir += "\\";
					m_curDir += strName;

					UpdateLookinCombo();

					UpdatePckListCtrl(pChild);
				}
			}
		}
	}
	
	m_backDir.Add(m_curDir);
	bChanged = true;

	*pResult = 0;
}

BOOL CCommonFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	HBITMAP hbmp;
	hbmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), "res\\bitmap1.bmp", 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	m_prevBut.SetBitmap(hbmp);

	hbmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), "res\\selectfolder.bmp", 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	m_upBut.SetBitmap(hbmp);


	hbmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), "res\\listicon.bmp", 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	bmp[0].Attach(hbmp);
	m_imageList.Create(16,16,TRUE,1,1);
	m_imageList.Add(&bmp[0], RGB(255, 0, 255));

	hbmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), "res\\bitmap3.bmp", 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	bmp[1].Attach(hbmp);
	m_dirImageList.Create(24,24,TRUE,1,1);
	m_dirImageList.Add(&bmp[1], RGB(255, 255, 255));

	m_lstFile.SetImageList(&m_imageList, LVSIL_SMALL);

	m_lstDir.SetImageList(&m_dirImageList, LVSIL_NORMAL);
	m_lstDir.SetBkColor(RGB(95, 95, 95));
	m_lstDir.SetTextBkColor(RGB(95, 95, 95));
	m_lstDir.SetTextColor(RGB(255, 255, 255));

	m_cboLookIn.SetImageList(&m_imageList);
	
	m_preview.SetCheck(TRUE);

	InitExt();

	if(!InitDir())
		return FALSE;

	if(hbmp != NULL)
		::DeleteObject(hbmp);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCommonFileDlg::OnPreview() 
{
	// TODO: Add your control notification handler code here
	m_preview.SetCheck((m_preview.GetCheck()==TRUE) ? 1 : 0);

	if(m_preview.GetCheck() && m_fileName != "")
	{
		GetRelCurRoot();
		CString sFileName = m_relRoot + "\\" + m_fileName;
		if(m_fileName.Find("bmp") != -1 || 
			m_fileName.Find("tga") != -1 ||
			m_fileName.Find("jpg") != -1 ||
			m_fileName.Find("dds") != -1)
		{
				LoadPrev(sFileName);
		}
		/*
		if(m_fileName.Find("mp3") != -1 || m_fileName.Find("wav") != -1)
		{
			m_pSoundBuf = m_pSoundBufMan->Load2DSound(m_relRoot+"\\"+m_fileName);
			if(m_pSoundBuf != NULL)
			{
				m_pSoundBuf->Stop();
				m_pSoundBuf->Play(0);
			}
		}
		*/	//del by xin
	}
	else
	{
		//if(m_pSoundBuf != NULL)//del by xin
		//	m_pSoundBuf->Stop();//del by xin
		m_strFileInfo.SetWindowText("");
		m_strImageInfo.SetWindowText("");
		UpdatePrevRegion();
	}	
}

void CCommonFileDlg::OnSelchangeComboFiletype() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_cboFileType.GetCurSel();
	if(nIndex == 0)
		m_ext = "*.*";
	else
	{
		m_cboFileType.GetLBText(nIndex, m_ext);

		int nCount = m_ext.Find(')')-m_ext.Find('(')-1;
		m_ext = m_ext.Mid(m_ext.Find('(')+1, nCount);
	}

	if(OpenMode == 1)
		UpdateDiskListCtrl(m_curDir);

	if(OpenMode == 2)
		UpdatePckListCtrl(m_pCurNode);	
}

void CCommonFileDlg::OnSelchangeComboboxexLookin() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_cboLookIn.GetCurSel();

	if(OpenMode == 1)
	{
		m_cboLookIn.GetLBText(nIndex, m_curDir);
		UpdateDiskListCtrl(m_curDir);
	}

	if(OpenMode == 2)
	{
		CPckTreeNode* pNode = m_pCurNode;
		for(int i=nIndex+1; i<m_cboLookIn.GetCount(); i++)
		{
			int nPos = m_curDir.ReverseFind('\\');
			m_curDir = m_curDir.Left(nPos);
			pNode = pNode->m_pParent;
		}
		UpdatePckListCtrl(pNode);
	}
	
	m_backDir.Add(m_curDir);
	bChanged = true;

	for(int i=m_cboLookIn.GetCount()-1; i>nIndex; i--)
		m_cboLookIn.DeleteString(i);

	m_cboLookIn.SetCurSel(m_cboLookIn.GetCount()-1);	
}

void CCommonFileDlg::OnbtnUp() 
{
	// TODO: Add your control notification handler code here
	if(m_cboLookIn.GetCount()-1 != 0)
	{
		int nPos = m_curDir.ReverseFind('\\');
		m_curDir = m_curDir.Left(nPos);

		if(OpenMode == 1)
			UpdateDiskListCtrl(m_curDir);
		
		if(OpenMode == 2)
		{
			CPckTreeNode* pNode = m_pCurNode->m_pParent;
			
			UpdatePckListCtrl(pNode);
		}
		
		m_backDir.Add(m_curDir);
		bChanged = true;
		
		m_cboLookIn.DeleteString(m_cboLookIn.GetCount()-1);
		m_cboLookIn.SetCurSel(m_cboLookIn.GetCount()-1);
	}	
}

void CCommonFileDlg::OnbtnBack() 
{
	// TODO: Add your control notification handler code here
	if(bChanged)
	{
		if(m_backDir.GetAt(m_backDir.GetSize()-1) != m_workRoot)
			m_backDir.RemoveAt(m_backDir.GetSize()-1);
		bChanged = false;
	}
	
	if(!bChanged)
	{
		if(m_backDir.GetSize() != 0)
		{
			m_curDir = m_backDir.GetAt(m_backDir.GetSize()-1);

			UpdateLookinCombo();

			if(OpenMode == 1)
				UpdateDiskListCtrl(m_curDir);

			if(OpenMode == 2)
			{
				CPckTreeNode* pNode = m_pRootNode;
				for(int i=1; i<m_cboLookIn.GetCount(); i++)
				{
					CString sDir;
					m_cboLookIn.GetLBText(i, sDir);
					pNode = pNode->FindChild(sDir);
				}
				UpdatePckListCtrl(pNode);
			}

			if(m_backDir.GetAt(m_backDir.GetSize()-1) != m_workRoot)
				m_backDir.RemoveAt(m_backDir.GetSize()-1);
		}
	}	
}

bool CCommonFileDlg::InitDir()
{
	m_pSoundBufMan = m_pA3DEngine->GetAMEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();

	m_curRoot = af_GetBaseDir();
	if(m_workRoot == "")
	{
		m_workRoot = m_curRoot;
	}
	else
	{
		m_workRoot = m_curRoot + "\\" + m_workRoot;
	}

	if(1)//g_pAFilePackage == NULL)
	{
		// Read package files from disk
		OpenMode = 1;

		if(!OpenFromDisk(m_workRoot))
			return false;
	}
	else
	{
		/*
		m_pPackage = g_pAFilePackage;
		// Read package files from package
		OpenMode = 2;
		if(!OpenFromPackage(m_workRoot))
			return false;
			*/
	}

	m_backDir.Add(m_workRoot);

	return true;
}

void CCommonFileDlg::InitExt()
{
	m_cboFileType.ResetContent();
	if(m_ext=="*.*" || m_ext=="")
	{
		m_ext = "*.*";
		m_cboFileType.AddString("Texture Files (*.bmp, *.tga, *.jpg, *.dds)");
		m_cboFileType.AddString("Sound Files (*.mp3, *.wav)");
		m_cboFileType.AddString("Text Files(*.txt)");
		m_cboFileType.AddString("All Files (*.*)");
		m_cboFileType.SetCurSel(0);
	}else
	{
		CString str;
		str.Format("Files(%s)",m_ext);
		m_cboFileType.AddString("Files(*.*)");
		m_cboFileType.AddString(str);
		m_cboFileType.SetCurSel(1);
	}
	
}

bool CCommonFileDlg::OpenFromDisk(CString sPath)
{
	m_curDir = sPath;

	UpdateLookinCombo();
	
	if(!UpdateDiskListCtrl(m_curDir))
		return false;

	if(!UpdateDiskDirCtrl(m_workRoot))
		return false;

	return true;
}

bool CCommonFileDlg::OpenFromPackage(CString sPath)
{
	if(sPath == "")
	{
		AfxMessageBox("Bad path name parameter for open pckfile!");
		return false;
	}

	m_tnRoot.m_strName = sPath;
	m_tnRoot.m_bIsDirectory = TRUE;
	m_tnRoot.m_dwNodeData = 0;
	m_tnRoot.m_pParent = NULL;

	int nCount = m_pPackage->GetFileNumber();
	for(int i=0; i<nCount; i++)
	{
		const AFilePackage::FILEENTRY* fe = m_pPackage->GetFileEntryByIndex(i);
		if (!fe)
		{
			AfxMessageBox("Cannot get file entry!");
			ClosePckListCtrl();
			return FALSE;
		}

		if (!m_tnRoot.AddFullName(fe->szFileName))
		{
			m_pPackage->RemoveFile(fe->szFileName);
			nCount--;
			i--;
		}
	}

	m_curDir = m_tnRoot.m_strName;
	
	UpdateLookinCombo();

	if(!UpdatePckListCtrl(&m_tnRoot))
		return false;

	if(!UpdatePckDirCtrl(&m_tnRoot))
		return false;

	m_pRootNode = &m_tnRoot;

	return true;
}

bool CCommonFileDlg::UpdateDiskListCtrl(CString sPath)
{
	m_lstFile.DeleteAllItems();
	m_arrDir.RemoveAll();

	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString FileName;

	BOOL fFinished = FALSE;

	hSearch = FindFirstFile(sPath+"\\"+"*", &FileData);
	if(hSearch == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("No files found!");
		return false;
	}
	
	while(!fFinished)
	{
		FileName = FileData.cFileName;
		if(strcmp(FileName, ".") != 0 && strcmp(FileName, "..") != 0)
		{
			if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				m_lstFile.InsertItem(0, FileName, 2);

				m_arrDir.Add(FileName);
			}
			else
			{
				int nCount;
				CString temp;
				if(FileName.Find('.') > 0)
					temp = FileName.Right(FileName.GetLength()-FileName.ReverseFind('.')-1);
				
				if(m_ext == "*.*")
				{
					if(temp.Find("bmp") != -1 || 
							temp.Find("dds") != -1 || 
							temp.Find("jpg") != -1 || 
							temp.Find("tga") != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, FileName, 9);	
					}
					else if(temp.Find("mp3") != -1 || 
								temp.Find("wav") != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, FileName, 10);
					}
					else
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, FileName, 8);
					}
				}
				else
				{
					if(m_ext.Find(temp) != -1)
					{
						if(m_ext.Find("bmp") != -1)
						{
							nCount = m_lstFile.GetItemCount();
							m_lstFile.InsertItem(nCount, FileName, 9);
						}
						else if(m_ext.Find("mp3") != -1)
						{
							nCount = m_lstFile.GetItemCount();
							m_lstFile.InsertItem(nCount, FileName, 10);
						}
						else
						{
							nCount = m_lstFile.GetItemCount();
							m_lstFile.InsertItem(nCount, FileName, 8);
						}
					}
				}
			}
		}
	
		if(!FindNextFile(hSearch, &FileData))
		{
			if(GetLastError() == ERROR_NO_MORE_FILES)
			{
				fFinished = TRUE;
			}
			else
			{
				AfxMessageBox("Couldn't find next file.");
				return false;
			}
		}
	}

	if(!FindClose(hSearch))
	{
		AfxMessageBox("Couldn't close search handle.");
		return false;
	}

	return true;
}


bool CCommonFileDlg::UpdateDiskDirCtrl(CString sPath)
{
	m_lstDir.DeleteAllItems();

	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString FileName;

	BOOL fFinished = FALSE;

	hSearch = FindFirstFile(sPath+"\\"+"*.*", &FileData);
	if(hSearch == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("No files found!");
		return false;
	}
	
	while(!fFinished)
	{
		FileName = FileData.cFileName;
		if(strcmp(FileName, ".") != 0 && strcmp(FileName, "..") != 0)
		{
			if(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				m_lstDir.InsertItem(0, FileName, 1);
			}
		}
	
		if(!FindNextFile(hSearch, &FileData))
		{
			if(GetLastError() == ERROR_NO_MORE_FILES)
			{
				fFinished = TRUE;
			}
			else
			{
				AfxMessageBox("Couldn't find next file.");
				return false;
			}
		}
	}

	if(!FindClose(hSearch))
	{
		AfxMessageBox("Couldn't close search handle.");
		return false;
	}

	return true;
}

void CCommonFileDlg::UpdateArrCurDir(CString sDir)
{
	m_arrCurDir.RemoveAll();
	if(m_curDir.GetLength() == 0)
		return;

	CString temp = sDir;
	int n = temp.Find('\\');
	while (n > 0)
	{
		m_arrCurDir.Add(temp.Left(n));
		temp = temp.Mid(n+1);
		n = temp.Find('\\');
	}
	m_arrCurDir.Add(temp);
}

void CCommonFileDlg::UpdateLookinCombo()
{
	COMBOBOXEXITEM item;
	ZeroMemory(&item, sizeof(item));
	
	m_cboLookIn.ResetContent();
	
	if(m_curDir == m_workRoot)
	{
		item.mask = CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
		item.iItem = 0;
		item.iImage = 1;
		item.pszText = (LPSTR)(LPCTSTR)m_curDir;
		m_cboLookIn.InsertItem (&item);
		m_cboLookIn.SetCurSel(0);
	}
	else
	{
		item.mask = CBEIF_IMAGE|CBEIF_INDENT|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
		item.iItem = 0;
		item.iImage = 1;
		item.pszText = (LPSTR)(LPCTSTR)m_workRoot;
		m_cboLookIn.InsertItem (&item);
		
		GetRelWorkRoot();
		UpdateArrCurDir(m_relworkRoot);

		for(int i=0; i<m_arrCurDir.GetSize(); i++)
		{
			item.iItem = i+1;
			item.iImage = 2;
			item.iSelectedImage = 2;
			item.iIndent = i+1;
			item.pszText = (LPSTR)(LPCTSTR)m_arrCurDir.GetAt(i);
			m_cboLookIn.InsertItem(&item);
		}

		m_cboLookIn.SetCurSel(m_cboLookIn.GetCount()-1);
	}
}

bool CCommonFileDlg::UpdatePckListCtrl(CPckTreeNode* pNode)
{
	m_lstFile.DeleteAllItems();
	m_arrDir.RemoveAll();

	m_pCurNode = pNode;
	if(pNode == NULL)
		return false;

	POSITION pos = pNode->m_lstChilds.GetHeadPosition();
	while(pos != NULL)
	{
		CPckTreeNode* pChild = pNode->m_lstChilds.GetNext(pos);
		if(pChild == NULL)
			continue;
		if(pChild->m_bIsDirectory)
		{
			m_lstFile.InsertItem(0, pChild->m_strName, 2);

			m_arrDir.Add(pChild->m_strName);
		}
		else
		{
			int nCount = m_lstFile.GetItemCount();
			CString temp;
			CString sFileName = pChild->m_strName;

			if(sFileName.Find('.') > 0)
				temp = sFileName.Right(sFileName.GetLength()-sFileName.ReverseFind('.')-1);
			
			if(m_ext == "*.*")
			{
				if(temp.Find("bmp") != -1 || 
							temp.Find("dds") != -1 || 
							temp.Find("jpg") != -1 || 
							temp.Find("tga") != -1)
				{
					nCount = m_lstFile.GetItemCount();
					m_lstFile.InsertItem(nCount, sFileName, 9);
				}
				else if(temp.Find("mp3") != -1 || 
							temp.Find("wav") != -1)
				{
					nCount = m_lstFile.GetItemCount();
					m_lstFile.InsertItem(nCount, sFileName, 10);
				}
				else
				{
					nCount = m_lstFile.GetItemCount();
					m_lstFile.InsertItem(nCount, sFileName, 8);
				}
			}
			else
			{
				if(m_ext.Find(temp) != -1)
				{
					if(m_ext.Find("bmp") != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, sFileName, 9);
					}
					else if(m_ext.Find("mp3") != -1)
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, sFileName, 10);
					}
					else
					{
						nCount = m_lstFile.GetItemCount();
						m_lstFile.InsertItem(nCount, sFileName, 8);
					}
				}
			}
		}
	}

	return true;
}

bool CCommonFileDlg::UpdatePckDirCtrl(CPckTreeNode* pNode)
{
	m_lstDir.DeleteAllItems();
	
	m_pCurNode = pNode;
	if(pNode == NULL)
		return false;

	POSITION pos = pNode->m_lstChilds.GetHeadPosition();
	while(pos != NULL)
	{
		CPckTreeNode* pChild = pNode->m_lstChilds.GetNext(pos);
		if(pChild == NULL)
			continue;
		if(pChild->m_bIsDirectory)
		{
			m_lstDir.InsertItem(0, pChild->m_strName, 1);
		}
	}

	return true;
}

void CCommonFileDlg::ClosePckListCtrl()
{
	m_tnRoot.FreeAllChilds();

	if(m_pPackage != NULL)
	{
		delete m_pPackage;
		m_pPackage = NULL;
	}
}

bool CCommonFileDlg::LoadPrev(CString sFile)
{

	LPDIRECT3DSURFACE8 surf;
	CRect rc;
	LPDIRECT3DDEVICE8 m_pD3DDevice;
	D3DXIMAGE_INFO hInfo;

	m_sPrev.GetWindowRect(&rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	m_pD3DDevice = m_pA3DEngine->GetA3DDevice()->GetD3DDevice();
	if(FAILED(m_pD3DDevice->CreateImageSurface(width, 
												height, 
												D3DFMT_A8R8G8B8, 
												&surf)))
	{
		return false;
	}

	AFileImage file;
	if(!file.Open(sFile, AFILE_OPENEXIST | AFILE_BINARY))
		return false;

	DWORD nfilesize = file.GetFileLength();
	if(FAILED(D3DXLoadSurfaceFromFileInMemory(surf, NULL, NULL, file.GetFileBuffer(),
										nfilesize,
										NULL, D3DX_FILTER_TRIANGLE, 0, &hInfo)))
	{
		return false;
	}
	
	file.Close();

	if(FAILED(D3DXSaveSurfaceToFile("D:\\temp.bmp", D3DXIFF_BMP, surf, NULL, NULL)))
	{
		return false;
	}
		
	surf->Release();

	DisplayBmp();	

	TCHAR szScrap[64];
	wsprintf(szScrap, TEXT("File Size:%d byte\0"), nfilesize);
	m_strFileInfo.SetWindowText(szScrap);
	wsprintf(szScrap, TEXT("%d x %d\0"), hInfo.Width, hInfo.Height);
	m_strImageInfo.SetWindowText(szScrap);

	return true;
}

bool CCommonFileDlg::DisplayBmp()
{
	
	HBITMAP hbmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), "D:\\temp.bmp",
									IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(hbmp == NULL)
		return false;

	if(m_bmpPrev.m_hObject != NULL)
	{
		m_bmpPrev.DeleteObject();
	}

	m_bmpPrev.Attach(hbmp);

	CDC* dc;
	CRect rcclient;
	CDC memdc;
	CBitmap* oldbitmap;

	dc = m_sPrev.GetDC();
    m_sPrev.GetClientRect(&rcclient);
    memdc.CreateCompatibleDC(dc);  
    oldbitmap = memdc.SelectObject(&m_bmpPrev);
	
	dc->BitBlt(rcclient.left, rcclient.top, rcclient.Width(), rcclient.Height(), 
         &memdc, rcclient.left, rcclient.top,SRCCOPY);

	memdc.SelectObject(oldbitmap);

	DeleteDC(memdc);
	ReleaseDC(dc);

	if(hbmp != NULL)
		::DeleteObject(hbmp);

	return true;
}

void CCommonFileDlg::UpdatePrevRegion()
{
	CRect rect;
	m_sPrev.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
}

bool CCommonFileDlg::GetMulFiles()
{
	m_arrFilelist.RemoveAll();
	if(m_lstFile.GetSelectedCount() > 1)
	{
		for(int i=0; i<m_lstFile.GetSelectedCount(); i++)
		{
			CString strName = m_lstFile.GetItemText(m_lstFile.GetSelectionMark()+i, 0);
			m_arrFilelist.Add(strName);
		}
	}

	return true;
}

void CCommonFileDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
//	DisplayBmp();
	// Do not call CDialog::OnPaint() for painting messages
}


void CCommonFileDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	ClosePckListCtrl();

	m_imageList.DeleteImageList();
	m_dirImageList.DeleteImageList();

	bmp[0].DeleteObject();
	bmp[1].DeleteObject();
	m_bmpPrev.DeleteObject();
	
	if(m_pSoundBuf != NULL)
		m_pSoundBufMan->Release2DSound(&m_pSoundBuf);

	DeleteFile("D:\\temp.bmp");
	
	CDialog::OnOK();
}

void CCommonFileDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	ClosePckListCtrl();

	m_imageList.DeleteImageList();
	m_dirImageList.DeleteImageList();

	bmp[0].DeleteObject();
	bmp[1].DeleteObject();
	m_bmpPrev.DeleteObject();
	
	if(m_pSoundBuf != NULL)
		m_pSoundBufMan->Release2DSound(&m_pSoundBuf);

	DeleteFile("D:\\temp.bmp");
	
	CDialog::OnCancel();
}
