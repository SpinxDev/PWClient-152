// DlgNewMap.cpp : implementation file
//

#include "Global.h"
#include "DlgNewMap.h"
#include "EL_RandomMapInfo.h"
//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgNewMap dialog


CDlgNewMap::CDlgNewMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewMap)
	m_strProjName = _T("");
	m_fMaxHeight = 0.0f;
	m_fMinHeight = 0.0f;
	m_fTileSize = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgNewMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewMap)
	DDX_Text(pDX, IDC_EDIT_PROJNAME, m_strProjName);
	DDX_Text(pDX, IDC_EDIT_MAXHEIGHT, m_fMaxHeight);
	DDX_Text(pDX, IDC_EDIT_MINHEIGHT, m_fMinHeight);
	DDX_Text(pDX, IDC_EDIT_TILESIZE, m_fTileSize);
	DDV_MinMaxFloat(pDX, m_fTileSize, 1.e-003f, 10000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewMap, CDialog)
	//{{AFX_MSG_MAP(CDlgNewMap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewMap message handlers

void CDlgNewMap::SetData(const DATA& Data)
{
	m_Data.bNewMap		= Data.bNewMap;
	m_Data.fTileSize	= Data.fTileSize;
	m_Data.fMinHeight	= Data.fMinHeight;
	m_Data.fMaxHeight	= Data.fMaxHeight;
	m_Data.strProjName	= Data.strProjName;
}

void CDlgNewMap::GetData(DATA& Data)
{
	Data.fTileSize		= m_Data.fTileSize;
	Data.fMinHeight		= m_Data.fMinHeight;
	Data.fMaxHeight		= m_Data.fMaxHeight;
	Data.strProjName	= m_Data.strProjName;
}

BOOL CDlgNewMap::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_fTileSize		= m_Data.fTileSize;
	m_fMinHeight	= m_Data.fMinHeight;
	m_fMaxHeight	= m_Data.fMaxHeight;
	m_strProjName	= m_Data.strProjName;

	if (!m_Data.bNewMap)
	{
		GetDlgItem(IDC_EDIT_PROJNAME)->EnableWindow(FALSE);
		SetWindowText("修改地形属性");
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNewMap::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	//	Create project directory
	if (m_Data.bNewMap && !AUX_CreateProjectDir(m_hWnd, m_strProjName))
		return;
	m_Data.fTileSize	= m_fTileSize;
	m_Data.fMinHeight	= m_fMinHeight;	
	m_Data.fMaxHeight	= m_fMaxHeight;
	m_Data.strProjName	= m_strProjName;
	
	if (AUX_IsRandomMapWorkSpace())
	{
		if(!CRandMapProp::OnCreateOneGrid(m_Data.strProjName))
			return;
	}

	if(!AUX_CheckOutProjList())
	{
		MessageBox("CDlgNewMap::OnOK(),Check out projlist.dat failed!");
		return;
	}
	char szProj[256];
	sprintf(szProj,"%s\\ProjList.dat",g_szEditMapDir);
	FILE* fp = fopen(szProj, "a");
	if(fp)
	{
		fprintf(fp,"%s\n",m_Data.strProjName);
		fclose(fp);
	}
	if(!AUX_CheckInProjList())
	{
		MessageBox("CDlgNewMap::OnOK(),Check in projlist.dat failed!");
		return;
	}

	CDialog::OnOK();
}


