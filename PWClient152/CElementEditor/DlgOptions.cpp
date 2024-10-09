// DlgOptions.cpp : implementation file
//

#include "Global.h"
#include "DlgOptions.h"
#include "MainFrm.h"
#include "AC.h"
#include "EL_BackMusic.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions dialog


CDlgOptions::CDlgOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOptions)
	m_bAlongGround = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptions)
	DDX_Control(pDX, IDC_SLIDER_SFX, m_sliderSfx);
	DDX_Control(pDX, IDC_SLIDER_MUSIC, m_sliderMusic);
	DDX_Control(pDX, IDC_EDIT_CAMERAROT, m_CamRotEdit);
	DDX_Control(pDX, IDC_EDIT_CAMERAMOVE, m_CamMoveEdit);
	DDX_Control(pDX, IDC_SPIN_CAMERAROT, m_CamRotSpin);
	DDX_Control(pDX, IDC_SPIN_CAMERAMOVE, m_CamMoveSpin);
	DDX_Check(pDX, IDC_CHECK_ALONGGROUND, m_bAlongGround);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOptions, CDialog)
	//{{AFX_MSG_MAP(CDlgOptions)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CAMERAMOVE, OnDeltaposSpinCameramove)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CAMERAROT, OnDeltaposSpinCamerarot)
	ON_BN_CLICKED(IDC_CHECK_ALONGGROUND, OnCheckAlongground)
	ON_BN_CLICKED(IDC_CHECK_OBJECT_ADD_CONTINUE, OnCheckObjectAddContinue)
	ON_EN_KILLFOCUS(IDC_EDIT_CAMERAMOVE, OnKillfocusEditCameramove)
	ON_EN_KILLFOCUS(IDC_EDIT_CAMERAROT, OnKillfocusEditCamerarot)
	ON_BN_CLICKED(IDC_CHECK_PICK_STATICOBJECT, OnCheckPickStaticobject)
	ON_BN_CLICKED(IDC_CHECK_PICK_LIGHT, OnCheckPickLight)
	ON_BN_CLICKED(IDC_CHECK_PICK_BOXAREA, OnCheckPickBoxarea)
	ON_BN_CLICKED(IDC_CHECK_PICK_DUMMY, OnCheckPickDummy)
	ON_BN_CLICKED(IDC_CHECK_PICK_SPECIALLY, OnCheckPickSpecially)
	ON_BN_CLICKED(IDC_CHECK_PICK_WATER, OnCheckPickWater)
	ON_BN_CLICKED(IDC_CHECK_PICK_AIAREA, OnCheckPickAiarea)
	ON_BN_CLICKED(IDC_CHECK_PICK_NPC_PATH, OnCheckPickNPCPath)
	ON_BN_CLICKED(IDC_CHECK_PICK_NON_NPC_PATH, OnCheckPickNonNPCPath)
	ON_BN_CLICKED(IDC_CHECK_PICK_AUDIO, OnCheckPickAudio)
	ON_BN_CLICKED(IDC_CHECK_PICK_CRITTER, OnCheckPickCritter)
	ON_BN_CLICKED(IDC_CHECK_PICK_SKIN_MODEL, OnCheckPickSkinModel)
	ON_BN_CLICKED(IDC_CHECK_PICK_FIXEDNPC, OnCheckPickFixednpc)
	ON_BN_CLICKED(IDC_CHECK_SHOW_HULL, OnCheckShowHull)
	ON_BN_CLICKED(IDC_CHECK_SHOW_HULL2, OnCheckShowHull2)
	ON_BN_CLICKED(IDC_CHECK_PICK_PRECINCT, OnCheckPickPrecinct)
	ON_BN_CLICKED(IDC_CHECK_PICK_GATHER, OnCheckPickGather)
	ON_BN_CLICKED(IDC_CHECK_BEZIER_PT_SNAP, OnCheckBezierPtSnap)
	ON_BN_CLICKED(IDC_CHECK_PICK_RANGE, OnCheckPickRange)
	ON_BN_CLICKED(IDC_CHECK_PICK_CLOUD_BOX, OnCheckPickCloudBox)
	ON_BN_CLICKED(IDC_CHECK_PICK_INSTANCE_BOX, OnCheckPickInstanceBox)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_MUSIC, OnReleasedcaptureSliderMusic)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SFX, OnReleasedcaptureSliderSfx)
	ON_BN_CLICKED(IDC_CHECK_PICK_DYNAMIC, OnCheckPickDynamic)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH_PICK_ALL, OnButtonSwitchPickAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions message handlers

