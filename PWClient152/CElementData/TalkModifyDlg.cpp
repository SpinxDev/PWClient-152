// TalkModifyDlg.cpp : implementation file
//

#include "stdafx.h"
#ifdef TASK_TEMPL_EDITOR
	#include "../../CTaskTemplEditor/TaskTemplEditor.h"
#else
	#include "Global.h"
	#include "ElementData.h"
	#include "TestDlg.h"
#endif

#include "TalkModifyDlg.h"
#include "TalkCreateDlg.h"
#include "BaseDataIDMan.h"
#include "VssOperation.h"

#define new A_DEBUG_NEW

extern CVssOperation g_VSS;
extern bool FileIsReadOnly(AString szFilePathName);

/////////////////////////////////////////////////////////////////////////////
// TALK_WINDOW class
TALK_WINDOW::TALK_WINDOW()
{

}

TALK_WINDOW::~TALK_WINDOW()
{
	
}


void TALK_WINDOW::AddChild(TALK_OPTION *pTO)
{
	ASSERT(pTO);
	listOptions.AddTail(pTO);
}

void TALK_WINDOW::DeleteChild(DWORD id)
{
	POSITION oldpos;
	POSITION pos = listOptions.GetHeadPosition();
	while(pos)
	{
		oldpos = pos;
		TALK_OPTION * pPt = (TALK_OPTION *)listOptions.GetNext(pos);
		if(pPt->dwID==id)
		{
			listOptions.RemoveAt(oldpos);
			delete pPt;
			break;
		}
	}
}

void TALK_WINDOW::DeleteAllChild()
{
	listOptions.RemoveAll();
}

TALK_OPTION* TALK_WINDOW::GetChild(int index)
{
	POSITION pos = listOptions.FindIndex(index);
	if(pos)
		return (TALK_OPTION*)listOptions.GetAt(pos);
	return NULL;
}

int  TALK_WINDOW::GetChildNum()
{
	return listOptions.GetCount();
}

void TALK_WINDOW::SetID(DWORD id)
{
	dwID = id;
}

void TALK_WINDOW::SetName(const AString name)
{
	//strName = name;
	strcpy(strName,name);
}

void TALK_WINDOW::SetCommand(const AString command)
{
	strCommand = command;
}

void TALK_WINDOW::SetDescribe(const AString desc)
{
	strDescribe = desc;
}

DWORD TALK_WINDOW::GetID()
{
	return dwID;
}

char* TALK_WINDOW::GetName()
{
	return strName;
}

AString TALK_WINDOW::GetDescribe()
{
	return strDescribe;
}

AString TALK_WINDOW::GetCommand()
{
	return strCommand;
}

bool TALK_WINDOW::Save(AFile *pFile,DWORD dwVersion)
{
	//Write base data
	DWORD wl;
	ASSERT(pFile);
	if(!pFile->Write(&dwID,sizeof(DWORD),&wl))
		return false;
	pFile->WriteString(strName);
	pFile->WriteString(strDescribe);
	pFile->WriteString(strCommand);
	
	//Write childrens
	int size = listOptions.GetCount();
	if(!pFile->Write(&size,sizeof(int),&wl))
		return false;
	
	POSITION pos = listOptions.GetHeadPosition();
	while(pos)
	{
		TALK_OPTION * pPt = (TALK_OPTION *)listOptions.GetNext(pos);
		if(!pFile->Write(&pPt->dwID,sizeof(DWORD),&wl))
			return false;
		if(!pFile->Write(&pPt->dwLinkID,sizeof(DWORD),&wl))
			return false;
		pFile->WriteString(pPt->strName);
		pFile->WriteString(pPt->strCommand);
	}
	
	return true;
}

bool TALK_WINDOW::Load(AFile *pFile,DWORD dwVersion)
{
	//Read base data
	DWORD wl;
	AString tempName;
	ASSERT(pFile);
	if(!pFile->Read(&dwID,sizeof(DWORD),&wl))
		return false;
	pFile->ReadString(tempName);
	strcpy(strName,tempName);
	pFile->ReadString(strDescribe);
    pFile->ReadString(strCommand);
	//Read childrens , old version
	if(dwVersion > 0)
	{
		int size;
		if(!pFile->Read(&size,sizeof(int),&wl))
			return false;
		for( int i = 0; i< size; i++)
		{
			TALK_OPTION * pPt = new TALK_OPTION;
			if(!pFile->Read(&pPt->dwID,sizeof(DWORD),&wl))
				return false;
			if(!pFile->Read(&pPt->dwLinkID,sizeof(DWORD),&wl))
				return false;
			pFile->ReadString(tempName);
			strcpy(pPt->strName,tempName);
			pFile->ReadString(pPt->strCommand);
			listOptions.AddTail(pPt);
		}
	}
	return true;
}

