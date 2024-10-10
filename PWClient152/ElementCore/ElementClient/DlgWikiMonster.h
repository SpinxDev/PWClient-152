// Filename	: DlgWikiMonster.h
// Creator	: Xu Wenbin
// Date		: 2010/04/06

#pragma once

#include "DlgWikiByNameBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"

struct MONSTER_ESSENCE;
class CDlgWikiMonster : public CDlgWikiByNameBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWikiMonster();
	
	void OnCommand_Drop(const char *szCommand);
	void OnCommand_Task(const char *szCommand);
	
	void OnEventLButtonDown_ListMonster(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_ListMonster(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual void ResetSearchCommand();

	// click this link command will jump to current wiki
	class ShowSpecficLinkCommand : public CDlgNameLink::LinkCommand
	{
	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		ShowSpecficLinkCommand(unsigned int monster_id, const ACString& name);
		
	protected:
		virtual ACString GetLinkText()  const;
		virtual LinkCommand* Clone() const;
		
	private:
		unsigned int m_MonsterID;
		ACString m_Name;
	};

protected:
	virtual bool OnInitDialog();
	virtual bool Release(void);

protected:
	
	// 搜索相关派生函数
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual void OnEndSearch();
	virtual void OnCommandCancel();

private:

	const MONSTER_ESSENCE * GetEssence(unsigned int id)const;
	const MONSTER_ESSENCE * GetCurEssence()const;
	
	PAUILISTBOX m_pList_Monster;
	
	PAUITEXTAREA m_pTxt_Link_MonsterName;
	PAUILABEL m_pTxt_FiveProperties;
	PAUILABEL m_pTxt_Level;
	PAUILABEL m_pTxt_Hp;
	PAUILABEL m_pTxt_Stealth;
	PAUILABEL m_pTxt_AntiImplicit;
	PAUILABEL m_pTxt_Exp;
	PAUILABEL m_pTxt_Sp;
	PAUILABEL m_pTxt_AttackType;
	PAUILABEL m_pTxt_Egg;
	PAUILABEL m_pTxt_HabitAt;
	
	PAUISTILLIMAGEBUTTON m_pBtn_Drop;
	PAUISTILLIMAGEBUTTON m_pBtn_Task;
};