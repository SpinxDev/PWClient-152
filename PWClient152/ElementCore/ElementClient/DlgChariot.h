// File		: DlgCrossSevActivity.h
// Creator	: WYD
// Date		: 2013/8/12

#pragma once

#include "DlgBase.h"
#include "AUIListBox.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "tankbattleplayergetrank_re.hpp"
#include "ExpTypes.h"
#include "AUITreeView.h"
#include "A3DGeometry.h"
#include "EC_GPDataType.h"

//#define CHARIOT_TYPE_NUM 3
//#define CHARIOT_LEVEL 3
//#define CHARIOT_SKILLNUM 3

class CECSkill;
class CECModel;

class CChariot
{
public:
	CChariot(CECHostPlayer* pHost);
	~CChariot();

	CHARIOT_CONFIG* GetChariot(int iType,int iLevel) { return m_chariot_vec[iType][iLevel];}
	const CHARIOT_CONFIG* GetChariot(int tid);
	const CHARIOT_CONFIG* GetNextChariot(int tid);
	void			ExtractChariotFromData();
	void			SetChariotImgHint(int iType,int iLevel, CECGameUIMan* pGameUIMan,PAUIIMAGEPICTURE pImg);
	int				GetChariotTid(int iSel);
	CECSkill*		GetCurChariotSkillByIndex(int index);
	CECSkill*		GetCurChariotSkillByID(int skill_id);
	void			PrepareChariot(int id);
	bool			IsUpdateChariot(int oldid, int newid);
	void			UpdateState(int oldid,int newid) { m_bUpdateState = IsUpdateChariot(oldid,newid);}
	bool			IsUpdateState() const { return m_bUpdateState;}
	void			OnChangeUI(CECGameUIMan* pUIMan,bool bBackup);
	bool			IsTopLevelChariot(int tid);
	int				GetCurChariotTid() const { return m_iCurChariotID;}
	CECModel*		LoadModel(int id);
	int				GetChariotTypeCnt() const { return m_chariot_vec.size();}
	int				GetChariotTypeByTid(int tid);
	int				GetChariotCnt(int iType) { return m_chariot_vec[iType].size();}
	bool			ChariotHasSkill(int chariot_id,int skill_id);
	bool			CanGatherMine();
protected:
	//CHARIOT_CONFIG*		m_chariot_vec[CHARIOT_TYPE_NUM][CHARIOT_LEVEL];
	typedef abase::vector<CHARIOT_CONFIG*> CHARIOTLIST;
	typedef abase::vector<CHARIOTLIST> CHARIOTMATRIX;
	
	CHARIOTMATRIX m_chariot_vec;

	int		m_iCurChariotID;
	//CECSkill*	m_pCurChariotSkill[CHARIOT_SKILLNUM];
	abase::vector<CECSkill*> m_pCurChariotSkill;

	CECHostPlayer* m_pHostPlayer;
	bool			m_bUpdateState;

	typedef abase::hash_map<AString,bool> DLGSTATE;
	DLGSTATE m_dlgShowState;
};

// NPC 报名界面
class CDlgChariotEnter : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgChariotEnter(){};
	virtual ~CDlgChariotEnter();

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);	
	void OnCommandCancel(const char * szCommand);
	void OnCommandEnterBattle(const char* szCommand);
	void OnCommandAboutChariot(const char* szCommand);
	

protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

protected:
	//PAUIIMAGEPICTURE m_pImg_Chariot[CHARIOT_TYPE_NUM];
	abase::vector<PAUIIMAGEPICTURE> m_pImg_Chariot;
};