void TALK_WINDOW::Release()
{
	POSITION pos = listOptions.GetHeadPosition();
	while(pos)
	{
		TALK_OPTION * pPt = (TALK_OPTION *)listOptions.GetNext(pos);
		delete pPt;
	}
	listOptions.RemoveAll();
}

TALK_WINDOW *CTalkModifyDlg::GetWindow(CString strName)
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		AString name = pPt->GetName();
		if(strcmp(name,strName)==0)
		{
			return pPt;
		}
	}
	return NULL;
}

TALK_OPTION *CTalkModifyDlg::GetOption(CString strName)
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		int num = pPt->GetChildNum();
		for(int  i = 0; i < num ; i++)
		{
			TALK_OPTION *pTO = pPt->GetChild(i);
			if(strcmp(pTO->strName,strName)==0)
				return pTO;
		}
	}
	return NULL;
}

TALK_OPTION *CTalkModifyDlg::GetOption(DWORD id)
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		int num = pPt->GetChildNum();
		for(int  i = 0; i < num ; i++)
		{
			TALK_OPTION *pTO = pPt->GetChild(i);
			if(pTO->dwID == id)
				return pTO;
		}
	}
	return NULL;
}

TALK_WINDOW *CTalkModifyDlg::GetWindow(DWORD   id)
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		if(pPt->GetID()==id)
		{
			return pPt;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CTalkModifyDlg dialog

BOOL CALLBACK TALK_ID_FUNCS::OnActivate(void)
{
	CTalkModifyDlg dlg;
	
	dlg.m_dwID = m_var;
	if (dlg.m_dwID)
	{
		dlg.m_strPathName = g_TalkIDMan.GetPathByID(dlg.m_dwID);
	}
	
	bool bCheckOut = false;
	//先做Check out
	if(!dlg.m_strPathName.IsEmpty())
	{	
		g_VSS.SetProjectPath("TalkData");
		g_VSS.CheckOutFile(dlg.m_strPathName);
		if(FileIsReadOnly(AString(dlg.m_strPathName)))
		{
			CString msg;
			msg.Format("文件(%s)不能Check out !",dlg.m_strPathName);
			return FALSE;
		}
		bCheckOut = true;
	}else
	{
		dlg.m_dwID = 0;
	}

	CString fileName = dlg.m_strPathName;
	fileName.Replace("TalkData\\",NULL);
	if (dlg.DoModal() != IDOK)
	{
		if(bCheckOut) 
		{	
			g_VSS.SetProjectPath("TalkData");
			g_VSS.CheckInFile(fileName);
		}
		return FALSE;
	}
	if(bCheckOut) 
	{	
		g_VSS.SetProjectPath("TalkData");
		g_VSS.CheckInFile(fileName);
	}

	m_strText = dlg.m_strTalkName;
	m_var = dlg.m_dwID;
	return TRUE;
}

LPCTSTR CALLBACK TALK_ID_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK TALK_ID_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK TALK_ID_FUNCS::OnSetValue(const AVariant& var)
{
	m_var = var;
	unsigned long ulID = unsigned long(m_var);

	AString str;
	str = g_TalkIDMan.GetPathByID(ulID);
	
	if (!str.IsEmpty())
	{
		CTalkModifyDlg dlg;
		dlg.m_strPathName = str;
		if (dlg.LoadTalk())
		{
			m_strText = dlg.m_strTalkName;
			dlg.ReleaseWindow();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTalkModifyDlg dialog

CTalkModifyDlg::CTalkModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTalkModifyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTalkModifyDlg)
	m_strTalkName = _T("");
	m_dwID = 0;
	//}}AFX_DATA_INIT
	m_bNewFile = false;
	m_bPreset = false;
	init_talk = 0;
}


void CTalkModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTalkModifyDlg)
	DDX_Control(pDX, IDC_TREE_TALK, m_treeTalk);
	DDX_Text(pDX, IDC_EDIT_TALK_NAME, m_strTalkName);
	DDX_Text(pDX, IDC_EDIT_TALK_ID, m_dwID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTalkModifyDlg, CDialog)
	//{{AFX_MSG_MAP(CTalkModifyDlg)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_TALK, OnRclickTreeTalk)
	ON_WM_DESTROY()
	ON_COMMAND(ID_CREATE_TALK_WINDOW, OnCreateTalkWindow)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_ITEM, OnButtonDeleteItem)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_ITEM_UP, OnButtonMoveItemUp)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_ITEM_DOWN, OnButtonMoveItemDown)
	ON_COMMAND(ID_MODIFY_TALK_WINDOW, OnModifyTalkWindow)
	ON_BN_CLICKED(IDC_BUTTON_TALK_TEST, OnButtonTalkTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTalkModifyDlg message handlers

void CTalkModifyDlg::OnRclickTreeTalk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point;
	GetCursorPos(&point);
	POINT pt = {point.x, point.y};
	m_treeTalk.ScreenToClient(&pt);
	CPoint client(pt.x,pt.y);
	pt.x = point.x;
	pt.y = point.y;

	UINT uFlags;
	HTREEITEM itemp = m_treeTalk.HitTest(client,&uFlags);
	if(itemp == NULL) return; 
	m_treeTalk.SelectItem(itemp);
	CString path = m_treeTalk.GetItemText(itemp);
	while(itemp = m_treeTalk.GetParentItem(itemp))
	{
		CString pathsub = m_treeTalk.GetItemText(itemp);
		path = pathsub + "\\" + path;	
	}
	path = "BaseData\\" + path;
	
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU_TALK);
	
	CMenu* pPopupMenu = Menu.GetSubMenu(0);
	ASSERT(pPopupMenu);
	
	
	UpdateMenu(pPopupMenu);
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
	*pResult = 0;
}

