// Filename	: DlgCountryMap.h
// Creator	: Han Guanghui
// Date		: 2012/7/18

#ifndef _DLGCOUNTRYMAP_H_
#define _DLGCOUNTRYMAP_H_ 

#include "DlgBase.h"
#include "vector.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUISubDialog.h"
#include "EC_DomainCountry.h"

namespace GNET{
	class Protocol;
}

class CDlgCountryMove;
class A3DVECTOR3;
class CDlgCountryMapSub;

class CDlgCountryMap : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

	friend class CDlgCountryMove;
	friend class CDlgCountryMapSub;
public:
	CDlgCountryMap();
	virtual ~CDlgCountryMap();

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventMButtonDBClick(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnCommandCancel(const char * szCommand);
	void OnCommandChat(const char * szCommand);
	void OnCommandRefresh(const char * szCommand);
	void OnCommandBack(const char * szCommand);
	
	void OnPrtcCountry(GNET::Protocol* pProtocol);
	void OnCountryChange();
	void OnCountryEnterWar();
	void OnClickDomain(int nHoverIndex, LPARAM lParam);

	void MoveConfirm(bool bYes);
	
	typedef DWORD TimeType;
	static TimeType GetCurrentTime();
	static int	ConverToSeconds(TimeType t);
	static TimeType ConvertFromSeconds(int sec);
	static TimeType GetTimeLeft(TimeType end_time);

	TimeType GetNodeMoveTimeLeft()const;
	TimeType GetNodeMoveTime()const;
	TimeType GetRouteMoveTime()const;
	TimeType GetRouteMoveTimeLeft()const;

	int	GetDomainLoc()const{return m_domainLoc;}
	void GetMap();
	void GetConfig();

	bool IsEnterWarCountDown();
	void UpdateEnterWarCountDown();
	void StopEnterWarCountDown(bool bLogLeftTime=false);

	bool CanBackToCapital();
	bool CanGetScore();
	bool SaveLoadCenter(int idInst, const A3DVECTOR3 &vPos, int iParallelWorldID);

	// 当前有限制条件的战场个数
	int GetLimitDomainCount() const { return m_iLimitDomainCnt; }
	
	bool IsConfigReady()const{ return m_bGetConfig; }
	bool IsVersionOK()const{ return IsConfigReady() && m_bVersionOK; }
	bool IsOpen()const{ return IsConfigReady() && m_bOpen; }
	void LogState()const;

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();

	void StopMove();
	void GetScore();

	void OnPrtcCountryMove_Re(GNET::Protocol* pProtocol);
	void OnPrtcCountrySynLocation(GNET::Protocol* pProtocol);
	void OnPrtcCountryGetMap_Re(GNET::Protocol* pProtocol);
	void OnPrtcCountryGetScore_Re(GNET::Protocol* pProtocol);
	void OnPrtcCountryGetConfig_Re(GNET::Protocol* pProtocol);
	void OnPrtcCountryEnterNotify(GNET::Protocol* pProtocol);
	void OnPrtcCountryBattleLimit_Re(GNET::Protocol* pProtocol);

	bool IsMapReady()const{ return m_bGetMap && IsVersionOK(); }
	bool IsLocationReady()const{ return m_domainLoc > 0; }
	bool IsCanMove()const{ return IsOpen() && IsMapReady() && IsLocationReady(); }
	void ToggleChatDlg();	
	void AddMessage(int idString);
	void ShowFalse();
	CDlgCountryMove * GetDlgCountryMove();
	void UpdateRestTime();
	void BringDialogToBefore(PAUIDIALOG pDlg);
	void UpdateAdditionalInfo();

	void StartMove();
	void ContinueMove();
	void ClearMove();
	void ClearRoute();
	void ClearMoreRoute();

	// 弹出国王操作
	void PopupKingOrderMenu(int x, int y, int iDomainID);

	PAUIOBJECT					m_pBtn_Back;
	PAUIOBJECT					m_pBtn_Refresh;

	PAUISUBDIALOG				m_pSubDlgControl;
	CDlgCountryMapSub*			m_pCountryMapSub;
	
	int							m_domainLoc;	//	国战地图中当前领土位置

	bool						m_bGetMap;		//	地图是否已更新
	TimeType					m_cntGetMap;	//	更新地图协议计时器

	TimeType					m_cntStopMove;	//	停止移动协议计时器

	bool						m_bGetConfig;		//	是否已获取 config 数据
	TimeType					m_cntGetConfig;		//	更新 Config 数据计时器
	bool						m_bVersionOK;		//	是否数据版本与服务器一致
	bool						m_bOpen;			//	国战是否开启
	int							m_countryStartTime;	//	国战地图开始时间（从当天零秒起的秒数）
	int							m_countryEndTime;	//	国战地图结束时间（从当天零秒起的秒数）
	int							m_nTotalBonus;		//	本次国战总奖励

	int							m_iKingID[DOMAIN2_INFO::COUNTRY_COUNT];	// 国王ID
	PAUILABEL					m_pLbl_KingName[DOMAIN2_INFO::COUNTRY_COUNT];

	int							m_nTotalScore;		//	个人总积分
	int							m_nCountryScore[DOMAIN2_INFO::COUNTRY_COUNT];	//	各国家总积分
	TimeType					m_cntGetScore;		//	更新积分计时器

	int							m_iLimitDomainCnt;	//  有限制条件的战场个数

	enum MoveState
	{
		MS_STOPPED,				//	停止
		MS_WAITING_MOVE,		//	等待回复移动
		MS_WAITING_ARRIVE,		//	移动中、等待到达目标
		MS_WAITING_STOP,		//	移动中、等待回复停止
		MS_WAITING_REDIRECT,	//	移动中、等待回复更改目标
	};
	MoveState					m_moveState;
	bool IsStopped()const{ return m_moveState == MS_STOPPED; }
	bool IsWaitingMove()const{ return m_moveState == MS_WAITING_MOVE; }
	bool IsWaitingArrive()const{ return m_moveState == MS_WAITING_ARRIVE; }
	bool IsWaitingStop()const{ return m_moveState == MS_WAITING_STOP; }
	bool IsWaitingRedirect()const{ return m_moveState == MS_WAITING_REDIRECT; }

	int							m_domainNext;	//	移动时移往目标
	TimeType					m_cntMoveStart;	//	开始移动时刻
	TimeType					m_cntMoveEnd;	//	到达移往目标时刻
	CECDomainCountryInfo::Route	m_route;		//	移动参考路径（含起点、目标）
	CECDomainCountryInfo::Route	m_routeCand;	//	参考路径临时变量

	bool						m_bLoadCenter;	//	m_vLoadCenter 是否有效
	A3DVECTOR3					m_vLoadCenter;	//	保存的将要跳往的 143 号图位置，当前对话框隐藏时递交
	bool CommitLoadCenter();
};

class CDlgCountryMove : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

	friend class CDlgCountryMap;

public:
	CDlgCountryMove();
	virtual ~CDlgCountryMove();
	virtual void OnTick();
	
	void OnCommandStop(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	
protected:	
	virtual bool OnInitDialog();

	CDlgCountryMap * GetDlgCountryMap();
	
	PAUIOBJECT	m_pTxt_RestTime;
	PAUIOBJECT	m_pBtn_Stop;
};

#endif //_DLGCOUNTRYMAP_H_ 