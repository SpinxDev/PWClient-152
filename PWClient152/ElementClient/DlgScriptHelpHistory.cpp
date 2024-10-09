#include "DlgScriptHelpHistory.h"
#include "AUITreeView.h"
#include "ECScriptContext.h"
#include "ECScriptController.h"
#include "ECScript.h"
#include "AUIStillImageButton.h"
#include "ECScriptOption.h"

#define NODE_TYPE	1
#define NODE_SCRIPT	2

AUI_BEGIN_COMMAND_MAP(CDlgScriptHelpHistory, CDlgBase)

AUI_ON_COMMAND("reactive", OnCommandReactive)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgScriptHelpHistory, CDlgBase)

AUI_ON_EVENT("Trv_History", WM_LBUTTONUP, OnLButtonUpHistoryTree)

AUI_END_EVENT_MAP()

const int MAX_SCRIPT_TYPE_NUM_IN_HISTORY = 6;

CDlgScriptHelpHistory::CDlgScriptHelpHistory() :
	m_pTVHistory(NULL),
	m_pContext(NULL),
	m_pBtnReactive(NULL)
{

}


void CDlgScriptHelpHistory::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Trv_History", m_pTVHistory);
//	DDX_Control("Btn_Reactive", m_pBtnReactive);
}

void CDlgScriptHelpHistory::ClearHistory()
{
	m_pTVHistory->DeleteAllItems();
}

void CDlgScriptHelpHistory::BuildTypeItems()
{
	int nIndex;
//	for( nIndex = 0; nIndex < 7; nIndex++ )
	for (nIndex = 0; nIndex <= MAX_SCRIPT_TYPE_NUM_IN_HISTORY; nIndex++)
	{
		P_AUITREEVIEW_ITEM pItem = m_pTVHistory->InsertItem(GetStringFromTable(1801 + nIndex));
		m_pTVHistory->SetItemData(pItem, static_cast<DWORD>(nIndex));
		m_pTVHistory->SetItemDataPtr(pItem, reinterpret_cast<void *>(NODE_TYPE));
	}
}

void CDlgScriptHelpHistory::BuildHistory()
{
	CScriptArray& vecScripts = m_pContext->GetController()->GetScriptArray();
	CScriptArrayIterator iter = vecScripts.begin();
	for (; iter != vecScripts.end(); ++iter)
	{
		CECScript *pScript = *iter;
		if (!m_pContext->GetOption()->IsScriptActive(pScript->GetID()) && 
			0 <= pScript->GetType() && pScript->GetType() <= MAX_SCRIPT_TYPE_NUM_IN_HISTORY)
			AddHistory(pScript->GetType(), pScript->GetName(), pScript->GetID());
	}
}

void CDlgScriptHelpHistory::AddHistory(int nType, AString strName, int nID)
{
	// find specified type node
	P_AUITREEVIEW_ITEM pRoot = m_pTVHistory->GetRootItem();
	P_AUITREEVIEW_ITEM pTypeNode = m_pTVHistory->GetFirstChildItem(pRoot);
	while (pTypeNode && static_cast<int>(m_pTVHistory->GetItemData(pTypeNode)) != nType)
	{
		pTypeNode = m_pTVHistory->GetNextSiblingItem(pTypeNode);
		ASSERT(pTypeNode);
	}
	ASSERT(pTypeNode);
	
	int len = MultiByteToWideChar(CP_UTF8, 0, strName, strName.GetLength(), NULL, 0);
	ACHAR *strText = (ACHAR*)a_malloctemp(sizeof(ACHAR)*(len + 1));
	MultiByteToWideChar(CP_UTF8, 0, strName, strName.GetLength(), strText, len + 1);
	strText[len] = 0;
	
	// add item as child of type node
	P_AUITREEVIEW_ITEM pNewItem = m_pTVHistory->InsertItem(strText, pTypeNode);
	a_freetemp(strText);

	m_pTVHistory->SetItemData(pNewItem, static_cast<DWORD>(nID));
	m_pTVHistory->SetItemDataPtr(pNewItem, reinterpret_cast<void *>(NODE_SCRIPT));
}

bool CDlgScriptHelpHistory::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	return true;
}

void CDlgScriptHelpHistory::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	SetCanMove(false);
//	m_pBtnReactive->Enable(false);

	ClearHistory();
	BuildTypeItems();
	BuildHistory();
}

void CDlgScriptHelpHistory::DeleteHistory(int nType, int nID)
{
	
}

void CDlgScriptHelpHistory::SetScriptContext(CECScriptContext * pContext)
{
	m_pContext = pContext;
}

void CDlgScriptHelpHistory::OnCommandReactive(const char * szCommand)
{
	P_AUITREEVIEW_ITEM pItem = m_pTVHistory->GetSelectedItem();
	if (!pItem || reinterpret_cast<int>(m_pTVHistory->GetItemDataPtr(pItem)) != NODE_SCRIPT)
		return;
	if (m_pContext->GetOption()->IsHelpDisabled())
		return;

	int nID = static_cast<int>(m_pTVHistory->GetItemData(pItem));
	m_pContext->GetController()->ForceActiveScript(nID);
}

void CDlgScriptHelpHistory::OnLButtonUpHistoryTree(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	bool bEnableBtnReactive = true;

	if (m_pContext->GetOption()->IsHelpDisabled()) 
		bEnableBtnReactive = false;
	
	P_AUITREEVIEW_ITEM pItem = m_pTVHistory->GetSelectedItem();
	if (!pItem || reinterpret_cast<int>(m_pTVHistory->GetItemDataPtr(pItem)) != NODE_SCRIPT)
		bEnableBtnReactive = false;

//	m_pBtnReactive->Enable(bEnableBtnReactive);
	
	if (bEnableBtnReactive)
	{
		OnCommandReactive(NULL);
	}
}
