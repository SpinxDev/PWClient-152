// Filename	: DlgForce.cpp
// Creator	: Feng Ning
// Date		: 2011/10/08

#include "DlgForce.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUICTranslate.h"
#include "A2DSprite.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_ForceMgr.h"
#include "elementdataman.h"
#include "DlgForceActivity.h"
#include "DlgForceActivityIcon.h"
#include "EC_IvtrItem.h"
#include "AFI.h"
#include "EC_Utility.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgForce, CDlgBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgForce, CDlgBase)
AUI_END_EVENT_MAP()

CDlgForce::CDlgForce()
{
	m_pConfig = NULL;

	m_pDiagramRender = new CECDiagramRender();
	m_pCanvas = NULL;
}

CDlgForce::~CDlgForce()
{
	delete m_pDiagramRender;
	m_pDiagramRender = NULL;
}

bool CDlgForce::OnInitDialog()
{
	if( !CDlgBase::OnInitDialog() )
		return false;

	m_pDiagramRender->Init();
	m_pCanvas = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Activity"));

	m_Icons.clear();
	int index = 1;
	while(true)
	{
		AString strName;
		strName.Format("Img_Item%02d", index++);
		PAUIIMAGEPICTURE pIcon = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if(!pIcon) break;
		m_Icons.push_back(pIcon);
	}

	return true;
}

void CDlgForce::OnShowDialog()
{
	int forceID = GetHostPlayer()->GetForce();
	if(forceID == 0)
	{
		// player didn't join any force
		if(IsShow())
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(9421), GP_CHAT_MISC);
			OnCommand("IDCANCEL");
		}
		return;
	}

	if(!UpdateAll(forceID))
	{
		// invalid force data
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9420), GP_CHAT_MISC);
		OnCommand("IDCANCEL");
		return;
	}

	m_pDiagramRender->ClearSummary();
}


bool CDlgForce::UpdateAll(int forceID)
{
	if(!m_pConfig || m_pConfig->id != forceID)
	{
		CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();

		const FORCE_CONFIG *pConfig = pMgr->GetForceData(forceID);
		if(!pConfig)
		{
			return false;
		}

		m_pConfig = pConfig;

		// init force flag
		PAUIIMAGEPICTURE pImgForce = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Force"));
		if(pImgForce)
		{
			A2DSprite* pSprite = pMgr->GetForceIcon(m_pA3DDevice, pConfig, CECForceMgr::FORCE_ICON_FLAG);
			pImgForce->SetCover(pSprite, 0);
		}

		PAUILABEL pTxtTitle = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Title"));
		if(pTxtTitle)
		{
			pTxtTitle->SetText(m_pConfig->name);
		}

		PAUITEXTAREA pTxtIntro = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Intro"));
		if(pTxtIntro)
		{
			AUICTranslate trans;
			pTxtIntro->SetText(trans.Translate(m_pConfig->desc));
		}
	}

	// update global force info
	UpdateGlobalForce();

	// update player force info
	UpdatePlayerForce();

	// update the suggest force item
	UpdateSuggestItem(GetHostPlayer()->GetForce());

	return true;
}

void CDlgForce::UpdatePlayerForce()
{
	CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
	if(!pMgr) return;

	int forceCount = pMgr->GetValidForce(NULL, 0);
	if(!forceCount) return;

	int* pForces = (int*)a_malloctemp(sizeof(int) * forceCount);
	pMgr->GetValidForce(pForces, forceCount);

	// update personal force info
	AString strName;
	int uiIndex = 0;
	int forceIndex = 0;
	while(true)
	{
		// enumerate the valid UI object
		PAUILABEL pTxtName = dynamic_cast<PAUILABEL>(GetDlgItem(strName.Format("Txt_Name%d", uiIndex)));
		if(!pTxtName) break;
		
		PAUILABEL pTxtContribution = dynamic_cast<PAUILABEL>(GetDlgItem(strName.Format("Txt_Contribution%d", uiIndex)));
		if(!pTxtContribution) break;

		PAUILABEL pTxtReputation = dynamic_cast<PAUILABEL>(GetDlgItem(strName.Format("Txt_Reputation%d", uiIndex)));
		if(!pTxtReputation) break;

		const CECHostPlayer::FORCE_INFO* pInfo = NULL;
		const FORCE_CONFIG* pConfig = NULL;
		if(forceIndex < forceCount)
		{
			int curForceID = pForces[forceIndex];
			pInfo = GetHostPlayer()->GetForceInfo(curForceID);
			pConfig = pMgr->GetForceData(curForceID);
			forceIndex++;
		}

		// fill "N/A" into the the duplicated UI object
		pTxtName->SetText(pConfig ? pConfig->name : GetStringFromTable(9405));

		ACString strNum;
		strNum.Format(_AL("%d/%d"), pInfo?pInfo->contribution:0, pConfig?pConfig->contribution_max:0);
		pTxtContribution->SetText(strNum);

		strNum.Format(_AL("%d/%d"), pInfo?pInfo->reputation:0, pConfig?pConfig->reputation_max:0);
		pTxtReputation->SetText(strNum);

		uiIndex++;
	}

	a_freetemp(pForces);
}

