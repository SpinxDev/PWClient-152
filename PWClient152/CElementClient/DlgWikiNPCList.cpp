// Filename	: DlgWikiNPCList.cpp
// Creator	: Feng Ning
// Date		: 2010/07/23

#include "DlgWikiNPCList.h"
#include "elementdataman.h"
#include "WikiSearchCommand.h"
#include "EC_HostPlayer.h"
#include "auto_delete.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiNPCList, CDlgWikiItemListBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiNPCList, CDlgWikiItemListBase)
AUI_END_EVENT_MAP()

CDlgWikiNPCList::CDlgWikiNPCList()
:m_pCmd(NULL)
,m_isAutoSelectNearest(false)
{
}
bool CDlgWikiNPCList::OnInitDialog()
{
	//
	return CDlgWikiItemListBase::OnInitDialog();
}

bool CDlgWikiNPCList::Release()
{
	SetDefaultCommand(NULL);
	return CDlgWikiItemListBase::Release();
}

void CDlgWikiNPCList::SetNPCProvider(const WikiNPCEssenceProvider* pData, bool isAutoSelectNearest)
{
	SetContentProvider(pData);
	SetSearchCommand(!pData ? NULL : &WikiSearchAll());
	m_isAutoSelectNearest = isAutoSelectNearest;
}

void CDlgWikiNPCList::OnBeginSearch()
{
	CDlgWikiItemListBase::OnBeginSearch();
	
	m_Nearest.Clear();
	SetDefaultCommand(NULL);
}

void CDlgWikiNPCList::OnEndSearch()
{
	CDlgWikiItemListBase::OnEndSearch();
	
	if(m_isAutoSelectNearest && m_pCmd)
	{
		(*m_pCmd)(NULL);
		WikiSearcher::Clear();
	}
}

bool CDlgWikiNPCList::OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName)
{
	WikiNPCEssenceProvider* pData = dynamic_cast<WikiNPCEssenceProvider*>(GetContentProvider());
	if(!pData) return false;

	WikiNPCEssenceProvider::Entity* pEE = dynamic_cast<WikiNPCEssenceProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	LinkCommand* pCmd = pData->CreateLinkCommand(pEE);

	if(!pCmd) return false;

	bool bNeedDelete = true;

	// add to list
	if(!m_isAutoSelectNearest)
	{
		// bind specific command
		BindLinkCommand(dynamic_cast<PAUITEXTAREA>(pObj), NULL, pCmd);
		pObj->Show(true);
		return true;
	}

	// auto select the nearest NPC
	const NPC_ESSENCE* pNPC = pEE->GetNPC();
	bool bInTable(false);
	A3DVECTOR3 targetPos;
	abase::vector<CECGame::OBJECT_COORD> targetCoord;
	targetPos = GetHostPlayer()->GetObjectCoordinates(pNPC->id, targetCoord, bInTable);
	if(bInTable)
	{
		if(!m_Nearest.IsZero())
		{
			float d1 = (m_Nearest - GetHostPlayer()->GetPos()).SquaredMagnitude();
			float d2 = (targetPos - GetHostPlayer()->GetPos()).SquaredMagnitude();
			if(d2 < d1)
			{
				m_Nearest = targetPos;
				SetDefaultCommand(pCmd);
				bNeedDelete = false;
			}
		}
		else
		{
			m_Nearest = targetPos;
			SetDefaultCommand(pCmd);
			bNeedDelete = false;
		}
	}
	else if(m_Nearest.IsZero())
	{
		// select first as default
		SetDefaultCommand(pCmd);
		bNeedDelete = false;
	}

	if(bNeedDelete)
		delete pCmd;

	return false;
}

void CDlgWikiNPCList::SetDefaultCommand(LinkCommand* pCmd)
{
	delete m_pCmd;
	m_pCmd = pCmd;
}