void CTalkModifyDlg::UpdateMenu(CMenu *pMenu)
{
	CTreeCtrl *pCtrl = (CTreeCtrl *)GetDlgItem(IDC_TREE_TALK);
	HTREEITEM item = m_treeTalk.GetSelectedItem();
	
	if(item == NULL) return;
	DWORD ptr = m_treeTalk.GetItemData(item);
	CString itemTxt;
	itemTxt.Format("%s",(char*)ptr);
	TALK_WINDOW *pParent = GetWindow(itemTxt);
	if(pParent==NULL)
	{
		pMenu->EnableMenuItem(ID_CREATE_TALK_WINDOW,MF_GRAYED);	
	}else if(pParent->GetID()>>30 == 3)
	{
		pMenu->EnableMenuItem(ID_CREATE_TALK_WINDOW,MF_GRAYED);	
	}
}

BOOL CTalkModifyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (init_talk)
	{
		UpdateData(false);
	}
	else if(!m_strPathName.IsEmpty())
	{
		LoadTalk();
		UpdateData(false);
	}

	if(m_listTalkWindow.GetCount()<1)
	{
		UpdateData(false);
		//CTalkCreateDlg dlg;
		//dlg.SetEnableCreateWindow(true);
		//if(dlg.DoModal()==IDOK)
		//{
			TALK_WINDOW *pNew = new TALK_WINDOW;
			pNew->SetID(m_IDGenerator.GenerateID());
			CString m_strName;
			m_strName.Format("MainWindow_%d",pNew->GetID());
			pNew->SetName(AString(m_strName));
			pNew->SetDescribe(AString(""));
			pNew->SetCommand(AString("RootNode"));
			m_listTalkWindow.AddTail(pNew);
			m_bNewFile = true;
		//}else
		//{
		//	EndDialog(0);
		//	return FALSE;
		//}
	}

	m_ImageList.Create (IDB_TALKLIST,16,3,RGB(255,0,255) );
	m_treeTalk.SetImageList ( &m_ImageList,TVSIL_NORMAL );
	FreshTreeView();

	CWnd *pWnd  = GetDlgItem(IDC_EDIT_TALK_NAME);
	if(pWnd) pWnd->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTalkModifyDlg::FreshTreeView()
{
	m_treeTalk.DeleteAllItems();
	HTREEITEM hItem;
	TALK_WINDOW *pTW = (TALK_WINDOW *)m_listTalkWindow.GetHead();
	if(pTW) 
	{
		hItem = m_treeTalk.InsertItem(pTW->GetCommand(),NULL,NULL);
		m_treeTalk.SetItemData(hItem,(DWORD)pTW->GetName());
	}
	m_treeTalk.SetItemImage(hItem,0,0);
	EnumTree(hItem,pTW);
}

