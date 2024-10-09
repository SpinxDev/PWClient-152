// CloudSetPanel.cpp : implementation file
//

#include "Global.h"
#include "elementeditor.h"
#include "CloudSetPanel.h"
#include "MainFrm.h"
#include "ElementMap.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CCloudSetPanel dialog
CString cloud_types[4] = { "0:base","1:stratus","2:solid","3:wispy"};

CCloudSetPanel::CCloudSetPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CCloudSetPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCloudSetPanel)
	m_fDist = 0.0f;
	m_nNum = 0;
	m_fSize = 0.0f;
	m_fHeight1 = 0.0f;
	m_fHeight2 = 0.0f;
	m_fHeight3 = 0.0f;
	m_fHeight4 = 0.0f;
	m_fHeight5 = 0.0f;
	m_dwSpriteNum = 0;
	m_fGroupSize = 0.0f;
	//}}AFX_DATA_INIT
	m_clrWnd1.SetNotifyWindow(this);
	m_clrWnd2.SetNotifyWindow(this);
	m_clrWnd3.SetNotifyWindow(this);
	m_clrWnd4.SetNotifyWindow(this);
	m_clrWnd5.SetNotifyWindow(this);
}


void CCloudSetPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCloudSetPanel)
	DDX_Control(pDX, IDC_COMBO_CLOUD_GROUP, m_cbGroup);
	DDX_Control(pDX, IDC_STATIC_COLOR5, m_clrWnd5);
	DDX_Control(pDX, IDC_STATIC_COLOR4, m_clrWnd4);
	DDX_Control(pDX, IDC_STATIC_COLOR3, m_clrWnd3);
	DDX_Control(pDX, IDC_STATIC_COLOR2, m_clrWnd2);
	DDX_Control(pDX, IDC_STATIC_COLOR1, m_clrWnd1);
	DDX_Text(pDX, IDC_EDIT_SPRITE_DIST, m_fDist);
	DDV_MinMaxFloat(pDX, m_fDist, 5.f, 30.f);
	DDX_Text(pDX, IDC_EDIT_SPRITE_NUM, m_nNum);
	DDV_MinMaxInt(pDX, m_nNum, 250, 1000);
	DDX_Text(pDX, IDC_EDIT_SPRITE_SIZE, m_fSize);
	DDV_MinMaxFloat(pDX, m_fSize, 20.f, 110.f);
	DDX_Text(pDX, IDC_EDIT_HEIGHT1, m_fHeight1);
	DDV_MinMaxFloat(pDX, m_fHeight1, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT_HEIGHT2, m_fHeight2);
	DDV_MinMaxFloat(pDX, m_fHeight2, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT_HEIGHT3, m_fHeight3);
	DDV_MinMaxFloat(pDX, m_fHeight3, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT_HEIGHT4, m_fHeight4);
	DDV_MinMaxFloat(pDX, m_fHeight4, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT_HEIGHT5, m_fHeight5);
	DDV_MinMaxFloat(pDX, m_fHeight5, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT_GROUP_SPRITE_NUM, m_dwSpriteNum);
	DDV_MinMaxUInt(pDX, m_dwSpriteNum, 1, 200);
	DDX_Text(pDX, IDC_EDIT_GROUP_SPRITE_SIZE, m_fGroupSize);
	DDV_MinMaxFloat(pDX, m_fGroupSize, 20.f, 110.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCloudSetPanel, CDialog)
	//{{AFX_MSG_MAP(CCloudSetPanel)
	ON_BN_CLICKED(IDC_BUTTON_RECREATE_CLOUD, OnButtonRecreateCloud)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN_SPRITE_NUM, OnOutofmemorySpinSpriteNum)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN_SPRITE_SIZE, OnOutofmemorySpinSpriteSize)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN_SPRITE_DIST, OnOutofmemorySpinSpriteDist)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SPRITE_DIST, OnDeltaposSpinSpriteDist)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SPRITE_NUM, OnDeltaposSpinSpriteNum)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SPRITE_SIZE, OnDeltaposSpinSpriteSize)
	ON_BN_CLICKED(IDC_BUTTON_RECREATE_GROUP, OnButtonRecreateGroup)
	ON_CBN_SELCHANGE(IDC_COMBO_CLOUD_GROUP, OnSelchangeComboCloudGroup)
	ON_BN_CLICKED(IDC_BUTTON_RECREATE_ALLGROUP, OnButtonRecreateAllgroup)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELCOLCHANGED, OnColorChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCloudSetPanel message handlers

