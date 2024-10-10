// File		: DlgItemDesc.cpp
// Creator	: Feng Ning
// Date		: 2010/11/01

#include "DlgItemDesc.h"
#include "DlgGeneralCard.h"
#include "ExpTypes.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "AUICTranslate.h"
#include "AUIStillImageButton.h"
#include "AUILabel.h"
#include "A3DFTFont.h"
#include "AUIManager.h"

#define new A_DEBUG_NEW
AUI_BEGIN_COMMAND_MAP(CDlgItemDesc, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgItemDesc, CDlgBase)
AUI_END_EVENT_MAP()
CDlgItemDesc::CDlgItemDesc():
m_pItemHover(NULL)
{
}
void CDlgItemDesc::OnCommandCancel(const char* szCommand)
{
	Show(false);
	CDlgGeneralCard* pDlg = NULL;
	if (stricmp(GetName(), "Win_GeneralCardDescHover") == 0) {
		pDlg = dynamic_cast<CDlgGeneralCard*>(GetGameUIMan()->GetDialog("Win_GeneralCardHintHover"));
	}
	else {
		pDlg = dynamic_cast<CDlgGeneralCard*>(GetGameUIMan()->GetDialog("Win_GeneralCardHint"));
	}
	if (pDlg && pDlg->IsShow()) pDlg->Show(false);
	m_pItemHover = NULL;

	if (stricmp(GetName(), "Win_ItemDesc") == 0) {
		GetGameUIMan()->GetDialog("Win_ItemDesc2")->Show(false);
		GetGameUIMan()->GetDialog("Win_ItemDesc3")->Show(false);
	} else if (stricmp(GetName(), "Win_GeneralCardDescHover") == 0) {
		GetGameUIMan()->GetDialog("Win_GeneralCardDescHover2")->Show(false);
	}
}
void CDlgItemDesc::ShowDesc(CECIvtrItem* pItem, int x, int y)
{
	ASSERT(AString(GetName()) == "Win_ItemDesc" || AString(GetName()) == "Win_GeneralCardDescHover");

	if (AString(GetName()) == "Win_ItemDesc") {
		OnCommandCancel(NULL);
	}	
	if( pItem->GetDesc() )
	{
		AUICTranslate trans;
		m_Desc = trans.Translate(pItem->GetDesc());
		m_HintX = x;
		m_HintY = y;
		RefreshDesc();

		
		// ��ʾ���ϵ�װ�����бȽϣ������������ʾ�򣨰�������ͼ��
		AUIDialog* pDlgItemDesc1 = this;		// ��һ����ʾ��
		AUIDialog* pDlgItemDesc2 = NULL;		// �ڶ�����ʾ��
		AUIDialog* pDlgItemDesc3 = NULL;		// ��������ʾ��

		if (GetGameUIMan()->IsShowItemDescCompare()) {
			std::vector<CECIvtrItem*> relatedItems;
			GetGameUIMan()->GetRelatedEquipItems(pItem, relatedItems);
			if (AString(GetName()) == "Win_ItemDesc") {
				// �ǿ�������
				if (relatedItems.size() > 0) {
					pDlgItemDesc2 = GetGameUIMan()->GetDialog("Win_ItemDesc2");
					pDlgItemDesc2->Show(true);
					dynamic_cast<CDlgItemDesc*>(pDlgItemDesc2)->ShowDesc(
						GetGameUIMan()->GetItemDescWithAlreadyEquipped(relatedItems[0]),
						x + GetSize().cx, y);
					if (relatedItems.size() > 1) {
						pDlgItemDesc3 = (GetGameUIMan()->GetDialog("Win_ItemDesc3"));
						pDlgItemDesc3->Show(true);
						dynamic_cast<CDlgItemDesc*>(pDlgItemDesc3)->ShowDesc(
							GetGameUIMan()->GetItemDescWithAlreadyEquipped(relatedItems[1]),
							pDlgItemDesc2->GetPos().x + pDlgItemDesc2->GetSize().cx, y);
					}
				}
			} else if (AString(GetName()) == "Win_GeneralCardDescHover") {
				// ��������
				if (relatedItems.size() > 0) {
					pDlgItemDesc2 = (GetGameUIMan()->GetDialog("Win_GeneralCardDescHover2"));
					pDlgItemDesc2->Show(true);
					dynamic_cast<CDlgItemDesc*>(pDlgItemDesc2)->ShowDesc(
						GetGameUIMan()->GetItemDescWithAlreadyEquipped(relatedItems[0]),
						x + GetSize().cx, y);
				}
			}
		}
		
		

		if (dynamic_cast<CECIvtrGeneralCard*>(pItem)) {
			CDlgGeneralCard* pDlg = NULL;
			if (stricmp(GetName(), "Win_GeneralCardDescHover") == 0)
				pDlg = dynamic_cast<CDlgGeneralCard*>(GetGameUIMan()->GetDialog("Win_GeneralCardHintHover"));
			else 
				pDlg = dynamic_cast<CDlgGeneralCard*>(GetGameUIMan()->GetDialog("Win_GeneralCardHint"));
			if (pDlg) {
				pDlg->SetCardImage(pItem);
				if (pDlgItemDesc2 == NULL) {
					pDlgItemDesc2 = pDlg;
				} else if (pDlgItemDesc3 == NULL) {
					pDlgItemDesc3 = pDlg;
				} else {
					ASSERT(false && "more than 3 item desc dlg3!");
				}
				if (pDlgItemDesc3) {
					// ����ǿ�������װ���ȶԣ��򽻻���ʾ���λ��
					std::swap(pDlgItemDesc1, pDlgItemDesc2);
				}
				pDlg->Show(true);
			}
		}
		// ��󣬵����������λ��
		int width = pDlgItemDesc1->GetSize().cx;
		if (pDlgItemDesc2) {
			width += pDlgItemDesc2->GetSize().cx;
		}
		if (pDlgItemDesc3) {
			width += pDlgItemDesc3->GetSize().cx;
		}
		
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		if (x < (int)p->Width / 2) {
			// ���������ſ�
			int xPos = x + 24;
			if (xPos + width > p->Width) {
				xPos = p->Width - width;
			}
			pDlgItemDesc1->SetPosEx(xPos, pDlgItemDesc1->GetPos().y);
			if (pDlgItemDesc2) {
				pDlgItemDesc2->AlignTo(pDlgItemDesc1, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_RIGHT, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
			}
			if (pDlgItemDesc3) {
				pDlgItemDesc3->AlignTo(pDlgItemDesc2, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_RIGHT, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
			}
		} else {
			// ���������ſ�
			int xPos = x - 24 - pDlgItemDesc1->GetSize().cx;
			if (width > x) {
				xPos = width - pDlgItemDesc1->GetSize().cx;;
			}
			pDlgItemDesc1->SetPosEx(xPos, pDlgItemDesc1->GetPos().y);
			if (pDlgItemDesc2) {
				pDlgItemDesc2->AlignTo(pDlgItemDesc1, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
			}
			if (pDlgItemDesc3) {
				pDlgItemDesc3->AlignTo(pDlgItemDesc2, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP);
			}
		}
		Show(true);
	}
}

void CDlgItemDesc::ShowDesc(const ACString& strDesc, int x, int y)
{
	AUICTranslate trans;
	m_Desc = trans.Translate(strDesc);
	m_HintX = x;
	m_HintY = y;
	RefreshDesc();
	Show(true);
}

void CDlgItemDesc::RefreshDesc()
{
	PAUISTILLIMAGEBUTTON pClose= (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Close");
	PAUILABEL pDesc = (PAUILABEL)GetDlgItem("Txt_Desc");
	
	const int margin = 6;

	int nWidth, nHeight, nLines;
	A3DFTFont *pFont = pDesc->GetFont();
	pFont->Resize(pDesc->GetFontHeight() * m_pAUIManager->GetWindowScale());
	AUI_GetTextRect(pFont, m_Desc, nWidth, nHeight, nLines, 3);
	SIZE s = this->GetFrame()->GetSizeLimit();
	int W = nWidth + s.cx + margin;
	int H = s.cy + nHeight + margin;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	if (m_pItemHover) {
		m_HintX = m_HintX > (int)p->Width / 2 ? m_HintX - W - 12 : m_HintX + 12;
		m_HintY = m_HintY > (int)p->Height / 2 ? m_HintY - H - 12 : m_HintY + 12;
	} else {
		m_HintY = m_HintY > (int)p->Height / 2 ? m_HintY - H : m_HintY + 6;
	}
	this->SetSize(W, H);
	this->SetPosEx(m_HintX, m_HintY);
	pDesc->SetPos(9, 12);
	pDesc->SetText(m_Desc);
	if (pClose) pClose->SetPos(W - pClose->GetSize().cx, 0);
}

void CDlgItemDesc::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		RefreshDesc();
	}
}

bool CDlgItemDesc::CallBackGeneralCard(const POINT &pt, DWORD param1, DWORD param2, DWORD param3)
{
	CECIvtrItem* pItem = (CECIvtrItem*)param1;
	CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pItem);
	bool ret(false);
	CDlgItemDesc* pDlg = (CDlgItemDesc*)
		(g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetDialog("Win_GeneralCardDescHover"));
	if (pDlg) {
		if (pCard) {
			if (pDlg->m_pItemHover != pItem) {
				pDlg->m_pItemHover = pItem;
				pDlg->ShowDesc(pItem, pt.x, pt.y);
			}
			ret = true;
		} else pDlg->OnCommandCancel(NULL);
	}
	return ret;
}
bool CDlgItemDesc::IsDlgOverlaped(PAUIDIALOG dlg1, PAUIDIALOG dlg2)
{
	bool ret (false);
	if (dlg1 && dlg2) {
		A3DRECT rect1, rect2, rect_intersect;
		POINT pos1 = dlg1->GetPos();
		POINT pos2 = dlg2->GetPos();
		SIZE size1 = dlg1->GetSize();
		SIZE size2 = dlg2->GetSize();
		rect1.left = pos1.x;
		rect1.right = pos1.x + size1.cx;
		rect1.top = pos1.y;
		rect1.bottom = pos1.y + size1.cy;
		rect2.left = pos2.x;
		rect2.right = pos2.x + size2.cx;
		rect2.top = pos2.y;
		rect2.bottom = pos2.y + size2.cy;
		rect_intersect.SetIntersect(rect1, rect2);
		ret = !rect_intersect.IsEmpty();
	}
	return ret;
}