BOOL CDlgOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//	Change edit boxes to flat style
	AUX_FlatEditBox(&m_CamMoveEdit);
	AUX_FlatEditBox(&m_CamRotEdit);

	CString str;
	str.Format("%.2f", g_Configs.fCameraMove);
	m_CamMoveEdit.SetWindowText(str);

	str.Format("%.2f", g_Configs.fCameraRot);
	m_CamRotEdit.SetWindowText(str);

	m_bAlongGround	= g_Configs.bAlongGround ? TRUE : FALSE;

	CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_OBJECT_ADD_CONTINUE);
	if(pButton) pButton->SetCheck(g_Configs.bObjectAddContinue);

	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_HULL);
	if(pButton) pButton->SetCheck(g_Configs.bShowModelHull);

	pButton = (CButton *)GetDlgItem(IDC_CHECK_SHOW_HULL2);
	if(pButton) pButton->SetCheck(g_Configs.bShowModelHull2);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_BEZIER_PT_SNAP);
	if(pButton) pButton->SetCheck(g_bBezierPtSnap);

	UpdatePickCheck();

	UpdateData(FALSE);
	m_sliderMusic.SetRange(1,100);
	m_sliderSfx.SetRange(1,100);
	m_sliderMusic.SetPos(g_Configs.iMusicVolume);
	m_sliderSfx.SetPos(g_Configs.iSfxVolume);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOptions::UpdatePickCheck()
{	
	CButton *pButton = NULL;
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_STATICOBJECT);
	if(pButton) pButton->SetCheck(g_Configs.bPickStaticModel);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_LIGHT);
	if(pButton) pButton->SetCheck(g_Configs.bPickLight);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_AUDIO);
	if(pButton) pButton->SetCheck(g_Configs.bPickAudio);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_BOXAREA);
	if(pButton) pButton->SetCheck(g_Configs.bPickBoxArea);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_DUMMY);
	if(pButton) pButton->SetCheck(g_Configs.bPickDummy);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_SPECIALLY);
	if(pButton) pButton->SetCheck(g_Configs.bPickSpecially);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_WATER);
	if(pButton) pButton->SetCheck(g_Configs.bPickWater);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_AIAREA);
	if(pButton) pButton->SetCheck(g_Configs.bPickAIArea);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_NPC_PATH);
	if(pButton) pButton->SetCheck(g_Configs.bPickNPCPath);

	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_NON_NPC_PATH);
	if(pButton) pButton->SetCheck(g_Configs.bPickNonNPCPath);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_CRITTER);
	if(pButton) pButton->SetCheck(g_Configs.bPickCritterGroup);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_SKIN_MODEL);
	if(pButton) pButton->SetCheck(g_Configs.bPickSkinModel);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_FIXEDNPC);
	if(pButton) pButton->SetCheck(g_Configs.bPickFixedNpc);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_GATHER);
	if(pButton) pButton->SetCheck(g_Configs.bPickGather);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_PRECINCT);
	if(pButton) pButton->SetCheck(g_Configs.bPickPrecinct);
		
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_RANGE);
	if(pButton) pButton->SetCheck(g_Configs.bPickRange);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_CLOUD_BOX);
	if(pButton) pButton->SetCheck(g_Configs.bPickCloudBox);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_INSTANCE_BOX);
	if(pButton) pButton->SetCheck(g_Configs.bPickInstanceBox);
	
	pButton = (CButton *)GetDlgItem(IDC_CHECK_PICK_DYNAMIC);
	if(pButton) pButton->SetCheck(g_Configs.bPickDynamic);
}

void CDlgOptions::OnDeltaposSpinCameramove(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	g_Configs.fCameraMove += -pNMUpDown->iDelta * 0.1f;
	a_ClampFloor(g_Configs.fCameraMove, 0.0f);

	CString str;
	str.Format("%.2f", g_Configs.fCameraMove);
	m_CamMoveEdit.SetWindowText(str);
}

void CDlgOptions::OnDeltaposSpinCamerarot(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	g_Configs.fCameraRot += -pNMUpDown->iDelta * 0.1f;
	a_ClampFloor(g_Configs.fCameraRot, 0.0f);

	CString str;
	str.Format("%.2f", g_Configs.fCameraRot);
	m_CamRotEdit.SetWindowText(str);
}

//	Accept settings
void CDlgOptions::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	CString str;
	m_CamMoveEdit.GetWindowText(str);
	g_Configs.fCameraMove = (float)atof(str);

	m_CamRotEdit.GetWindowText(str);
	g_Configs.fCameraRot = (float)atof(str);

	str.Format("%.2f", g_Configs.fCameraMove);
	m_CamMoveEdit.SetWindowText(str);

	str.Format("%.2f", g_Configs.fCameraRot);
	m_CamRotEdit.SetWindowText(str);

	//	Activate main window
	AUX_GetMainFrame()->SetFocus();
}

void CDlgOptions::OnCheckAlongground() 
{
	m_bAlongGround = !m_bAlongGround;
	g_Configs.bAlongGround = m_bAlongGround ? true : false;
}



void CDlgOptions::OnCheckObjectAddContinue() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bObjectAddContinue = !g_Configs.bObjectAddContinue;
}

void CDlgOptions::OnKillfocusEditCameramove() 
{
	// TODO: Add your control notification handler code here
	CString str;
	m_CamMoveEdit.GetWindowText(str);
	g_Configs.fCameraMove = (float)atof(str);
}

