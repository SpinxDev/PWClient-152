// WorkSpaceSetDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "WorkSpaceSetDlg.h"
#include "CreateWorkspaceDlg.h"
#include "VssOperation.h"
#include "AF.h"
#include "EL_RandomMapInfo.h"
#include <vector>

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceSetDlg dialog
const int g_randMapVersion = 3;
// version 3: 添加起始点相对位置

CWorkSpaceSetDlg::CWorkSpaceSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkSpaceSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWorkSpaceSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWorkSpaceSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkSpaceSetDlg)
	DDX_Control(pDX, IDC_LIST_WORKSPACE, m_listWorkSpace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkSpaceSetDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkSpaceSetDlg)
	ON_BN_CLICKED(IDC_BUTTON_NEW_WORKSPACE, OnButtonNewWorkspace)
	ON_BN_CLICKED(IDC_BUTTON_DEL_WORKSPACE, OnButtonDelWorkspace)
	ON_LBN_DBLCLK(IDC_LIST_WORKSPACE, OnDblclkListWorkspace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceSetDlg message handlers

void CWorkSpaceSetDlg::OnButtonNewWorkspace() 
{
	// TODO: Add your control notification handler code here
	std::vector<CString> list;
	CCreateWorkspaceDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		char szDst[256];
		sprintf(szDst,"%sEditMaps\\%s",g_szWorkDir,dlg.m_strNewWorkspace);
		if(!CreateDirectory(szDst, NULL))
		{
			MessageBox("创建工作空间文件夹失败!");
			return;
		}
		sprintf(szDst,"%sEditMaps\\%s\\projlist.dat",g_szWorkDir,dlg.m_strNewWorkspace);
		FILE *pFile = fopen(szDst,"w");
		if(pFile==NULL) 
		{
			MessageBox("创建列表文件失败!");
			return;
		}
		fclose(pFile);

		//////////////////////////////////////////////////////////////////////////
		if (dlg.m_bRandomMap == TRUE)
		{
			sprintf(szDst,"%sEditMaps\\%s\\randommap.dat",g_szWorkDir,dlg.m_strNewWorkspace);
		
			if(!CRandMapProp::CreateRandomMapFile(szDst))
			{
				MessageBox("创建随机地图失败!");
				return;
			}
		}
		//////////////////////////////////////////////////////////////////////////

		sprintf(szDst,"%sEditMaps\\%s\\EditorTempMap",g_szWorkDir,dlg.m_strNewWorkspace);
		if(!CreateDirectory(szDst, NULL))
		{
			MessageBox("创建工作空间文件夹失败!");
			return;
		}

		sprintf(szDst,"%s\\MapsRenderData\\%s",g_Configs.szServerPath,dlg.m_strNewWorkspace);
		if(!CreateDirectory(szDst, NULL))
		{
			MessageBox("在服务器上创建工作空间文件夹失败!");
			return;
		}
		
		if(!AUX_CheckOutWorkspace())
		{
			MessageBox("不能从服务器上取出workspace.ws文件，请检测机器是否跟VSS服务器连接正常!");
			return;
		}

		AScriptFile File;
		if (!File.Open("EditMaps\\workspace.ws"))
		{//Clear current project list
			MessageBox("Failed to open EditMaps\\workspace.ws");
			goto fail;
		}
		
		while (File.GetNextToken(true))
		{
			list.push_back(CString(File.m_szToken));
		}
		File.Close();
		list.push_back(dlg.m_strNewWorkspace);
		
		FILE* pf = fopen("EditMaps\\workspace.ws","w");
		if(pf==NULL) goto fail;
		for(int i = 0; i < list.size(); i++)
		{
			fprintf(pf,list[i] + "\n");
		}
		fclose(pf);

		if(!AUX_CheckInWorkspace()) 
		{
			MessageBox("工作空间文件(workspace.ws)不能正常放回服务器！");
			goto fail;
		}
		
		sprintf(szDst,"%s",dlg.m_strNewWorkspace);
		g_VSS.SetProjectPath("EditMaps");
		g_VSS.CreateProject(szDst);
		g_VSS.SetProjectPath(szDst);
		sprintf(szDst,"EditMaps\\%s",dlg.m_strNewWorkspace);
		g_VSS.SetProjectPath(szDst);
		sprintf(szDst,"%sEditMaps\\%s\\projlist.dat",g_szWorkDir,dlg.m_strNewWorkspace);
		g_VSS.AddFile(szDst);
		
		////
		sprintf(szDst,"%sEditMaps\\%s\\randommap.dat",g_szWorkDir,dlg.m_strNewWorkspace);
		g_VSS.AddFile(szDst);
		////

		FreshList();
	}
	list.clear();
	MessageBox("创建成功!");
	return;
fail:
	AUX_UndoCheckOutWorkspace();
	list.clear();
}

void CWorkSpaceSetDlg::OnButtonDelWorkspace() 
{
	// TODO: Add your control notification handler code here
	
}

void CWorkSpaceSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	int i = m_listWorkSpace.GetCurSel();
	if(i != -1)
	{
		CString text;
		m_listWorkSpace.GetText(i,text);
		strcpy(g_Configs.szCurProjectName,text);
		sprintf(g_szEditMapDir, "EditMaps\\%s",g_Configs.szCurProjectName);
	}
	CDialog::OnOK();
}

void CWorkSpaceSetDlg::FreshList()
{
	// TODO: Add extra initialization here
	AScriptFile File;
	if (File.Open("EditMaps\\workspace.ws"))
	{//Clear current project list
		int num = m_listWorkSpace.GetCount();
		for( int i = 0; i < num; i++)
			m_listWorkSpace.DeleteString(0);
		while (File.GetNextToken(true))
		{
			m_listWorkSpace.AddString(File.m_szToken);
		}
		File.Close();
		
		CString temp;
		num = m_listWorkSpace.GetCount();
		for(i = 0; i < num; i++)
		{
			m_listWorkSpace.GetText(i,temp);
			if(strcmp(temp,g_Configs.szCurProjectName)==0)
			{
				m_listWorkSpace.SetCurSel(i);
				break;
			}
		}
	}
}

BOOL CWorkSpaceSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString str;
	str.Format("%sEditMaps",g_szWorkDir);
	g_VSS.SetWorkFolder(str);
	g_VSS.SetProjectPath("EditMaps");
	g_VSS.GetFile("EditMaps\\workspace.ws");
	FreshList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWorkSpaceSetDlg::OnDblclkListWorkspace() 
{
	OnOK();
}
