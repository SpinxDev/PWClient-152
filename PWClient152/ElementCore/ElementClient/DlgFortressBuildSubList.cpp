#include "DlgFortressBuildSubList.h"
#include "DlgFortressBuildSub.h"
#include "EC_GameUIMan.h"
#include "EC_Algorithm.h"
#include "elementdataman.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressBuildSubList, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressBuildSubList, CDlgBase)
AUI_END_EVENT_MAP()

CDlgFortressBuildSubList::CDlgFortressBuildSubList()
{
	m_pSub_Build = NULL;
	m_nSubDialog = 0;
}

bool CDlgFortressBuildSubList::OnInitDialog()
{
	//	��ȡģ�� SUBDIALOG ������ӶԻ�������
	DDX_Control("Sub_Build", m_pSub_Build);
	m_pSub_Build->Show(false);

	PAUIDIALOG pDlg = m_pSub_Build->GetSubDialog();
	m_strDialogFile = pDlg->GetFilename();
	//	��д��Ĭ���ӶԻ���ɾ��֮
	m_pSub_Build->SetDialog(NULL);

	InitBuildingTemplates();
	return true;
}

void CDlgFortressBuildSubList::OnShowDialog()
{
}

void CDlgFortressBuildSubList::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgFortressBuildSubList::UpdateInfo()
{
	CECHostPlayer *pHost = GetHostPlayer();
	const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = pHost->GetFactionFortressInfo();
	if (!pInfo)
		return;

	//	���������ʩ
	ClearBuilding();

	CandidateList candidates;

	//	��ӵ�ǰ�ѽ����ڽ���ʩ
	//
	typedef abase::hash_map<unsigned int, unsigned int> SubTypeArray;
	SubTypeArray existSubTypes;
	for (size_t i = 0; i < pInfo->building.size(); ++ i)
	{
		const CECHostPlayer::FACTION_FORTRESS_INFO::building_data &cur = pInfo->building[i];

		//	����ͬ���������ɽ���ʩ������������
		const FACTION_BUILDING_ESSENCE *pEssence = pHost->GetBuildingEssence(cur.id);
		if (pEssence)
			candidates = FindCandidates(pEssence->id_sub_type, pEssence->id);
		else
			candidates.clear();

		//	��Ӽ�����ʩ������ؿɽ���ʩ
		AppendBuilding(cur, candidates);

		//	��¼��ʩ�������ͣ�������������ɽ���ʩ
		existSubTypes[pEssence->id_sub_type] = cur.id;
	}

	//	��������ɽ���ʩ
	//
	for (BuildingTemplateArray::iterator it = m_templates.begin(); it != m_templates.end(); ++ it)
	{
		unsigned int idSubType = it->first;
		const BuildingTemplate &buildingTemplate = it->second;
		const FACTION_BUILDING_ESSENCE * pEssenceFirst = buildingTemplate[0];
		if (existSubTypes.find(idSubType) == existSubTypes.end())
		{
			//	��������ʩ��δ�й�
			candidates = FindCandidates(idSubType, 0);
			AppendBuildingCandidate(candidates);
		}
	}
}

class BuildingEssenceSortOrder
{
public:
	bool operator() (
		const FACTION_BUILDING_ESSENCE *lhs,
		const FACTION_BUILDING_ESSENCE *rhs)const
	{
		return lhs->level > rhs->level;
	}
};

//	��ʼ������������ʩģ�壬���ں��ڲ���
//
void CDlgFortressBuildSubList::InitBuildingTemplates()
{
	m_templates.clear();

	CECHostPlayer *pHost = GetHostPlayer();
	elementdataman * pDataMan = GetGame()->GetElementDataMan();

	const FACTION_BUILDING_ESSENCE *pEssence = NULL;
	DATA_TYPE dt = DT_INVALID;
	unsigned int id = pDataMan->get_first_data_id(ID_SPACE_ESSENCE, dt);
	while (id > 0)
	{
		if (pEssence = pHost->GetBuildingEssence(id))
		{
			//	�����������Ӧ��ʩ
			m_templates[pEssence->id_sub_type].push_back(pEssence);
		}
		id = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}

	//	��ÿ������𣬰���ʩ�ȼ��ӵ͵��߽�������
	for (BuildingTemplateArray::iterator it = m_templates.begin(); it != m_templates.end(); ++ it)
		BubbleSort(it->second.begin(), it->second.end(), BuildingEssenceSortOrder());
}

