// ResManDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "ResManDlg.h"
#include "CommonFileDlg.h"
#include "MainFrm.h"
#include "ElementMap.h"
#include "SceneObjectManager.h"
#include "StaticModelObject.h"
#include "ResFinderDlg.h"


//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CResManDlg dialog


CResManDlg::CResManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResManDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResManDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	mTypeList[ElementResMan::RES_TYPE_BUILDING] = CString("��̬ģ��"); 
	mTypeList[ElementResMan::RES_TYPE_BUILDING_TEXTURE] = CString("��̬ģ������");
	mTypeList[ElementResMan::RES_TYPE_GFX] = CString("��Чģ��");
	mTypeList[ElementResMan::RES_TYPE_GRASS] = CString("��");
	mTypeList[ElementResMan::RES_TYPE_TREE] = CString("��");

	mRefTypeList[REF_TYPE_ALL] = CString("ȫ��");
	mRefTypeList[REF_TYPE_NO_REF] = CString("û����");
	mRefTypeList[REF_TYPE_ONLY_REF] = CString("������");
}


void CResManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResManDlg)
	DDX_Control(pDX, IDC_LIST_RES, m_listRes);
	DDX_Control(pDX, IDC_LIST_REF, m_listRef);
	DDX_Control(pDX, IDC_COMBO_RES_TYPE, m_cbResType);
	DDX_Control(pDX, IDC_COMBO_RES_REF_TYPE, m_cbResRefType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResManDlg, CDialog)
	//{{AFX_MSG_MAP(CResManDlg)
	ON_WM_DESTROY()
	ON_NOTIFY(HDN_ITEMCHANGED, IDC_LIST_RES, OnItemchangedListRes)
	ON_CBN_SELCHANGE(IDC_COMBO_RES_TYPE, OnSelchangeComboResType)
	ON_CBN_SELCHANGE(IDC_COMBO_RES_REF_TYPE, OnSelchangeComboResRefType)
	ON_BN_CLICKED(IDC_BUTTON_ADD_RES, OnButtonAddRes)
	ON_BN_CLICKED(IDC_BUTTON_DEL_RES, OnButtonDelRes)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE_RES, OnButtonReplaceRes)
	ON_BN_CLICKED(IDC_BUTTON_TRANS_OLD_MAP, OnButtonTransOldMap)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_RES, OnButtonUpdateRes)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RES, OnItemchangedListRes)
	ON_BN_CLICKED(IDC_BUTTON_RES_FIND, OnButtonResFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResManDlg message handlers

BOOL CResManDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	g_VSS.SetProjectPath("ResTable");
	g_VSS.GetFile("ResTable\\ElementRes.table");
	if(AUX_FileIsExist("ResTable\\ElementRes.table"))
		mResMan.Load();
	else 
	{
		AfxMessageBox("���ܼ�����Դ��(ResTable\\ElementRes.table)!�������������Ƿ����������Ƿ��������Ƿ�����Դ���ļ�!");
		return true;
	}

	for( int i = 0; i < ElementResMan::RES_TYPE_COUNT; i++)
	{
		m_cbResType.InsertString(i,mTypeList[i]);
	}
	for( i = 0; i < REF_TYPE_COUNT; i++)
		m_cbResRefType.InsertString(i,mRefTypeList[i]);

	m_cbResType.SetCurSel(0);
	m_cbResRefType.SetCurSel(0);

	nResType = ElementResMan::RES_TYPE_BUILDING;
	nRefType = REF_TYPE_ALL;

	m_listRes.InsertColumn(0, "��Դ��", LVCFMT_LEFT, 200);
	m_listRes.InsertColumn(1, "MD5����", LVCFMT_LEFT, 80);
	m_listRes.InsertColumn(2, "ԭʼ����", LVCFMT_LEFT, 300);
	m_listRes.InsertColumn(3, "���ô���", LVCFMT_LEFT,60);
	m_listRes.SetExtendedStyle(m_listRes.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_listRef.InsertColumn(0, "��ͼ", LVCFMT_LEFT, 200);
	m_listRef.InsertColumn(1, "����", LVCFMT_LEFT, 120);

	UpdateList();

	CWnd *pWnd = GetDlgItem(IDC_BUTTON_DEL_RES);
	//if(strcmp(g_Configs.szUserName,"wangguoguang")!=0)
	//	pWnd->EnableWindow(false);
	pWnd = GetDlgItem(IDC_BUTTON_REPLACE_RES);
	//if(strcmp(g_Configs.szUserName,"wangguoguang")!=0)
	//	pWnd->EnableWindow(false);
	//pWnd = GetDlgItem(IDC_BUTTON_TRANS_OLD_MAP);
	//if(strcmp(g_Configs.szUserName,"admin")!=0)
	//	pWnd->EnableWindow(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CResManDlg::UpdateList()
{
	m_listRes.DeleteAllItems();
	m_listRef.DeleteAllItems();
	
	int nResCount = mResMan.GetResCount(nResType);
	int nItem = 0;
	for(int i = 0; i < nResCount; i++)
	{
		Res_Item *pItem = mResMan.GetRes(nResType,i);
		if(pItem==NULL) continue;
		
		if(nRefType == REF_TYPE_NO_REF)
			if(pItem->listReference.GetCount() > 0 ) continue;
		if(nRefType == REF_TYPE_ONLY_REF)
			if(pItem->listReference.GetCount() == 0) continue;
		
		m_listRes.InsertItem(nItem,pItem->szID);
		m_listRes.SetItemText(nItem, 1, pItem->szMd5Name);
		m_listRes.SetItemText(nItem, 2, pItem->szOrgName);
		char szItemNum[10];
		itoa(pItem->listReference.GetCount(),szItemNum,10);
		m_listRes.SetItemText(nItem, 3, szItemNum);

		nItem ++;
	}
}

void CResManDlg::UpdateRefList(APtrList<char *>*pRefList)
{
	
	m_listRef.DeleteAllItems();

	m_listRef.DeleteColumn(1);
	m_listRef.DeleteColumn(0);

	CString strType;

	switch(nResType)
	{
		case ElementResMan::RES_TYPE_BUILDING_TEXTURE:
				m_listRef.InsertColumn(0, "���ö�����", LVCFMT_LEFT, 200);
				m_listRef.InsertColumn(1, "����", LVCFMT_LEFT, 120);
				strType = mTypeList[ElementResMan::RES_TYPE_BUILDING];
				break;
		case ElementResMan::RES_TYPE_BUILDING:
				m_listRef.InsertColumn(0, "����", LVCFMT_LEFT, 200);
				m_listRef.InsertColumn(1, "��ͼ", LVCFMT_LEFT, 120);
				break;
	}
	
	if(pRefList)
	{
		ALISTPOSITION pos = pRefList->GetHeadPosition();
		int nItem = 0;
		while(pos)
		{
			CString right = pRefList->GetNext(pos);
			int pos = right.FindOneOf("..");
			if(pos==-1) 
			{
				m_listRef.InsertItem(nItem,right);
				m_listRef.SetItemText(nItem, 1, strType);
			}else
			{
				CString left = right.Left(pos);
				right.Replace(left + "..",NULL);
				m_listRef.InsertItem(nItem,right);
				m_listRef.SetItemText(nItem, 1, left);
			}
			nItem ++;
		}
	}
}


void CResManDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	//mResMan.Save();
}


void CResManDlg::OnItemchangedListRes(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem < 0 || pNMListView->iSubItem != 0)
		return;

	bool bSel1 = (pNMListView->uOldState & LVIS_SELECTED) ? true : false;
	bool bSel2 = (pNMListView->uNewState & LVIS_SELECTED) ? true : false;

	if (bSel1 != bSel2 && bSel2)
	{
		CString sel = m_listRes.GetItemText(pNMListView->iItem,0);
		UpdateRefList(mResMan.GetReferenceList(nResType,sel));
	}
	*pResult = 0;
}

