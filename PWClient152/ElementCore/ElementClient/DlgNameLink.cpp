// Filename	: DlgNameLink.cpp
// Creator	: Feng Ning
// Date		: 2010/05/10

#include "DlgNameLink.h"
#include "EC_Resource.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Instance.h"
#include "EC_HostPlayer.h"
#include "EC_Manager.h"
#include "EC_FixedMsg.h"
#include "EC_UIHelper.h"
#include "TaskTemplMan.h"
#include "DlgTask.h"
#include "DlgWorldMap.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgNameLink, CDlgBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgNameLink, CDlgBase)
AUI_ON_EVENT("Txt_Link_*",	WM_LBUTTONUP,	OnEventLButtonClick_NameLink)
AUI_ON_EVENT("Txt_Link_*",	WM_LBUTTONDOWN,	OnEventLButtonClick_NameLink)
AUI_ON_EVENT("Txt_Link_*",	WM_MOUSEMOVE,	OnEventMouseMove_NameLink)
AUI_END_EVENT_MAP()

// =======================================================================
// Link Command
// =======================================================================

CDlgNameLink::LinkCommand::LinkCommand()
:m_pArea(NULL)
{
}

bool CDlgNameLink::BindLinkCommand(PAUITEXTAREA pArea, const ACString* pName, const LinkCommand* pCmdType)
{
	// pAres's name combined with input name to make the key
	ASSERT(pArea || pName);
	ACString key;
	if(pArea) key = AS2AC(pArea->GetName());
	if(pName) key += *pName;
	
	bool isDuplicated = false;
	
	CommandMap::iterator itr = m_Commands.find(key);
	if(itr != m_Commands.end())
	{
		LinkCommand* pTmp = itr->second;
		if(pTmp) delete pTmp;
		itr->second = NULL;
		isDuplicated = true;
	}

	if(pCmdType)
	{
		LinkCommand* pTmp = pCmdType->Clone();
		pTmp->m_pArea = pArea;
		pTmp->AppendText();
		m_Commands[key] = pTmp;
	}
	
	return isDuplicated;
}

void CDlgNameLink::LinkCommand::AppendText()
{
	// binding link text to AUITextArea
	if(m_pArea) m_pArea->SetText(this->GetLinkText());
}

CDlgNameLink::LinkCommand* CDlgNameLink::GetLinkCommand(PAUITEXTAREA pArea, const ACString* pName)
{
	// pAres's name combined with input name to make the key
	ASSERT(pArea || pName);
	
	// search by combined name
	if(pArea && pName)
	{
		ACString key = AS2AC(pArea->GetName());
		key += *pName;
		CommandMap::iterator itr = m_Commands.find(key);
		if(itr != m_Commands.end())
		{
			return itr->second;
		}
	}
	
	// search by area name
	if(pArea)
	{
		ACString key = AS2AC(pArea->GetName());
		CommandMap::iterator itr = m_Commands.find(key);
		if(itr != m_Commands.end())
		{
			return itr->second;
		}
	}

	// search by item name
	if(pName)
	{
		ACString key = *pName;
		CommandMap::iterator itr = m_Commands.find(key);
		if(itr != m_Commands.end())
		{
			return itr->second;
		}
	}

	return NULL;
}

// =======================================================================
void CDlgNameLink::ClearCommands()
{
	for(CommandMap::iterator itr = m_Commands.begin(); itr != m_Commands.end(); ++itr)
	{
		LinkCommand* pTmp = itr->second;
		if(pTmp)
		{
			delete pTmp;
			itr->second = NULL;
		}
	}
	m_Commands.clear();
}

bool CDlgNameLink::Release()
{
	ClearCommands();
	return CDlgBase::Release();
}

bool CDlgNameLink::PtInRect(const AUITEXTAREA_NAME_LINK &name, int x, int y)
{
	return name.rc.PtInRect(x, y);
}

bool CDlgNameLink::GetNameLinkMouseOn(int x, int y, PAUITEXTAREA pText, P_AUITEXTAREA_NAME_LINK pLink, int* pArea)
{
	bool bClicked = false;
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	POINT ptPos = pText->GetPos();

	if (GetGame()->GetCurCursor() == RES_CUR_SWALLOW) return false;
	GetGame()->ChangeCursor(RES_CUR_NORMAL);
	int area = pText->GetHitArea(x - ptPos.x, y - ptPos.y);
	if( area == AUITEXTAREA_RECT_FRAME )
	{
		int i;
		abase::vector<AUITEXTAREA_NAME_LINK> &vecNameLink = pText->GetNameLink();
		
		x += rcWindow.left;
		y += rcWindow.top;
		for( i = 0; i < (int)vecNameLink.size(); i++ )
		{
			if (PtInRect(vecNameLink[i], x, y))
			{
				GetGame()->ChangeCursor(RES_CUR_HAND);
				if(pLink) *pLink = vecNameLink[i];
				bClicked = true;
				break;
			}
		}
	}

	if(pArea) *pArea = area;

	return bClicked;
}

