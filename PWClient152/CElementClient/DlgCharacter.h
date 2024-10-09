// Filename	: DlgCharacter.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"

class CDlgCharacter : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCharacter();
	virtual ~CDlgCharacter();
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
	void OnCommand_force(const char * szCommand);
	void OnCommand_Title(const char * szCommand);
	void OnCommand_ReincarnationBook(const char * szCommand);

	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_add(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_minus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLBUttonUp_stop(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void PropertyAdd(AUIObject *pObj);
	void PropertyMinus(AUIObject *pObj);
	void ResetPoints();

protected:
	int m_nStatusPtUsed;
	ROLEEXTPROP_BASE m_repBase;

	PAUILABEL m_pTxt_CharName;
	PAUILABEL m_pTxt_CharLevel;
	PAUILABEL m_pTxt_Profession;
	PAUILABEL m_pTxt_Faction;
	PAUILABEL m_pTxt_Partner;
	PAUILABEL m_pTxt_ExpCurrent;
	PAUILABEL m_pTxt_ExpRequire;
	PAUILABEL m_pTxt_Distinction;
	PAUILABEL m_pTxt_xz;
	PAUILABEL m_pTxt_HP;
	PAUILABEL m_pTxt_MP;
	PAUILABEL m_pTxt_SP;
	PAUILABEL m_pTxt_Point;
	PAUILABEL m_pTxt_Str;
	PAUILABEL m_pTxt_Agi;
	PAUILABEL m_pTxt_Vit;
	PAUILABEL m_pTxt_Int;
	PAUILABEL m_pTxt_Attack;
	PAUILABEL m_pTxt_AtkSpeed;
	PAUILABEL m_pTxt_Definition;
	PAUILABEL m_pTxt_Evade;
	PAUILABEL m_pTxt_MoveSpeed;
	PAUILABEL m_pTxt_MgcAttack;
	PAUILABEL m_pTxt_Critical;
	PAUILABEL m_pTxt_PhyDefense;
	PAUILABEL m_pTxt_MgcDefense;
	PAUILABEL m_pTxt_AttackLevel;
	PAUILABEL m_pTxt_DefenseLevel;
	PAUILABEL m_pNew_PhyDefense;
	PAUILABEL m_pNew_MgcDefense;
	PAUILABEL m_pNew_AttackLevel;
	PAUILABEL m_pNew_DefenseLevel;
	PAUIIMAGEPICTURE m_pImg_Char;
	PAUILABEL m_pTxt_CrtPower;
	PAUILABEL m_pTxt_SoulPower;
	PAUIOBJECT m_pBtn_Force;
	PAUILABEL m_pTxt_Force;
	PAUIOBJECT m_pBtn_Book;

	int m_nMouseLastX;
	int m_nMouseOffset;
	int m_nMouseOffsetThis;
	DWORD m_dwStartTime;
	DWORD m_dwLastTime;
	int m_iIntervalTime;
	PAUIOBJECT m_pButtonPress;
	BOOL m_bAdd;

	void RefreshHostDetails();
	virtual bool OnInitDialog();
};
