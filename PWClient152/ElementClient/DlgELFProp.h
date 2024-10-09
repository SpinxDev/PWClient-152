// Filename	: DlgELFProp.h
// Creator	: Chen Zhixin
// Date		: October 07, 2008

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIProgress.h"
#include "EC_Inventory.h"

struct ROLEEXTELF_BASE{
	int		vitality;       //	ÃÂ
	int		energy;         //	¡È
	int		strength;       //	¡¶
	int 	agility;        //	√Ù
};

class CDlgELFProp : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgELFProp();
	virtual ~CDlgELFProp();
	virtual bool Render(void);
	virtual void OnTick(void);

	void OnCommand_add(const char * szCommand);
	void OnCommand_minusstr(const char * szCommand);
	void OnCommand_minusagi(const char * szCommand);
	void OnCommand_minusvit(const char * szCommand);
	void OnCommand_minusint(const char * szCommand);
	void OnCommand_addauto(const char * szCommand);
	void OnCommand_confirm(const char * szCommand);
	void OnCommand_reset(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Genius(const char * szCommand);
	void OnCommand_Equip(const char * szCommand);
	void OnCommand_Safe(const char * szCommand);
	void OnCommand_InExp(const char * szCommand);

	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void PropertyAdd(AUIObject *pObj);
	void PropertyMinus(AUIObject *pObj);
	void ResetPoints();
	void OnCommand_YES();
	void UpdateEquip();

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	int m_nStatusPtUsed;
	ROLEEXTELF_BASE m_repBase;

	PAUILABEL m_pTxt_ELFName;	//
	PAUILABEL m_pTxt_ELFLevel;	//
	PAUILABEL m_pTxt_Describe;	//
	PAUILABEL m_pTxt_PointRemain;	//
	PAUILABEL m_pTxt_Str;	//
	PAUILABEL m_pTxt_Agi;	//
	PAUILABEL m_pTxt_Vit;	//
	PAUILABEL m_pTxt_Nig;	//
	PAUILABEL m_pTxt_Power;	//
	PAUILABEL m_pTxt_ReplyPower;	//
	PAUILABEL m_pTxt_Endurance;		//
	PAUILABEL m_pTxt_SuccinctLevel;	//
	PAUILABEL m_pTxt_SuccinctFAQ;	//
	PAUILABEL m_pTxt_EXP;	//
	PAUILABEL m_pTxt_PointEx;
	PAUILABEL m_pTxt_Chengzhang;
	PAUIIMAGEPICTURE m_pImg_Char;
	PAUIPROGRESS m_pPGS_EXP;

	int m_nMouseLastX;
	int m_nMouseOffset;
	int m_nMouseOffsetThis;
	DWORD m_dwStartTime;
	DWORD m_dwLastTime;
	int m_iIntervalTime;
	PAUIOBJECT m_pButtonPress;
	BOOL m_bAdd;
	DWORD m_dwFreezeTime;
	CECInventory*		m_pEquip;
	PAUIIMAGEPICTURE	m_pImgEquip[4];

	void RefreshHostDetails();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
