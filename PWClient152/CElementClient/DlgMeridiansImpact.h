/********************************************************************
	created:	2012/1/22
	created:	12:11:2012   16:31
	file base:	DlgMeridiansImpact
	file ext:	h
	author:		zhougaomin01305
	
	purpose:	经脉冲穴系统
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "EC_Counter.h"
#include "AUIImagePicture.h"
#include <vector>

// 经脉界面
class CDlgMeridiansImpact : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:

	enum
	{
		IMAGE_UNKNOWNGATE,		// 未知门
		IMAGE_FAILGATE,			// 死门
		IMAGE_SUCCESSGATE,		// 生门  
		IMAGE_ERROR,			// 错误
		IMAGE_UNKNOWNGATE_HOWER,// 未知门高亮
		IMAGE_UNKNOWNGATE_DOWN, // 未知们按下
		IMAGE_NUM,			//
	};
	char* m_pbtnImages[IMAGE_NUM];

	int m_level;
	int m_index;
	bool m_bCloseCheckPoint;
	bool m_bReadyToHide;

	PAUILABEL m_pLabelContinuousSuccess ;
	PAUILABEL m_pLabelSuccessGate       ;
	PAUILABEL m_pLabelName              ;
	PAUILABEL m_pLabelFreeUpgradeNum    ;
	PAUILABEL m_pLabelChargableUpgradeNum;
	PAUILABEL m_pLabelContinuousLogin   ;
	typedef std::vector<int> CONSUMABLES_VECTOR;
	PAUIIMAGEPICTURE m_pImpactGfx;
	
protected:

	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	void DoDataExchange(bool bSave);

	virtual void OnChangeLayoutEnd(bool bAllDone);

	void SetImpactInfo();
	int GetBtnState(int index);
	bool CheckItem(bool bFreeItem);
	bool CanImpact();
public:
	void SetImpactName(const ACHAR*pImpactName);
	void SendImpactProtocol();
	void SetBtnInfo();
	void OnImpactResult(int index, int result);
public:
	void OnCommandOpenGate(const char * szCommand);

	void OnCommandCancel(const char * szCommand);


	CDlgMeridiansImpact();
	virtual ~CDlgMeridiansImpact();
};
