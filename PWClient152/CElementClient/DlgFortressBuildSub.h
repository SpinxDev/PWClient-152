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
		BS_INVALID,			//	非法状态
		BS_BEFORE_BUILD,	//	待建
		BS_IN_BUILD,		//	建设中
		BS_AFTER_BUILD,		//	已建成
	};
	BuildState GetState() { return m_state; }

	//	高亮显示
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

	enum {BUILDING_MATERIAL_COUNT = 5};		//	可以处理的材料总数

	PAUIOBJECT			m_pEdit_Highlight;	//	对话框高亮显示控件
	PAUICOMBOBOX		m_pCombo_Candidates;//	可选建的设施
	PAUIIMAGEPICTURE	m_pImg_Build;		//	设施图标
	PAUIOBJECT			m_pLab_Name;		//	设施名称
	PAUIOBJECT			m_pEdit[BUILDING_MATERIAL_COUNT];	//	设施材料个数
	PAUIPROGRESS		m_pPro_Time;		//	建造时间进度条
	PAUIOBJECT			m_pLab_Time;		//	建造时间数值
	PAUIOBJECT			m_pLab_Speed;		//	建造速度
	PAUIOBJECT			m_pBtn_Build;		//	建造按钮
	PAUIOBJECT			m_pBtn_Add;			//	加速建造
	PAUIOBJECT			m_pBtn_Sub;			//	恢复建造速度
	PAUIOBJECT			m_pLab_Gold;		//  消耗金钱

	int					m_idBuilding;		//	关联设施ID
	int					m_nFinishTime;		//	关联设施完成时间（为0表示尚未开始建造）

	BuildState			m_state;			//	当前建造状态

	enum
	{
		INTERVAL_WAIT_BEGIN = 618,
		INTERVAL_WAIT_NEXT = 100,
	};
	DWORD				m_dwInterval;		//	自动加点的时间间隔
	DWORD				m_dwStartTime;		//	开始点击按钮时间
	DWORD				m_dwLastTime;		//	上次自动按钮时间

	bool				m_bSelected;		//	是否为当前选中
};