// MusicSelDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "MusicSelDlg.h"
#include "CommonFileDlg.h"
#include "Render.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CMusicSelDlg dialog


CMusicSelDlg::CMusicSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMusicSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMusicSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSceneBoxArea = NULL;
}


void CMusicSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMusicSelDlg)
	DDX_Control(pDX, IDC_MUSIC_LIST, m_listMusic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMusicSelDlg, CDialog)
	//{{AFX_MSG_MAP(CMusicSelDlg)
	ON_BN_CLICKED(ID_MUSIC_ADD, OnMusicAdd)
	ON_BN_CLICKED(ID_MUSIC_DEL, OnMusicDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMusicSelDlg message handlers

BOOL CMusicSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	FreshList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMusicSelDlg::OnMusicAdd() 
{
	// TODO: Add your control notification handler code here
	if(m_pSceneBoxArea==NULL) return;
	
	CCommonFileDlg dlg(g_Render.GetA3DEngine(),"Music", "*.mp3",this);
	if(IDOK == dlg.DoModal())
	{
		AString *pMusic = new AString(dlg.GetFullFileName());
		m_pSceneBoxArea->AddMusic(pMusic);
		FreshList();
	}
}

void CMusicSelDlg::OnMusicDel() 
{
	// TODO: Add your control notification handler code here
	if(m_pSceneBoxArea == NULL) return;

	int sel = m_listMusic.GetCurSel();
	if(sel==-1) return;
	CString temp;
	m_listMusic.GetText(sel,temp);
	AString txt(temp);
	m_pSceneBoxArea->DelMusic(&txt);
	
	FreshList();
}

void CMusicSelDlg::FreshList()
{
	if(m_pSceneBoxArea==NULL) return;
	
	int n = m_listMusic.GetCount();
	for( int i = 0; i < n; i++ )
		m_listMusic.DeleteString(0);

	n = m_pSceneBoxArea->GetMusicNum();
	for( i = 0; i < n; i++ )
		m_listMusic.AddString(m_pSceneBoxArea->GetMusic(i));
}
