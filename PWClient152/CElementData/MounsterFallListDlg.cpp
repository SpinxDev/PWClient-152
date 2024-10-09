// MounsterFallListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Global.h"
#include "ElementData.h"
#include "MounsterFallListDlg.h"
#include "MounsterFallItemDlg.h"
#include "VssOperation.h"
#include "AFile.h"
#include "BaseDataIDMan.h"
#include "BaseDataTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallListDlg dialog


CMounsterFallListDlg::CMounsterFallListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMounsterFallListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMounsterFallListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pData = 0;
	m_bReadOnly = false;
	m_bChanged = false;
}


void CMounsterFallListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMounsterFallListDlg)
	DDX_Control(pDX, IDC_LIST_ITEM, m_listItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMounsterFallListDlg, CDialog)
	//{{AFX_MSG_MAP(CMounsterFallListDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_LBN_DBLCLK(IDC_LIST_ITEM, OnDblclkListItem)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_CHECK_IN, OnRadioCheckIn)
	ON_BN_CLICKED(IDC_RADIO_CHECK_OUT, OnRadioCheckOut)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_TXT_TEST,OnExportTestTxt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallListDlg message handlers

void CMounsterFallListDlg::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	CMounsterFallItemDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		m_pData->AddItem(dlg.m_pItem);
		FreshList();
		m_bChanged = true;
	}
}

void CMounsterFallListDlg::OnDel() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	int sel = m_listItem.GetCurSel();
	if(sel!=-1) 
	{
		if(AfxMessageBox("你确定要删除指定元素吗？",MB_YESNO|MB_ICONQUESTION)!=IDYES) return;
		m_pData->DeleteItem(sel);
		m_bChanged = true;
	}
	FreshList();
}

void CMounsterFallListDlg::OnDblclkListItem() 
{
	// TODO: Add your control notification handler code here
	int sel = m_listItem.GetCurSel();
	if(sel!=-1)
	{
		CMounsterFallItemDlg dlg;
		dlg.m_bReadOnly = m_bReadOnly;
		dlg.m_pItem = m_pData->GetItem(sel);
		if(IDOK==dlg.DoModal())
		{
			m_pData->SetItem(sel,dlg.m_pItem);	
			FreshList();
			m_bChanged = true;
		}else
		{
			m_pData->SetItem(sel,dlg.m_pItem);	
		}
	}
}

void CMounsterFallListDlg::FreshList()
{
	int n = m_listItem.GetCount();
	int sel = m_listItem.GetCurSel();
	int i(0);
	for( i = 0; i < n; ++i) m_listItem.DeleteString(0);

	for( i = 0; i < m_pData->GetItemNum(); ++i)
	{
		CMounsterFallItem *pItem = m_pData->GetItem(i);
		m_listItem.AddString(pItem->GetItemName());
	}
	if(sel!=-1 && sel < m_listItem.GetCount()) m_listItem.SetCurSel(sel);
}

BOOL CMounsterFallListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pData = new CMounsterResetFallListData;
	if(FileIsReadOnly("FallItemData\\FallItem.dat"))
	{
		CreateDirectory("FallItemData", NULL);
		g_VSS.SetProjectPath("FallItemData");
		g_VSS.GetAllFiles("FallItemData");
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_CHECK_IN);
		pButton->SetCheck(1);
		m_bReadOnly = true;
	}else
	{
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_CHECK_OUT);
		pButton->SetCheck(1);
		m_bReadOnly = false;
	}
	
	if(!m_pData->Load("FallItemData\\FallItem.dat"))
	{
		MessageBox("不能打开 FallItemData\\FallItem.dat !");
	}
	FreshList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMounsterFallListDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pData)
	{
		if(!m_bReadOnly)
		{
			if(m_bChanged)
			{
				if(AfxMessageBox("掉落表数据已经改变! 你需要保存吗?",MB_YESNO|MB_ICONQUESTION)==IDYES)
					m_pData->Save("FallItemData\\FallItem.dat");
			}
		}
		m_pData->Release();
		delete m_pData;
	}
}

void CMounsterFallListDlg::OnRadioCheckIn() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	
	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_CHECK_IN);
	if(m_pData)
	{
		if(!m_bReadOnly)
		{
			if(m_bChanged)
			{
				m_pData->Save("FallItemData\\FallItem.dat");
				m_bChanged = false;
			}
			g_VSS.SetProjectPath("FallItemData");
			g_VSS.CheckInFile("FallItem.dat");
			if(FileIsReadOnly("FallItemData\\FallItem.dat"))
			{
				m_bReadOnly = true;
				pButton->SetCheck(1);
			}else
			{ 
				m_bReadOnly = false;
				pButton->SetCheck(0); 
			}
		}
	}
}

void CMounsterFallListDlg::OnRadioCheckOut() 
{
	// TODO: Add your control notification handler code here
	if(!m_bReadOnly) return;

	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_CHECK_OUT);
	g_VSS.SetProjectPath("FallItemData");
	g_VSS.CheckOutFile("FallItemData\\FallItem.dat");
	if(FileIsReadOnly("FallItemData\\FallItem.dat"))
	{
		MessageBox("取出文件失败！");
		pButton->SetCheck(0);
		return;
	}
	pButton->SetCheck(1);
	m_bReadOnly = false;
	if(m_pData==0) m_pData = new CMounsterResetFallListData;
	else m_pData->Release();
	if(!m_pData->Load("FallItemData\\FallItem.dat"))
	{
		MessageBox("不能打开 FallItemData\\FallItem.dat !");
	}
	FreshList();
}

