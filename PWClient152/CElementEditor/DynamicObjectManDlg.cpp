// DynamicObjectManDlg.cpp : implementation file
//

#include "Global.h"
#include "elementeditor.h"
#include "DynamicObjectManDlg.h"
#include "DynamicObjectDlg.h"
#include "VssOperation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void InitDynamicObj(DYNAMIC_OBJECT * pObj)
{
	pObj->id = 0;
	strcpy(pObj->szName,"");
	strcpy(pObj->szObjectPath,"");
}


CDynamicObjectMan g_DynamicObjectMan;
/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectManDlg dialog


CDynamicObjectManDlg::CDynamicObjectManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicObjectManDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDynamicObjectManDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_uMaxID = 0;
	m_bReadOnly = false;
	m_bChanged = false;
}


void CDynamicObjectManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDynamicObjectManDlg)
	DDX_Control(pDX, IDC_STATIC_FLAG, m_FlagCtrl);
	DDX_Control(pDX, IDC_LIST_DYNAMIC_OBJECT, m_listObject);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDynamicObjectManDlg, CDialog)
	//{{AFX_MSG_MAP(CDynamicObjectManDlg)
	ON_BN_CLICKED(ID_CHECK_OUT, OnCheckOut)
	ON_BN_CLICKED(ID_CHECK_IN, OnCheckIn)
	ON_BN_CLICKED(ID_ADD_NEWOBJECT, OnAddNewobject)
	ON_BN_CLICKED(ID_DEL_OBJECT, OnDelObject)
	ON_BN_CLICKED(ID_EXPORT, OnExport)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DYNAMIC_OBJECT, OnDblclkListDynamicObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectManDlg message handlers

BOOL CDynamicObjectManDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_IconWriteable =LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_WRITEABLE));
	m_IconReadOnly = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_READONLY));
	// TODO: Add extra initialization here
	m_listObject.SetExtendedStyle(m_listObject.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_listObject.InsertColumn(0, "ID", LVCFMT_LEFT, 80);
	m_listObject.InsertColumn(1, "名字", LVCFMT_LEFT, 100);
	m_listObject.InsertColumn(2, "对应文件", LVCFMT_LEFT, 350);
	
	if(g_VSS.IsCheckOutStatus("ResTable\\DynamicObject.dat"))
	{
		m_bReadOnly = false;
		m_FlagCtrl.SetIcon(m_IconWriteable);
	}else 
	{
		m_FlagCtrl.SetIcon(m_IconReadOnly);
		m_bReadOnly = true;
	}
	if(!Load()) MessageBox("加载动态对象文件数据失败！");
	FreshObjectList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDynamicObjectManDlg::OnCheckOut() 
{
	// TODO: Add your control notification handler code here
	g_VSS.SetProjectPath("ResTable");
	g_VSS.CheckOutFile("ResTable\\DynamicObject.dat");
	CString rc = g_VSS.GetLastCommandResult();
	if(g_VSS.IsCheckOutStatus("ResTable\\DynamicObject.dat"))
	{
		m_bReadOnly = false;
		m_FlagCtrl.SetIcon(m_IconWriteable);
		if(!Load()) MessageBox("加载动态对象文件(ResTable\\DynamicObject.dat)失败！");
		g_DynamicObjectMan.Load();
		FreshObjectList();
	}else
	{
		m_bReadOnly = true;
		m_FlagCtrl.SetIcon(m_IconReadOnly);
		MessageBox(rc);
	}
}

void CDynamicObjectManDlg::OnCheckIn() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	if(!Save()) 
	{
		MessageBox("保存数据(ResTable\\DynamicObject.dat)失败!");
		return;
	}
	g_VSS.SetProjectPath("ResTable");
	g_VSS.CheckInFile("DynamicObject.dat");
	CString rc = g_VSS.GetLastCommandResult();
	if(g_VSS.IsCheckOutStatus("ResTable\\DynamicObject.dat"))
	{
		m_bReadOnly = false;
		m_FlagCtrl.SetIcon(m_IconWriteable);
		MessageBox(rc);
	}else 
	{
		m_FlagCtrl.SetIcon(m_IconReadOnly);
		m_bReadOnly = true;
	}
	g_DynamicObjectMan.Load();
}

