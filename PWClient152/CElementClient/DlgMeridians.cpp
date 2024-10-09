/********************************************************************
	created:	2012/1/22
	created:	12:11:2012   16:31
	file base:	DlgMeridians
	file ext:	cpp
	author:		zhougaomin01305
	
*********************************************************************/

#include "DlgMeridians.h"
#include "DlgMeridiansImpact.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "CSplit.h"
#include "EC_TimeSafeChecker.h"

#include "A3DGFXExMan.h"
#include "AUILabel.h"

#include "EC_HostPlayer.h"
#include "EC_Inventory.h"

#include "EC_Player.h"

static const char* MERIDIANS_LAYER[CECMeridians::MeridiansLevelLayer + 1] = 
{"界面\\炼精化气.gfx", "界面\\炼气化神.gfx", "界面\\炼神还虚.gfx", "界面\\炼虚合道.gfx","界面\\道法自然.gfx"};
static const char* MERIDIANS_LEVEL[CECMeridians::MeridiansLevelLayer + 1][2] = 
{"界面\\破_出现.gfx", "界面\\破_消失.gfx", "界面\\逆_出现.gfx", "界面\\逆_消失.gfx","界面\\虚_出现.gfx", "界面\\虚_消失.gfx", "界面\\道_出现.gfx", "界面\\道_消失.gfx", "", ""};
static const char* MERIDIANS_TEXT_GFX[CECMeridians::MeridiansLevelLayer + 1] =
{"界面\\炼精化气描述.gfx", "界面\\炼气化神描述.gfx", "界面\\炼神还虚描述.gfx", "界面\\炼虚合道描述.gfx","界面\\道法自然描述.gfx"};

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgMeridians, CDlgBase)

AUI_ON_COMMAND("Btn_Impact*", OnCommandImpact)

AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMeridians, CDlgBase)
AUI_ON_EVENT("Btn_Impact20",	WM_LBUTTONUP,	OnEventLButtonUp_LevelGfx)
AUI_END_EVENT_MAP()

static CECPlayer::MeridiansProp gMeridiansProp;

CTextAppearAnimator::CTextAppearAnimator(const ACString& linesoftext, PAUITEXTAREA pTextArea, int timeInterval)
{
	SetText(linesoftext);
	SetTextArea(pTextArea);
	SetTimeInterval(timeInterval);
}

void CTextAppearAnimator::Reset()
{
	m_dwCurrentLine = 0;
	m_dwLastTick = GetTickCount();
	if (m_pTextArea)
		m_pTextArea->SetText(_AL(""));
}

void CTextAppearAnimator::SetTimeInterval(int timeInterval)
{ 
	Reset();
	m_dwTimeIntervalBetweenLines = timeInterval; 
}
void CTextAppearAnimator::SetText(const ACString& strLines)
{
	int start = 0;
	int size = strLines.GetLength();
	int index;
	m_vecLinesOfText.clear();

	while (start < size)
	{
		index = strLines.Find(_AL("\r"), start);
		if (index >= 0)
		{
			ACString temp = strLines.Mid(start, index - start + 1);
			m_vecLinesOfText.push_back(strLines.Mid(start, index - start + 1));
			start = index + 1;
		}
		else
		{
			ACString temp = strLines.Mid(start, size - start);
			m_vecLinesOfText.push_back(strLines.Mid(start, size - start));
			break;
		}
	}	
}

void CTextAppearAnimator::Tick()
{
	DWORD dwTick = GetTickCount();
	if (m_pTextArea &&
		CECTimeSafeChecker::ElapsedTime(dwTick, m_dwLastTick) > m_dwTimeIntervalBetweenLines &&
		m_dwCurrentLine < m_vecLinesOfText.size())
	{
		m_dwLastTick = dwTick;
		m_pTextArea->AppendText(m_vecLinesOfText[m_dwCurrentLine++]);
	}
}

CDlgMeridians::CDlgMeridians():
m_pMeridiansGfx(NULL),
m_pLevelGfx(NULL),
m_pTextGfx(NULL),
m_iLastLayer(-1)
{
	gMeridiansProp.level = -1;
}

CDlgMeridians::~CDlgMeridians()
{

}

