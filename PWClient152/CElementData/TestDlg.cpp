// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "A3D.h"
#include "TestDlg.h"


#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
	m_strWndTxt = _T("");
	//}}AFX_DATA_INIT
}


void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Control(pDX, IDC_LIST_COMMAND, m_ListBoxCommand);
	DDX_Text(pDX, IDC_EDIT_WNDTXT, m_strWndTxt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_LIST_COMMAND, OnDblclkListCommand)
	ON_BN_CLICKED(ID_RETURN, OnReturn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

LRESULT CTestDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::WindowProc(message, wParam, lParam);
}

bool  CTestDlg::Init(CPtrList *pDataList,TALK_WINDOW *pTWnd)
{
	ASSERT(pDataList);
	ASSERT(pTWnd);
	m_pDataList = pDataList;
	m_pTWnd = pTWnd;
	return true;
}

void CTestDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	POSITION pos = m_listButton.GetHeadPosition();
	while(pos)
	{
		CButton *ptemp = (CButton *)m_listButton.GetNext(pos);
		delete ptemp;
	}
	m_listButton.RemoveAll();
}

BOOL CTestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_strWndTxt = m_pTWnd->GetDescribe();
	UpdateData(false);
	int num = m_pTWnd->GetChildNum();
	for(int i = 0; i< num; i++)
	{
		TALK_OPTION *pTO = m_pTWnd->GetChild(i);
		m_ListBoxCommand.AddString(pTO->strCommand);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CTestDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}



void CTestDlg::OnDblclkListCommand() 
{
	// TODO: Add your control notification handler code here
	int sel = m_ListBoxCommand.GetCurSel();
	if(sel==-1) return;
	CString txt;
	m_ListBoxCommand.GetText(sel,txt);

	if(m_pTWnd)
	{
		int num = m_pTWnd->GetChildNum();
		for(int i = 0; i< num; i++)
		{
			TALK_OPTION *pTO = m_pTWnd->GetChild(i);
			if(strcmp(pTO->strCommand,txt)==0)
			{
				DWORD r = pTO->dwID & 0x80000000;
				if(r==0)
				{
					m_pTWnd = FindWnd(pTO->dwID);
					FreshList();
					UpdateWndTxt();	
				}else
				{
					r = pTO->dwID>>30;
					if( r == 2)
					{
						CString msg;
						msg.Format("执行(ID=%d)功能!",pTO->dwID);
						MessageBox(msg,"功能",MB_OK);
						
					}else if( r == 3)
					{
						TALK_WINDOW* wnd = FindWnd(pTO->dwID);
						if(wnd) MessageBox(wnd->GetDescribe(),"弹出窗口",MB_OK);
						else MessageBox("错误: 窗口不存在!");
					}
					
				}
				break;
			}
		}
	}
}

void CTestDlg::FreshList()
{
	int itemnum = m_ListBoxCommand.GetCount();
	int i(0);
	for (i=0;i < itemnum ;i++)
	{
		m_ListBoxCommand.DeleteString(0);
	}
		
	if(m_pTWnd==NULL) return;
	
	int num = m_pTWnd->GetChildNum();
	for(i = 0; i< num; i++)
	{
		TALK_OPTION *pTO = m_pTWnd->GetChild(i);
		m_ListBoxCommand.AddString(pTO->strCommand);
	}
}
void CTestDlg::UpdateWndTxt()
{
	if(m_pTWnd)
	m_strWndTxt = m_pTWnd->GetDescribe();
	UpdateData(false);
}

TALK_WINDOW *CTestDlg::FindWnd(DWORD id)
{
	POSITION pos = m_pDataList->GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW *pTemp = (TALK_WINDOW *)m_pDataList->GetNext(pos);
		if(pTemp->GetID() == id)
			return pTemp;
	}
	return NULL;
}

TALK_WINDOW *CTestDlg::GetParentWnd(DWORD id)
{
	POSITION pos = m_pDataList->GetHeadPosition();
	while(pos)
	{
		TALK_WINDOW *pTemp = (TALK_WINDOW *)m_pDataList->GetNext(pos);
		int num = pTemp->GetChildNum();
		for(int i = 0; i< num; i++)
		{
			TALK_OPTION *pTO = pTemp->GetChild(i);
			if(pTO->dwID == id)
				return pTemp;
		}
	}
	return NULL;
}

void CTestDlg::OnReturn() 
{
	// TODO: Add your control notification handler code here
	TALK_WINDOW *pTemp = GetParentWnd(m_pTWnd->GetID());
	if(pTemp == NULL)
	{
		MessageBox("已经是根接点,不能再返回!");
	}else
	{
		m_pTWnd = pTemp;
		FreshList();
		UpdateWndTxt();
	}
}
