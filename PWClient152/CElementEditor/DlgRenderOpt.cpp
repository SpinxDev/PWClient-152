// DlgRenderOpt.cpp : implementation file
//

#include "Global.h"
#include "DlgRenderOpt.h"
#include "MainFrm.h"
#include "Render.h"
#include "A3D.h"
#include "SceneBoxArea.h"
#include "TerrainWater.h"
#include "PersViewport.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgRenderOpt dialog


CDlgRenderOpt::CDlgRenderOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRenderOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRenderOpt)
	m_fViewRadius = 0.0f;
	m_bImmeMaskPaint = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgRenderOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRenderOpt)
	DDX_Control(pDX, IDC_COMBO_COPY, m_cbCopySel);
	DDX_Control(pDX, IDC_COMBO_WEIGHT, m_WeightCombo);
	DDX_Text(pDX, IDC_EDIT_VIEWRADIUS, m_fViewRadius);
	DDV_MinMaxFloat(pDX, m_fViewRadius, 20.f, 2000.f);
	DDX_Check(pDX, IDC_CHECK_IMMEMASKPAINT, m_bImmeMaskPaint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRenderOpt, CDialog)
	//{{AFX_MSG_MAP(CDlgRenderOpt)
	ON_BN_CLICKED(IDC_CHECK_IMMEMASKPAINT, OnCheckImmemaskpaint)
	ON_BN_CLICKED(IDC_CHECK_RENDER_SKY, OnCheckRenderSky)
	ON_CBN_SELCHANGE(IDC_COMBO_WEIGHT, OnSelchangeComboWeight)
	ON_BN_CLICKED(IDC_CHECK_RENDER_SHOW_NAME, OnCheckRenderShowName)
	ON_EN_KILLFOCUS(IDC_EDIT_VIEWRADIUS, OnKillfocusEditViewradius)
	ON_EN_CHANGE(IDC_EDIT_VIEWRADIUS, OnChangeEditViewradius)
	ON_BN_CLICKED(IDC_CHECK_SHOW_STATICOBJECT, OnCheckShowStaticobject)
	ON_BN_CLICKED(IDC_CHECK_SHOW_LIGHT, OnCheckShowLight)
	ON_BN_CLICKED(IDC_CHECK_SHOW_AUDIO, OnCheckShowAudio)
	ON_BN_CLICKED(IDC_CHECK_SHOW_BOXAREA, OnCheckShowBoxarea)
	ON_BN_CLICKED(IDC_CHECK_SHOW_DUMMY, OnCheckShowDummy)
	ON_BN_CLICKED(IDC_CHECK_SHOW_SPECIALLY, OnCheckShowSpecially)
	ON_BN_CLICKED(IDC_CHECK_SHOW_WATER, OnCheckShowWater)
	ON_BN_CLICKED(IDC_CHECK_SHOW_AIAREA, OnCheckShowAiarea)
	ON_BN_CLICKED(IDC_CHECK_SHOW_NPC_PATH, OnCheckShowNPCPath)
	ON_BN_CLICKED(IDC_CHECK_SHOW_NON_NPC_PATH, OnCheckShowNonNPCPath)
	ON_BN_CLICKED(IDC_CHECK_SHOW_SKINMODEL, OnCheckShowSkinmodel)
	ON_BN_CLICKED(IDC_CHECK_SHOW_CRITTER_GROUP, OnCheckShowCritterGroup)
	ON_BN_CLICKED(DC_CHECK_RENDER_ENABLE_FOG, OnCheckRenderEnableFog)
	ON_BN_CLICKED(IDC_CHECK_SHOW_FIXED_NPC, OnCheckShowFixedNpc)
	ON_BN_CLICKED(IDC_CHECK_SHOW_TERRAIN_BLOCK, OnCheckShowTerrainBlock)
	ON_BN_CLICKED(IDC_CHECK_SHOW_TREE, OnCheckShowTree)
	ON_BN_CLICKED(IDC_CHECK_SHOW_GRASS, OnCheckShowGrass)
	ON_BN_CLICKED(IDC_CHECK_SHOW_GATHER, OnCheckShowGather)
	ON_BN_CLICKED(IDC_CHECK_SHOW_TERRAIN, OnCheckShowTerrain)
	ON_BN_CLICKED(IDC_CHECK_SHOW_RANGE, OnCheckShowRange)
	ON_BN_CLICKED(IDC_CHECK_SHOW_CLOUD, OnCheckShowCloud)
	ON_BN_CLICKED(IDC_CHECK_SHOW_CLOUD_LINE, OnCheckShowCloudLine)
	ON_BN_CLICKED(DC_CHECK_RENDER_SAMPLE_WATER, OnCheckRenderSampleWater)
	ON_BN_CLICKED(IDC_CHECK_SHOW_INSTANCE_BOX, OnCheckShowInstanceBox)
	ON_CBN_SELCHANGE(IDC_COMBO_COPY, OnSelchangeComboCopy)
	ON_BN_CLICKED(IDC_CHECK_SHOW_DYNAMIC, OnCheckShowDynamic)
	ON_BN_CLICKED(IDC_CHECK_SHOW_MONSTER_VIEW, OnCheckShowMonsterView)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_SHOW_ALL, OnButtonSwitchShowAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRenderOpt message handlers