// 战车信息界面
class CDlgChariotInfo : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgChariotInfo(){};
	virtual ~CDlgChariotInfo();

	void SetChariotImgHint(int iType,int iLevel, PAUIIMAGEPICTURE pImg);
	void SelectChariot(int tid);

	void OnEventLButtonDown_Model(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Model(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Tree(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	void UpdateRenderCallback();
protected:
	PAUIIMAGEPICTURE m_pImg_Chariot;
//	PAUIIMAGEPICTURE m_pImg_Skill[CHARIOT_SKILLNUM];
	abase::vector<PAUIIMAGEPICTURE> m_pImg_Skill;

	PAUIOBJECT		 m_pLbl_Name;
	PAUIOBJECT		 m_pTxt_Info;
	PAUITREEVIEW	 m_pTree;

	PAUIOBJECT		 m_pLbl_HP;
	PAUIOBJECT		 m_pLbl_Speed;
//	PAUIOBJECT		 m_pLbl_Attack;

	CECModel* m_pModel;
	A3DAABB m_aabb;
	int m_curChariotID;

	int m_nMouseLastX;
	int m_nMouseOffset;
	int m_nMouseOffsetThis;
};

// 复活后选择界面
class CDlgChariotRevive : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgChariotRevive(){};
	virtual ~CDlgChariotRevive(){};

	void OnCommandRevive(const char* szCommand);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool OnInitDialog();
protected:
	//PAUIIMAGEPICTURE m_pImg_Chariot[CHARIOT_TYPE_NUM];
	abase::vector<PAUIIMAGEPICTURE> m_pImg_Chariot;
	PAUIOBJECT		 m_pLbl_Time;
	int				 m_iTimeToRevive;
};

// 操作界面
class CDlgChariotOpt : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgChariotOpt(){};
	virtual ~CDlgChariotOpt(){};	

	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

protected:
	
	abase::vector<PAUIIMAGEPICTURE> m_pImg_Skill;
	PAUIPROGRESS	 m_pProgressMagic;
	PAUIOBJECT		 m_pImg_LevelUp;
	PAUIOBJECT		 m_pTxt_LevelUp;
};

// 跨服活动排行榜
class CDlgChariotRankList : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

	enum
	{
		NAME_NULL = 0,
		NAME_REQUESTING,
		NAME_DONE,
	};
public:
	CDlgChariotRankList():m_bNPCFlag(false){};
	virtual ~CDlgChariotRankList(){};

	void OnTankBattleNotify(Protocol* pProtocol);
	void OnCommandRefresh(const char* szCommand);
	void SetSelfRand(const GNET::TankBattlePlayerScoreInfo& info);
	void SetRankList(const GNET::TankBattlePlayerScoreInfoVector& vec);
	void FormatInfo(int i, const ACHAR* name, ACString& outStr);

	void SetNPCSevFlag(bool bNPC) { m_bNPCFlag = bNPC;}
	
protected:
	virtual void OnShowDialog(){};
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnHideDialog();

protected:
	GNET::TankBattlePlayerScoreInfoVector m_rankList;
	GNET::TankBattlePlayerScoreInfo	m_selfRank;
	PAUILISTBOX m_pList;
	bool m_bNPCFlag;
};

// 得分
class CDlgChariotScore: public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:
	void UpdateScore();
	void OnCommandExitBattle(const char* szCommand);
	void OnCommandRankList(const char* szCommand);
	void OnCommandQueryAmount(const char* szCommand);

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool Release();
};

class CDlgChariotMultiKill: public CDlgBase
{
public:
	void UpdateKill();
protected:
	virtual void OnTick();
protected:
	float m_fCloseKill;
};

// 战车数量分布
class CDlgChariotAmount: public CDlgBase
{
	
public:
	void SetInfo(int atk,S2C::cmd_player_query_chariots::chariot* pAtk,int def,S2C::cmd_player_query_chariots::chariot* pDef);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();

protected:
	abase::vector<PAUIIMAGEPICTURE> m_pImg_ChariotDefVec;
	abase::vector<PAUIPROGRESS> m_pProg_AmountDefVec;
	abase::vector<PAUILABEL> m_pLbl_NumberDefVec;

	abase::vector<PAUIIMAGEPICTURE> m_pImg_ChariotAtkVec;
	abase::vector<PAUIPROGRESS> m_pProg_AmountAtkVec;
	abase::vector<PAUILABEL> m_pLbl_NumberAtkVec;
};