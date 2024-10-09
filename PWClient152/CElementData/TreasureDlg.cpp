// TreasureDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "global.h"
#include "elementdata.h"
#include "TreasureDlg.h"
#include "TreasureItemDlg.h"
#include "VssOperation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif






/////////////////////////////////////////////////////////////////////////////
// CTreasureDlg dialog


CTreasureDlg::CTreasureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreasureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreasureDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bReadOnly = true;
	m_bChanged = false;
}


void CTreasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreasureDlg)
	DDX_Control(pDX, IDC_FLOAT_ICON, m_staticIcon);
	//DDX_Control(pDX, IDC_LIST_ALL_TREASURE, m_listTreasure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreasureDlg, CDialog)
	//{{AFX_MSG_MAP(CTreasureDlg)
	ON_BN_CLICKED(ID_ADD_ITEM, OnAddItem)
	ON_BN_CLICKED(ID_DEL_ITEM, OnDelItem)
	ON_LBN_DBLCLK(IDC_LIST_ALL_TREASURE, OnDblclkListAllTreasure)
	ON_BN_CLICKED(ID_EXPORT, OnExport)
	ON_BN_CLICKED(ID_CHECK_OUT, OnCheckOut)
	ON_BN_CLICKED(ID_CHECK_IN, OnCheckIn)
	ON_BN_CLICKED(ID_FRESH_TREASURE, OnFreshTreasure)
	ON_BN_CLICKED(IDC_TREASURE_MOVE_UP, OnTreasureMoveUp)
	ON_BN_CLICKED(IDC_TREASURE_MOVE_DOWN, OnTreasureMoveDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreasureDlg message handlers

void CTreasureDlg::OnAddItem() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	CTreasureItemDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		TREASURE_ITEM *pNewItem = new TREASURE_ITEM;
		memcpy(pNewItem, &dlg.m_Item,sizeof(TREASURE_ITEM));
		pNewItem->tid = m_TreasureMan.GenTID();
		m_TreasureMan.Add(pNewItem);
		m_bChanged = true;
		FreshList();
	}
}

void CTreasureDlg::OnDelItem() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CTreasureDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_TreasureMan.Release();
	return CDialog::DestroyWindow();
}

void CTreasureDlg::FreshList()
{
	m_treeTreasue.DeleteAllItems();
	for( int i = 0; i < 9 ; ++i)
	{
		MAIN_TYPE *pList = m_TreasureMan.GetMainTypeList();
		HTREEITEM hParent = m_treeTreasue.InsertItem(pList[i].szName);
		m_treeTreasue.SetItemData(hParent,0);
		m_treeTreasue.SetItemImage(hParent,0,0);
		for( size_t j = 0; j < pList[i].listSubType.size(); ++j)
		{
			
			HTREEITEM hItem = m_treeTreasue.InsertItem(pList[i].listSubType[j].szName,hParent);
			int sub_id = pList[i].listSubType[j].id;
			m_treeTreasue.SetItemData(hItem,sub_id);
			m_treeTreasue.SetItemImage(hItem,1,1);
			for( int k = 0; k < m_TreasureMan.GetTreasureCount(); ++k)
			{
				TREASURE_ITEM *pItem = m_TreasureMan.GetTreasure(k);
				if(pItem->main_type == i && pItem->sub_type == sub_id)
				{
					char szTemp[128];
					int s1,s2,s3;
					if(pItem->others & BIT_PRESENT) s1 = 1;
					else s1 = 0;

					if(pItem->others & BIT_RECOMMEND) s2 = 1;
					else s2 = 0;

					if(pItem->others & BIT_SALES_PROMOTION) s3 = 1;
					else s3 = 0;
						
					sprintf(szTemp,"%d%d%d-%s",s1,s2,s3,pItem->szName);
					HTREEITEM hLeaf = m_treeTreasue.InsertItem(szTemp,hItem);
					m_treeTreasue.SetItemData(hLeaf,pItem->tid);
					m_treeTreasue.SetItemImage(hLeaf,2,2);
				}
			}
		}
	}
}

void CTreasureDlg::OnDblclkListAllTreasure() 
{
	
}

