/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   13:04
	file base:	DlgHelp
	file ext:	cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "EC_Global.h"
#include "DlgHelp.h"
#include <AWScriptFile.h>
#include <AFI.h>
#include <A2DSprite.h>
#include <A3DEngine.h>
#include <A3DViewport.h>
#include "AUITextArea.h"
#include "AUIStillImageButton.h"
#include "AUITreeView.h"
#include "AUIImagePicture.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECScriptUI.h"


#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define new A_DEBUG_NEW


AUI_BEGIN_COMMAND_MAP(CDlgHelp, CDlgBase)

AUI_ON_COMMAND("helpmain*", OnCommandHelpMain)
AUI_ON_COMMAND("wizard",	OnCommandWizard)
AUI_ON_COMMAND("IDCANCEL",	OnCommandIDCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgHelp, CDlgBase)

AUI_ON_EVENT("Tree", WM_LBUTTONDOWN, OnLButtonDownTree)

AUI_END_EVENT_MAP()


const static char * _pszBtnName[] = 
{ 
	"Btn_Basic", 
	"Btn_Newhand",
	"Btn_Friend", 
	"Btn_Faction", 
	"Btn_Faq",
};

CDlgHelp::CDlgHelp() :
	m_nCurHelpGroup(-1),
	m_pA2DSpriteImage(NULL),
	m_pTextArea(NULL),
	m_pTree(NULL),
	m_pImage(NULL)
{
	for (int i = 0; i < 5; i++)
		m_ppHelpGroupButton[i] = NULL;
}

CDlgHelp::~CDlgHelp()
{
	A3DRELEASE(m_pA2DSpriteImage);
}

bool CDlgHelp::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	OnCommandHelpMain("helpmain1");

	return true;
}

void CDlgHelp::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	for (int i = 0; i < 5; i++)
		DDX_Control(_pszBtnName[i], m_ppHelpGroupButton[i]);
	DDX_Control("Tree", m_pTree);
	DDX_Control("Content", m_pTextArea);
	DDX_Control("Image", m_pImage);
}

void CDlgHelp::OnCommandHelpMain(const char *szCommand)
{

	int nNewHelpGroup = atoi(szCommand + strlen("helpmain")) - 1;

	if( nNewHelpGroup == m_nCurHelpGroup ) return;

	// unpush old button
	if( m_nCurHelpGroup >= 0 )
		m_ppHelpGroupButton[m_nCurHelpGroup]->SetPushed(false);

	// push new button
	m_nCurHelpGroup = nNewHelpGroup;
	m_ppHelpGroupButton[m_nCurHelpGroup]->SetPushed(true);

	BuildHelp(m_nCurHelpGroup, -1);
}

void CDlgHelp::OnCommandWizard(const char *szCommand)
{
	CECScriptMan* pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
	if (pScriptMan)
	{
		pScriptMan->GetContext()->GetUI()->ShowHelpDialog();
		Show(false);
	}
}

void CDlgHelp::OnCommandIDCancel(const char *szCommand)
{
	Show(false);
}

