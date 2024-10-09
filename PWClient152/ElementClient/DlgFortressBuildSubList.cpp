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
	//	获取模板 SUBDIALOG ，清除子对话框并隐藏
	DDX_Control("Sub_Build", m_pSub_Build);
	m_pSub_Build->Show(false);

	PAUIDIALOG pDlg = m_pSub_Build->GetSubDialog();
	m_strDialogFile = pDlg->GetFilename();
	//	填写了默认子对话框，删除之
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

	//	清除既有设施
	ClearBuilding();

	CandidateList candidates;

	//	添加当前已建和在建设施
	//
	typedef abase::hash_map<unsigned int, unsigned int> SubTypeArray;
	SubTypeArray existSubTypes;
	for (size_t i = 0; i < pInfo->building.size(); ++ i)
	{
		const CECHostPlayer::FACTION_FORTRESS_INFO::building_data &cur = pInfo->building[i];

		//	查找同类型其它可建设施、供建设升级
		const FACTION_BUILDING_ESSENCE *pEssence = pHost->GetBuildingEssence(cur.id);
		if (pEssence)
			candidates = FindCandidates(pEssence->id_sub_type, pEssence->id);
		else
			candidates.clear();

		//	添加既有设施及其相关可建设施
		AppendBuilding(cur, candidates);

		//	记录设施的子类型，用于添加其它可建设施
		existSubTypes[pEssence->id_sub_type] = cur.id;
	}

	//	添加其它可建设施
	//
	for (BuildingTemplateArray::iterator it = m_templates.begin(); it != m_templates.end(); ++ it)
	{
		unsigned int idSubType = it->first;
		const BuildingTemplate &buildingTemplate = it->second;
		const FACTION_BUILDING_ESSENCE * pEssenceFirst = buildingTemplate[0];
		if (existSubTypes.find(idSubType) == existSubTypes.end())
		{
			//	此类型设施尚未有过
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

//	初始化所有类型设施模板，便于后期查找
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
			//	按子类别归类对应设施
			m_templates[pEssence->id_sub_type].push_back(pEssence);
		}
		id = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}

	//	对每个子类别，按设施等级从低到高进行排序
	for (BuildingTemplateArray::iterator it = m_templates.begin(); it != m_templates.end(); ++ it)
		BubbleSort(it->second.begin(), it->second.end(), BuildingEssenceSortOrder());
}

void CDlgFortressBuildSubList::ClearBuilding()
{
	//	清除所有遗留的设施相关的控件（自创而来）
	//

	bool bUpdated(false);

	//	更新父控件的滚动位置到初始位置
	//	用于恢复以 m_pSub_Build 为主的模板到初始位置
	//	避免从 m_pSub_Build 复制的其它 subdialog 初始位置受其影响
	//	而且 FitBuildingSize 里调用当前对话框的父 subdialog 的 SetSubDialogOrgSize 里，会重置滚动条位置到0（bReset参数默认为true）
	//
	PAUISUBDIALOG pParent = GetParentDlgControl();
	if (pParent)
	{
		pParent->SetHBarPos(0);
		pParent->SetVBarPos(0);
	}

	//	清除所有设施 SUBDIALOG
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
				//	删除除模板 SUBDIALOG 外的其它所有 SUBDIALOG
				pSub = (PAUISUBDIALOG)pElement->pThis;
				pDlg = pSub->GetSubDialog();
				DeleteControl(pElement->pThis);
				if (pDlg)
				{
					//	由于是程序单独加载对话框文件，因此需要手动设置删除选项
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
		//	更新对话框大小
		FitBuildingSize();
	}
}

bool CDlgFortressBuildSubList::IsCanBuild(int id)
{
	//	给定设施 id，判断其是否满足建造条件（不考虑是否已有同类型设施在建或已建）
	bool bRet(false);

	while (true)
	{
		const CECHostPlayer::FACTION_FORTRESS_INFO *pInfo = GetHostPlayer()->GetFactionFortressInfo();
		if (!pInfo)
		{
			//	尚未查得当前基地信息
			break;
		}

		const FACTION_BUILDING_ESSENCE *pEssence = GetHostPlayer()->GetBuildingEssence(id);
		if (!pEssence)
		{
			//	设施 id 非法
			break;
		}

		if (pEssence->require_level > pInfo->level)
		{
			//	等级不符
			break;
		}

		int i(0);

		const int nTechnology = sizeof(pEssence->technology)/sizeof(pEssence->technology[0]);
		for (i = 0; i < nTechnology; ++ i)
		{
			if (pEssence->technology[i] > pInfo->technology[i])
			{
				//	科技天赋点数不符
				break;
			}
		}
		if (i < nTechnology)
		{
			//	科技天赋点数不符
			break;
		}

		//	基本条件满足，可以建造
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

	//	通知包含此 Dialog 的 SUBDIALOG 控件
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
	//	在最下方添加新的 SUBDIALOG 及未初始化的设施
	//

	//	从模板克隆新 SUBDIALOG
	PAUISUBDIALOG pSub = (PAUISUBDIALOG)CloneControl(m_pSub_Build);
	pSub->Show(true);

	AString strName;
	
	//	设置 SUBDIALOG 名称
	strName = MakeSubDialogName(m_nSubDialog);
	pSub->SetName((char *)(const char *)strName);

	//	设置位置
	SIZE s = m_pSub_Build->GetDefaultSize();
	pSub->SetDefaultPos(0, m_nSubDialog*s.cy);
	
	//	创建 SUBDIALOG 相关对话框
	PAUIDIALOG pDlg = NULL;
	if (m_strDialogName.IsEmpty())
	{
		//	首次创建，记录模板名称
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
			//	对话框不存在，创建
			pDlg = m_pAUIManager->GetDialog(m_pAUIManager->CreateDlg(m_pA3DEngine, m_pA3DDevice, m_strDialogFile));
			pDlg->SetName(strName);
		}
		else
		{
			ASSERT(pDlg->GetDestroyFlag() == true);
			
			//	为遗留待删除对话框、恢复之
			if (pDlg->GetDestroyFlag())
			{
				pDlg->SetDestroyFlag(false);
				pDlg->Show(true);
			}

			//	初始化设施
			CDlgFortressBuildSub *pBuildSub = (CDlgFortressBuildSub *)pDlg;
			pBuildSub->ResetContent();
		}
	}

	//	新创建的对话框，都需要根据当前窗口缩放进行调整
	pDlg->Resize(m_pAUIManager->GetRect(), m_pAUIManager->GetRect());

	//	设置正确的对话框
	pSub->SetDialog(pDlg);

	//	更新子对话框个数及对话框大小
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
	//	查找指定子类型下的所有可建立设施，除去给定类型外
	//
	CandidateList candidates;

	BuildingTemplateArray::iterator it = m_templates.find(idSubType);
	if (it != m_templates.end())
	{
		//	找到子类型
		BuildingTemplate &buildingTemplate = it->second;
		for (BuildingTemplate::iterator it2 = buildingTemplate.begin(); it2 != buildingTemplate.end(); ++ it2)
		{
			const FACTION_BUILDING_ESSENCE * pEssence = *it2;
			if (pEssence->id != idExcept &&
				IsCanBuild(pEssence->id))
			{
				//	设施可建但未建，添加到待建列表中
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