void CResManDlg::OnSelchangeComboResType() 
{
	// TODO: Add your control notification handler code here
	int nsel = m_cbResType.GetCurSel();
	if(nsel!=-1) nResType = nsel;
	UpdateList();
}

void CResManDlg::OnSelchangeComboResRefType() 
{
	// TODO: Add your control notification handler code here
	int nsel = m_cbResRefType.GetCurSel();
	if(nsel!=-1) nRefType = nsel;
	UpdateList();
}

void CResManDlg::OnButtonAddRes() 
{
	// TODO: Add your control notification handler code here
	char szNewRes[LENGTH_ID];
	char szFilter[] = "Resorce Files (*.mox)|*.mox||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT ;
	CFileDialog	FileDia(TRUE, "*.mox", NULL, dwFlags, szFilter, NULL);
	
	if(FileDia.DoModal()==IDOK)
	{
		
		if(!AUX_CheckOutResTable())
		{
			AfxMessageBox("CResManDlg::OnButtonTransOldMap(),���ܴӷ�������CheckOut�ļ�ElementRes.table!");
			return;
		}
		if(!mResMan.Load()) 
		{
			if(!AUX_UndoCheckOutResTable())
			{
				AfxMessageBox("CResManDlg::OnButtonTransOldMap(),���ܰ��ļ�ElementRes.table ��Undo check out!");
				return;
			}
			return;
		}
		
		CString strFileName;
		POSITION pos = FileDia.GetStartPosition();
		while(pos)
		{
			strFileName = FileDia.GetNextPathName(pos);
			strFileName.MakeLower();
			if(-1 != strFileName.Find(".mox"))
			{ 
				switch(nResType)
				{
				case ElementResMan::RES_TYPE_BUILDING:
					
					if(!mResMan.AddRes(ElementResMan::RES_TYPE_BUILDING, strFileName, szNewRes))
					{
						CString msg;
						msg.Format("������Դ(%s)ʧ��!",strFileName);
						MessageBox(msg);
					}
					break;
				case ElementResMan::RES_TYPE_GRASS:
					break;
				case ElementResMan::RES_TYPE_GFX:
					break;
				case ElementResMan::RES_TYPE_TREE:
					break;
				}
			}
		}
		
		if(mResMan.Save())
		{
			if(!AUX_CheckInResTable())
			{
				AfxMessageBox("CResManDlg::OnButtonAddRes(),���ܰ��ļ�ElementRes.table CheckIn��������!");
				return;
			}
		}else AUX_UndoCheckOutResTable();
		
	}
	
	UpdateList();
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateModelList();
}