void CCloudSetPanel::OnButtonRecreateCloud() 
{
	// TODO: Add your control notification handler code here
	if(IDNO==AfxMessageBox("将要重新生成组数据，先前组数据将丢失!",MB_YESNO)) return;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap) 
	{
		
		CELCloudCreator* pCreator = pMap->GetCloudCreator();
		if(pCreator)
		{
			CloudCreatorParam *pParam = pCreator->GetCloudCreatorParam();
			UpdateData(true);
			pParam->fSpriteSize = m_fSize;
			pParam->nSpriteNum  = m_nNum;
			pParam->fCullDist   = m_fDist;
			pParam->nCloudType  = m_nType;
			for( int i = 0; i < 5; i ++)
			{
				pParam->color[i].r = sr[i];
				pParam->color[i].g = sg[i];
				pParam->color[i].b = sb[i];
			}
			pParam->height[0] = m_fHeight1;
			pParam->height[1] = m_fHeight2;
			pParam->height[2] = m_fHeight3;
			pParam->height[3] = m_fHeight4;
			pParam->height[4] = m_fHeight5;
			
			pCreator->SetCloudCreatorParam(pParam);
			pMap->RecreateCloud();

			abase::vector<CloudSprite>* pSprite = NULL;
			abase::vector<CloudGroup>* pGroup   = NULL;
			pCreator->GetCloudInfo(&pSprite,&pGroup);
			int num = m_cbGroup.GetCount();
			for( int n = 0; n < num ; n++)
				m_cbGroup.DeleteString(0);
			if(pGroup)
			{
				CString groupName;
				for(int n = 0; n < pGroup->size(); n++)
				{
					if((*pGroup)[n].vSprites.size()!=0)
					{
						groupName.Format("Group_%d",n);
						m_cbGroup.AddString(groupName);
					}
				}
				if(m_cbGroup.GetCount()>0) 
				{	
					m_cbGroup.SetCurSel(0);
					CString txt;
					m_cbGroup.GetLBText(0,txt);
					txt.Replace("Group_",0);
					int index = atof(txt);
					m_dwSpriteNum = (*pGroup)[index].vSprites.size(); 
					m_fGroupSize  = (*pGroup)[index].fSpriteSize;
					pMap->GetCloudRender()->SetGroupSelected(index);
					UpdateData(false);
				}
			}
		}
	}
}

void CCloudSetPanel::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
}