void CDlgForce::UpdateGlobalForce()
{
	CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
	if(!pMgr) return;

	const CECForceMgr::FORCE_GLOBAL* pDetail = pMgr->GetForceGlobal(m_pConfig->id);

	PAUIPROGRESS pBarDev = dynamic_cast<PAUIPROGRESS>(GetDlgItem("Pro_Development"));
	PAUIPROGRESS pBarBuild = dynamic_cast<PAUIPROGRESS>(GetDlgItem("Pro_Build"));
	PAUIPROGRESS pBarActivity = dynamic_cast<PAUIPROGRESS>(GetDlgItem("Pro_Activity"));

	PAUILABEL pTxtBuild = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Build"));
	PAUILABEL pTxtDev = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Development"));
	PAUILABEL pTxtActivity = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Activity"));
	PAUILABEL pTxtStatus = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Status"));
	
	if(!pDetail)
	{
		ACString strNA = GetStringFromTable(9405);
		if(pBarDev) pBarDev->SetProgress(0);
		if(pTxtDev) pTxtDev->SetText(strNA);
		if(pBarBuild) pBarBuild->SetProgress(0);
		if(pTxtBuild) pTxtBuild->SetText(strNA);
		if(pBarActivity) pBarActivity->SetProgress(0);
		if(pTxtActivity) pTxtActivity->SetText(strNA);

		if(pTxtStatus) pTxtStatus->SetText(strNA);
	}
	else
	{
		ACString strValue;

		const CECForceMgr::FORCE_GLOBAL& detail = *pDetail;
		if(pBarDev) pBarDev->SetProgress(0);
		if(pTxtDev) pTxtDev->SetText(strValue.Format(_AL("%d"), detail.development));

		if(pBarBuild) pBarBuild->SetProgress(0);
		if(pTxtBuild) pTxtBuild->SetText(strValue.Format(_AL("%d"), detail.construction));

		if(pBarActivity) pBarActivity->SetProgress(0);
		if(pTxtActivity) pTxtActivity->SetText(strValue.Format(_AL("%d"), detail.activity));

		int order = min(0, max(8, detail.activity_level));
		if(pTxtStatus) pTxtStatus->SetText(GetStringFromTable(9406 + order));
	}
}

void CDlgForce::OnTick()
{
	CECForceMgr* pMgr = GetHostPlayer()->GetForceMgr();
	bool dataReady = (pMgr && !pMgr->IsForceGlobalDataEmpty());

	if(!dataReady)
	{
		// reuse the refresh logic in icon
		CDlgForceActivityIcon* pDlg = dynamic_cast<CDlgForceActivityIcon*>
			(m_pAUIManager->GetDialog("Win_ForceActivityIcon"));
		if(pDlg && !pDlg->IsShow()) pDlg->RefreshForceGlobalData(dataReady);

		if(m_pCanvas) m_pCanvas->SetHint(GetStringFromTable(9422));
	}
	else if(!m_pDiagramRender->GetSummary())
	{
		// show the diagram of activity first
		CECForceMgr::FORCE_GLOBAL detail;
		int activeType = ((int*)(&detail.activity)) - ((int*)&detail);

		m_pDiagramRender->RefreshSummary(activeType, m_pCanvas);
	}
}

bool CDlgForce::Render()
{
	if( !AUIDialog::Render() )
		return false;

	// render the diagram
	if(m_pDiagramRender->GetSummary())
	{
		m_pDiagramRender->Render(m_pDiagramRender->GetSummary(), m_pCanvas);
	}

	return true;
}

void CDlgForce::UpdateSuggestItem(int force_id)
{
	CECIvtrItem** items = NULL;

	const CECHostPlayer::FORCE_INFO* pInfo = GetHostPlayer()->GetForceInfo(force_id);
	int current = pInfo ? pInfo->reputation : 0;

	items = (CECIvtrItem**)a_malloctemp(sizeof(CECIvtrItem*) * m_Icons.size());

	// compare the reputation and get the suggest item
	int found = GetHostPlayer()->GetForceMgr()->GetSuggestItems
		( force_id, GetHostPlayer()->GetProfession(), current, items, m_Icons.size() );

	// clear icons if not found
	if(!found) {a_freetemp(items); items = NULL;}

	if(!m_Icons.empty())
	{
		AString strFile;
		for(size_t i=0;i<m_Icons.size();i++)
		{
			if(items && items[i])
			{
				af_GetFileTitle(items[i]->GetIconFile(), strFile);
				strFile.MakeLower();
				m_Icons[i]->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
									GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY,strFile) );

				// show the correct item desc
				items[i]->GetDetailDataFromLocal();
				const ACHAR* pDesc = items[i]->GetDesc();
				if(pDesc)
				{
					AUICTranslate trans;
					m_Icons[i]->SetHint(trans.Translate(pDesc));
				}
				else
				{
					ACString strHint = A3DCOLOR_TO_STRING(items[i]->GetNameColor());
					strHint += items[i]->GetName();
					m_Icons[i]->SetHint(strHint);
				}
				
				delete items[i];
			}
			else
			{
				m_Icons[i]->SetCover(NULL, -1);
				m_Icons[i]->SetHint(_AL(""));					
			}
		}
	}

	PAUILABEL pTxtItem = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Item"));
	if(items && pTxtItem)
	{
		ACString strMsg;
		const FORCE_CONFIG* pConfig = GetHostPlayer()->GetForceMgr()->GetForceData(force_id);
		pTxtItem->SetText(strMsg.Format(GetStringFromTable(9433), pConfig ? pConfig->name : _AL("")));
	}
	else
	{
		pTxtItem->SetText(GetStringFromTable(9434));
	}

	a_freetemp(items);
}