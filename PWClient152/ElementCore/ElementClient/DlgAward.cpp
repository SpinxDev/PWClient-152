// Filename	: DlgAward.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "AFI.h"
#include "DlgAward.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "AUIRadioButton.h"
#include "elementdataman.h"
#include "TaskTemplMan.h"
#include "EC_TaskInterface.h"
#include "EC_HostPlayer.h"
#include "AUICTranslate.h"
#include "AUIImagePicture.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAward, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("select",		OnCommand_select)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

const int MIN_ROW_COUNT_TO_SHOW = 6;

CDlgAward::CDlgAward()
{
}

CDlgAward::~CDlgAward()
{
}

bool CDlgAward::OnInitDialog()
{
	m_pBtn_Confirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Confirm");
	m_bThroughNPC = false;
	m_bSpendContribution = false;
	return true;
}

void CDlgAward::OnCommand_confirm(const char * szCommand)
{
	int i;
	char szName[40];
	PAUIRADIOBUTTON pRadio;
	int idTask = GetData();

	for( i = 1; ; i++ )
	{
		sprintf(szName, "Rdo_Award%d", i);
		pRadio = (PAUIRADIOBUTTON)GetDlgItem(szName);
		if( !pRadio || !pRadio->IsShow() ) break;

		if( pRadio->IsChecked() )
		{
			if (m_bThroughNPC) {
				GetGameSession()->c2s_CmdNPCSevReturnTask(idTask, i - 1);
				GetGameUIMan()->EndNPCService();
			} else if (m_bSpendContribution) {
				CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
				pTask->FinishTaskSpendingWorldContribution(idTask, i - 1);
			} else {
				CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
				pTask->ChooseAward(idTask, i - 1);
			}
			Show(false);
			break;
		}
	}
}

void CDlgAward::OnCommand_select(const char * szCommand)
{
	m_pBtn_Confirm->Enable(true);
}

void CDlgAward::OnCommand_CANCEL(const char * szCommand)
{
	if (m_bThroughNPC)
		GetGameUIMan()->EndNPCService();
	Show(false);
}

