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

	//	�ؼ��б�
	PAUISUBDIALOG	m_pSub_Build;			//	��ʩ SUBDIALOG �ؼ�ģ��

	//	���������������б�
	typedef abase::vector<const FACTION_BUILDING_ESSENCE *>	BuildingTemplate;		//	ͬһ������ʩ�����м���ģ�壨���������ã�
	typedef abase::hash_map<unsigned int, BuildingTemplate>	BuildingTemplateArray;	//	����������ʩ�б�
	BuildingTemplateArray	m_templates;

	enum {BUILDING_MATERIAL_COUNT = 5};		//	���Դ���Ĳ��������������жϽ�����������������

	int		m_nSubDialog;					//	��ǰ�����е�����ʩ�����ѽ�δ������Ӧ�� SUBDIALOG ����
	AString MakeSubDialogName(int index);	//	��ѯ�����±���ʩ���ӶԻ�������
	PAUISUBDIALOG GetSubDialog(int index);	//	��ѯ�����±���ʩ���ӶԻ���

	AString		m_strDialogName;			//	�ӶԻ�������
	AString		m_strDialogFile;			//	�ӶԻ����ļ�
};