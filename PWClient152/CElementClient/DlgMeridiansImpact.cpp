/********************************************************************
	created:	2012/1/22
	created:	12:11:2012   16:31
	file base:	DlgMeridiansImpact
	file ext:	cpp
	author:		zhougaomin01305
	
	purpose:	经脉冲穴系统
*********************************************************************/

#include "DlgMeridiansImpact.h"
#include "DlgAutoLock.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "EC_IvtrItem.h"

#include "A3DGFXExMan.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

#include "EC_Player.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"

#include "EC_Meridians.h"
#include "EC_Player.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgMeridiansImpact, CDlgBase)

AUI_ON_COMMAND("Btn_Point*", OnCommandOpenGate)

AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)

AUI_END_COMMAND_MAP()

static const char* IMPACT_GFX[4] = {
	"界面\\生门.gfx",
	"界面\\冲击成功.gfx",
	"界面\\死门.gfx",
	"界面\\冲击失败.gfx",
};

CDlgMeridiansImpact::CDlgMeridiansImpact()
:   m_pLabelContinuousSuccess(NULL) ,
	m_pLabelSuccessGate(NULL)       ,
	m_pLabelName(NULL)             ,
	m_pLabelFreeUpgradeNum(NULL)    ,
	m_pLabelChargableUpgradeNum(NULL),
	m_pLabelContinuousLogin(NULL)   ,
	m_bCloseCheckPoint(false),
	m_bReadyToHide(false),
	m_index(0),
	m_pImpactGfx(NULL)

{
}

CDlgMeridiansImpact::~CDlgMeridiansImpact()
{

}

void CDlgMeridiansImpact::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_CheckBox(bSave,"Chk_Point"    ,m_bCloseCheckPoint   );
}

bool CDlgMeridiansImpact::OnInitDialog()
{
	DDX_Control("Label_ContinuousSuccess"  ,m_pLabelContinuousSuccess );
	DDX_Control("Label_SuccessGate"        ,m_pLabelSuccessGate       );
	DDX_Control("Label_Name"			   ,m_pLabelName             );
	DDX_Control("Label_FreeUpgradeNum"     ,m_pLabelFreeUpgradeNum    );
	DDX_Control("Label_ChargableUpgradeNum",m_pLabelChargableUpgradeNum);
	DDX_Control("Label_ContinuousLogin"    ,m_pLabelContinuousLogin   );
	DDX_Control("Img_Gfx"				   ,m_pImpactGfx );
	m_pLabelFreeUpgradeNum->SetHint(GetStringFromTable(10227));
	m_pLabelContinuousLogin->SetHint(GetStringFromTable(10227));
	m_pLabelChargableUpgradeNum->SetHint(GetStringFromTable(10228));
	// 状态图片
	m_pbtnImages[IMAGE_UNKNOWNGATE]		  = "version02\\系统\\灵脉\\未知门.tga";
	m_pbtnImages[IMAGE_SUCCESSGATE]		  = "version02\\系统\\灵脉\\生门.tga";
	m_pbtnImages[IMAGE_FAILGATE]		  = "version02\\系统\\灵脉\\死门.tga";
	m_pbtnImages[IMAGE_ERROR]			  = "version02\\系统\\灵脉\\按钮不可用.tga";
	m_pbtnImages[IMAGE_UNKNOWNGATE_HOWER] = "version02\\系统\\灵脉\\未知门高亮.tga";
	m_pbtnImages[IMAGE_UNKNOWNGATE_DOWN]  = "version02\\系统\\灵脉\\未知门按下.tga";

	return true;
}

void CDlgMeridiansImpact::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_pImpactGfx->SetGfx("");
	const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();
	m_level = meridianProp.level + 1;

	SetImpactInfo();
	SetBtnInfo();
	m_pImpactGfx->Show(false);
	m_bReadyToHide = false;

	EC_VIDEO_SETTING vs = GetGame()->GetConfigs()->GetVideoSettings();
	m_bCloseCheckPoint = vs.bCloseMeridiansHint;
	UpdateData(false);
}