void CDlgAward::UpdateAwardItem(unsigned short idTask, bool throughNPC, bool spendContribution)
{
	AWARD_DATA ad;
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	m_bThroughNPC = throughNPC;
	m_bSpendContribution = spendContribution;
	pTask->GetAwardCandidates(idTask, &ad);
	if( ad.m_ulCandItems > 1 )
	{
		char szName[40];
		AString strFile;
		ACHAR szText[40];
		CECIvtrItem *pItem;
		AUICTranslate trans;
		int i, j, idItem, nNum;
		PAUIIMAGEPICTURE pImage;
		PAUIIMAGEPICTURE pImage2;
		
		SetData(idTask);
		CheckRadioButton(1, 0);
		GetDlgItem("Btn_Confirm")->Enable(false);
		
		for( i = 1; ; i++ )
		{
			sprintf(szName, "Item_%02d01", i);
			pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
			if( !pImage ) break;		// All done.
			
			sprintf(szName, "Rdo_Award%d", i);
			GetDlgItem(szName)->Show(false);
			
			for( j = 1; ; j++ )
			{
				sprintf(szName, "Item_%02d%02d", i, j);
				pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				if( !pImage ) break;	// Line done.
				
				if( i - 1 < (int)ad.m_ulCandItems &&
					j - 1 < (int)ad.m_CandItems[i - 1].m_ulAwardItems )
				{
					idItem = ad.m_CandItems[i - 1].m_AwardItems[j - 1].m_ulItemTemplId;
					pItem = CECIvtrItem::CreateItem(idItem, 0, 1);
					
					nNum = ad.m_CandItems[i - 1].m_AwardItems[j - 1].m_ulItemNum;
					if( nNum > 1 )
					{
						a_sprintf(szText, _AL("%d"), nNum);
						pImage->SetText(szText);
					}
					else
						pImage->SetText(_AL(""));
					
					pItem->GetDetailDataFromLocal();
					pImage->SetHint(trans.Translate(pItem->GetDesc()));
					af_GetFileTitle(pItem->GetIconFile(), strFile);
					strFile.MakeLower();
					pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
					
					pImage->SetColor(
						(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
						? A3DCOLORRGB(128, 128, 128)
						: A3DCOLORRGB(255, 255, 255));
					
					delete pItem;
					
					sprintf(szName, "Rdo_Award%d", i);
					GetDlgItem(szName)->Show(true);
				}
				else
				{
					pImage->ClearCover();
					pImage->SetHint(_AL(""));
					pImage->SetText(_AL(""));
				}
			}
		}

		if (ad.m_ulCandItems <= MIN_ROW_COUNT_TO_SHOW) {
			// 如果行数较少，则隐藏剩余行
			for (i = MIN_ROW_COUNT_TO_SHOW + 1; ; i++) {
				sprintf(szName, "Item_%02d01", i);
				pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				if( !pImage ) break;		// All done.
				
				sprintf(szName, "Rdo_Award%d", i);
				GetDlgItem(szName)->Show(false);
				for (j = 1; ; j++) {
					sprintf(szName, "Item_%02d%02d", i, j);
					pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
					if( !pImage ) break;
					pImage->Show(false);

					pImage2 = (PAUIIMAGEPICTURE)GetDlgItem(AString().Format("Img_%02d%02d", i, j));
					pImage2->Show(false);
				}
			}
			// 设置确认按钮的位置，设置对话框大小
			AUIObject* pBtnConfirm = GetDlgItem("Btn_Confirm");
			AUIObject* pImgLineMid = GetDlgItem(AString().Format("Item_%02d01", MIN_ROW_COUNT_TO_SHOW));
			AUIObject* pEdiBg = GetDlgItem("background");
			pBtnConfirm->SetPos(pBtnConfirm->GetPos(true).x, pImgLineMid->GetPos(true).y + pImgLineMid->GetSize().cy + 10);
			SetSize(GetSize().cx, pBtnConfirm->GetPos(true).y + pBtnConfirm->GetSize().cy + 30);
			pEdiBg->SetSize(pEdiBg->GetSize().cx, GetSize().cy - pEdiBg->GetPos(true).y - 25);
		} else {
			// 否则，显示全部行
			for (i = MIN_ROW_COUNT_TO_SHOW + 1; ; i++) {
				sprintf(szName, "Item_%02d01", i);
				pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				if( !pImage ) break;		// All done.
				
				if (ad.m_ulCandItems >= i) {
					sprintf(szName, "Rdo_Award%d", i);
					GetDlgItem(szName)->Show(true);
				}
				for (j = 1; ; j++) {
					sprintf(szName, "Item_%02d%02d", i, j);
					pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
					if( !pImage ) break;
					pImage->Show(true);
					
					pImage2 = (PAUIIMAGEPICTURE)GetDlgItem(AString().Format("Img_%02d%02d", i, j));
					pImage2->Show(true);
				}
			}
			// 设置确认按钮的位置，设置对话框大小
			AUIObject* pBtnConfirm = GetDlgItem("Btn_Confirm");
			AUIObject* pImgLineLast = GetDlgItem(AString().Format("Item_%02d01", i - 1));
			AUIObject* pEdiBg = GetDlgItem("background");
			pBtnConfirm->SetPos(pBtnConfirm->GetPos(true).x, pImgLineLast->GetPos(true).y + pImgLineLast->GetSize().cy + 10);
			SetSize(GetSize().cx, pBtnConfirm->GetPos(true).y + pBtnConfirm->GetSize().cy + 30);
			pEdiBg->SetSize(pEdiBg->GetSize().cx, GetSize().cy - pEdiBg->GetPos(true).y - 25);
		}
	}
}
