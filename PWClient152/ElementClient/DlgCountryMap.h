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

	// ��ǰ������������ս������
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

	// ������������
	void PopupKingOrderMenu(int x, int y, int iDomainID);

	PAUIOBJECT					m_pBtn_Back;
	PAUIOBJECT					m_pBtn_Refresh;

	PAUISUBDIALOG				m_pSubDlgControl;
	CDlgCountryMapSub*			m_pCountryMapSub;
	
	int							m_domainLoc;	//	��ս��ͼ�е�ǰ����λ��

	bool						m_bGetMap;		//	��ͼ�Ƿ��Ѹ���
	TimeType					m_cntGetMap;	//	���µ�ͼЭ���ʱ��

	TimeType					m_cntStopMove;	//	ֹͣ�ƶ�Э���ʱ��

	bool						m_bGetConfig;		//	�Ƿ��ѻ�ȡ config ����
	TimeType					m_cntGetConfig;		//	���� Config ���ݼ�ʱ��
	bool						m_bVersionOK;		//	�Ƿ����ݰ汾�������һ��
	bool						m_bOpen;			//	��ս�Ƿ���
	int							m_countryStartTime;	//	��ս��ͼ��ʼʱ�䣨�ӵ����������������
	int							m_countryEndTime;	//	��ս��ͼ����ʱ�䣨�ӵ����������������
	int							m_nTotalBonus;		//	���ι�ս�ܽ���

	int							m_iKingID[DOMAIN2_INFO::COUNTRY_COUNT];	// ����ID
	PAUILABEL					m_pLbl_KingName[DOMAIN2_INFO::COUNTRY_COUNT];

	int							m_nTotalScore;		//	�����ܻ���
	int							m_nCountryScore[DOMAIN2_INFO::COUNTRY_COUNT];	//	�������ܻ���
	TimeType					m_cntGetScore;		//	���»��ּ�ʱ��

	int							m_iLimitDomainCnt;	//  ������������ս������

	enum MoveState
	{
		MS_STOPPED,				//	ֹͣ
		MS_WAITING_MOVE,		//	�ȴ��ظ��ƶ�
		MS_WAITING_ARRIVE,		//	�ƶ��С��ȴ�����Ŀ��
		MS_WAITING_STOP,		//	�ƶ��С��ȴ��ظ�ֹͣ
		MS_WAITING_REDIRECT,	//	�ƶ��С��ȴ��ظ�����Ŀ��
	};
	MoveState					m_moveState;
	bool IsStopped()const{ return m_moveState == MS_STOPPED; }
	bool IsWaitingMove()const{ return m_moveState == MS_WAITING_MOVE; }
	bool IsWaitingArrive()const{ return m_moveState == MS_WAITING_ARRIVE; }
	bool IsWaitingStop()const{ return m_moveState == MS_WAITING_STOP; }
	bool IsWaitingRedirect()const{ return m_moveState == MS_WAITING_REDIRECT; }

	int							m_domainNext;	//	�ƶ�ʱ����Ŀ��
	TimeType					m_cntMoveStart;	//	��ʼ�ƶ�ʱ��
	TimeType					m_cntMoveEnd;	//	��������Ŀ��ʱ��
	CECDomainCountryInfo::Route	m_route;		//	�ƶ��ο�·��������㡢Ŀ�꣩
	CECDomainCountryInfo::Route	m_routeCand;	//	�ο�·����ʱ����

	bool						m_bLoadCenter;	//	m_vLoadCenter �Ƿ���Ч
	A3DVECTOR3					m_vLoadCenter;	//	����Ľ�Ҫ������ 143 ��ͼλ�ã���ǰ�Ի�������ʱ�ݽ�
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