void CCloudSetPanel::UpdateCloudData()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CELCloudCreator *pCreator = pMap->GetCloudCreator();
		if(pCreator)
		{
			CloudCreatorParam *pParam = pCreator->GetCloudCreatorParam();
			m_nNum = pParam->nSpriteNum;
			m_fDist = pParam->fCullDist;
			m_fSize = pParam->fSpriteSize;
			
			sr[0] = pParam->color[0].r;
			sg[0] = pParam->color[0].g;
			sb[0] = pParam->color[0].b;

			sr[1] = pParam->color[1].r;
			sg[1] = pParam->color[1].g;
			sb[1] = pParam->color[1].b;

			sr[2] = pParam->color[2].r;
			sg[2] = pParam->color[2].g;
			sb[2] = pParam->color[2].b;

			sr[3] = pParam->color[3].r;
			sg[3] = pParam->color[3].g;
			sb[3] = pParam->color[3].b;

			sr[4] = pParam->color[4].r;
			sg[4] = pParam->color[4].g;
			sb[4] = pParam->color[4].b;

			m_clrWnd1.SetColor(RGB(sr[0]*255,sg[0]*255,sb[0]*255));
			m_clrWnd2.SetColor(RGB(sr[1]*255,sg[1]*255,sb[1]*255));
			m_clrWnd3.SetColor(RGB(sr[2]*255,sg[2]*255,sb[2]*255));
			m_clrWnd4.SetColor(RGB(sr[3]*255,sg[3]*255,sb[3]*255));
			m_clrWnd5.SetColor(RGB(sr[4]*255,sg[4]*255,sb[4]*255));

			m_fHeight1 = pParam->height[0];
			m_fHeight2 = pParam->height[1];
			m_fHeight3 = pParam->height[2];
			m_fHeight4 = pParam->height[3];
			m_fHeight5 = pParam->height[4];

			abase::vector<CloudSprite>* pSprite = NULL;
			abase::vector<CloudGroup>* pGroup   = NULL;
			pCreator->GetCloudInfo(&pSprite,&pGroup);
			int num = m_cbGroup.GetCount();
			for( int n = 0; n < num ; n++)
				m_cbGroup.DeleteString(0);
			if(pGroup)
			{
				CString groupName;
				for(int n = 0; n < pGroup->size(); n++)
				{
					if((*pGroup)[n].vSprites.size()!=0)
					{
						groupName.Format("Group_%d",n);
						m_cbGroup.AddString(groupName);
					}
				}
			}
			if(m_cbGroup.GetCount()>0) 
			{	
				m_cbGroup.SetCurSel(0);
				CString txt;
				m_cbGroup.GetLBText(0,txt);
				txt.Replace("Group_",0);
				int index = atof(txt);
				m_dwSpriteNum = (*pGroup)[index].vSprites.size(); 
				m_fGroupSize  = (*pGroup)[index].fSpriteSize;
				pMap->GetCloudRender()->SetGroupSelected(index);
			}

			UpdateData(false);
		}
	}
}

BOOL CCloudSetPanel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_clrWnd1.SetColor(RGB(255, 0, 0));
	m_clrWnd1.Redraw();
	m_clrWnd1.EnableWindow(TRUE);

	m_clrWnd2.SetColor(RGB(255, 0, 0));
	m_clrWnd2.Redraw();
	m_clrWnd2.EnableWindow(TRUE);


	m_clrWnd3.SetColor(RGB(255, 0, 0));
	m_clrWnd3.Redraw();
	m_clrWnd3.EnableWindow(TRUE);


	m_clrWnd4.SetColor(RGB(255, 0, 0));
	m_clrWnd4.Redraw();
	m_clrWnd4.EnableWindow(TRUE);

	m_clrWnd5.SetColor(RGB(255, 0, 0));
	m_clrWnd5.Redraw();
	m_clrWnd5.EnableWindow(TRUE);
	
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCloudSetPanel::OnOutofmemorySpinSpriteNum(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CCloudSetPanel::OnOutofmemorySpinSpriteSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CCloudSetPanel::OnOutofmemorySpinSpriteDist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CCloudSetPanel::OnDeltaposSpinSpriteDist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_fDist += -pNMUpDown->iDelta*1;
	if(m_fDist>30)
		m_fDist = 30;
	else if(m_fDist<5)
		m_fDist = 5;
	UpdateData(false);	
	*pResult = 0;
}

void CCloudSetPanel::OnDeltaposSpinSpriteNum(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_nNum += -pNMUpDown->iDelta*1;
	if(m_nNum>1000)
		m_nNum = 1000;
	else if(m_nNum<250)
		m_nNum = 250;
	UpdateData(false);	
	*pResult = 0;
}

void CCloudSetPanel::OnDeltaposSpinSpriteSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_fSize += -pNMUpDown->iDelta*1;
	if(m_fSize>110)
		m_fSize = 110;
	else if(m_fSize<20)
		m_fSize = 20;
	UpdateData(false);	
	*pResult = 0;
}