bool CDlgHelp::BuildHelp(int nGroup, int nDoc)
{
	char szFile[MAX_PATH];
	if (nGroup >= 0)
	{
		AWScriptFile s;
		ACString strText;
		AString strFile, strImage;
		int i, nIndent, nCurIndent = 0;
		APtrStack<P_AUITREEVIEW_ITEM> stackRoot;
		P_AUITREEVIEW_ITEM pRoot = AUITREEVIEW_ROOT, pItem;

		sprintf(szFile, "%s\\Info\\Group%d.txt", af_GetBaseDir(), nGroup);
		if(!s.Open(szFile)) return AUI_ReportError(__LINE__, __FILE__);

		m_vecFile.clear();
		m_vecImage.clear();
		stackRoot.Push(pRoot);
		m_pTree->DeleteAllItems();

		while( s.PeekNextToken(true) )
		{
			nIndent = s.GetNextTokenAsInt(true);
			for( i = 0; i <= nIndent; i++ )
				s.GetNextToken(true);
			strText = s.m_szToken;

			s.GetNextToken(true);
			strFile = AC2AS(s.m_szToken);
			m_vecFile.push_back(strFile);
			
			s.GetNextToken(true);
			strImage = AC2AS(s.m_szToken);
			m_vecImage.push_back(strImage);

			if( nIndent > nCurIndent )
			{
				stackRoot.Push(pItem);
			}
			else if( nIndent < nCurIndent )
			{
				for( i = 0; i < nCurIndent - nIndent; i++ )
					stackRoot.Pop();
			}
			pRoot = stackRoot.Peek();
			pItem = m_pTree->InsertItem(strText, pRoot);
			m_pTree->SetItemData(pItem, m_vecFile.size() - 1);
			nCurIndent = nIndent;
		}

		s.Close();
	}

	m_pImage->ClearCover();
	A3DRELEASE(m_pA2DSpriteImage);
	m_pTextArea->SetText(_AL(""));
	if( nDoc >= 0 )
	{
		if( m_vecFile[nDoc].GetLength() > 0 )
		{
			sprintf(szFile, "%s\\Info\\%s", af_GetBaseDir(), m_vecFile[nDoc]);
			FILE *file = fopen(szFile, "rb");
			if( !file ) return AUI_ReportError(__LINE__, __FILE__);

			BYTE szHeader[2] = { 0, 0 };
			fread(szHeader, sizeof(BYTE), 2, file);
			if( szHeader[0] != 0xFF || szHeader[1] != 0xFE )
				return AUI_ReportError(__LINE__, __FILE__);
			
			fseek(file, 0, SEEK_END);
			int nLen = ftell(file) - 2;
			if( nLen <= 0 ) return AUI_ReportError(__LINE__, __FILE__);

			ACHAR *pszText = (ACHAR*)a_malloctemp(sizeof(ACHAR) * (nLen / 2 + 1));
			fseek(file, 2, SEEK_SET);
			fread(pszText, sizeof(ACHAR), nLen / 2, file);
			pszText[nLen / 2] = 0;
			m_pTextArea->SetText(pszText);

			a_freetemp(pszText);

			fclose(file);
		}

		if( m_vecImage[nDoc].GetLength() > 0 )
		{
			m_pA2DSpriteImage = new A2DSprite;

			sprintf(szFile, "%s\\Info\\%s", af_GetBaseDir(), m_vecImage[nDoc]);
			if( !m_pA2DSpriteImage->Init(m_pA3DDevice, szFile, AUI_COLORKEY) )
			{
				delete m_pA2DSpriteImage;
				m_pA2DSpriteImage = NULL;
				return AUI_ReportError(__LINE__, __FILE__);
			}

			m_pImage->Show(true);
			m_pImage->SetCover(m_pA2DSpriteImage, 0);
			m_pTextArea->SetSize(m_pTextArea->GetSize().cx, m_pTree->GetSize().cy - m_pImage->GetSize().cy - 2);
			m_pTextArea->SetPos(m_pTextArea->GetPos(true).x, m_pImage->GetPos(true).y + m_pImage->GetSize().cy + 2);
		}
		else
		{
			m_pImage->Show(false);
			m_pTextArea->SetSize(m_pTextArea->GetSize().cx, m_pTree->GetSize().cy);
			m_pTextArea->SetPos(m_pTextArea->GetPos(true).x, m_pImage->GetPos(true).y);
		}
	}

	return true;
}


void CDlgHelp::OnLButtonDownTree(WPARAM, LPARAM lParam, AUIObject *)
{
	POINT ptPos = m_pTree->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;
	P_AUITREEVIEW_ITEM pItem = m_pTree->HitTest(x, y);
	
	if( pItem ) BuildHelp(-1, m_pTree->GetItemData(pItem));
}