void CTalkModifyDlg::EnumTree(HTREEITEM hItem,TALK_WINDOW *pTalkWnd)
{
	HTREEITEM hNewItem;
	int num = pTalkWnd->GetChildNum();
	for(int i = 0; i< num; i++)
	{
		TALK_OPTION* pTO = pTalkWnd->GetChild(i);
		DWORD result = pTO->dwID & 0x80000000;
		if( result == 0)
		{
			TALK_WINDOW* pNewTWnd = GetWindow(pTO->dwID);
			hNewItem = m_treeTalk.InsertItem(pNewTWnd->GetCommand(),hItem,NULL);
			m_treeTalk.SetItemData(hNewItem,(DWORD)pNewTWnd->GetName());
			m_treeTalk.SetItemImage(hNewItem,0,0);
			m_treeTalk.Expand(hItem,TVE_EXPAND);
			EnumTree(hNewItem,pNewTWnd);
		}else
		{
			result = pTO->dwID>>30;
			hNewItem = m_treeTalk.InsertItem(pTO->strCommand,hItem,NULL);
			m_treeTalk.SetItemData(hNewItem,(DWORD)pTO->strName);
			if(result==2)
				m_treeTalk.SetItemImage(hNewItem,1,1);
			else m_treeTalk.SetItemImage(hNewItem,2,2);
			m_treeTalk.Expand(hItem,TVE_EXPAND);
		}
	}
}

//修补窗口数目不删除的错误(F_TALK_VERSION==0x1)
void CTalkModifyDlg::Repair_error1()
{
	bool bLoop = true;
	while(bLoop)
	{
		bLoop = false;
		POSITION pos = m_listTalkWindow.GetHeadPosition();
		while(pos)
		{
			TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
			if(!FindIsUsed((TALK_WINDOW *)m_listTalkWindow.GetHead(),pPt)) 
			{
				DeleteWindow(pPt->GetName());	
				bLoop = true;
				break;
			}
		}
	}
}

//查看该窗口是否被用
bool CTalkModifyDlg::FindIsUsed(TALK_WINDOW *pRoot,TALK_WINDOW *pTalkWnd)
{
	TALK_WINDOW * pPt = pRoot;
	if(pPt)
	{
		if(strcmp(pPt->GetName(),pTalkWnd->GetName())==0) return true;
		for(int i = 0; i < pPt->GetChildNum(); i++)
		{
			TALK_OPTION *pOP = pPt->GetChild(i);
			DWORD result = pOP->dwID & 0x80000000;
			if(result == 0)
			{
				TALK_WINDOW * pChild = GetWindow(pOP->dwID);
				if(pChild)
				{
					if(FindIsUsed(pChild,pTalkWnd))
						return true;
				}
			}
		}
	}

	return false;
}

bool CTalkModifyDlg::LoadTalk()
{
	DWORD dwResult;
	CString msg;
	AFile File;
	if(File.Open(m_strPathName,AFILE_OPENEXIST))
	{
		DWORD dwVersion;
		if(!File.Read(&dwVersion,sizeof(DWORD),&dwResult))
			return false;
		if(!File.Read(&m_dwID,sizeof(DWORD),&dwResult))
			return false;
		
		if(dwVersion>0)
		{
			DWORD base;
			if(!File.Read(&base,sizeof(DWORD),&dwResult))
				return false;
			m_IDGenerator.AddID(base);
		}
		
		AString str;
		File.ReadString(str);
		m_strTalkName = str; 
		DWORD wndNum;
		if(!File.Read(&wndNum,sizeof(DWORD),&dwResult))
			return false;
		for(DWORD i = 0; i< wndNum; i++)
		{
			TALK_WINDOW * pPt = new TALK_WINDOW;
			if(!pPt->Load(&File,dwVersion))
			{
				msg.Format("CTalkModifyDlg::LoadTalk(),加载文件(%s)失败！",m_strPathName);
				MessageBox(msg);
				return false;
			}
			m_listTalkWindow.AddTail(pPt);
			if(dwVersion<1) m_IDGenerator.AddID(pPt->GetID());
		}	
		if(dwVersion<2) Repair_error1();
	}else
	{
		msg.Format("不能打开文件:%s！",m_strPathName);
		MessageBox(msg);
		return false;
	}
	//UpdateData(false);
	return true;		
}

bool CTalkModifyDlg::SaveTalk()
{
	DWORD dwResult;
	CString msg;
	AFile File;
	if(File.Open(m_strPathName,AFILE_CREATENEW))
	{
		DWORD dwVersion = F_TALK_VERSION;
		if(!File.Write(&dwVersion,sizeof(DWORD),&dwResult))
			return false;
		if(!File.Write(&m_dwID,sizeof(DWORD),&dwResult))
			return false;
		
		DWORD base = m_IDGenerator.GetBaseData();
		if(!File.Write(&base,sizeof(DWORD),&dwResult))
			return false;
	
		AString str(m_strTalkName);
		File.WriteString(str);
		DWORD wndNum = m_listTalkWindow.GetCount();
		if(!File.Write(&wndNum,sizeof(DWORD),&dwResult))
			return false;
		POSITION pos = m_listTalkWindow.GetHeadPosition();
		while(pos)
		{
			TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
			if(!pPt->Save(&File,dwVersion))
			{
				msg.Format("保存文件(%s)失败！",m_strPathName);
				MessageBox(msg);
				return false;
			}
		}	
	}else
	{
		msg.Format("不能打开文件:%s！",m_strPathName);
		MessageBox(msg);
		return false;
	}
	return true;
}

