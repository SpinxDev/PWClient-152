// File		: DlgPwdProtect.cpp
// Creator	: Feng Ning
// Date		: 2010/10/20

#include "AFI.h"
#include "AIniFile.h"

#include "DlgPwdProtect.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_LoginUIMan.h"

#include "matrixchallenge.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPwdProtect, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_Cancel)
AUI_ON_COMMAND("link", OnCommand_Link)
AUI_ON_COMMAND("confirm", OnCommand_Confirm)
AUI_ON_COMMAND("clear", OnCommand_Clear)
AUI_END_COMMAND_MAP()


int CDlgPwdProtect::GetProtectType()
{
	int ret(-1);

	const char *szName = GetName();
	if (!stricmp(szName, "Win_PwdProtect")) ret = 0;
	else if (!stricmp(szName, "Win_PwdProtect1")) ret = 1;
	else if (!stricmp(szName, "Win_PwdProtect2")) ret = 2;

	return ret;
}

bool CDlgPwdProtect::CanConfirm()
{
	bool bEnable = false;

	int type = GetProtectType();
	switch (type)
	{
	case 0:
		bEnable = a_strlen(GetDlgItem("Input_1")->GetText()) > 0 &&
			a_strlen(GetDlgItem("Input_2")->GetText()) > 0 &&
			a_strlen(GetDlgItem("Input_3")->GetText()) > 0;
		break;

	case 1:
		bEnable = a_strlen(GetDlgItem("Input_1")->GetText()) > 0;
		break;

	case 2:
		bEnable = a_strlen(GetDlgItem("Input_1")->GetText()) == 6;
		break;
	}

	return bEnable;
}

void CDlgPwdProtect::OnTick()
{
	PAUIOBJECT pObj = GetDlgItem("Btn_Decide");
	if (pObj) pObj->Enable(CanConfirm());
	CDlgBase::OnTick();
}