void CDlgFortressBuildSubList::ClearBuilding()
{
	//	���������������ʩ��صĿؼ����Դ�������
	//

	bool bUpdated(false);

	//	���¸��ؼ��Ĺ���λ�õ���ʼλ��
	//	���ڻָ��� m_pSub_Build Ϊ����ģ�嵽��ʼλ��
	//	����� m_pSub_Build ���Ƶ����� subdialog ��ʼλ������Ӱ��
	//	���� FitBuildingSize ����õ�ǰ�Ի���ĸ� subdialog �� SetSubDialogOrgSize ������ù�����λ�õ�0��bReset����Ĭ��Ϊtrue��
	//
	PAUISUBDIALOG pParent = GetParentDlgControl();
	if (pParent)
	{
		pParent->SetHBarPos(0);
		pParent->SetVBarPos(0);
	}

	//	���������ʩ SUBDIALOG
	PAUISUBDIALOG pSub = NULL;
	PAUIDIALOG pDlg = NULL;
	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	while (pElement)
	{
		PAUIOBJECTLISTELEMENT pNext = GetNextControl(pElement);
		if (pElement->pThis->GetType() == AUIOBJECT_TYPE_SUBDIALOG)
		{
			if (pElement->pThis != m_pSub_Build)
			{
				//	ɾ����ģ�� SUBDIALOG ����������� SUBDIALOG
				pSub = (PAUISUBDIALOG)pElement->pThis;
				pDlg = pSub->GetSubDialog();
				DeleteControl(pElement->pThis);
				if (pDlg)
				{
					//	�����ǳ��򵥶����ضԻ����ļ��������Ҫ�ֶ�����ɾ��ѡ��
					pDlg->SetDestroyFlag(true);
				}

				m_nSubDialog --;

				bUpdated = true;
			}
		}
		pElement = pNext;
	}

	if (bUpdated)
	{
		//	���¶Ի����С
		FitBuildingSize();
	}
}

bool CDlgFortressBuildSubList::IsCanBuild(int id)
{
	//	������ʩ id���ж����Ƿ����㽨���������������Ƿ�����ͬ������ʩ�ڽ����ѽ���
	bool bRet(false);

	while (true)
	{
		const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
		if (!pInfo)
		{
			//	��δ��õ�ǰ������Ϣ
			break;
		}

		const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(id);
		if (!pEssence)
		{
			//	��ʩ id �Ƿ�
			break;
		}

		if (pEssence->require_level > pInfo->level)
		{
			//	�ȼ�����
			break;
		}

		int i(0);

		const int nTechnology = sizeof(pEssence->technology)/sizeof(pEssence->technology[0]);
		for (i = 0; i < nTechnology; ++ i)
		{
			if (pEssence->technology[i] > pInfo->technology[i])
			{
				//	�Ƽ��츳��������
				break;
			}
		}
		if (i < nTechnology)
		{
			//	�Ƽ��츳��������
			break;
		}

		//	�����������㣬���Խ���
		bRet = true;
		break;
	}

	return bRet;
}

AString CDlgFortressBuildSubList::MakeSubDialogName(int index)
{
	AString strName;
	strName.Format("%s_%d", m_pSub_Build->GetName(), index);
	return strName;
}

PAUISUBDIALOG CDlgFortressBuildSubList::GetSubDialog(int index)
{
	AString strName = MakeSubDialogName(index);
	PAUIOBJECT pObj = GetDlgItem(strName);
	return (PAUISUBDIALOG)pObj;
}

void CDlgFortressBuildSubList::FitBuildingSize()
{
	SIZE s = GetDefaultSize();

	SIZE buildSize = m_pSub_Build->GetDefaultSize();
	s.cy = max(1, m_nSubDialog) * buildSize.cy;

	SetDefaultSize(s.cx, s.cy);

	//	֪ͨ������ Dialog �� SUBDIALOG �ؼ�
	PAUISUBDIALOG pParent = GetParentDlgControl();
	if (pParent)
	{
		float fWindowScale = m_pAUIManager->GetWindowScale();
		int cx = (int)(s.cx * fWindowScale + 0.5f);
		int cy = (int)(s.cy * fWindowScale + 0.5f);
		pParent->SetSubDialogOrgSize(cx, cy);
	}
}