bool CTreasureDlg::ObjIsEqual(TREASURE_ITEM *pObj1, TREASURE_ITEM *pObj2)
{
	if(pObj1->count == pObj2->count &&
		pObj1->id == pObj2->id &&
		pObj1->others == pObj2->others &&
		pObj1->buy_fashion[0].price == pObj2->buy_fashion[0].price &&
		pObj1->buy_fashion[0].until_time == pObj2->buy_fashion[0].until_time &&
		pObj1->buy_fashion[0].time == pObj2->buy_fashion[0].time &&
		pObj1->buy_fashion[1].price == pObj2->buy_fashion[1].price &&
		pObj1->buy_fashion[1].until_time == pObj2->buy_fashion[1].until_time &&
		pObj1->buy_fashion[1].time == pObj2->buy_fashion[1].time &&
		pObj1->buy_fashion[2].price == pObj2->buy_fashion[2].price &&
		pObj1->buy_fashion[2].until_time == pObj2->buy_fashion[2].until_time &&
		pObj1->buy_fashion[2].time == pObj2->buy_fashion[2].time &&
		pObj1->buy_fashion[3].price == pObj2->buy_fashion[3].price &&
		pObj1->buy_fashion[3].until_time == pObj2->buy_fashion[3].until_time &&
		pObj1->buy_fashion[3].time == pObj2->buy_fashion[3].time &&
		stricmp(pObj1->szName,pObj2->szName)==0 &&
		_wcsicmp(pObj1->desc,pObj2->desc) == 0 &&
		stricmp(pObj1->file_icon,pObj2->file_icon) == 0 &&
		pObj1->tid == pObj2->tid && 
		pObj1->main_type == pObj2->main_type &&
		pObj1->sub_type == pObj2->sub_type

		) return true;
	return false;
}

void CTreasureDlg::OnExport() 
{
	// TODO: Add your control notification handler code here
	//if(m_TreasureMan.GetTreasureCount() < 1) return;
	
	BROWSEINFO Info;
	memset(&Info, 0, sizeof (Info));

	static char szPath[MAX_PATH] = "\0";
	//if (!szPath[0])
	//	strcpy(szPath, g_szWorkDir);

	Info.hwndOwner		= m_hWnd;
	Info.pidlRoot		= NULL;
	Info.pszDisplayName	= szPath;
	Info.lpszTitle		= "请选择导出数据的存放路径：";
	Info.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_DONTGOBELOWDOMAIN ;
	
	LPITEMIDLIST pPIDL;
	if (!(pPIDL = SHBrowseForFolder(&Info)))
		return;

	SHGetPathFromIDList(pPIDL, szPath);
	
	m_treeTreasue.SortTreasure();
	
	if(m_strData.Find("1") != -1)
		sprintf(szPath,"%s\\gshop1.data",szPath);
	else sprintf(szPath,"%s\\gshop.data",szPath);
	if(!m_TreasureMan.Export(szPath))
		MessageBox("数据输出失败!");
}

BOOL CTreasureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CRect rc,rc2;
	GetWindowRect(rc2);
	GetClientRect(rc);
	int h = rc2.Height() - rc.Height() - 8;
	CWnd *pw = GetDlgItem(IDC_TREE_TREASURE);
	pw->GetWindowRect(rc);

	int width = rc.Width();
	int height = rc.Height();

	rc.left = rc.left - rc2.left;
	rc.top  = rc.top - rc2.top - h;
	rc.right = rc.left + width;
	rc.bottom = rc.top + height;

	m_treeTreasue.Create(WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER
   | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT
   | TVS_EDITLABELS | TVS_SHOWSELALWAYS,
   rc, this, 0x1005);

	m_ImageList.Create (IDB_LIST_TREASURE,16,3,RGB(255,0,255) );
	m_treeTreasue.SetImageList ( &m_ImageList,TVSIL_NORMAL );
	
	m_IconWriteable =LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_WRITEABLE));
	m_IconReadOnly = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_OBJECT));
	
	CString path;
	path.Format("TreasureData\\%s",m_strData);
	
	if(FileIsExist(AString(path)))
	{
		if(FileIsReadOnly(AString(path)))
		{
			CreateDirectory("TreasureData", NULL);
			g_VSS.SetProjectPath("TreasureData");
			g_VSS.GetAllFiles("TreasureData");
			m_staticIcon.SetIcon(m_IconReadOnly);
			m_bReadOnly = true;
		}else
		{
			m_staticIcon.SetIcon(m_IconWriteable);
			m_bReadOnly = false;
		}
		if(!m_TreasureMan.Load(m_strData))
		{
			CString msg;
			msg.Format("不能打开 %s",path);
			MessageBox(msg);
		}
		FreshList();
	}else 
	{
		m_staticIcon.SetIcon(m_IconWriteable);
		m_bReadOnly = false;
	}

	m_treeTreasue.SetTreasureMan(&m_TreasureMan);
	m_treeTreasue.m_bReadOnly = m_bReadOnly;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTreasureDlg::OnCheckOut() 
{
	// TODO: Add your control notification handler code here
	if(!m_bReadOnly) return;

	CString path;
	path.Format("TreasureData\\%s",m_strData);
	g_VSS.SetProjectPath("TreasureData");
	g_VSS.CheckOutFile(path);
	if(FileIsReadOnly(AString(path)))
	{
		MessageBox("取出文件失败！");
		return;
	}
	m_bReadOnly = false;
	m_staticIcon.SetIcon(m_IconWriteable);
	
	m_TreasureMan.Release();
	if(!m_TreasureMan.Load(m_strData))
	{
		CString msg;
		msg.Format("不能打开 %s", path);
		MessageBox(msg);
	}
	m_treeTreasue.m_bReadOnly = m_bReadOnly;
	FreshList();
}