void CDlgOptions::OnKillfocusEditCamerarot() 
{
	// TODO: Add your control notification handler code here
	CString str;
	m_CamRotEdit.GetWindowText(str);
	g_Configs.fCameraRot = (float)atof(str);
}



void CDlgOptions::OnCheckPickStaticobject() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickStaticModel = !g_Configs.bPickStaticModel;
}

void CDlgOptions::OnCheckPickLight() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickLight = !g_Configs.bPickLight;
}

void CDlgOptions::OnCheckPickBoxarea() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickBoxArea = !g_Configs.bPickBoxArea;
}

void CDlgOptions::OnCheckPickDummy() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickDummy = !g_Configs.bPickDummy;
}

void CDlgOptions::OnCheckPickSpecially() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickSpecially = !g_Configs.bPickSpecially;
}

void CDlgOptions::OnCheckPickWater() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickWater = !g_Configs.bPickWater;
}

void CDlgOptions::OnCheckPickAiarea() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickAIArea = !g_Configs.bPickAIArea;
}

void CDlgOptions::OnCheckPickNPCPath() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickNPCPath = !g_Configs.bPickNPCPath;
}

void CDlgOptions::OnCheckPickNonNPCPath() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickNonNPCPath = !g_Configs.bPickNonNPCPath;
}

void CDlgOptions::OnCheckPickAudio() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickAudio = !g_Configs.bPickAudio;
}

void CDlgOptions::OnCheckPickCritter() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickCritterGroup = !g_Configs.bPickCritterGroup;
}

void CDlgOptions::OnCheckPickSkinModel() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickSkinModel = !g_Configs.bPickSkinModel;
}

void CDlgOptions::OnCheckPickFixednpc() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickFixedNpc = !g_Configs.bPickFixedNpc;
}

void CDlgOptions::OnCheckShowHull() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowModelHull = !g_Configs.bShowModelHull;
}

void CDlgOptions::OnCheckShowHull2() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bShowModelHull2 = !g_Configs.bShowModelHull2;
}

void CDlgOptions::OnCheckPickPrecinct() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickPrecinct = !g_Configs.bPickPrecinct;
}

void CDlgOptions::OnCheckPickGather() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickGather = !g_Configs.bPickGather;
}

void CDlgOptions::OnCheckBezierPtSnap() 
{
	// TODO: Add your control notification handler code here
	g_bBezierPtSnap = !g_bBezierPtSnap;
}

void CDlgOptions::OnCheckPickRange() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickRange = !g_Configs.bPickRange;
}

void CDlgOptions::OnCheckPickCloudBox() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickCloudBox = !g_Configs.bPickCloudBox;
}

void CDlgOptions::OnCheckPickInstanceBox() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickInstanceBox = !g_Configs.bPickInstanceBox;
}

void CDlgOptions::OnReleasedcaptureSliderMusic(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
    g_Configs.iMusicVolume = m_sliderMusic.GetPos();
	g_BackMusic.SetVolume(g_Configs.iMusicVolume);
	*pResult = 0;
}

void CDlgOptions::OnReleasedcaptureSliderSfx(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	g_Configs.iSfxVolume = m_sliderSfx.GetPos();
	g_BackMusic.SetSFXVolume(g_Configs.iSfxVolume);
	*pResult = 0;
}

void CDlgOptions::OnCheckPickDynamic() 
{
	// TODO: Add your control notification handler code here
	g_Configs.bPickDynamic = !g_Configs.bPickDynamic;
}

void CDlgOptions::OnButtonSwitchPickAll() 
{
	bool bCurrentPick = (g_Configs.bPickStaticModel);	//	使用第一个“拾取静态模型"作为切换开关
	bool bNewPick = !bCurrentPick;
	
	g_Configs.bPickStaticModel = bNewPick;
	g_Configs.bPickLight = bNewPick;
	g_Configs.bPickAudio = bNewPick;
	g_Configs.bPickBoxArea = bNewPick;
	g_Configs.bPickDummy = bNewPick;
	g_Configs.bPickSpecially = bNewPick;
	g_Configs.bPickWater = bNewPick;
	g_Configs.bPickAIArea = bNewPick;
	g_Configs.bPickNPCPath = bNewPick;
	g_Configs.bPickNonNPCPath = bNewPick;
	g_Configs.bPickSkinModel = bNewPick;
	g_Configs.bPickCritterGroup = bNewPick;
	g_Configs.bPickFixedNpc = bNewPick;
	g_Configs.bPickPrecinct = bNewPick;
	g_Configs.bPickGather = bNewPick;
	g_Configs.bPickRange = bNewPick;
	g_Configs.bPickCloudBox = bNewPick;
	g_Configs.bPickInstanceBox = bNewPick;
	g_Configs.bPickDynamic = bNewPick;

	UpdatePickCheck();
}