void CTalkModifyDlg::ReleaseWindow()
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		pPt->Release();
		delete pPt;
	}
	m_listTalkWindow.RemoveAll();
}

void CTalkModifyDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	ReleaseWindow();
}

void CTalkModifyDlg::OnOK()
{
	UpdateData(true);

	if (init_talk)
	{
		ExportData(init_talk, 0, init_talk->id_talk);
		EndDialog(IDOK);
		return;
	}

	if (m_dwID == 0)
	{
		int k = 0;
		while(1)
		{
			m_strTalkName.Format("Talk_%d",g_TalkIDMan.GetIdNum() + k);
			CString strPath("TalkData\\");
			strPath = strPath + m_strTalkName + ".talk";
			m_strPathName = strPath;
			m_dwID = g_TalkIDMan.CreateID(AString(m_strPathName));
			if(m_dwID == 0)
			{	
				k++;
				continue;
			}else break;
			if(k>500000)
			{
				MessageBox("不能为对话分配ID,对话加入失败!");
				return;
			}
		}
	}
	SaveTalk();
	
	if(m_bNewFile)
	{
		g_VSS.SetProjectPath("TalkData");
		g_VSS.AddFile(m_strPathName);
		g_VSS.CheckInFile(m_strPathName);
	}
	
	EndDialog(IDOK);
}

void CTalkModifyDlg::OnCreateTalkWindow() 
{
	// TODO: Add your command handler code here
	CTreeCtrl *pCtrl = (CTreeCtrl *)GetDlgItem(IDC_TREE_TALK);
	HTREEITEM item = m_treeTalk.GetSelectedItem();
	HTREEITEM hNewItem;
	if(item == NULL) return;
	DWORD ptr = m_treeTalk.GetItemData(item);
	CString itemTxt;
	itemTxt.Format("%s",(char*)ptr);
	TALK_WINDOW *pParent = GetWindow(itemTxt);
	if(pParent==NULL) return;

	CTalkCreateDlg dlg;
	if (m_bPreset)
	{
		dlg.m_bPreset = true;
		dlg.m_dwServiceID = m_dwSvrToSel;
		dlg.m_dwSvrToSel = m_dwSvrToSel;
		dlg.m_dwServiceLinkID = m_dwParamToSel;
		dlg.m_dwParamToSel = m_dwParamToSel;
		dlg.m_dwParamToSel2 = m_dwParamToSel2;
	}

	dlg.m_dwID = m_IDGenerator.GenerateID();

	if(dlg.DoModal()==IDOK)
	{
		dlg.m_strName.Format("window_%d",dlg.m_dwID);
		if(FindName(dlg.m_strName))
		{
			ASSERT(FALSE);
		}
		TALK_OPTION *pOp = new TALK_OPTION;
		ASSERT(pOp);
		strcpy(pOp->strName,dlg.m_strName);
		pOp->strCommand = AString(dlg.m_strCommandName);
		if(dlg.m_bWindow)
		{
			TALK_WINDOW *pNew = new TALK_WINDOW;
			ASSERT(pNew);
			pNew->SetID(dlg.m_dwID);
			pNew->SetName(AString(dlg.m_strName));
			pNew->SetDescribe(AString(dlg.m_strDesc));
			pNew->SetCommand(AString(dlg.m_strCommandName));
			m_listTalkWindow.AddTail(pNew);
			pOp->dwID = dlg.m_dwID;
			pOp->dwLinkID = 0;
			//pOp->dwParams = 0;
			hNewItem = m_treeTalk.InsertItem(dlg.m_strCommandName,item,NULL);
			m_treeTalk.SetItemData(hNewItem,(DWORD)pNew->GetName());
			if(pOp->dwID>>30 == 3)
				m_treeTalk.SetItemImage(hNewItem,2,2);
			else m_treeTalk.SetItemImage(hNewItem,0,0);
		}else
		{
			pOp->dwID = dlg.m_dwServiceID;
			pOp->dwLinkID = dlg.m_dwServiceLinkID;
			//pOp->dwParams = dlg.m_dwParams;
			hNewItem = m_treeTalk.InsertItem(dlg.m_strCommandName,item,NULL);
			m_treeTalk.SetItemData(hNewItem,(DWORD)pOp->strName);
			m_treeTalk.SetItemImage(hNewItem,1,1);
		}
		pParent->AddChild(pOp);
		
	}

	CWnd *pWnd  = GetDlgItem(IDC_EDIT_TALK_NAME);
	if(pWnd) pWnd->SetFocus();
}