BOOL CDlgRenderOpt::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Change edit boxes to flat style
	AUX_FlatEditBox((CEdit*)GetDlgItem(IDC_EDIT_VIEWRADIUS));

	//	Maximum layer weight
	char szStr[10];
	for (int i=0; i <= A3DTerrain2::MAX_LAYERWEIGHT; i++)
	{
		itoa(i, szStr, 10);
		m_WeightCombo.InsertString(-1, szStr);
	}
	int sel = 0;
	for(i = -1; i < 21; ++i)
	{
		CString temp;
		temp.Format("%d",i);
		int cur = m_cbCopySel.AddString(temp);
		if(i==g_Configs.nShowCopyNum) sel= cur;
	}
	m_cbCopySel.SetCurSel(sel);

	m_WeightCombo.SetCurSel(g_Configs.iMaxLayerWeight);

	m_fViewRadius		= g_Configs.fViewRadius;
	m_bImmeMaskPaint	= g_Configs.bImmeMaskPaint ? TRUE : FALSE;

	CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_RENDER_SKY);
	if(pButton) pButton->SetCheck(g_Configs.bEnableSky);
	
	pButton = (CButton *)GetDlgItem(DC_CHECK_RENDER_ENABLE_FOG);
	if(pButton) pButton->SetCheck(g_Configs.bEnableFog);

	pButton = (CButton *)GetDlgItem(DC_CHECK_RENDER_SAMPLE_WATER);
	if(pButton) pButton->SetCheck( g_Configs.bSampleWater);

	UpdateShowOptionsCheck();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRenderOpt::UpdateShowOptionsCheck()
{	
	CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_RENDER_SHOW_NAME);
	if(pButton) pButton->SetCheck(g_Configs.bShowObjectName);	
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_STATICOBJECT);
	if(pButton) pButton->SetCheck(g_Configs.bShowStaticModel);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_LIGHT);
	if(pButton) pButton->SetCheck(g_Configs.bShowLight);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_AUDIO);
	if(pButton) pButton->SetCheck(g_Configs.bShowAudio);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_BOXAREA);
	if(pButton) pButton->SetCheck(g_Configs.bShowBoxArea);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_DUMMY);
	if(pButton) pButton->SetCheck(g_Configs.bShowDummy);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_SPECIALLY);
	if(pButton) pButton->SetCheck(g_Configs.bShowSpecially);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_WATER);
	if(pButton) pButton->SetCheck(g_Configs.bShowWater);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_AIAREA);
	if(pButton) pButton->SetCheck(g_Configs.bShowAIArea);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_NPC_PATH);
	if(pButton) pButton->SetCheck(g_Configs.bShowNPCPath);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_NON_NPC_PATH);
	if(pButton) pButton->SetCheck(g_Configs.bShowNonNPCPath);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_CRITTER_GROUP);
	if(pButton) pButton->SetCheck(g_Configs.bShowCritterGroup);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_SKINMODEL);
	if(pButton) pButton->SetCheck(g_Configs.bShowSkinModel);
		
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_FIXED_NPC);
	if(pButton) pButton->SetCheck(g_Configs.bShowFixedNpc);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_TERRAIN_BLOCK);
	if(pButton) pButton->SetCheck(g_Configs.bShowTerrainBlock);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_TREE);
	if(pButton) pButton->SetCheck(	g_Configs.bShowTree);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_GRASS);
	if(pButton) pButton->SetCheck(	g_Configs.bShowGrass);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_GATHER);
	if(pButton) pButton->SetCheck(	g_Configs.bShowGather);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_TERRAIN);
	if(pButton) pButton->SetCheck( g_Configs.bShowTerrain);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_RANGE);
	if(pButton) pButton->SetCheck( g_Configs.bShowRange);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_CLOUD);
	if(pButton) pButton->SetCheck( g_Configs.bShowCloudBox);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_INSTANCE_BOX);
	if(pButton) pButton->SetCheck( g_Configs.bShowInstanceBox);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_DYNAMIC);
	if(pButton) pButton->SetCheck( g_Configs.bShowDynamic);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_MONSTER_VIEW);
	if(pButton) pButton->SetCheck( g_Configs.bShowMonsterView);

	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_CLOUD_LINE);
	if(pButton) pButton->SetCheck( g_Configs.bShowCloudLine);
}