void CDlgPwdProtect::OnCommand_Cancel(const char* szCommand)
{
	int type = GetProtectType();
	int idMsg = (type == 2) ? 238 : 226;
	m_pAUIManager->MessageBox("PwdProtect_Cancel", 
		GetStringFromTable(idMsg), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgPwdProtect::OnCommand_Link(const char* szCommand)
{
	AString strURL;

	const char *szKey = NULL;
	int type = GetProtectType();
	switch (type)
	{
	case 0: szKey = "Link2"; break;
	case 1: szKey = "Link3"; break;
	case 2: szKey = "Link4"; break;
	}
	if (szKey != NULL)
		strURL = GetBaseUIMan()->GetURL("PWDPROTECT", szKey);

	GetBaseUIMan()->NavigateURL(strURL);
}

void CDlgPwdProtect::OnCommand_Confirm(const char* szCommand)
{
	if( !g_pGame->GetGameSession()->IsConnected() )
	{
		Show(false);
		m_pAUIManager->GetDialog("Win_Login")->Show(true);
		m_pAUIManager->MessageBox("", GetStringFromTable(225), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		if (!CanConfirm()) return;
		
		int ret(0);
		int type = GetProtectType();
		switch (type)
		{
		case 0:
			{
				int i1 = a_atoi(GetDlgItem("Input_1")->GetText());
				int i2 = a_atoi(GetDlgItem("Input_2")->GetText());
				int i3 = a_atoi(GetDlgItem("Input_3")->GetText());
				ret = i1 * 10000 + i2 * 100 + i3;
				break;
			}

		case 1:
		case 2:
			ret = a_atoi(GetDlgItem("Input_1")->GetText());
			break;
		}
		
		g_pGame->GetGameSession()->matrix_response(ret);
		Show(false);
		m_pAUIManager->GetDialog("Win_LoginWait")->Show(true);
	}
}

void CDlgPwdProtect::OnCommand_Clear(const char* szCommand)
{
	int type = GetProtectType();
	switch (type)
	{
	case 0:
		GetDlgItem("Input_1")->SetText(_AL(""));
		GetDlgItem("Input_2")->SetText(_AL(""));
		GetDlgItem("Input_3")->SetText(_AL(""));
		break;
		
	case 1:
	case 2:
		GetDlgItem("Input_1")->SetText(_AL(""));
		break;
	}
	ChangeFocus(GetDlgItem("Input_1"));
}

void CDlgPwdProtect::HideMatrix()
{
	PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_PwdProtect");
	pDlg->Show(false);
	PAUIDIALOG pDlg1 = m_pAUIManager->GetDialog("Win_PwdProtect1");
	pDlg1->Show(false);
	PAUIDIALOG pDlg2 = m_pAUIManager->GetDialog("Win_PwdProtect2");
	pDlg2->Show(false);
}

bool CDlgPwdProtect::ShowMatrix(GNET::MatrixChallenge* p)
{
	int pos = p->nonce;

	if( HIWORD(p->algorithm) == 1 )
	{
		PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_PwdProtect");
		if (pDlg)
		{
			ACString strText;
			unsigned int idx0, idx1, idx2;
			idx0 = pos % 80;
			pos /= 80; 
			idx1 = pos % 80;
			pos /= 80; 
			idx2 = pos % 80;
			strText.Format(_AL("%c%d"), (idx0 >> 3) + 'A', (idx0 & 7) + 1);
			pDlg->GetDlgItem("txt_1")->SetText(strText);
			strText.Format(_AL("%c%d"), (idx1 >> 3) + 'A', (idx1 & 7) + 1);
			pDlg->GetDlgItem("txt_2")->SetText(strText);
			strText.Format(_AL("%c%d"), (idx2 >> 3) + 'A', (idx2 & 7) + 1);
			pDlg->GetDlgItem("txt_3")->SetText(strText);
			pDlg->GetDlgItem("Input_1")->SetText(_AL(""));
			pDlg->GetDlgItem("Input_2")->SetText(_AL(""));
			pDlg->GetDlgItem("Input_3")->SetText(_AL(""));
			dynamic_cast<PAUIEDITBOX>(pDlg->GetDlgItem("Input_1"))->SetIsNumberOnly(true);
			dynamic_cast<PAUIEDITBOX>(pDlg->GetDlgItem("Input_2"))->SetIsNumberOnly(true);
			dynamic_cast<PAUIEDITBOX>(pDlg->GetDlgItem("Input_3"))->SetIsNumberOnly(true);
			pDlg->Show(true);
			pDlg->ChangeFocus(pDlg->GetDlgItem("Input_1"));
			
			m_pAUIManager->GetDialog("Win_LoginWait")->Show(false);
		}
	}
	else if( HIWORD(p->algorithm) == 2 )
	{
		PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_PwdProtect1");
		if (pDlg)
		{
			ACString strText;
			strText.Format(_AL("%d"), pos);
			pDlg->GetDlgItem("txt_1")->SetText(strText);
			pDlg->GetDlgItem("Input_1")->SetText(_AL(""));
			dynamic_cast<PAUIEDITBOX>(pDlg->GetDlgItem("Input_1"))->SetIsNumberOnly(true);
			pDlg->Show(true);
			pDlg->ChangeFocus(pDlg->GetDlgItem("Input_1"));
			
			m_pAUIManager->GetDialog("Win_LoginWait")->Show(false);
		}
	}
	else if(HIWORD(p->algorithm) == 6 )
	{
		PAUIDIALOG pDlg = m_pAUIManager->GetDialog("Win_PwdProtect2");
		if (pDlg)
		{
			pDlg->GetDlgItem("Input_1")->SetText(_AL(""));
			dynamic_cast<PAUIEDITBOX>(pDlg->GetDlgItem("Input_1"))->SetIsNumberOnly(true);
			pDlg->Show(true);
			pDlg->ChangeFocus(pDlg->GetDlgItem("Input_1"));
			
			m_pAUIManager->GetDialog("Win_LoginWait")->Show(false);
		}
	}

	return true;
}