void CResManDlg::OnButtonDelRes() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_listRes.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	else
	{
		CString msg("��ȷ��Ҫ�ӷ�������������ɾ��������Դ:\n\n");
		while (pos)
		{
			int nItem = m_listRes.GetNextSelectedItem(pos);
			CString temp = m_listRes.GetItemText(nItem,2);
			msg +=temp;
			msg +="\n";
		}
		if(IDYES != AfxMessageBox(msg,MB_YESNO|MB_ICONQUESTION)) return;
		
		pos = m_listRes.GetFirstSelectedItemPosition();
		if(!AUX_CheckOutResTable())
		{
			AfxMessageBox("CResManDlg::OnButtonDelRes() ,���ܴӷ�������CheckOut�ļ�ElementRes.table!");
			return;
		}
		if(!mResMan.Load()) 
		{
			if(!AUX_UndoCheckOutResTable())
			{
				AfxMessageBox("CResManDlg::OnButtonDelRes() ,���ܰ��ļ�ElementRes.table ��Undo check out!");
				return;
			}
			return;
		}
		while (pos)
		{
			int nItem = m_listRes.GetNextSelectedItem(pos);
			CString sel = m_listRes.GetItemText(nItem,0);
			mResMan.DeleteRes(nResType,sel);
		}
		
		mResMan.Save();
		if(!AUX_CheckInResTable())
		{
			AfxMessageBox("CResManDlg::OnButtonDelRes() ,���ܰ��ļ�ElementRes.table CheckIn��������!");
			return;
		}
	}
	UpdateList();
	AUX_GetMainFrame()->GetToolTabWnd()->UpdateModelList();
}