void CDlgRenderOpt::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	g_Configs.fViewRadius		= m_fViewRadius;
	g_Configs.bImmeMaskPaint	= m_bImmeMaskPaint ? true : false;
}

//	Initlaize device objects
bool CDlgRenderOpt::InitDeviceObjects()
{
	return true;
}

void CDlgRenderOpt::OnCheckImmemaskpaint() 
{
	m_bImmeMaskPaint = !m_bImmeMaskPaint;
	g_Configs.bImmeMaskPaint = m_bImmeMaskPaint ? true : false;
}
void CDlgRenderOpt::OnCheckRenderSky() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bEnableSky = !g_Configs.bEnableSky;
}

void CDlgRenderOpt::OnSelchangeComboWeight() 
{
	g_Configs.iMaxLayerWeight = m_WeightCombo.GetCurSel();	
}

void CDlgRenderOpt::OnCheckRenderShowName() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowObjectName = !g_Configs.bShowObjectName;
}



void CDlgRenderOpt::OnKillfocusEditViewradius() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if(m_fViewRadius>99 && m_fViewRadius<2001)
	{
		g_Configs.fViewRadius		= m_fViewRadius;
		CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
		CRenderWnd *pRWnd = pFrame->GetRenderWnd();
		CPersViewport* vp =  (CPersViewport*)pRWnd->GetViewport(VIEW_PERSPECTIVE);
		if(vp) 
		{
			if(vp->GetCamera()) vp->GetCamera()->SetZFrontAndBack(0.1f,g_Configs.fViewRadius);
		}
	}
	else 
	{
		MessageBox("渲染半径范围必须在 100 与 2000 之间");
	}
}

void CDlgRenderOpt::OnChangeEditViewradius() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CDlgRenderOpt::OnCheckShowStaticobject() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowStaticModel = !g_Configs.bShowStaticModel;
}

void CDlgRenderOpt::OnCheckShowLight() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowLight = !g_Configs.bShowLight;
}

void CDlgRenderOpt::OnCheckShowAudio() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowAudio = !g_Configs.bShowAudio;
}

void CDlgRenderOpt::OnCheckShowBoxarea() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowBoxArea = !g_Configs.bShowBoxArea;
}

void CDlgRenderOpt::OnCheckShowDummy() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowDummy = !g_Configs.bShowDummy;
}

void CDlgRenderOpt::OnCheckShowSpecially() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowSpecially = !g_Configs.bShowSpecially;
}

void CDlgRenderOpt::OnCheckShowWater() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowWater = !g_Configs.bShowWater;
}

void CDlgRenderOpt::OnCheckShowAiarea() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowAIArea = !g_Configs.bShowAIArea;
}

void CDlgRenderOpt::OnCheckShowNPCPath() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowNPCPath = !g_Configs.bShowNPCPath;
}

void CDlgRenderOpt::OnCheckShowNonNPCPath() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowNonNPCPath = !g_Configs.bShowNonNPCPath;
}

void CDlgRenderOpt::OnCheckShowSkinmodel() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowSkinModel = !g_Configs.bShowSkinModel;
}