void CTreasureDlg::OnCheckIn() 
{
	if(m_bReadOnly) return;
	
	if(!m_bReadOnly)
	{
		if(m_bChanged || m_TreasureMan.IsChanged() || m_treeTreasue.m_bChanged)
		{
			if(IDYES==MessageBox("数据已经改动，你需要把改动后的数据保存到服务器吗？","存盘",MB_YESNO|MB_ICONQUESTION))
			{
				m_treeTreasue.SortTreasure();
				m_TreasureMan.Save(m_strData);
			}
			m_bChanged = false;
		}

		CString path;
		path.Format("TreasureData\\%s",m_strData);
		g_VSS.SetProjectPath("TreasureData");
		g_VSS.CheckInFile(m_strData);
		if(FileIsReadOnly(AString(path)))
		{
			m_bReadOnly = true;
			m_staticIcon.SetIcon(m_IconReadOnly);
		}else
		{ 
			m_bReadOnly = false;
		}
	}
	m_treeTreasue.m_bReadOnly = m_bReadOnly;
	
}

void CTreasureDlg::OnOK()
{
	if((m_bChanged || m_TreasureMan.IsChanged() || m_treeTreasue.m_bChanged) && !m_bReadOnly )
	{
		if(IDYES==MessageBox("数据已经改动，你需要存盘吗？","存盘",MB_YESNO|MB_ICONQUESTION))
		{
			m_treeTreasue.SortTreasure();
			m_TreasureMan.Save(m_strData);
		}
	}
	CDialog::OnOK();
}
void CTreasureDlg::OnCancel()
{
	OnOK();
}

void CTreasureDlg::OnFreshTreasure() 
{
	CString path;
	path.Format("TreasureData\\%s",m_strData);
	if(FileIsExist(AString(path)))
	{
		if(!FileIsReadOnly(AString(path)))
		{
			return;
		}
	}else CreateDirectory("TreasureData", NULL);

	g_VSS.SetProjectPath("TreasureData");
	g_VSS.GetAllFiles("TreasureData");
	m_staticIcon.SetIcon(m_IconReadOnly);
	m_bReadOnly = true;
	m_TreasureMan.Release();
	m_TreasureMan.Load(m_strData);
	FreshList();
}

BOOL CTreasureDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (pMsg->message == WM_KEYDOWN &&
         pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
      {
         CEdit* edit = m_treeTreasue.GetEditControl();
         if (edit)
         {
            edit->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
            return TRUE;
         }
      }
      return CDialog::PreTranslateMessage(pMsg);
}


void CTreasureDlg::OnTreasureMoveUp() 
{
	// TODO: Add your control notification handler code here
		m_treeTreasue.MoveUp();
}

void CTreasureDlg::OnTreasureMoveDown() 
{
	// TODO: Add your control notification handler code here
	m_treeTreasue.MoveDown();
}
