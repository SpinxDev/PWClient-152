// Filename	: DlgAutoMove.h
// Creator	: Chen Zhixin
// Date		: 2009/01/05

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUISlider.h"
#include "AUICheckBox.h"
#include "AUILabel.h"

class CDlgAutoMove : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoMove();
	virtual ~CDlgAutoMove();
		
	void OnCommand_Apply(const char * szCommand);
	void OnCommand_Confirm(const char * szCommand);
	void OnCommand_Cancel(const char * szCommand);
	void OnCommand_Stop(const char * szCommand);
	
	void RefreshHigh(); // refresh high value by internal value

	void OnEventLButtonDownMin(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDownMax(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	A3DPOINT2 GetTargetPosAs2D() const;
	A3DPOINT2 Get2DTargetPos() const { return m_ptTargetPos; }
	void Set2DTargetPos(int x, int y);
	void Clear2DTargetPos();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual void OnChangeLayoutEnd(bool bAllDone);

	bool	CheckFlyCondition();

	// set hight value, only affect UI visual
	void	SetHight(int iHigh);
	
	PAUIOBJECT m_pEdt_High;
	PAUISLIDER	m_pSlider_high;
	PAUICHECKBOX m_pCheck_fly;
	PAUILABEL	m_pLab_High;

	int m_iHigh;
	bool m_bAutoLand;
	A3DPOINT2 m_ptTargetPos;
};
