#include "DlgCustomizeStature.h"

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_CustomizeMgr.h"
#include "AUISlider.h"
#include "EC_CustomizeMgr.h"
#include "EC_CustomizeBound.h"
#include "FaceBoneController.h"
#include "EC_LoginUIMan.h"
#include "EC_UIManager.h"
#include "EC_Viewport.h"


AUI_BEGIN_COMMAND_MAP(CDlgCustomizeStature, CDlgCustomizeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("Reset", OnCommandReset)

AUI_END_COMMAND_MAP()

CDlgCustomizeStature::CDlgCustomizeStature()
{
	m_pSliderHeadScale = NULL;
	m_pSliderUpScale = NULL;
	m_pSliderWaistScale = NULL;
	m_pSliderArmWidth = NULL;
	m_pSliderLegWidth = NULL;
	m_pSliderBreastScale = NULL;

	m_pSliderBreastScale = NULL;
	m_pLabelBreastScale = NULL;
}

CDlgCustomizeStature::~CDlgCustomizeStature()
{

}

AUIStillImageButton * CDlgCustomizeStature::GetSwithButton()
{
	return NULL;
}

void CDlgCustomizeStature::LoadDefault()
{
	GetCustomizeData()->headScale = GetCustomizeMan()->m_nDefaultHeadScale;
	GetCustomizeData()->upScale = GetCustomizeMan()->m_nDefaultUpScale;
	GetCustomizeData()->waistScale = GetCustomizeMan()->m_nDefaultWaistScale;
	GetCustomizeData()->armWidth = GetCustomizeMan()->m_nDefaultArmWidth;
	GetCustomizeData()->legWidth = GetCustomizeMan()->m_nDefaultLegWidth;
	GetCustomizeData()->breastScale = GetCustomizeMan()->m_nDefaultBreastScale;

	UpdateData(false);
	
}

void CDlgCustomizeStature::DoCalculate()
{
	GetPlayer()->UpdateBodyScales();
}

bool CDlgCustomizeStature::InitResource()
{
	return true;
}

void CDlgCustomizeStature::SetSliderBound()
{
	if (!IsContextReady()) return;
	
	CECCustomizeBound * pSliderBound = GetSliderBound();

	m_pSliderHeadScale->SetTotal(pSliderBound->m_nHeadScaleMax, pSliderBound->m_nHeadScaleMin);
	m_pSliderUpScale->SetTotal(pSliderBound->m_nUpScaleMax, pSliderBound->m_nUpScaleMin);
	m_pSliderWaistScale->SetTotal(pSliderBound->m_nWaistScaleMax, pSliderBound->m_nWaistScaleMin);
	m_pSliderArmWidth->SetTotal(pSliderBound->m_nArmWidthMax, pSliderBound->m_nArmWidthMin);
	m_pSliderLegWidth->SetTotal(pSliderBound->m_nLegWidthMax, pSliderBound->m_nLegWidthMin);
	m_pSliderBreastScale->SetTotal(pSliderBound->m_nBreastScaleMax, pSliderBound->m_nBreastScaleMin);
}


bool CDlgCustomizeStature::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;
	
	if (!InitResource())
		return false;
	
	SetSliderBound();
	
	// init member
	LoadDefault();

	if( GetPlayer()->GetGender() == 0)//ÄÐ
	{
		m_pSliderBreastScale->Show(false);
		m_pLabelBreastScale->Show(false);
	}
	else
	{
		m_pSliderBreastScale->Show(true);
		m_pLabelBreastScale->Show(true);
	}
	
	return true;
}

void CDlgCustomizeStature::DoDataExchange(bool bSave)
{
	CDlgCustomizeBase::DoDataExchange(bSave);

	// ddx control
	DDX_Control("Slider_HeadScale", m_pSliderHeadScale);
	DDX_Control("Slider_UpScale", m_pSliderUpScale);
	DDX_Control("Slider_WaistScale", m_pSliderWaistScale);
	DDX_Control("Slider_ArmWidth", m_pSliderArmWidth);
	DDX_Control("Slider_LegWidth", m_pSliderLegWidth);

	DDX_Control("Slider_BreastScale", m_pSliderBreastScale);
	DDX_Control("Label_BreastScale", m_pLabelBreastScale);

	// ddx data
	if (!IsContextReady()) return;
	DDX_Slider(bSave, "Slider_HeadScale", GetCustomizeData()->headScale);
	DDX_Slider(bSave, "Slider_UpScale", GetCustomizeData()->upScale);
	DDX_Slider(bSave, "Slider_WaistScale", GetCustomizeData()->waistScale);
	DDX_Slider(bSave, "Slider_ArmWidth", GetCustomizeData()->armWidth);
	DDX_Slider(bSave, "Slider_LegWidth", GetCustomizeData()->legWidth);
	DDX_Slider(bSave, "Slider_BreastScale", GetCustomizeData()->breastScale);
}


void CDlgCustomizeStature::OnCommandMoveSlider(const char *szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}

void CDlgCustomizeStature::OnCommandReset(const char * szCommand)
{
	GetCustomizeData()->headScale = GetCustomizeMan()->m_nDefaultHeadScale;
	GetCustomizeData()->upScale = GetCustomizeMan()->m_nDefaultUpScale;
	GetCustomizeData()->waistScale = GetCustomizeMan()->m_nDefaultWaistScale;
	GetCustomizeData()->armWidth = GetCustomizeMan()->m_nDefaultArmWidth;
	GetCustomizeData()->legWidth = GetCustomizeMan()->m_nDefaultLegWidth;
	GetCustomizeData()->breastScale = GetCustomizeMan()->m_nDefaultBreastScale;
	UpdateData(false);
	DoCalculate();
}

void CDlgCustomizeStature::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
	SetCanMove(false);

	int nCurUIMan = GetGame()->GetGameRun()->GetUIManager()->GetCurrentUIMan();
	if(nCurUIMan == CECUIManager::UIMAN_LOGIN)
	{
		((CECLoginUIMan*)m_pAUIManager)->ChangeScene(
			(CECLoginUIMan::LOGIN_SCENE)(CECLoginUIMan::LOGIN_SCENE_BODYCUSTOMIZE_BEGIN +  GetPlayer()->GetProfession()));
	}
	else if( nCurUIMan == CECUIManager::UIMAN_INGAME)
	{
		GetCustomizeMan()->ChangeScene(FACE_SCENE_STATURE);
	}
}