bool CDlgMeridians::OnInitDialog()
{
	char* strBtnName[] = { "Btn_Impact1","Btn_Impact2","Btn_Impact3","Btn_Impact4","Btn_Impact5",
		"Btn_Impact6","Btn_Impact7","Btn_Impact8","Btn_Impact9","Btn_Impact10",
		"Btn_Impact11","Btn_Impact12","Btn_Impact13","Btn_Impact14","Btn_Impact15",
		"Btn_Impact16","Btn_Impact17","Btn_Impact18","Btn_Impact19","Btn_Impact20",};

	int i;
	PAUISTILLIMAGEBUTTON pbtn;
	// 第一境界
	for(i = CECMeridians::FRONT_MERIDIAN_HUIYIN;i < CECMeridians::MERIDIAN_LEVEL_THRESHOLD;i++)
	{
		pbtn = (PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[i]);
		m_btns[0].push_back(pbtn);
		if(pbtn)
			pbtn->SetData(i);
	}
	// 第二境界
	for(i = CECMeridians::FRONT_MERIDIAN_HUIYIN;i <= CECMeridians::FRONT_MERIDIAN_TIANTU;i++)
	{
		m_btns[1].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[i]));
	}
	for(i = CECMeridians::BACK_MERIDIAN_SHENTING;i >= CECMeridians::BACK_MERIDIAN_CHANGQIANG;i--)
	{
		m_btns[1].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[i]));
	}
	
	// 第三境界
	for(i = CECMeridians::BACK_MERIDIAN_CHANGQIANG;i <= CECMeridians::BACK_MERIDIAN_SHENTING;i++)
	{
		m_btns[2].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[i]));
	}
	for(i = CECMeridians::FRONT_MERIDIAN_TIANTU;i >= CECMeridians::FRONT_MERIDIAN_HUIYIN;i--)
	{
		m_btns[2].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[i]));
	}
	
	// 第四境界
	for(i = 0;i < 9;i++)
	{
		m_btns[3].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[CECMeridians::BACK_MERIDIAN_CHANGQIANG + 9 - i]));
		m_btns[3].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[CECMeridians::FRONT_MERIDIAN_HUIYIN  + i]));
	}
	m_btns[3].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[CECMeridians::BACK_MERIDIAN_CHANGQIANG]));

	// 道法自然
	for(i = CECMeridians::FRONT_MERIDIAN_HUIYIN;i < CECMeridians::MERIDIAN_LEVEL_THRESHOLD;i++)
	{
		m_btns[4].push_back((PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[i]));
	}
	// 状态图片
	m_pbtnImages[IMAGE_IMPACTED]		= "version02\\系统\\灵脉\\按钮已冲.tga";
	m_pbtnImages[IMAGE_TO_IMPACT]		= "version02\\系统\\灵脉\\按钮可冲.tga";
	m_pbtnImages[IMAGE_WAIT_IMPACT]		= "version02\\系统\\灵脉\\按钮不可用.tga";
	m_pbtnImages[IMAGE_TO_IMPACT_HOVER] = "version02\\系统\\灵脉\\按钮可冲高亮.tga";
	m_pbtnImages[IMAGE_TO_IMPACT_DOWN]  = "version02\\系统\\灵脉\\按钮已冲按下.tga";
	// 经脉gfx
	DDX_Control("Img_Gfx", m_pMeridiansGfx);
	DDX_Control("Btn_Impact20", m_pLevelGfx);
	DDX_Control("Img_Text_Gfx", m_pTextGfx);
	m_pLevelGfx->SetData(19);

	return true;
}

void CDlgMeridians::OnTick()
{
	CDlgBase::OnTick();
	if (m_pLevelGfx->IsShow())
	{
		A3DGFXEx* pGfx = m_pLevelGfx->GetGfx();
		if (pGfx && pGfx->GetState() == ST_STOP)
		{
			const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();
			int layer = meridianProp.level / CECMeridians::MERIDIAN_LEVEL_COUNT;
			m_pLevelGfx->SetGfx(MERIDIANS_LEVEL[layer][0]);
			m_pLevelGfx->Resize();
			// 境界按钮按下后，播放完gfx再显示冲击界面
			GetGameUIMan()->m_pDlgMeridiansImpact->Show(true);
		}
	}
}
void CDlgMeridians::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	OnMeridiansProcess();
	CDlgMeridiansImpact *pDlg = GetGameUIMan()->m_pDlgMeridiansImpact;
	A3DVIEWPORTPARAM * param = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	this->SetPosEx(0,(param->Height - this->GetSize().cy) / 2);
}