PAUISUBDIALOG CDlgFortressBuildSubList::AppendSubDialog()
{
	//	�����·�����µ� SUBDIALOG ��δ��ʼ������ʩ
	//

	//	��ģ���¡�� SUBDIALOG
	PAUISUBDIALOG pSub = (PAUISUBDIALOG)CloneControl(m_pSub_Build);
	pSub->Show(true);

	AString strName;
	
	//	���� SUBDIALOG ����
	strName = MakeSubDialogName(m_nSubDialog);
	pSub->SetName((char *)(const char *)strName);

	//	����λ��
	SIZE s = m_pSub_Build->GetDefaultSize();
	pSub->SetDefaultPos(0, m_nSubDialog*s.cy);
	
	//	���� SUBDIALOG ��ضԻ���
	PAUIDIALOG pDlg = NULL;
	if (m_strDialogName.IsEmpty())
	{
		//	�״δ�������¼ģ������
		pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_strDialogFile));
		m_strDialogName = pDlg->GetName();
		strName.Format("%s_%d", m_strDialogName, m_nSubDialog);
		pDlg->SetName(strName);
	}
	else
	{
		strName.Format("%s_%d", m_strDialogName, m_nSubDialog);
		pDlg = GetGameUIMan()->GetDialog(strName);
		if (!pDlg)
		{
			//	�Ի��򲻴��ڣ�����
			pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_strDialogFile));
			pDlg->SetName(strName);
		}
		else
		{
			ASSERT(pDlg->GetDestroyFlag() == true);
			
			//	Ϊ������ɾ���Ի��򡢻ָ�֮
			if (pDlg->GetDestroyFlag())
			{
				pDlg->SetDestroyFlag(false);
				pDlg->Show(true);
			}

			//	��ʼ����ʩ
			CDlgFortressBuildSub *pBuildSub = (CDlgFortressBuildSub *)pDlg;
			pBuildSub->ResetContent();
		}
	}

	//	�´����ĶԻ��򣬶���Ҫ���ݵ�ǰ�������Ž��е���
	pDlg->Resize(m_pAUIManager->GetRect(), m_pAUIManager->GetRect());

	//	������ȷ�ĶԻ���
	pSub->SetDialog(pDlg);

	//	�����ӶԻ���������Ի����С
	m_nSubDialog ++;
	FitBuildingSize();

	return pSub;
}

void CDlgFortressBuildSubList::AppendBuilding(const CECHostPlayer::FACTION_FORTRESS_INFO::building_data &rhs, const CandidateList &candidates)
{
	PAUISUBDIALOG pSubDialog = AppendSubDialog();
	CDlgFortressBuildSub *pBuildSub = (CDlgFortressBuildSub *)pSubDialog->GetSubDialog();
	pBuildSub->SetBuildingAndCandidates(rhs.id, rhs.finish_time, candidates);
}

void CDlgFortressBuildSubList::AppendBuildingCandidate(const CandidateList &candidates)
{
	if (!candidates.empty())
	{
		PAUISUBDIALOG pSubDialog = AppendSubDialog();
		CDlgFortressBuildSub *pBuildSub = (CDlgFortressBuildSub *)pSubDialog->GetSubDialog();
		pBuildSub->SetCandidates(candidates);
	}
}

CDlgFortressBuildSubList::CandidateList CDlgFortressBuildSubList::FindCandidates(unsigned int idSubType, unsigned int idExcept)
{
	//	����ָ���������µ����пɽ�����ʩ����ȥ����������
	//
	CandidateList candidates;

	BuildingTemplateArray::iterator it = m_templates.find(idSubType);
	if (it != m_templates.end())
	{
		//	�ҵ�������
		BuildingTemplate &buildingTemplate = it->second;
		for (BuildingTemplate::iterator it2 = buildingTemplate.begin(); it2 != buildingTemplate.end(); ++ it2)
		{
			const FACTION_BUILDING_ESSENCE * pEssence = *it2;
			if (pEssence->id != idExcept &&
				IsCanBuild(pEssence->id))
			{
				//	��ʩ�ɽ���δ������ӵ������б���
				candidates.push_back(pEssence->id);
			}
		}
	}

	return candidates;
}

void CDlgFortressBuildSubList::SelectBuilding(CDlgFortressBuildSub *pDlgBuilding)
{
	if (!pDlgBuilding)
		return;

	PAUISUBDIALOG pSub = NULL;
	for (int i = 0; i < m_nSubDialog; ++ i)
	{
		pSub = GetSubDialog(i);
		if (pSub && pSub->GetSubDialog())
		{
			CDlgFortressBuildSub *pDlg = (CDlgFortressBuildSub *)pSub->GetSubDialog();
			pDlg->SetSelect(pDlg == pDlgBuilding);
		}
	}
}