bool CTalkModifyDlg::FindName(CString name)
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		if(strcmp(pPt->GetName(),name)==0)
		{
			return true;
		}
		
		int num = pPt->GetChildNum();
		for(int i = 0; i< num; i++)
		{
			TALK_OPTION *pTO = pPt->GetChild(i);
			if(strcmp(pTO->strName,name)==0)
				return true;
		}
	}
	return false;
}

void CTalkModifyDlg::BackupData()
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		int num = pPt->GetChildNum();
		for(int i = 0; i< num; i++)
		{
			TALK_OPTION *pTO = pPt->GetChild(i);
			DWORD result = pTO->dwID & 0x80000000;
			if(result != 0)
			{
				m_listBackup.AddTail(pTO);
			}else
			{
				delete pTO;
				pTO = NULL;
			}
		}
	}
}

void CTalkModifyDlg::ReleaseBackupData()
{
	m_listBackup.RemoveAll();
}

TALK_OPTION *CTalkModifyDlg::GetOptionFromBackup(CString name)
{
	POSITION pos = m_listBackup.GetHeadPosition();
	while(pos)
	{
		TALK_OPTION * pPt = (TALK_OPTION *)m_listBackup.GetNext(pos);
		if(strcmp(pPt->strName,name)==0)
			return pPt;
	}
	return NULL;
}

void CTalkModifyDlg::ReBuildTalkWindow()
{
	BackupData();
	HTREEITEM hItem = m_treeTalk.GetRootItem();
	if(hItem)
	{
		TALK_WINDOW *pTW = (TALK_WINDOW *)m_listTalkWindow.GetHead();
		GetTreeData(hItem,pTW);
	}
	ReleaseBackupData();
}

void CTalkModifyDlg::DeleteItem(HTREEITEM hItem)
{
	HTREEITEM hnext = hItem;
	DWORD ptr = m_treeTalk.GetItemData(hnext);
	CString itemTxt;
	itemTxt.Format("%s",(char*)ptr);
	DeleteWindow(itemTxt);
	
	HTREEITEM child = m_treeTalk.GetChildItem(hItem);
	while(child) 
	{
		DeleteItem(child);
		child = m_treeTalk.GetNextItem(child,TVGN_NEXT);
	}
}

void CTalkModifyDlg::DeleteWindow(CString strName)
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	POSITION posOld;
	while(pos)
	{
		posOld = pos;
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		AString name = pPt->GetName();
		if(strcmp(name,strName)==0)
		{
			pPt->Release();
			delete pPt;
			m_listTalkWindow.RemoveAt(posOld);
			return;
		}
	}
}

void CTalkModifyDlg::GetTreeData(HTREEITEM hItem,TALK_WINDOW *pTalkWnd)
{
	
	pTalkWnd->DeleteAllChild();
	HTREEITEM child = m_treeTalk.GetChildItem(hItem);
	
	while(child)
	{
		DWORD ptr = m_treeTalk.GetItemData(child);
		CString itemTxt;
		itemTxt.Format("%s",(char*)ptr);
		TALK_WINDOW *pTW = GetWindow(itemTxt);
		if(pTW)
		{
			TALK_OPTION *pTO = new TALK_OPTION;
			pTO->dwID = pTW->GetID();
			pTO->dwLinkID = 0;
			strcpy(pTO->strName,pTW->GetName());
			pTO->strCommand = pTW->GetCommand();
			pTalkWnd->AddChild(pTO);
			GetTreeData(child,pTW);
		}else
		{
			TALK_OPTION *pold = GetOptionFromBackup(itemTxt);
			pTalkWnd->AddChild(pold);
		}
		child = m_treeTalk.GetNextItem(child,TVGN_NEXT);
	}
	
}

void CTalkModifyDlg::OnButtonDeleteItem() 
{
	// TODO: Add your control notification handler code here
	int n = m_listTalkWindow.GetCount();
	HTREEITEM hItem = m_treeTalk.GetSelectedItem();
	if(hItem)
	{
		if(hItem == m_treeTalk.GetRootItem()) return;
		
		HTREEITEM hDel = hItem;
		DeleteItem(hDel);//new
		m_treeTalk.DeleteItem(hItem);
	}
	ReBuildTalkWindow();
	n = m_listTalkWindow.GetCount();
}

