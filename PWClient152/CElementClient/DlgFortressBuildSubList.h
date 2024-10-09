// Filename	: DlgFortressBuildSubList.h
// Creator	: Xu Wenbin
// Date		: 2010/10/09

#pragma once

#include "DlgBase.h"
#include "AUISubDialog.h"
#include "EC_HostPlayer.h"

struct FACTION_BUILDING_ESSENCE;
class CDlgFortressBuildSub;

class CDlgFortressBuildSubList : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressBuildSubList();
	
	void OnCommand_CANCEL(const char *szCommand);

	void UpdateInfo();
	void ClearBuilding();
	
	void SelectBuilding(CDlgFortressBuildSub *pDlgBuilding);
	
protected:

	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void InitBuildingTemplates();
	
	typedef abase::vector<int> CandidateList;
	void AppendBuilding(const CECHostPlayer::FACTION_FORTRESS_INFO::building_data &, const CandidateList &);
	void AppendBuildingCandidate(const CandidateList &);
	CandidateList FindCandidates(unsigned int idSubType, unsigned int idExcept);
	bool IsCanBuild(int id);

	PAUISUBDIALOG AppendSubDialog();
	void FitBuildingSize();

	//	控件列表
	PAUISUBDIALOG	m_pSub_Build;			//	设施 SUBDIALOG 控件模板

	//	变量类型声明及列表
	typedef abase::vector<const FACTION_BUILDING_ESSENCE *>	BuildingTemplate;		//	同一类型设施的所有级别模板（有序供升级用）
	typedef abase::hash_map<unsigned int, BuildingTemplate>	BuildingTemplateArray;	//	所有类型设施列表
	BuildingTemplateArray	m_templates;

	enum {BUILDING_MATERIAL_COUNT = 5};		//	可以处理的材料总数（用于判断建筑建造升级条件）

	int		m_nSubDialog;					//	当前界面中的与设施（含已建未建）对应的 SUBDIALOG 个数
	AString MakeSubDialogName(int index);	//	查询给定下标设施的子对话框名称
	PAUISUBDIALOG GetSubDialog(int index);	//	查询给定下标设施的子对话框

	AString		m_strDialogName;			//	子对话框名称
	AString		m_strDialogFile;			//	子对话框文件
};