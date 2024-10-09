// File		: DlgFriendColor.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "DlgFriendColor.h"
#include "DlgFriendList.h"
#include "EC_GameUIMan.h"
#include "EC_Friend.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendColor, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("start",		OnCommandStart)
AUI_ON_COMMAND("Color*",	OnCommandColor)

AUI_END_COMMAND_MAP()

CDlgFriendColor::CDlgFriendColor()
{
	m_pSliderColorR = NULL;
	m_pSliderColorG = NULL;
	m_pSliderColorB = NULL;
	m_pTxtColorR = NULL;
	m_pTxtColorG = NULL;
	m_pTxtColorB = NULL;
	m_pImgColor = NULL;
}

CDlgFriendColor::~CDlgFriendColor()
{
}

void CDlgFriendColor::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Slider_ColorR", m_pSliderColorR);
	DDX_Control("Slider_ColorG", m_pSliderColorG);
	DDX_Control("Slider_ColorB", m_pSliderColorB);
	DDX_Control("Txt_ColorR", m_pTxtColorR);
	DDX_Control("Txt_ColorG", m_pTxtColorG);
	DDX_Control("Txt_ColorB", m_pTxtColorB);
	DDX_Control("Img_Color", m_pImgColor);
}

void CDlgFriendColor::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
}

void CDlgFriendColor::OnCommandStart(const char *szCommand)
{
	int r = m_pSliderColorR->GetLevel();
	int g = m_pSliderColorG->GetLevel();
	int b = m_pSliderColorB->GetLevel();
	CDlgFriendList* pDlgFriend = GetGameUIMan()->m_pDlgFriendList;
	PAUITREEVIEW pTree = pDlgFriend->m_pTvFriendList;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	if( !pItem ) return;

	int idGroup = GetData();
	CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();

	pTree->SetItemTextColor(pItem, A3DCOLORRGB(r, g, b));
	pMan->SetGroupColor(idGroup, A3DCOLORRGB(r, g, b));
	Show(false);
}

void CDlgFriendColor::OnCommandColor(const char *szCommand)
{
	int r = m_pSliderColorR->GetLevel();
	int g = m_pSliderColorG->GetLevel();
	int b = m_pSliderColorB->GetLevel();
	ACHAR szText[40];

	a_sprintf(szText, _AL("%d"), r);
	m_pTxtColorR->SetText(szText);

	a_sprintf(szText, _AL("%d"), g);
	m_pTxtColorG->SetText(szText);

	a_sprintf(szText, _AL("%d"), b);
	m_pTxtColorB->SetText(szText);

	m_pImgColor->SetColor(A3DCOLORRGB(r, g, b));
}
