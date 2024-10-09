// File		: DlgPlantPet.cpp
// Creator	: Xu Wenbin
// Date		: 2011/6/15

#include "AFI.h"
#include "DlgPlantPet.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_PetCorral.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPlantPet, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPlantPet, CDlgBase)
AUI_END_EVENT_MAP()

void CDlgPlantPet::OnCommand_CANCEL(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

bool CDlgPlantPet::OnInitDialog()
{
	AString strName;
	int i(0);
	PAUIOBJECT pObj = NULL;
	while (true)
	{
		strName.Format("P_%02d", i+1);
		pObj = GetDlgItem(strName);
		if (!pObj)	break;
		m_ImgList.push_back((PAUIIMAGEPICTURE)pObj);
		++i;
	}

	i = 0;
	while (true)
	{
		strName.Format("L_%02d", i+1);
		pObj = GetDlgItem(strName);
		if (!pObj)	break;
		m_LabelList.push_back((PAUILABEL)pObj);
		++i;
	}

	if (m_LabelList.empty() || m_LabelList.size() != m_ImgList.size())
		return false;

	return true;
}

void CDlgPlantPet::OnTick()
{
	CECPetCorral *pPetCorral = GetHostPlayer()->GetPetCorral();
	if (pPetCorral->GetPlantCount() <= 0)	return;
	
	elementdataman* pDBMan = GetGame()->GetElementDataMan();
	DATA_TYPE DataType = DT_INVALID;
	for (int i(0), j(0); i < (int)m_ImgList.size(); ++ i)
	{
		PAUIIMAGEPICTURE pImg = m_ImgList[i];
		PAUILABEL pLabel = m_LabelList[i];

		if (j >= pPetCorral->GetPlantCount())
		{
			pImg->Show(false);
			pImg->ClearCover();
			pImg->SetData(0);

			pLabel->Show(false);
			pLabel->SetText(_AL(""));

			continue;
		}

		//	过滤掉显示时间极短的植物宠
		for (; j < pPetCorral->GetPlantCount(); ++ j)
		{
			const CECPlantPetData *pPlant = ((const CECPetCorral *)pPetCorral)->GetPlant(j);
			if (pPlant->GetLifeTime() <= 0 || pPlant->GetLifeTime() >= 20)
				break;
		}
		if (j >= pPetCorral->GetPlantCount())
			continue;

		const CECPlantPetData *pPlant = ((const CECPetCorral *)pPetCorral)->GetPlant(j++);

		if (!pImg->IsShow())	pImg->Show(true);
		if (!pLabel->IsShow())	pLabel->Show(true);

		//	更新图标
		if ((int)pImg->GetData() != pPlant->m_nid)
		{
			pImg->SetData(pPlant->m_nid);
			pImg->ClearCover();
			
			PET_ESSENCE *pEssence = (PET_ESSENCE*)pDBMan->get_data_ptr(pPlant->m_tid, ID_SPACE_ESSENCE, DataType);
			if (pEssence)
			{
				AString strFile;
				af_GetFileTitle(pEssence->file_icon, strFile);
				strFile.MakeLower();
				pImg->SetCover(
					GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_PET],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_PET][strFile]);				
				pImg->SetHint(pEssence->name);
				if (pEssence->plant_group == 3)
				{
					pImg->SetFlash(true);
				}
				else
					pImg->SetFlash(false);
			}
		}

		//	更新时间
		pLabel->SetText(GetTimeString(pPlant->GetLifeTime(), pPlant->GetLifeTimeLeft()));
	}
}

ACString CDlgPlantPet::GetTimeString(int timeLength, int timeLeft)
{
	const int TIME_DAY = 3600 * 24 * 1000;
	const int TIME_HOUR = 3600 * 1000;
	const int TIME_MINUTE = 60 * 1000;
	const int TIME_SECOND = 1000;

	ACString strTime;

	if (timeLength <= 0)
		strTime = GetStringFromTable(1620);
	else if (timeLeft > TIME_DAY)
		strTime.Format(GetStringFromTable(1621), timeLeft / TIME_DAY);
	else if (timeLeft > TIME_HOUR)
		strTime.Format(GetStringFromTable(1622), timeLeft / TIME_HOUR);
	else if (timeLeft > TIME_MINUTE)
		strTime.Format(GetStringFromTable(1623), timeLeft / TIME_MINUTE);
	else
		strTime.Format(GetStringFromTable(1624), timeLeft / TIME_SECOND);
	
	return strTime;
}