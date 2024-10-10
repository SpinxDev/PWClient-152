// Filename	: DlgFortressBuildSub.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "AUICombobox.h"

class CDlgFortressBuildSub : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressBuildSub();
	
	void OnCommand_Build(const char *szCommand);
	void OnCommand_SpeedUp(const char *szCommand);
	void OnCommand_SlowDown(const char *szCommand);
	void OnCommand_ComboCandidates(const char *szCommand);
	void OnCommand_CANCEL(const char *szCommand);

	void OnEventLButtonDown_Add(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	void OnEventLButtonDown_Sub(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	void OnEventLButtonDown_(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);

	typedef abase::vector<int> CandidateList;

	void ResetContent();
	void SetCandidates(const CandidateList &candidates);
	void SetBuildingAndCandidates(int id, int finish_time, const CandidateList &candidates);

	enum BuildState
	{
		BS_INVALID,			//	�Ƿ�״̬
		BS_BEFORE_BUILD,	//	����
		BS_IN_BUILD,		//	������
		BS_AFTER_BUILD,		//	�ѽ���
	};
	BuildState GetState() { return m_state; }

	//	������ʾ
	bool GetSelect();
	void SetSelect(bool bSelect);
	void SetHighlight(bool bHighlight);
	bool GetHighlight();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	
	void ResetBuilding(int id = 0, int finish_time = -1);
	void AppendCandidate(int idBuilding, int nFinishTime);
	void AppendCandidates(const CandidateList &candidates);

	int		GetAccelerate();
	bool	SetAccelerate(int nNewAccel);
	void	UpdateBuildingProcess();
	void	ShowTimeLeft(int timeLeft);
	void	ShowProgress(int timeLeft, int timeAll);
	void	UpdateColor();
	ACString	GetMaterialToReturn();
	int			FindDefaultSel();

	enum {BUILDING_MATERIAL_COUNT = 5};		//	���Դ���Ĳ�������

	PAUIOBJECT			m_pEdit_Highlight;	//	�Ի��������ʾ�ؼ�
	PAUICOMBOBOX		m_pCombo_Candidates;//	��ѡ������ʩ
	PAUIIMAGEPICTURE	m_pImg_Build;		//	��ʩͼ��
	PAUIOBJECT			m_pLab_Name;		//	��ʩ����
	PAUIOBJECT			m_pEdit[BUILDING_MATERIAL_COUNT];	//	��ʩ���ϸ���
	PAUIPROGRESS		m_pPro_Time;		//	����ʱ�������
	PAUIOBJECT			m_pLab_Time;		//	����ʱ����ֵ
	PAUIOBJECT			m_pLab_Speed;		//	�����ٶ�
	PAUIOBJECT			m_pBtn_Build;		//	���찴ť
	PAUIOBJECT			m_pBtn_Add;			//	���ٽ���
	PAUIOBJECT			m_pBtn_Sub;			//	�ָ������ٶ�
	PAUIOBJECT			m_pLab_Gold;		//  ���Ľ�Ǯ

	int					m_idBuilding;		//	������ʩID
	int					m_nFinishTime;		//	������ʩ���ʱ�䣨Ϊ0��ʾ��δ��ʼ���죩

	BuildState			m_state;			//	��ǰ����״̬

	enum
	{
		INTERVAL_WAIT_BEGIN = 618,
		INTERVAL_WAIT_NEXT = 100,
	};
	DWORD				m_dwInterval;		//	�Զ��ӵ��ʱ����
	DWORD				m_dwStartTime;		//	��ʼ�����ťʱ��
	DWORD				m_dwLastTime;		//	�ϴ��Զ���ťʱ��

	bool				m_bSelected;		//	�Ƿ�Ϊ��ǰѡ��
};