void CMounsterFallListDlg::OnExport() 
{
	// TODO: Add your control notification handler code here
	if(m_pData==0) return;
	
	//extra_drops.sev
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
	
	
	if(!m_bReadOnly && m_bChanged)
	{
		m_pData->Save("FallItemData\\FallItem.dat");
		m_bChanged = false;
	}
	
	for( int j = 0; j < m_pData->GetItemNum(); ++j)
	{
		CMounsterFallItem *m_pItem = m_pData->GetItem(j);
		ITEM_ELEMENT *pItem = m_pItem->GetElementList();
		float total = 0.0f;
		int i(0);
		for( i = 0; i < 256; ++i)
		{
			if(pItem[i].id == 0) pItem[i].fProbability = 0;
			total += pItem[i].fProbability;
		}
		//归一处理
		if(total > 0.0f)
		{
			for( i = 0; i < 256; ++i) pItem[i].fProbability *= 1.0f/total;
		}else 
		{
			for( i = 0; i < 256; ++i) 
			{
				if( pItem[i].id !=0 )
				{
					pItem[i].fProbability = 1.0f;
					break;
				}
			}
		}
		
		float *pPro = m_pItem->GetProbabilityList();
		total = 0.0f;
		for( i = 0; i < 8; ++i)
			total += pPro[i];
		if(total > 0.0f) for( i = 0; i < 8; ++i) pPro[i] *=1.0f/total; 
		else pPro[0] = 1.0f;
		
		
	}
	sprintf(szPath,"%s\\extra_drops.sev",szPath);
	if(!m_pData->Save(szPath))
		AfxMessageBox("输出失败！", MB_ICONSTOP);
	
	m_pData->Release();
	m_pData->Load("FallItemData\\FallItem.dat");
	
}


void	CMounsterFallListDlg::OnExportTestTxt()
{
	if(m_pData == 0) 
		return;

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

	CString strPath(szPath);
	AFile file;
	strPath += "\\test.txt";
	if (!file.Open(strPath, AFILE_CREATENEW | AFILE_TEXT))
	{
		AfxMessageBox("创建文本文件失败!");
		return;
	}
	
	char	szLine[AFILE_LINEMAXLEN];
	
	const char* _Mounster_Export_Version_	= "Sign: Mounster,Version: %u"; 
	const char* _Mounster_Fall_Item_		= "***********怪物掉落项%d: %s***********"; 
	const char* _Mounster_List_				= "怪物列表%d: %s";
	const char* _Mounster_Fall_NUM_			= "掉落%d个物品概率: %f";
	const char* _Mounster_Fall_				= "掉落物品%d的ID: %u";
	const char* _Mounster_Fall_NUM_Pro_		= "物品%d的掉落概率: %f"; 

	sprintf(szLine,_Mounster_Export_Version_,1);
	file.WriteLine(szLine);

	int nNum = m_pData->GetItemNum();
	for (int i=0;i<nNum;i++)
	{
		//怪物掉落项
		CMounsterFallItem *pItem = m_pData->GetItem(i);
		
		sprintf(szLine,_Mounster_Fall_Item_,i,pItem->GetItemName());
		file.WriteLine(szLine);
		
		int nItemNum = pItem->GetMounsterNum();
		for (int j=0;j<nItemNum;j++)
		{
			CString strName;
			unsigned int uID = pItem->GetMounsterID(j);
			if (uID != 0)
			{
				strName = g_BaseIDMan.GetPathByID(uID);
				if (!strName.IsEmpty())
				{
					BaseDataTempl tmpl;
					if (tmpl.Load(strName) == 0)
					{
						strName = tmpl.GetName();
						tmpl.Release();
					}
				}
				else
					strName.Format("(错误的ID = %d)", uID);
			}
			else
				strName = "(无)";

			sprintf(szLine,_Mounster_List_,j,strName);
			file.WriteLine(szLine);
		}
		
		memset(szLine,0,AFILE_LINEMAXLEN);
		file.WriteLine(szLine);

		float *pPro = pItem->GetProbabilityList();
		for (int p=0;p<8;p++)
		{
			sprintf(szLine,_Mounster_Fall_NUM_,p,pPro[p]);
			file.WriteLine(szLine);
		}
		
		memset(szLine,0,AFILE_LINEMAXLEN);
		file.WriteLine(szLine);

		ITEM_ELEMENT *pEle = pItem->GetElementList();
		for (int q=0;q<256;q++)
		{
			sprintf(szLine,_Mounster_Fall_,q,pEle[q].id);
			file.WriteLine(szLine);
			
			sprintf(szLine,_Mounster_Fall_NUM_Pro_,q,pEle[q].fProbability);
			file.WriteLine(szLine);
		}

		memset(szLine,0,AFILE_LINEMAXLEN);
		file.WriteLine(szLine);
	}


	file.Close();
}	