void CTalkModifyDlg::OnButtonMoveItemUp() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelectedItem = m_treeTalk.GetSelectedItem();
	if(hSelectedItem)
	{
		HTREEITEM hLastLastItem = NULL; 
		HTREEITEM hParent = m_treeTalk.GetParentItem(hSelectedItem);
		HTREEITEM hLastItem = m_treeTalk.GetPrevSiblingItem(hSelectedItem);
		if(hLastItem) 
			hLastLastItem = m_treeTalk.GetPrevSiblingItem(hLastItem);
		if(hLastLastItem==NULL) hLastLastItem = TVI_FIRST;
		HTREEITEM hNewItem =  m_treeTalk.CopyBranch(hSelectedItem,hParent,hLastLastItem);
		m_treeTalk.DeleteItem(hSelectedItem);
		m_treeTalk.SelectItem(hNewItem);
	}
	ReBuildTalkWindow();
}

void CTalkModifyDlg::OnButtonMoveItemDown() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelectedItem = m_treeTalk.GetSelectedItem();
	if(hSelectedItem)
	{
		HTREEITEM hParent = m_treeTalk.GetParentItem(hSelectedItem);
		HTREEITEM hNextItem = m_treeTalk.GetNextSiblingItem(hSelectedItem);
		HTREEITEM hNewItem =  m_treeTalk.CopyBranch(hSelectedItem,hParent,hNextItem);
		m_treeTalk.DeleteItem(hSelectedItem);
		m_treeTalk.SelectItem(hNewItem);
	}
	ReBuildTalkWindow();
}

bool CTalkModifyDlg::IsLeaf(CString name)
{
	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW * pPt = (TALK_WINDOW *)m_listTalkWindow.GetNext(pos);
		if(strcmp(name,pPt->GetName())==0)
			return false;
	}
	return true;
}

void CTalkModifyDlg::OnModifyTalkWindow() 
{
	// TODO: Add your command handler code here
	
	CTreeCtrl *pCtrl = (CTreeCtrl *)GetDlgItem(IDC_TREE_TALK);
	HTREEITEM item = m_treeTalk.GetSelectedItem();
	if(item == NULL) return;
	DWORD ptr = m_treeTalk.GetItemData(item);
	CString itemTxt;
	itemTxt.Format("%s",(char*)ptr);
	TALK_WINDOW *pParent = GetWindow(itemTxt);
	TALK_OPTION *pTO = NULL;
	
	CTalkCreateDlg dlg;
	dlg.m_bModified = true;
	if(pParent==NULL)
	{//
		 pTO = GetOption(itemTxt);
		dlg.m_dwServiceID = pTO->dwID;
		dlg.m_bWindow = false;
		dlg.m_strCommandName = pTO->strCommand;
		dlg.m_strName = pTO->strName;
		dlg.m_dwServiceLinkID = pTO->dwLinkID;
		//dlg.m_dwParams = pTO->dwParams;
	}else
	{
		pTO = GetOption(pParent->GetID());
		dlg.m_dwID = pParent->GetID();
		dlg.m_bWindow = true;
		dlg.m_strDesc = pParent->GetDescribe();
		dlg.m_strName = pParent->GetName();
		dlg.m_strCommandName = pParent->GetCommand();
		if(pTO)
		{
			dlg.m_dwServiceLinkID = pTO->dwLinkID;
			//dlg.m_dwParams = pTO->dwParams;
		}else
		{
			dlg.m_dwServiceLinkID = 0;
		}
	}

	if (m_bPreset)
	{
		dlg.m_bPreset = true;
		dlg.m_dwSvrToSel = m_dwSvrToSel;
		dlg.m_dwParamToSel = m_dwParamToSel;
		dlg.m_dwParamToSel2 = m_dwParamToSel2;
	}

	if(dlg.DoModal()==IDOK)
	{
		
		if(dlg.m_bWindow)
		{
			pParent->SetDescribe(AString(dlg.m_strDesc));
			pParent->SetCommand(AString(dlg.m_strCommandName));
			if(pTO) 
			{
				pTO->dwLinkID = 0;
				//pTO->dwParams = 0;
			}
		}else
		{
			if(pTO) 
			{
				pTO->dwID = dlg.m_dwServiceID;
				pTO->dwLinkID = dlg.m_dwServiceLinkID;
				//pTO->dwParams = dlg.m_dwParams;
			}
		}
		if(pTO) 
		{
			pTO->strCommand = dlg.m_strCommandName;
			m_treeTalk.SetItemText(item,pTO->strCommand);
		}
	}

	CWnd *pWnd  = GetDlgItem(IDC_EDIT_TALK_NAME);
	if(pWnd) pWnd->SetFocus();
}

void CTalkModifyDlg::OnButtonTalkTest() 
{
#ifndef TASK_TEMPL_EDITOR
	CTestDlg dlg;
	dlg.Init(&m_listTalkWindow,(TALK_WINDOW*)m_listTalkWindow.GetHead());
	dlg.DoModal();
#endif
}

