// Filename	: DlgTarget.h
// Creator	: Tom Zhou
// Date		: October 12, 2005

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"

class AUILabel;
class AUIProgress;
class A2DSprite;

class CECPlayer;
class CECNPC;

class CDlgTargetShowPlayerPolicy;

class CDlgTarget : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgTarget();
	virtual ~CDlgTarget();

	void OnCommand_change(const char * szCommand);
	void OnCommand_size(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_traceplayer(const char * szCommand);
	void OnCommand_tracepet(const char * szCommand);
	void OnCommand_showhp(const char *szCommand);
	
	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void OnEventLButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void OnEventRButtonUp_size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown_size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	

	void RefreshTargetStat();

protected:
	bool OnInitDialog();

	void SetShowLevel(PAUIDIALOG pShow, int nShowLevel);

	void RefreshForceStatus(CDlgTargetShowPlayerPolicy *pShowPolicy);
	void RefreshAsPlayer(int idSelObj);
	void RefreshASNPC(int idSelObj);
	void RefreshTargetOfTarget(int idTarget);

	abase::vector<PAUIIMAGEPICTURE> m_vecImgState;

	static bool m_bShowFaction;
	static bool m_bShowPercent;
	static bool m_bShowMonsterHP;

	static int m_nMouseLastX;
	static int m_nMouseOffset;
	static int m_nMouseOffsetThis;
	static A2DSprite* m_pA2DSpritePetLevel;

	bool m_bAggressive;		// 是否主动攻击，只对怪物生效
	void SetAggressive(bool bAggressive);

	AUIOBJECT_SETPROPERTY m_aggressiveHPImg;	// 主动攻击怪的血量底板
	AUIOBJECT_SETPROPERTY m_unaggressiveHPImg;	// 非主动攻击怪的血量底板

	AUIOBJECT_SETPROPERTY m_aggressiveSymbolImg;	// 主动攻击怪的标签
	AUIOBJECT_SETPROPERTY m_unaggressiveSymbolImg;	// 非主动攻击怪的标签
};

class CDlgTargetOfTarget : public CDlgBase {
	
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
	
public:
	CDlgTargetOfTarget();
	static void ShowTargetOfTarget(int iTargetID);					// 外部的唯一入口
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnCommand_SelectMaster(const char* szCommand);
protected:
	bool OnInitDialog();
private:
	void ShowPlayer(int id);
	void ShowNPC(CECNPC* pNPC);
	void ShowPet(CECNPC* pNPC);
	void SetTargetID(int iTargetID);
private:
	int m_iTargetID;
	int m_iMasterOfTargetID;
	
	AUIProgress* m_pPrgHP;
	AUILabel* m_pLblName;

	int m_iLButtonDownX;
	int m_iLButtonDownY;

	bool m_bClick;
};