void CDynamicObjectManDlg::OnAddNewobject() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	CDynamicObjectDlg dlg;
	if(IDOK == dlg.DoModal())
	{
		if(IsObjPathExsit(&dlg.m_Object))
		{
			CString temp;
			temp.Format("路径指向：%s 的动态对象已经存在!",dlg.m_Object.szObjectPath);
			MessageBox(temp);
			return;
		}
		dlg.m_Object.id = ++m_uMaxID;
		m_listDynamicObject.push_back(dlg.m_Object);
		m_bChanged = true;
		FreshObjectList();
	}
}

bool CDynamicObjectManDlg::IsObjPathExsit(DYNAMIC_OBJECT* pObj)
{
	for( int i = 0; i < m_listDynamicObject.size(); ++i)
	{
		if(stricmp(m_listDynamicObject[i].szObjectPath,pObj->szObjectPath)==0)
			return true;
	}
	return false;
}

void CDynamicObjectManDlg::FreshObjectList()
{
	m_listObject.DeleteAllItems();
	int index = 0;
	for( int i = 0; i < m_listDynamicObject.size(); ++i)
	{
		CString temp;
		temp.Format("%d",m_listDynamicObject[i].id);
		int sel = m_listObject.InsertItem(index++,temp);
		m_listObject.SetItemText(sel, 1, m_listDynamicObject[i].szName);
		m_listObject.SetItemText(sel, 2, m_listDynamicObject[i].szObjectPath);
	}
}

void CDynamicObjectManDlg::OnDelObject() 
{
	// TODO: Add your control notification handler code here
	if(m_bReadOnly) return;
	POSITION pos = m_listObject.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	
	CString msg("你确认要删除下列资源:\n\n");
	while (pos)
	{
		int nItem = m_listObject.GetNextSelectedItem(pos);
		CString temp = m_listObject.GetItemText(nItem,2);
		msg +=temp;
		msg +="\n";
	}
	if(IDYES != AfxMessageBox(msg,MB_YESNO|MB_ICONQUESTION)) return;
	
	pos = m_listObject.GetFirstSelectedItemPosition();
	while(pos)
	{
		int nItem = m_listObject.GetNextSelectedItem(pos);
		CString temp = m_listObject.GetItemText(nItem,0);
		unsigned int id = (unsigned int)atof(temp);
		DelDynamicObj(id);
	}
	m_bChanged = true; 
	FreshObjectList();
}

void CDynamicObjectManDlg::DelDynamicObj( unsigned int id)
{
	abase::vector<DYNAMIC_OBJECT>::iterator it = m_listDynamicObject.begin();
	for( it; it != m_listDynamicObject.end(); it++)
	{
		if((*it).id == id)
		{
			m_listDynamicObject.erase(it);
			return;
		}
	}
}

void CDynamicObjectManDlg::GetDynamicObj(unsigned int id, DYNAMIC_OBJECT &obj)
{
	abase::vector<DYNAMIC_OBJECT>::iterator it = m_listDynamicObject.begin();
	for( it; it != m_listDynamicObject.end(); it++)
	{
		if((*it).id == id)
		{
			obj.id = (*it).id;
			strcpy(obj.szName,(*it).szName);
			strcpy(obj.szObjectPath,(*it).szObjectPath);
			return;
		}
	}
	InitDynamicObj(&obj);
}

void CDynamicObjectManDlg::SetDynamicObj( unsigned int id, char * szName, char * szPath)
{
	abase::vector<DYNAMIC_OBJECT>::iterator it = m_listDynamicObject.begin();
	for( it; it != m_listDynamicObject.end(); it++)
	{
		if((*it).id == id)
		{
			strcpy((*it).szName,szName);
			strcpy((*it).szObjectPath,szPath);
			return;
		}
	}
}

void CDynamicObjectManDlg::OnExport() 
{
	// TODO: Add your control notification handler code here
	if(m_listDynamicObject.size() < 1) return;
	
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
	sprintf(szPath,"%s\\DynamicObjects.data",szPath);
	if(!Export(szPath))
		MessageBox("数据输出失败!");
}

void CDynamicObjectManDlg::OnOK()
{
	if(m_bChanged && !m_bReadOnly)
	{
		if(IDYES==MessageBox("数据已经改变，你需要存盘吗？","询问",MB_YESNO | MB_ICONQUESTION))
		{
			Save();
			g_DynamicObjectMan.Load();
		}
	}
	CDialog::OnOK();
}
void CDynamicObjectManDlg::OnCancel()
{
	CDialog::OnCancel();
}

