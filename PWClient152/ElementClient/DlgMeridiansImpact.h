/********************************************************************
	created:	2012/1/22
	created:	12:11:2012   16:31
	file base:	DlgMeridiansImpact
	file ext:	h
	author:		zhougaomin01305
	
	purpose:	������Ѩϵͳ
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "EC_Counter.h"
#include "AUIImagePicture.h"
#include <vector>

// ��������
class CDlgMeridiansImpact : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:

	enum
	{
		IMAGE_UNKNOWNGATE,		// δ֪��
		IMAGE_FAILGATE,			// ����
		IMAGE_SUCCESSGATE,		// ����  
		IMAGE_ERROR,			// ����
		IMAGE_UNKNOWNGATE_HOWER,// δ֪�Ÿ���
		IMAGE_UNKNOWNGATE_DOWN, // δ֪�ǰ���
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