void CResManDlg::OnButtonReplaceRes() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_listRes.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	else
	{
		while (pos)
		{
			int nItem = m_listRes.GetNextSelectedItem(pos);
			CString sel = m_listRes.GetItemText(nItem,0);
			
			char szFilter[] = "Resorce Files (*.*)|*.*||";
			DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
			CFileDialog	FileDia(TRUE, "*.*", NULL, dwFlags, szFilter, NULL);
			
			if(FileDia.DoModal()==IDOK)
			{
				
				if(!AUX_CheckOutResTable())
				{
					AfxMessageBox("CResManDlg::OnButtonReplaceRes() ,���ܴӷ�������CheckOut�ļ�ElementRes.table!");
					return;
				}
				if(!mResMan.Load()) 
				{
					if(!AUX_UndoCheckOutResTable())
					{
						AfxMessageBox("CResManDlg::OnButtonReplaceRes() ,���ܰ��ļ�ElementRes.table ��Undo check out!");
						return;
					}
					return;
				}
				
				CString strFileName = FileDia.GetPathName();
				strFileName.MakeLower();
				bool bResult = mResMan.ReplaceRes(nResType,sel,strFileName);
				if(bResult)
				{
					mResMan.Save();
					if(!AUX_CheckInResTable())
					{
						AfxMessageBox("CResManDlg::OnButtonReplaceRes(),���ܰ��ļ�ElementRes.table CheckIn��������!");
						return;
					}
					MessageBox("��Դ�滻�ɹ�!");
				}else
				{
					AUX_UndoCheckOutResTable();
					MessageBox("��Դ�滻ʧ��!");
				}
			}
		}
	}
}

BOOL CResManDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DestroyWindow();
}

void CResManDlg::OnButtonTransOldMap() 
{
	/*
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = AUX_GetMainFrame();
	if(pFrame)
	{
		if(pFrame->GetMap()) return;
		
		g_VSS.SetProjectPath("ResTable");
		g_VSS.GetFile("ResTable\\ElementRes.table");
		if(AUX_FileIsExist("ResTable\\ElementRes.table"))
		{
			if(!AUX_CheckOutResTable()) return;
		}else
		{
			mResMan.Save();
			g_VSS.SetProjectPath("ResTable");
			g_VSS.AddFile("ResTable\\ElementRes.table");
			bool bOut = AUX_CheckOutResTable(); 
			bool bIn = AUX_CheckInResTable(); 

			if(!bOut || !bIn) 
			{
				MessageBox("����VSS������û�гɹ�!");
				return;
			}

			if(!AUX_CheckOutResTable())
			{
				AfxMessageBox("CResManDlg::OnButtonTransOldMap(),���ܴӷ�������CheckOut�ļ�ElementRes.table!");
				return;
			}
		}
		
		CString mapName;
		CElementMap emap;
		
		// Load project file list
		AScriptFile File;
		DWORD dwCol;
		if (!File.Open("EditMaps\\ProjList.dat"))
		{
			g_Log.Log("Open file EditMaps\\ProjList.dat failed!");
			return;
		}
		
		if(File.GetNextToken(true))
		{
			CString strCol(File.m_szToken);
			if(strCol.Find("col=")!=-1)
			{
				strCol.Replace("col=","");
				dwCol = (int)atof(strCol);
			}else
			{
				File.ResetScriptFile();
				dwCol = 1;
			}
			
		}

		if(!mResMan.Load()) 
		{
			if(!AUX_UndoCheckOutResTable())
			{
				AfxMessageBox("CResManDlg::OnButtonTransOldMap(),���ܰ��ļ�ElementRes.table ��Undo check out!");
				return;
			}
			return;
		}

		BeginWaitCursor();
		while (File.GetNextToken(true))
		{
			mapName.Format("%sEditMaps\\%s\\%s.elproj",g_szWorkDir,File.m_szToken,File.m_szToken);
			emap.Load(mapName);
			TransMap(&emap);
			//�ѵ�ͼ���̺���뵽������
			emap.Save(mapName);
			emap.Release();
			mapName.Format("EditMaps\\%s",File.m_szToken);
			g_VSS.SetProjectPath("EditMaps");
			g_VSS.CreateProject(File.m_szToken);
			g_VSS.SetProjectPath(mapName);
			mapName.Format("%sEditMaps\\%s\\%s.elproj",g_szWorkDir,File.m_szToken,File.m_szToken);
			g_VSS.AddFile(mapName);
			CString dstPath,srcPath;
			srcPath.Format("%sEditMaps\\%s",g_szWorkDir,File.m_szToken);
			dstPath.Format("%s\\MapsData\\%s",g_Configs.szServerPath,File.m_szToken);
			AUX_CopyFolder(dstPath,srcPath);
		}
		File.Close();
		EndWaitCursor();
		
		if(!mResMan.Save())
		{
			AUX_UndoCheckOutResTable();
			return;
		}
		if(!AUX_CheckInResTable())
		{
			AfxMessageBox("CResManDlg::OnButtonTransOldMap(),���ܰ��ļ�ElementRes.table CheckIn��������!");
			return;
		}
	}*/
}