void CDlgNameLink::OnEventMouseMove_NameLink(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{	
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	if ((wParam & MK_LBUTTON) || (wParam & MK_RBUTTON))
	{
		//	鼠标右键按下时，由子类处理
		OnEventMouseMove(x, y, wParam, NULL);
		return;
	}
	
	int area = AUITEXTAREA_RECT_MIN;
	AUITEXTAREA_NAME_LINK item;
	bool bFound = GetNameLinkMouseOn(x, y, dynamic_cast<PAUITEXTAREA>(pObj), &item, &area);
	
	// trans this message to derived class
	if(area == AUITEXTAREA_RECT_FRAME)
	{
		OnEventMouseMove(x, y, wParam, bFound ? &item : NULL);
	}
}

void CDlgNameLink::OnEventLButtonClick_NameLink(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	
	// execute command on this item
	int area = AUITEXTAREA_RECT_MIN;
	AUITEXTAREA_NAME_LINK item;
	bool bFound = GetNameLinkMouseOn(x, y, dynamic_cast<PAUITEXTAREA>(pObj), &item, &area);
	if(bFound && (wParam & MK_LBUTTON) == 0) // only execute when button up
	{
		LinkCommand* pCmd = GetLinkCommand(dynamic_cast<PAUITEXTAREA>(pObj), &item.strName);
		if(pCmd) (*pCmd)(&item);
	}

	// trans this message to derived class
	if(area == AUITEXTAREA_RECT_FRAME)
	{
		OnEventLButtonClick(x, y, wParam, bFound ? &item : NULL);
	}
}

// =======================================================================
// Link Command
// =======================================================================

CDlgNameLink::MoveToLinkCommand::MoveToLinkCommand(int idTarget, const ACString &targetName, int idTask)
:m_TargetName(targetName)
,m_TargetPos(0)
,m_TargetId(idTarget)
,m_TaskId(idTask)
{
	bool bInTable(false);
	m_TargetPos = g_pGame->GetGameRun()->GetHostPlayer()->GetObjectCoordinates(
		idTarget, m_Targets, bInTable);
	
	// only research the NPC in major map
	if(!bInTable && MAJOR_MAP == g_pGame->GetGameRun()->GetWorld()->GetInstanceID())
	{
		ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
		const NPC_INFO* pInfo = pMan->GetTaskNPCInfo(idTarget);
		if(pInfo)
		{
			// NOTICE: different sequence
			m_TargetPos.Set(pInfo->x, pInfo->z, pInfo->y);
		}
	}
}

CDlgNameLink::MoveToLinkCommand::MoveToLinkCommand(int idTarget, 
												   const A3DVECTOR3& targetPos,
												   const ACString &targetName,
												   int idTask)
:m_TargetName(targetName)
,m_TargetPos(targetPos)
,m_TargetId(idTarget)
,m_TaskId(idTask)
{
	// only set one
	CECGame::OBJECT_COORD coor;
	coor.vPos = m_TargetPos;
	m_Targets.push_back(coor);
}

CDlgNameLink::MoveToLinkCommand::MoveToLinkCommand(const MoveToLinkCommand& rhs)
:m_TargetName(rhs.m_TargetName)
,m_TargetPos(rhs.m_TargetPos)
,m_Targets(rhs.m_Targets)
,m_TargetId(rhs.m_TargetId)
,m_TaskId(rhs.m_TaskId)
{

}

CDlgNameLink::MoveToLinkCommand::MoveToLinkCommand(const abase::vector<CECGame::OBJECT_COORD>& targets, 
												   const ACString &targetName)
:m_TargetName(targetName)
,m_Targets(targets)
,m_TargetPos(0)
,m_TargetId(0)
,m_TaskId(0)
{
	if(!targets.empty())
	{
		const CECGame::OBJECT_COORD& coor = *m_Targets.begin();
		m_TargetPos = coor.vPos;
	}
}

CDlgNameLink::LinkCommand* CDlgNameLink::MoveToLinkCommand::Clone() const
{
	return new MoveToLinkCommand(*this);
}

bool CDlgNameLink::MoveToLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	if (pLink && pLink->strName == m_TargetName)
	{
		if (!m_TargetPos.IsZero())
		{
			// show the flag on worldmap
			CDlgTask::SetTraceObjects(m_Targets, m_TargetName);
			CECUIHelper::AutoMoveStartComplex(m_TargetPos, m_TargetId, m_TaskId);
		}
		else
		{
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ERR_FC_INVALID_OPERATION);
		}
	}

	return false;
}

ACString CDlgNameLink::MoveToLinkCommand::GetLinkText() const
{	
	if(m_TargetPos.IsZero())
	{
		return m_TargetName;
	}
	else
	{
		ACString szNameLink;
		szNameLink.Format(_AL("^00FF00^u&%s&^u^FFFFFF"), m_TargetName);
		return szNameLink;
	}
}