HRESULT CCloudSetPanel::OnColorChange(WPARAM wParam, LPARAM lParam)
{
	float r, g, b;

	
	if (wParam == (WPARAM)m_clrWnd1.m_hWnd)
	{
		DWORD dwCol = m_clrWnd1.GetColor();
		r = GetRValue(dwCol)/255.0f;
		g = GetGValue(dwCol)/255.0f;
		b = GetBValue(dwCol)/255.0f;
		sr[0] = r;
		sg[0] = g;
		sb[0] = b;
	}

	if (wParam == (WPARAM)m_clrWnd2.m_hWnd)
	{
		DWORD dwCol = m_clrWnd2.GetColor();
		r = GetRValue(dwCol)/255.0f;
		g = GetGValue(dwCol)/255.0f;
		b = GetBValue(dwCol)/255.0f;
		sr[1] = r;
		sg[1] = g;
		sb[1] = b;
	}

	if (wParam == (WPARAM)m_clrWnd3.m_hWnd)
	{
		DWORD dwCol = m_clrWnd3.GetColor();
		r = GetRValue(dwCol)/255.0f;
		g = GetGValue(dwCol)/255.0f;
		b = GetBValue(dwCol)/255.0f;
		sr[2] = r;
		sg[2] = g;
		sb[2] = b;
	}


	if (wParam == (WPARAM)m_clrWnd4.m_hWnd)
	{
		DWORD dwCol = m_clrWnd4.GetColor();
		r = GetRValue(dwCol)/255.0f;
		g = GetGValue(dwCol)/255.0f;
		b = GetBValue(dwCol)/255.0f;
		sr[3] = r;
		sg[3] = g;
		sb[3] = b;
	}


	if (wParam == (WPARAM)m_clrWnd5.m_hWnd)
	{
		DWORD dwCol = m_clrWnd5.GetColor();
		r = GetRValue(dwCol)/255.0f;
		g = GetGValue(dwCol)/255.0f;
		b = GetBValue(dwCol)/255.0f;
		sr[4] = r;
		sg[4] = g;
		sb[4] = b;
	}
	return TRUE;
}

void CCloudSetPanel::OnButtonRecreateGroup() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CELCloudCreator *pCreator = pMap->GetCloudCreator();
		if(pCreator)
		{
			int cur = m_cbGroup.GetCurSel();
			if(cur!=-1)
			{
				CString txt;
				m_cbGroup.GetLBText(cur,txt);
				txt.Replace("Group_",0);
				int index = atof(txt);
				UpdateData(true);
				pMap->RecreateCloudGroup(index,m_fGroupSize,m_dwSpriteNum);
			}
		}
	}
}

void CCloudSetPanel::OnSelchangeComboCloudGroup() 
{
	// TODO: Add your control notification handler code here
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CELCloudCreator *pCreator = pMap->GetCloudCreator();
		if(pCreator)
		{
			abase::vector<CloudSprite>* pSprite = NULL;
			abase::vector<CloudGroup>* pGroup   = NULL;
			pCreator->GetCloudInfo(&pSprite,&pGroup);
			if(m_cbGroup.GetCount()>0) 
			{	
				int cur = m_cbGroup.GetCurSel();
				CString txt;
				m_cbGroup.GetLBText(cur,txt);
				txt.Replace("Group_",0);
				int index = atof(txt);
				m_dwSpriteNum = (*pGroup)[index].vSprites.size(); 
				m_fGroupSize  = (*pGroup)[index].fSpriteSize;
				pMap->GetCloudRender()->SetGroupSelected(index);
			}
		}
	}
	UpdateData(false);
}

void CCloudSetPanel::OnOK()
{
}

void CCloudSetPanel::OnCancel()
{
}

void CCloudSetPanel::OnButtonRecreateAllgroup() 
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap) pMap->RecreateAllCloudGroup();
}
