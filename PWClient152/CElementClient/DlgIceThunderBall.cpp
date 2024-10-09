// Filename	: DlgIceThunderBall.cpp
// Creator	: Ruan ZhiJian
// Date		: 2014/10/11

#include "DlgIceThunderBall.h"
#include "AUIImagePicture.h"
#include "EC_HostPlayer.h"
#include "A3DGFXEx.h"
#include "A3DGFXElement.h"
#include "EC_GameUIMan.h"

CDlgIceThunderBall::CDlgIceThunderBall()
{
	m_iCurrentState = 0;
	m_pImg_Iceball1 = NULL;
	m_pImg_Iceball2 = NULL;
	m_pImg_Iceball3 = NULL;

	m_pImg_ThunderBall1 = NULL;
	m_pImg_ThunderBall2 = NULL;
	m_pImg_ThunderBall3 = NULL;
	
	m_pGfx_Move1 = NULL;
	m_pGfx_Move2 = NULL;
}
CDlgIceThunderBall::~CDlgIceThunderBall()
{
}

bool CDlgIceThunderBall::OnInitDialog()
{
	m_pImg_Iceball1 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Iceball1"));
	m_pImg_Iceball2 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Iceball2"));
	m_pImg_Iceball3 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Iceball3"));
	
	m_pImg_ThunderBall1 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_ThunderBall1"));
	m_pImg_ThunderBall2 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_ThunderBall2"));
	m_pImg_ThunderBall3 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_ThunderBall3"));
	
	m_pGfx_Move1 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Gfx_Move1"));
	m_pGfx_Move2 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem("Gfx_Move2"));
	
	return true;
}

void CDlgIceThunderBall::OnShowDialog()
{
	m_pImg_Iceball1->Show(false);
	m_pImg_Iceball2->Show(false);
	m_pImg_Iceball3->Show(false);
	
	m_pImg_ThunderBall1->Show(false);
	m_pImg_ThunderBall2->Show(false);
	m_pImg_ThunderBall3->Show(false);

	m_pGfx_Move1->SetGfx(NULL);   
	m_pGfx_Move2->SetGfx(NULL);
}

bool CDlgIceThunderBall::Tick()
{
	PAUIDIALOG pDlgHost = GetGameUIMan()->GetDialog("Win_Hpmpxp");
	if( pDlgHost->IsShow() ){
		PAUIOBJECT pObject = pDlgHost->GetDlgItem("CombatMask");
		this->AlignToDlgItem(pObject, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM, 2, 4);
	}else{
		PAUIDIALOG pDlgHideHP = GetGameUIMan()->GetDialog("Win_HideHP");
		PAUIOBJECT pObject = pDlgHideHP->GetDlgItem("Chk_CloseHP");
		this->AlignToDlgItem(pObject, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM, 2, 4);
	}
	return CDlgBase::Tick();	
}

void CDlgIceThunderBall::UpdateState(const S2C::IconState &state)
{
	if( m_iCurrentState == state.param[0] ) {
		return;
	}
	m_iCurrentState = state.param[0];
	if( GetHundredsDigit(m_iCurrentState) == 0 
	    && GetTensDigit(m_iCurrentState) == 0 
		&& GetUnitsDigit(m_iCurrentState) != 0 )
	{
		HandleFirstBallEnter(state);
	}else{
		HandleBallMove(state);
	}
}

void CDlgIceThunderBall::HandleFirstBallEnter(const S2C::IconState &state)
{
	Show(true);                  // 显示冰雷球对话框
	HideAllBall();
	ShowCurrentBall(state);      // 显示当前状态对应位置的冰雷球
}

void CDlgIceThunderBall::HandleBallMove(const S2C::IconState &state)
{
	HideAllBall();               // 隐藏所有冰雷球
    PlayMoveGfx(state);          // 播放当前状态对应的冰雷球移动特效
	ShowCurrentBall(state);      // 显示当前状态对应位置的冰雷球
}

// 根据上一状态设置并播放移动特效
void CDlgIceThunderBall::PlaySecondMoveGfx(int second_state){
	if( second_state == 1){
		m_pGfx_Move2->SetGfx("界面\\2014_技能槽_移动冰球.gfx");
	}
	else if( second_state == 5){
		m_pGfx_Move2->SetGfx("界面\\2014_技能槽_移动雷球.gfx");
	}
}
void CDlgIceThunderBall::PlayFirstMoveGfx(int first_state){
	if( first_state == 1){
		m_pGfx_Move1->SetGfx("界面\\2014_技能槽_移动冰球.gfx");
	}
	else if( first_state == 5){
		m_pGfx_Move1->SetGfx("界面\\2014_技能槽_移动雷球.gfx");
	}
}

// 两个移动特效根据上一状态动态设置
void CDlgIceThunderBall::PlayMoveGfx(const S2C::IconState &state)  
{
	int ball_state = state.param[0];
	PlaySecondMoveGfx(GetHundredsDigit(ball_state));      // 设置并播放第二个（右侧）移动特效
	PlayFirstMoveGfx(GetTensDigit(ball_state));       // 设置并播放第一个（左侧）移动特效
	
	if( m_pGfx_Move2->GetGfx() )         // 移动特效缩放
		m_pGfx_Move2->GetGfx()->SetScale( GetGameUIMan()->GetWindowScale() * GetScale());
	if( m_pGfx_Move1->GetGfx() )
		m_pGfx_Move1->GetGfx()->SetScale( GetGameUIMan()->GetWindowScale() * GetScale());
}


void CDlgIceThunderBall::HideAllBall()
{
	m_pImg_Iceball1->Show(false);
	m_pImg_Iceball2->Show(false);
	m_pImg_Iceball3->Show(false);

	m_pImg_ThunderBall1->Show(false);
	m_pImg_ThunderBall2->Show(false);
	m_pImg_ThunderBall3->Show(false);
}

void CDlgIceThunderBall::ShowCurrentBall(const S2C::IconState &icon_state)
{
	ShowCurrentBall(icon_state.param[0]);
}

void CDlgIceThunderBall::ShowCurrentBall(int ball_state)
{
	ShowHunderdsBall(GetHundredsDigit(ball_state) );
	ShowTensBall(GetTensDigit(ball_state) );
	ShowUnitsBall(GetUnitsDigit(ball_state) );
}

void CDlgIceThunderBall::ShowHunderdsBall(int state_of_hundreds){
	if(state_of_hundreds == 1){
		m_pImg_Iceball3->Show(true);
	}
	else if(state_of_hundreds == 5){
		m_pImg_ThunderBall3->Show(true);
	}
}
void CDlgIceThunderBall::ShowTensBall(int state_of_tens){
	if(state_of_tens == 1){
		m_pImg_Iceball2->Show(true);
	}
	else if(state_of_tens == 5){
		m_pImg_ThunderBall2->Show(true);
	}
}
void CDlgIceThunderBall::ShowUnitsBall(int state_of_units){
	if(state_of_units == 1){
		m_pImg_Iceball1->Show(true);
	}
	else if(state_of_units == 5){
		m_pImg_ThunderBall1->Show(true);
	}
}