void CDlgRenderOpt::OnCheckShowCritterGroup() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowCritterGroup = !g_Configs.bShowCritterGroup;
}

void CDlgRenderOpt::OnCheckRenderEnableFog() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bEnableFog = !g_Configs.bEnableFog;
}

void CDlgRenderOpt::OnCheckShowFixedNpc() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowFixedNpc = !g_Configs.bShowFixedNpc;
}

void CDlgRenderOpt::OnCheckShowTerrainBlock() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowTerrainBlock = !g_Configs.bShowTerrainBlock;
}

void CDlgRenderOpt::OnCheckShowTree() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowTree = !g_Configs.bShowTree;
}

void CDlgRenderOpt::OnCheckShowGrass() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowGrass = !g_Configs.bShowGrass;
}

void CDlgRenderOpt::OnCheckShowGather() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowGather = !g_Configs.bShowGather;
}

void CDlgRenderOpt::OnCheckShowTerrain() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowTerrain = !g_Configs.bShowTerrain;
}

void CDlgRenderOpt::OnCheckShowRange() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowRange = !g_Configs.bShowRange;
}

void CDlgRenderOpt::OnCheckShowCloud() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowCloudBox = !g_Configs.bShowCloudBox;
}

void CDlgRenderOpt::OnCheckShowCloudLine() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowCloudLine =!g_Configs.bShowCloudLine;
}

void CDlgRenderOpt::OnCheckRenderSampleWater() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bSampleWater = !g_Configs.bSampleWater;

	if(AUX_GetMainFrame()->GetMap())
	{
		AUX_GetMainFrame()->GetMap()->GetTerrainWater()->GetA3DTerrainWater()->SetSimpleWaterFlag(g_Configs.bSampleWater);
	}
}

void CDlgRenderOpt::OnCheckShowInstanceBox() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowInstanceBox = !g_Configs.bShowInstanceBox;
}

void CDlgRenderOpt::OnSelchangeComboCopy() 
{
	// TODO: Add your control notification handler code here
	int sel = m_cbCopySel.GetCurSel();
	CString rString;
	if(sel != -1)
	{
		m_cbCopySel.GetLBText(sel,rString);
		g_Configs.nShowCopyNum = (int)atof(rString);
	}
}

void CDlgRenderOpt::OnCheckShowDynamic() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowDynamic = !g_Configs.bShowDynamic;
}

void CDlgRenderOpt::OnCheckShowMonsterView() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowMonsterView = !g_Configs.bShowMonsterView;
}

void CDlgRenderOpt::OnButtonSwitchShowAll() 
{	
	bool bCurrentPick = (g_Configs.bShowStaticModel);	//	使用第一个“拾取静态模型"作为切换开关
	bool bNewPick = !bCurrentPick;
	
	g_Configs.bShowStaticModel = bNewPick;
	g_Configs.bShowLight = bNewPick;
	g_Configs.bShowAudio = bNewPick;
	g_Configs.bShowBoxArea = bNewPick;
	g_Configs.bShowDummy = bNewPick;
	g_Configs.bShowSpecially = bNewPick;
	g_Configs.bShowWater = bNewPick;
	g_Configs.bShowAIArea = bNewPick;
	g_Configs.bShowNPCPath = bNewPick;
	g_Configs.bShowNonNPCPath = bNewPick;
	g_Configs.bShowSkinModel = bNewPick;
	g_Configs.bShowCritterGroup = bNewPick;
	g_Configs.bShowFixedNpc = bNewPick;
	g_Configs.bShowTerrainBlock = bNewPick;
	g_Configs.bShowGrass = bNewPick;
	g_Configs.bShowTree = bNewPick;
	g_Configs.bShowGather = bNewPick;
	g_Configs.bShowTerrain = bNewPick;
	g_Configs.bShowRange = bNewPick;
	g_Configs.bShowCloudBox = bNewPick;
	g_Configs.bShowCloudLine = bNewPick;
	g_Configs.bShowInstanceBox = bNewPick;
	g_Configs.bShowDynamic = bNewPick;
	g_Configs.bShowMonsterView = bNewPick;
	g_Configs.bShowObjectName = bNewPick;
	
	UpdateShowOptionsCheck();
}