bool CDynamicObjectManDlg::Save()
{
	FILE *pFile = fopen("ResTable\\DynamicObject.dat","wb");
	if(pFile==NULL) return false;
	DWORD dwVersion = DYNAMIC_VERSION;
	fwrite(&dwVersion,sizeof(DWORD),1,pFile);
	int size = m_listDynamicObject.size();
	fwrite(&size,sizeof(int),1,pFile);
	for( int i = 0; i < size; ++i)
	{
		fwrite(&m_listDynamicObject[i].id,sizeof(unsigned int),1,pFile);
		fwrite(&m_listDynamicObject[i].szName,128,1,pFile);
		fwrite(&m_listDynamicObject[i].szObjectPath,256,1,pFile);
	}
	fclose(pFile);
	return true;
}

bool CDynamicObjectManDlg::Load()
{
	m_listDynamicObject.clear();
	
	FILE *pFile = fopen("ResTable\\DynamicObject.dat","rb");
	if(pFile==NULL) return false;
	DWORD dwVersion;
	fread(&dwVersion,sizeof(DWORD),1,pFile);
	if(dwVersion==1)
	{
		int size = 0;
		fread(&size,sizeof(DWORD),1,pFile);
		for( int i = 0; i < size; ++i)
		{
			DYNAMIC_OBJECT obj;
			fread(&obj.id,sizeof(unsigned int),1,pFile);
			if(obj.id > m_uMaxID) m_uMaxID = obj.id;
			fread(&obj.szName,128,1,pFile);
			fread(&obj.szObjectPath,256,1,pFile);
			m_listDynamicObject.push_back(obj);
		}
	}
	fclose(pFile);
	return true;
}

#define	PATHMAP_IDENTIFY		(('P'<<24) | ('M'<<16) | ('I'<<8) | 'D')
bool CDynamicObjectManDlg::Export(const char* szPath)
{
	FILE *pFile = fopen(szPath,"wb");
	if(pFile==NULL) return false;
	
	unsigned long Identify = PATHMAP_IDENTIFY;
	fwrite(&Identify,sizeof(unsigned long),1,pFile);
	
	unsigned long size = m_listDynamicObject.size();
	fwrite(&size,sizeof(unsigned long),1,pFile);
	for( int i = 0; i < size; ++i)
	{
		unsigned long id = m_listDynamicObject[i].id;
		fwrite(&id,sizeof(unsigned long),1,pFile);
		unsigned int sLen = strlen(m_listDynamicObject[i].szObjectPath);
		fwrite(&sLen,sizeof(unsigned int),1,pFile);
		fwrite(&m_listDynamicObject[i].szObjectPath,sLen,1,pFile);
	}

	fclose(pFile);
	return true;
}

void CDynamicObjectManDlg::OnDblclkListDynamicObject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POINT pt;
	GetCursorPos(&pt);
	m_listObject.ScreenToClient(&pt);
	int sel = m_listObject.HitTest(pt);
	if(sel!=-1)
	{
		CString temp = m_listObject.GetItemText(sel,0);
		unsigned int id = (unsigned int)atof(temp);
		DYNAMIC_OBJECT obj;
		GetDynamicObj(id, obj);
		CDynamicObjectDlg dlg;
		dlg.m_bReadOnly = m_bReadOnly;
		dlg.m_Object.id = obj.id;
		strcpy(dlg.m_Object.szName,obj.szName); 
		strcpy(dlg.m_Object.szObjectPath,obj.szObjectPath);
		if(IDOK == dlg.DoModal())
		{
			if(stricmp(obj.szName,dlg.m_Object.szName) == 0 && stricmp(obj.szObjectPath,dlg.m_Object.szObjectPath) == 0)
				return;

			if(stricmp(obj.szObjectPath,dlg.m_Object.szObjectPath) != 0)
			{
				if(IsObjPathExsit(&dlg.m_Object))
				{
					temp.Format("路径指向：%s 的动态对象已经存在!",dlg.m_Object.szObjectPath);
					MessageBox(temp);
					return;	
				}
			}

			SetDynamicObj(id,dlg.m_Object.szName,dlg.m_Object.szObjectPath);
			m_bChanged = true;
			FreshObjectList();
			m_listObject.SetItemState(sel,LVIS_SELECTED, LVIS_SELECTED);
		}
	}
	*pResult = 0;
}
