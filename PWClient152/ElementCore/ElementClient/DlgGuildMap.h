// Filename	: DlgGuildMap.h
// Creator	: Xiao Zhou
// Date		: 2005/12/31

#pragma once

#include "DlgBase.h"
#include "vector.h"
#include "AUIImagePicture.h"

#define BATTLEFUND_MAX 200000000

class CDlgGuildMap : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgGuildMap();
	virtual ~CDlgGuildMap();

	void OnCommandCancel(const char * szCommand);

	void OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void BattleAction(int idAction, void *pData);

	void AlignMyFactionColor();
	void AlignFactionPVPStatus();

	enum
	{
		BATTLE_ACTION_BATTLECHALLENGE_RE,
		BATTLE_ACTION_BATTLECHALLENGEMAP_RE,
		BATTLE_ACTION_BATTLEGETMAP_RE,
		BATTLE_ACTION_BATTLEENTER_RE,
		BATTLE_ACTION_BATTLESTATUS_RE,
	};

	enum
	{
		AREA_TYPE_NORMAL = 0,
		AREA_TYPE_OWNER = 1,
		AREA_TYPE_CANCHALLENGE = 2,
		AREA_TYPE_TEMPCHALLENGED = 4,
		AREA_TYPE_CHALLENGED = 8,
		AREA_TYPE_BECHALLENGED = 16,
		AREA_TYPE_CANENTER = 32,
		AREA_TYPE_HOVER = 64,
	};

	enum
	{
		FACTIONMAP_TYPE_NORMAL,
		FACTIONMAP_TYPE_CHALLENGE,
		FACTIONMAP_TYPE_ENTER,
	};

	void SetType(int nType) { m_nType = nType; }
	int  GetType() { return m_nType;	}
	void SetMapStatus(bool b) { m_bGetMap = b; }
	bool GetMapStatus() { return m_bGetMap; }
	void GetMap();
	bool IsBattleOpen() { return m_bBattleOpen; }
	void SetBattleChallengeOpen(bool b);
	bool IsBattleChallengeOpen() { return m_bBattleChallengeOpen; }
	bool IsBattleInFight(){ return m_bBattleInFight; }
	void SetBattleChallengeRandNum(int rand_num) {m_nBattleChallengeRandNum = rand_num;}
	int	 GetBattleChallengeRandNum(){ return m_nBattleChallengeRandNum; }
	bool GetFactionColor(int colorIndex, A3DCOLOR & color)const;

	void UpdateNormalMap();
	void UpdateChallengeMap();
	
public:
	abase::vector<int>			m_vecAreaType;

	// cache the battle bid message
	abase::vector<ACString>		m_Msgs;
	int							m_MsgPos;
	int							m_LastUpdateTime;
	const ACString&	AddMessage(const ACString& msg);
	void ClearMessage();
	void ShowCachedMessage();

	struct BonusInfo
	{
		int item_id;
		int count1;
		int count2;
		int count3;
	};
	const BonusInfo& GetBonusInfo() const { return m_Info; }

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Render();
	
	void RenderArea(int nIndex, A3DCOLOR cl);
	void RenderArrow(int nIndex1, int nIndex2, A2DSprite *pArrow);

	A3DPOINT2 ScreenToPixel(A3DPOINT2 pt);
	A3DPOINT2 PixelToScreen(A3DPOINT2 pt);

	PAUIIMAGEPICTURE			m_pImg_GuildMap;
	int							m_nHoverAreaIndex;
	abase::vector<A3DRECT>		m_vecAreaRect;
	abase::vector<A3DCOLOR>		m_vecFactionColor;
	int							m_nType;
	bool						m_bGetMap;
	bool						m_bGetChallengeMap;
	A2DSprite*					m_pA2DSpriteArrow1;
	A2DSprite*					m_pA2DSpriteArrow2;
	A2DSprite*					m_pA2DSpriteArrow3;
	bool						m_bBattleOpen;
	bool						m_bBattleChallengeOpen;
	bool						m_bBattleInFight;
	int							m_nMyFactionColor;
	int							m_nMaxBid;

	int							m_nBattleChallengeRandNum;	//	Challenge 协议验证信息生成所需随机数，由服务器在 BattleChanllengeMap_Re 中返回

	BonusInfo					m_Info;
};