void CDlgMeridians::OnCommandImpact(const char *szCommand)
{
	// 级别是否达到可以冲击的级别
	int minLevelToImpact(0);
	if (!GetGameRun()->GetHostPlayer()->IsLevelMeetMeridianReq(minLevelToImpact))
	{
		ACString temp;
		temp.Format(GetStringFromTable(10226), minLevelToImpact);
		GetGameUIMan()->MessageBox("", temp, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	CDlgMeridiansImpact *pDlg = GetGameUIMan()->m_pDlgMeridiansImpact;
	PAUIOBJECT pObj = pDlg->GetDlgItem("Img_Gfx");
	if (pObj)
	{
		int iAlignCenterOffsetX = m_pLevelGfx->GetDefaultPos().x + m_pLevelGfx->GetDefaultSize().cx / 2;
		int iAlignOffsetX = iAlignCenterOffsetX - pObj->GetDefaultPos().x - pObj->GetDefaultSize().cx / 2;
		pDlg->AlignTo(this,AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT,
			AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP, iAlignOffsetX, 0);
	}

	const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();
	PAUIOBJECT pbtn = GetDlgItem(szCommand);
	if(pbtn)
	{
		int layer = (meridianProp.level + 1) / CECMeridians::MERIDIAN_LEVEL_COUNT;
		int level = min(4, meridianProp.level / CECMeridians::MERIDIAN_LEVEL_COUNT);
		int layerLevel = pbtn->GetData();
		if(layerLevel == CECMeridians::MERIDIAN_LEVEL_THRESHOLD)
		{
			pDlg->SetImpactName(GetStringFromTable(10213 + layer));
			m_pLevelGfx->SetGfx(MERIDIANS_LEVEL[level][1]);
			m_pLevelGfx->Resize();
		}
		else
		{
			pDlg->SetImpactName(GetStringFromTable(10180 + layerLevel));
			// 普通冲击按钮按下后立即显示冲击界面
			pDlg->Show(true);
		}
	}
}

void CDlgMeridians::OnEventLButtonUp_LevelGfx(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnCommandImpact("Btn_Impact20");
}

void CDlgMeridians::OnCommandCancel(const char * szCommand)
{
	Show(false);
}

void CDlgMeridians::OnMeridiansProcess()
{
	const CECPlayer::MeridiansProp& meridianProp = GetGameRun()->GetHostPlayer()->GetMeridiansProp();

	SetBtnState(meridianProp.level);
	SetLayerInfo(meridianProp.level);

	// todo: 转换动画
	{

	}

	gMeridiansProp = meridianProp;
}

void CDlgMeridians::SetLayerInfo(int level)
{
	int layer = level / CECMeridians::MERIDIAN_LEVEL_COUNT;
	if(level >= 0 && layer <= CECMeridians::MeridiansLevelLayer && layer != m_iLastLayer)
	{
		m_iLastLayer = layer;
		//	境界图片
		PAUIIMAGEPICTURE pImg_Layer = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Layer"));
		if (pImg_Layer)
		{
			pImg_Layer->FixFrame(layer);
		}

		// 设置描述文字gfx
		m_pTextGfx->SetGfx(MERIDIANS_TEXT_GFX[layer]);
		m_pTextGfx->Resize();
		
		// 设置经脉gfx
		m_pMeridiansGfx->SetGfx(MERIDIANS_LAYER[layer]);
		m_pMeridiansGfx->Resize();
	}
	
}
void CDlgMeridians::SetBtnState(int level)
{
	int i;
	AUIOBJECT_SETPROPERTY prop;

	if(level >= 0 && level < CECMeridians::MeridiansLevelLayer * CECMeridians::MERIDIAN_LEVEL_COUNT)
	{
		int layer = level / CECMeridians::MERIDIAN_LEVEL_COUNT;
		const MeridiansBtnVec& vecBtn = m_btns[layer];
		int levelInLayer = level % CECMeridians::MERIDIAN_LEVEL_COUNT;
		
		// 已冲
		for(i = 0;i < levelInLayer;i++)
		{
			vecBtn[i]->Enable(false);
			strcpy(prop.fn,m_pbtnImages[IMAGE_IMPACTED]);
			ScopedAUIControlSpriteModify _dummy(vecBtn[i]);
			vecBtn[i]->SetProperty("Disabled Frame File",&prop);
		}
		// 可冲
		if(levelInLayer < CECMeridians::MERIDIAN_LEVEL_THRESHOLD)
		{
			vecBtn[levelInLayer]->Enable(true);

			ScopedAUIControlSpriteModify _dummy(vecBtn[levelInLayer]);

			strcpy(prop.fn,m_pbtnImages[IMAGE_TO_IMPACT]);
			vecBtn[levelInLayer]->SetProperty("Up Frame File",&prop);
			strcpy(prop.fn,m_pbtnImages[IMAGE_TO_IMPACT_HOVER]);
			vecBtn[levelInLayer]->SetProperty("OnHover Frame File",&prop);
			strcpy(prop.fn,m_pbtnImages[IMAGE_TO_IMPACT_DOWN]);
			vecBtn[levelInLayer]->SetProperty("Down Frame File",&prop);
		}
		// 未冲
		for(i = levelInLayer + 1;i < CECMeridians::MERIDIAN_LEVEL_THRESHOLD;i++)
		{
			vecBtn[i]->Enable(false);
			strcpy(prop.fn,m_pbtnImages[IMAGE_WAIT_IMPACT]);
			ScopedAUIControlSpriteModify _dummy(vecBtn[i]);
			vecBtn[i]->SetProperty("Disabled Frame File",&prop);
		}
		// 境界按钮
		m_pLevelGfx->SetGfx(MERIDIANS_LEVEL[layer][0]);
		m_pLevelGfx->Resize();
		m_pLevelGfx->Show(levelInLayer == CECMeridians::MERIDIAN_LEVEL_THRESHOLD);

		if (levelInLayer < (int)vecBtn.size())
			vecBtn[levelInLayer]->Enable(true);
	}
	else if(level == CECMeridians::MeridiansLevelLayer * CECMeridians::MERIDIAN_LEVEL_COUNT)
	{
		const MeridiansBtnVec& vecBtn = m_btns[level / CECMeridians::MERIDIAN_LEVEL_COUNT];
		// 已冲
		for(i = 0;i < CECMeridians::MERIDIAN_LEVEL_THRESHOLD;i++)
		{
			vecBtn[i]->Enable(false);
			strcpy(prop.fn,m_pbtnImages[IMAGE_IMPACTED]);			
			ScopedAUIControlSpriteModify _dummy(vecBtn[i]);
			vecBtn[i]->SetProperty("Up Frame File",&prop);
			vecBtn[i]->SetProperty("OnHover Frame File",&prop);
			vecBtn[i]->SetProperty("Down Frame File",&prop);
		}
		m_pLevelGfx->Show(false);
	}
	// 设置已获得属性
	int iProp[5];
	CECMeridians::GetSingleton().GetLevelPropBonus(GetHostPlayer()->GetProfession(),level,iProp[0],iProp[1],iProp[2],iProp[3],iProp[4]);
	
	ACString temp;
	PAUILABEL pLabelProp[5];
	//
	pLabelProp[0] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_HP"));
	pLabelProp[1] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_PhyDefence"));
	pLabelProp[2] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_MgiDefence"));
	pLabelProp[3] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_PhyAttack"));
	pLabelProp[4] = dynamic_cast<PAUILABEL>(GetDlgItem("Label_MgiAttack"));
	
	for(i = 0;i < 5;i++)
	{
		temp.Format(GetStringFromTable(10220 + i),iProp[i]);
		if(pLabelProp[i])
			pLabelProp[i]->SetText(temp);
	}
}

void CDlgMeridians::OnChangeLayoutEnd(bool bAllDone)
{
	OnMeridiansProcess();
}