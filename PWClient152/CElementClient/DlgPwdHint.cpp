// Filename	: DlgPwdHint.cpp
// Creator	: Xu Wenbin
// Date		: 2010/02/26

#include "AFI.h"
#include "DlgPwdHint.h"
#include "AUICTranslate.h"
#include "EC_Resource.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_BaseUIMan.h"
#include "EC_InputCtrl.h"
#include <AUIManager.h>
#include <A3DEngine.h>
#include <A3DViewport.h>
#include <WindowsX.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPwdHint, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("IDOK", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPwdHint, CDlgBase)
AUI_ON_EVENT("Txt_Content", WM_LBUTTONDOWN, OnEventLButtonDown_Txt_Content)
AUI_ON_EVENT("Txt_Content", WM_MOUSEMOVE, OnEventMouseMove_Txt_Content)
AUI_END_EVENT_MAP()

bool CDlgPwdHint::OnInitDialog()
{
	m_pTxt_Content = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	return true;
}

void CDlgPwdHint::OnShowDialog()
{	
	ACString strText = GetStringFromTable(233);
	m_pTxt_Content->SetText(strText);
}

void CDlgPwdHint::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgPwdHint::OnTick()
{
	int x(0), y(0);
	GetGameRun()->GetInputCtrl()->GetMousePos(&x, &y);
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	x -= p->X;
	y -= p->Y;
	AUITEXTAREA_NAME_LINK Link;	
	GetNameLinkMouseOn(x, y, m_pTxt_Content, &Link);
}

void CDlgPwdHint::OnEventLButtonDown_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	
	AUITEXTAREA_NAME_LINK Link;
	GetNameLinkMouseOn(x, y, pObj, &Link);
	if (Link.strName.GetLength() == 0) return;

	ACString strText = GetStringFromTable(234);
	AString strURL = AC2AS(strText);
	GetBaseUIMan()->NavigateURL(strURL);

	Show(false);
}

void CDlgPwdHint::OnEventMouseMove_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	AUITEXTAREA_NAME_LINK Link;
	
	GetNameLinkMouseOn(x, y, pObj, &Link);
}

bool CDlgPwdHint::GetNameLinkMouseOn(int x, int y,
								   PAUIOBJECT pObj, P_AUITEXTAREA_NAME_LINK pLink)
{
	bool bClickedChatPart = false;
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	POINT ptPos = pObj->GetPos();
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pObj);
	
	GetGame()->ChangeCursor(RES_CUR_NORMAL);
	if( pText->GetHitArea(x - ptPos.x, y - ptPos.y) == AUITEXTAREA_RECT_FRAME )
	{
		int i;
		abase::vector<AUITEXTAREA_NAME_LINK> &vecNameLink = pText->GetNameLink();
		
		x += rcWindow.left;
		y += rcWindow.top;
		for( i = 0; i < (int)vecNameLink.size(); i++ )
		{
			if( vecNameLink[i].rc.PtInRect(x, y) )
			{
				GetGame()->ChangeCursor(RES_CUR_HAND);
				*pLink = vecNameLink[i];
				break;
			}
		}
		bClickedChatPart = true;
	}
	
	return bClickedChatPart;
}