void CResManDlg::TransMap(CElementMap *pMap)
{
	char szNewRes[LENGTH_ID];

	CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
	ALISTPOSITION pos = pMan->m_listSceneObject.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = pMan->m_listSceneObject.GetPrev(pos);
		int type = ptemp->GetObjectType();
		if(type == CSceneObject::SO_TYPE_STATIC_MODEL)
		{
			CStaticModelObject *pSM = (CStaticModelObject*)ptemp;
			CString mpath;
			PESTATICMODEL pESM = pSM->GetA3DModel();
			if(pESM==NULL) continue;

			mpath = pSM->GetModelPath();
			//����Դ�������м�����Դ
			if(!mResMan.AddRes(ElementResMan::RES_TYPE_BUILDING,mpath,szNewRes))
			{
				g_Log.Log("CResManDlg::TransMap(),������Դʧ��! %s",mpath);
				continue;
			}
			//���ĵ�ͼ��������·��
			pSM->SetModelPath(szNewRes);
			CString strRefName = pMap->GetMapName();
			strRefName = strRefName + "..";
			strRefName = strRefName + CString(pSM->GetObjectName());
			mResMan.AddReference(ElementResMan::RES_TYPE_BUILDING,szNewRes,strRefName);
		}
	}
}

void CResManDlg::OnButtonUpdateRes() 
{
	// TODO: Add your control notification handler code here
	g_VSS.SetProjectPath("ResTable");
	g_VSS.GetFile("ResTable\\ElementRes.table");
	if(mResMan.Load())
		UpdateList();
}

void CResManDlg::OnButtonResFind() 
{
	// TODO: Add your control notification handler code here
	char szBuffer[128];
	CResFinderDlg dlg;
	dlg.Init(&mResMan,nResType);
	if(dlg.DoModal()==IDOK)
	{
		if(!dlg.m_strResultItem.IsEmpty())
		{
			int left = dlg.m_strResultItem.FindOneOf("--");
			CString mark = dlg.m_strResultItem.Left(left);
			int n = m_listRes.GetItemCount();
			for( int i  = 0; i < n; i++)
			{
				m_listRes.GetItemText(i,0,szBuffer,128);
				if(strcmp(szBuffer,mark)==0)
				{
					POSITION pos = m_listRes.GetFirstSelectedItemPosition();
					while(pos)
					{
						int nItem = m_listRes.GetNextSelectedItem(pos);
						m_listRes.SetItemState(nItem,!TVIS_SELECTED, TVIS_SELECTED);
					}

					m_listRes.SetHotItem(i);
					m_listRes.SetItemState(i,TVIS_SELECTED, TVIS_SELECTED);
					ScrollItem();
					UpdateRefList(mResMan.GetReferenceList(nResType,mark));	
				}
			}
		}
	}
}

void CResManDlg::ScrollItem()
{
	if(m_listRes.GetItemCount()<=0) return;
	int num = m_listRes.GetCountPerPage();
	int oldPos = m_listRes.GetScrollPos(SB_VERT);
	int scr = m_listRes.GetItemCount() - num;
	int sel = m_listRes.GetHotItem() - oldPos;
	if(scr>0)
	{
		CRect r;
		m_listRes.GetItemRect(0, &r, LVIR_BOUNDS);
		CSize size;
		size.cx = 0;
		size.cy = sel * r.Height();
		m_listRes.Scroll(size);
	}
}