void CTalkModifyDlg::LoadTalkFromTalkProc(talk_proc* talk)
{
	init_talk = talk;
	m_dwID = talk->id_talk;
	int op_id = 0;

	for (int i = 0; i < talk->num_window; i++)
	{
		TALK_WINDOW* win = new TALK_WINDOW;
		const talk_proc::window* talk_win = &talk->windows[i];

		m_IDGenerator.AddID(talk_win->id);
		win->SetID(talk_win->id);
		AString strName;
		strName.Format("window_%d", talk_win->id);
		win->SetName(strName);
		win->SetDescribe(CSafeString(talk_win->talk_text).GetAnsi());
		win->SetCommand("");
		m_listTalkWindow.AddTail(win);

		for (int j = 0; j < talk_win->num_option; j++)
		{
			TALK_OPTION* op = new TALK_OPTION;
			const talk_proc::option* talk_op = &talk_win->options[j];

			op->dwID = talk_op->id;
			op->dwLinkID = talk_op->param;
			op->strCommand = CSafeString(talk_op->text).GetAnsi();
			AString strName;
			strName.Format("option_%d", op_id++);
			strcpy(op->strName, strName);
			win->AddChild(op);
		}
	}

	POSITION pos = m_listTalkWindow.GetHeadPosition();
	while (pos)
	{
		TALK_WINDOW* win = (TALK_WINDOW*)m_listTalkWindow.GetNext(pos);
		TALK_OPTION* op = GetOption(win->GetID());

		if (op) win->SetCommand(op->strCommand);
		else win->SetCommand(CSafeString(talk->text).GetAnsi());
	}
}

bool CTalkModifyDlg::ExportData(talk_proc* pData, const char* szTalkFile, unsigned int uID)
{
	if (szTalkFile)
	{
		m_strPathName = szTalkFile;
		if (!LoadTalk()) return false;
	}

	if (m_listTalkWindow.GetCount() == 0)
	{
		ReleaseWindow();
		return false;
	}

	pData->id_talk		= uID;
	pData->num_window	= m_listTalkWindow.GetCount();
	delete[] pData->windows;
	pData->windows		= new talk_proc::window[pData->num_window];

	memset(pData->windows, 0, sizeof(talk_proc::window) * pData->num_window);

	POSITION pos = m_listTalkWindow.GetHeadPosition();
	TALK_WINDOW* p = (TALK_WINDOW*)m_listTalkWindow.GetNext(pos);

	talk_proc::window* pRoot = pData->windows;

	wcsncpy(
		pData->text,
		CSafeString(p->GetCommand()),
		sizeof(pData->text) / sizeof(namechar) - 1);

	FillWindowData(pRoot, p, -1);

	if (pRoot != pData->windows + pData->num_window - 1)
	{
		int nActual = pRoot - pData->windows + 1;
		CString str;
		str.Format("Error, wrong child window num = %d!", nActual);
		AfxMessageBox(str, MB_ICONSTOP);
		pData->num_window = nActual;
	}

	ReleaseWindow();
	return true;
}

void CTalkModifyDlg::FillWindowData(talk_proc::window*& pData, TALK_WINDOW* pW, unsigned int uParentID)
{
	pData->id				= pW->GetID();
	pData->id_parent		= uParentID;

	CSafeString strName(pW->GetDescribe());
	pData->talk_text_len	= wcslen(strName) + 1;
	pData->talk_text		= new namechar[pData->talk_text_len];
	wcscpy(pData->talk_text, strName);

	pData->num_option		= pW->GetChildNum();
	pData->options			= new talk_proc::option[pData->num_option];
	memset(pData->options, 0, sizeof(talk_proc::option) * pData->num_option);

	int i(0);
	for (i = 0; i < pW->GetChildNum(); i++)
	{
		TALK_OPTION* pOption	= pW->GetChild(i);
		pData->options[i].id	= pOption->dwID;
		pData->options[i].param	= pOption->dwLinkID;
		wcsncpy(
			pData->options[i].text,
			CSafeString(pOption->strCommand),
			sizeof(pData->options[i].text) / sizeof(namechar) - 1);
	}

	for (i = 0; i < pW->GetChildNum(); i++)
	{
		TALK_OPTION* pOption = pW->GetChild(i);
		if ((pOption->dwID & 0xC0000000) == 0x80000000) continue;

		TALK_WINDOW* pChild = GetWindow(pOption->dwID);

		if (!pChild)
		{
			AfxMessageBox("Error, cant find child window!", MB_ICONSTOP);
			continue;
		}

		pData++;
		FillWindowData(pData, pChild, pW->GetID());
	}
}