void CDlgMeridiansImpact::SendImpactProtocol()
{
	GetGameSession()->c2s_CmdMeridiansImpact(m_index);
	PAUISTILLIMAGEBUTTON pbtn(NULL);
	ACString temp;
	for(int i = 1;i <= 48;i++)
	{
		temp.Format(_AL("Btn_Point%d"),i);
		pbtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(AC2AS(temp)));
		if(pbtn)
			pbtn->Enable(false);
	}
}

bool CDlgMeridiansImpact::CheckItem(bool bFreeItem)
{
	bool bFind = false;
	CECInventory *pPack = GetHostPlayer()->GetPack();
	int size = pPack->GetSize();
	for (int i = 0; i < size; ++i){
		CECIvtrItem* pItem = pPack->GetItem(i);
		if (pItem) {
			if (bFreeItem) bFind = CECUIConfig::Instance().GetGameUI().IsMeridianFreeItem(pItem->GetTemplateID());
			else bFind = CECUIConfig::Instance().GetGameUI().IsMeridianNotFreeItem(pItem->GetTemplateID());
			if (bFind) break;
		}
	}
	return bFind;
}

bool CDlgMeridiansImpact::CanImpact()
{
	CECHostPlayer* pHost = GetHostPlayer();
	bool ret = pHost && 
			   pHost->GetBoothState() == 0 &&
			   !pHost->IsTrading() &&
			   !pHost->IsDead() &&
			   !GetGameUIMan()->m_pDlgAutoLock->IsLocked();
	return ret;
}
void CDlgMeridiansImpact::OnCommandOpenGate(const char *szCommand)
{
	if (!CanImpact())
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(10199),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	m_index = atoi(szCommand + strlen("Btn_Point")) - 1;
	const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();
	// 发送协议
	UpdateData(true);

	// 检测玄鉴丹
	if(!CheckItem(true))
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(10210),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// 检测绝圣丹
	if(!meridianProp.freeUpgradeNum)
	{
		if(!meridianProp.chargableUpgradeNum)	
		{
			//次数用完
			GetGameUIMan()->MessageBox("",GetStringFromTable(10211),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		else
		{
			//绝圣丹
			if(!CheckItem(false))
			{
				GetGameUIMan()->MessageBox("",GetStringFromTable(10212),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
		}
	}

	if(meridianProp.freeUpgradeNum || m_bCloseCheckPoint)
	{
		// 直接发送协议
		SendImpactProtocol();
	}
	else
	{
		// 提示需要绝圣丹
		ACString str = GetGameUIMan()->GetStringFromTable(10218);
		GetGameUIMan()->MessageBox("MeridiansImpact",str,MB_YESNO,A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgMeridiansImpact::OnTick()
{
	CDlgBase::OnTick();
	
	if (m_bReadyToHide && m_pImpactGfx->IsShow())
	{
		A3DGFXEx* pGfx = m_pImpactGfx->GetGfx();
		if (pGfx && pGfx->GetState() == ST_STOP)
			OnCommandCancel("");
	}
}

void CDlgMeridiansImpact::OnCommandCancel(const char * szCommand)
{
	UpdateData(true);

	EC_VIDEO_SETTING vs = GetGame()->GetConfigs()->GetVideoSettings();
	vs.bCloseMeridiansHint = m_bCloseCheckPoint;
	GetGame()->GetConfigs()->SetVideoSettings(vs);
	
	Show(false);
}

void CDlgMeridiansImpact::SetImpactInfo()
{
	ACString temp;
	const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();

	if(m_level >= 0 && m_level <= CECMeridians::MeridiansLevelLayer * CECMeridians::MERIDIAN_LEVEL_COUNT)
	{
		// 设置将获得的属性
		int iProp[5];
		CECMeridians::GetSingleton().GetLevelPropBonus(GetHostPlayer()->GetProfession(),m_level,iProp[0],iProp[1],iProp[2],iProp[3],iProp[4]);
		
		ACString temp;
		PAUILABEL pLabelProp[5];
		//
		pLabelProp[0] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_HP"));
		pLabelProp[1] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_PhyDefence"));
		pLabelProp[2] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_MgiDefence"));
		pLabelProp[3] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_PhyAttack"));
		pLabelProp[4] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_MgiAttack"));
		
		for(int i = 0;i < 5;i++)
		{
			temp.Format(GetStringFromTable(10220 + i),iProp[i]);
			if(pLabelProp[i])
				pLabelProp[i]->SetText(temp);
		}
	}

	const CECMeridians::MeridianlevelParam& param = CECMeridians::GetSingleton().GetLevelParam(m_level-1);
	//
	int continuousSuccess = param.continuousSuccessGateNum;
	if(continuousSuccess == 1)
	{
		m_pLabelContinuousSuccess->SetText(GetStringFromTable(10204));
	}
	else
	{
		temp.Format(GetStringFromTable(10205),continuousSuccess);
		m_pLabelContinuousSuccess->SetText(temp);
	}
	//
	int successGate = param.successGate;
	temp.Format(GetStringFromTable(10206),successGate);
	m_pLabelSuccessGate->SetText(temp);

}

void CDlgMeridiansImpact::SetBtnInfo()
{
	ACString temp;
	const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();
	//
	temp.Format(GetStringFromTable(10207),meridianProp.freeUpgradeNum);
	m_pLabelFreeUpgradeNum->SetText(temp);
	//
	temp.Format(GetStringFromTable(10208),meridianProp.chargableUpgradeNum);
	m_pLabelChargableUpgradeNum->SetText(temp);
	//
	if (meridianProp.continuousLoginDays < 5)
		temp.Format(GetStringFromTable(10209),meridianProp.continuousLoginDays);
	else
		temp.Format(GetStringFromTable(10229));

	
	m_pLabelContinuousLogin->SetText(temp);
	// 按钮刷新
	PAUISTILLIMAGEBUTTON pbtn;
	AUIOBJECT_SETPROPERTY prop;
	int state;
	for(int i = 1;i <= 48;i++)
	{
		temp.Format(_AL("Btn_Point%d"),i);
		pbtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(AC2AS(temp)));
		if(pbtn)
		{
			state = GetBtnState(i);
			if(meridianProp.level + 1 != m_level)
				pbtn->Enable(false);
			else
				pbtn->Enable(!state);
			strcpy(prop.fn,m_pbtnImages[state]);			
			ScopedAUIControlSpriteModify _dummy(pbtn);
			pbtn->SetProperty("Up Frame File",&prop);
			pbtn->SetProperty("Disabled Frame File",&prop);
			if (state == IMAGE_UNKNOWNGATE)
			{
				strcpy(prop.fn,m_pbtnImages[IMAGE_UNKNOWNGATE_HOWER]);
				pbtn->SetProperty("OnHover Frame File",&prop);
				strcpy(prop.fn,m_pbtnImages[IMAGE_UNKNOWNGATE_DOWN]);
				pbtn->SetProperty("Down Frame File",&prop);
			}
			else
			{
				pbtn->SetProperty("OnHover Frame File",&prop);
				pbtn->SetProperty("Down Frame File",&prop);
			}
		}
	}
}

int CDlgMeridiansImpact::GetBtnState(int index)
{
	const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();
	
	int state = 0;
	
	if(index <= 16)
	{
		state = meridianProp.eightTrigramsState1;
	}
	else if(index <= 32)
	{
		state = meridianProp.eightTrigramsState2;
	}
	else if(index <= 48)
	{
		state = meridianProp.eightTrigramsState3;
	}

	index = (index -1) % 16;

	state = (state >> (index * 2)) & 0x3;

	return state;
}

void CDlgMeridiansImpact::SetImpactName(const ACHAR*pImpactName)
{
	//
	m_pLabelName->SetText(pImpactName);
}

void CDlgMeridiansImpact::OnImpactResult(int index, int result)
{
	m_pImpactGfx->SetGfx(IMPACT_GFX[min(result - 1, 3)]);
	m_pImpactGfx->Resize();
	m_pImpactGfx->Show(true);
	m_bReadyToHide = result == 2;
}

void CDlgMeridiansImpact::OnChangeLayoutEnd(bool bAllDone)
{
	m_pImpactGfx->Show(false);	
}