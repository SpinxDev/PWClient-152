// Filename	: DlgHost.h
// Creator	: Tom Zhou
// Date		: October 13, 2005

#pragma once

#include "DlgBase.h"
#include "EC_Configs.h"
#include "AUILabel.h"
#include "AUICheckBox.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "AUIComboBox.h"

#include "hashmap.h"
#include "EC_GPDataType.h"
#include "EC_Counter.h"

class CDlgHost : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

	friend class CDlgSettingVideo;
public:
	CDlgHost();
	virtual ~CDlgHost();
	virtual bool Render(void);

	void OnCommand_closeHP(const char * szCommand);
	void OnCommand_headhp(const char * szCommand);
	void OnCommand_headmp(const char * szCommand);
	void OnCommand_headexp(const char * szCommand);
	void OnCommand_stop(const char * szCommand);
	void OnCommand_lock(const char * szCommand);
	void OnCommand_addexp(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_autohpmp(const char *szCommand);
	void OnCommand_GT(const char *szCommand);
	void OnCommand_OfflineShop(const char* szCommand);
	void OnCommand_Setting(const char *szCommand);
	void OnCommand_Line(const char *szCommand);
	void OnCommand_safe(const char *szCommand);
	void OnCommand_sight(const char* szCommand);
	void OnCommand_player(const char* szCommand);
	void OnCommand_surround(const char* szCommand);

	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void OnEventLButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void OnEventRButtonUp_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDBlclk_Size(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	

	void ProcessGTEnter();
	// Profit time
	void UpdateParallelWorldUI();

protected:

	PAUIIMAGEPICTURE m_pImgSize;
	PAUIIMAGEPICTURE m_pImgCombatMask;
	PAUILABEL m_pTxt_Name;
	PAUILABEL m_pTxt_Prof;
	PAUILABEL m_pTXT_LV;
	PAUIPROGRESS m_pPrgs_HP;
	PAUIPROGRESS m_pPrgs_MP;
	PAUIPROGRESS m_pPrgs_AP;
	PAUIPROGRESS m_pPrgs_EXP;
	PAUIIMAGEPICTURE m_pPic_MinusExp;
	PAUILABEL m_pTxt_HP;
	PAUILABEL m_pTxt_MP;
	PAUILABEL m_pTXT_EXP;
	PAUIIMAGEPICTURE m_pImg_Leader;
	PAUICOMBOBOX	m_pCombo_Line;
	PAUIIMAGEPICTURE	m_pImg_Realm;
	PAUIIMAGEPICTURE	m_pImg_RealmBG;
	PAUICHECKBOX	m_pChk_SwitchSafety;
	abase::vector<PAUIIMAGEPICTURE> m_vecImgAPOn;
	abase::vector<PAUIIMAGEPICTURE> m_vecImgAPOff;
	abase::vector<PAUIIMAGEPICTURE> m_vecImgAPFlash;
	abase::vector<PAUIIMAGEPICTURE> m_vecImgState;

	PAUICHECKBOX m_pChk_Player;
	PAUICHECKBOX m_pChk_Surround;
	PAUICHECKBOX m_pChk_Sight;

	int	m_nLastAP;

	int m_nMouseLastX;
	int m_nMouseOffset;
	int m_nMouseOffsetThis;

	bool m_bGTWaiting;

	CECCounter	m_cntQueryParallelWorld;

	void RefreshHostStats();
	void RefreshStatusBar();
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	bool RefreshFlightBar(bool bActive);
	bool RefreshOxigenBar(bool bActive);
	bool RefreshGatherBar(bool bActive);

	bool RefreshPowerBar(bool bOtherShown, bool bActive);
	bool RefreshArrayBar(bool bOtherShown, bool bActive);
	bool RefreshIncantBar(bool bOtherShown, bool bActive);

	bool RefreshAutoHPBar(bool bActive);
	bool RefreshAutoMPBar(bool bActive);

	void RefreshPetStatus();
	void RefreshElfStatus();

	void RefreshForceStatus();
};

// =======================================================================
// CECIconStateMgr
// =======================================================================
class IconStateUpdater;
class CECIconStateMgr
{
public:
	struct IconLayout
	{
		// return the position of next icon
		virtual POINT operator()(int index, POINT start, PAUIIMAGEPICTURE pIcon) = 0;
	};

	CECIconStateMgr();
	~CECIconStateMgr();

	// get the updater pointer
	IconStateUpdater* GetUpdater(int id);

	// updater pointer will be stored here
	void RegisterUpdater(IconStateUpdater* pUpdater);

	// release all updater pointer
	void ClearUpdater();

	// refresh the state icon by different updater
	void RefreshStateIcon( PAUIIMAGEPICTURE* pIcons, int size, const S2C::IconStates* pStates,
							IconLayout& layout, bool defaultOnly );

protected:
	abase::hash_map<int, IconStateUpdater*> m_UpdaterMap;
};
// layout the icon by vertical
struct IconLayoutVertical : public CECIconStateMgr::IconLayout
{
	IconLayoutVertical(int limit):_limit(limit){}
	// return the position of next icon
	virtual POINT operator()(int index, POINT start, PAUIIMAGEPICTURE pIcon);
protected:
	int _limit;
};
// layout the icon by horizontal
struct IconLayoutHorizontal : public CECIconStateMgr::IconLayout
{
	IconLayoutHorizontal(int limit):_limit(limit){}
	// return the position of next icon
	virtual POINT operator()(int index, POINT start, PAUIIMAGEPICTURE pIcon);
protected:
	int _limit;
};
// layout the icon by horizontal
struct IconLayoutBig : public IconLayoutHorizontal
{
	IconLayoutBig(int limit)
		:IconLayoutHorizontal(limit), _lastBigIcon(-1)
	{}

	// return the position of next icon
	virtual POINT operator()(int index, POINT start, PAUIIMAGEPICTURE pIcon);

protected:
	int _lastBigIcon;
};
// =======================================================================
// IconStateUpdater
// =======================================================================
class IconStateUpdater
{
public:
	enum PRIORITY
	{
		PRIORITY_NORMAL,
		PRIORITY_BIG,
	};

	virtual SIZE Update(const S2C::IconState& state, int x, int y, PAUIIMAGEPICTURE pIcon) = 0;

	int GetPriority() const { return m_Priority;}
	int GetID() const { return m_ID;}

protected:
	IconStateUpdater(int id, int priority)
		:m_ID(id), m_Priority(priority){};

	int m_ID;
	int m_Priority;

public:
	struct Less
	{
		Less(CECIconStateMgr& mgr):_mgr(mgr){};
		bool operator()(const S2C::IconState& lhs, const S2C::IconState& rhs)
		{
			IconStateUpdater* lu = _mgr.GetUpdater(lhs.id);
			IconStateUpdater* ru = _mgr.GetUpdater(rhs.id);
			int lp = lu ? lu->GetPriority() : PRIORITY_NORMAL;
			int rp = ru ? ru->GetPriority() : PRIORITY_NORMAL;
			return rp < lp;
		}
	private:
		CECIconStateMgr& _mgr;
	};
};