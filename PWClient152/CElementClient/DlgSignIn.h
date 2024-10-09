// Filename	: DlgTitleChallenge.h
// Creator	: Han Guanghui
// Date		: 2013/6/13

#ifndef _DLGSIGN_IN_H_ 
#define _DLGSIGN_IN_H_

#include "DlgBase.h"

const int AWARD_LIST_SIZE = 7;
const int MONTH_COUNT_IN_YEAR = 12;
const int MAX_LATE_SIGN_IN_COUNT = 4;

struct SIGN_AWARD_CONFIG;
class AUIStillImageButton;
class AUIImagePicture;

class CDlgSignIn : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:

	CDlgSignIn();
	virtual ~CDlgSignIn();

	void				OnCommandSignIn(const char* szCommand);
	void				OnCommandGetYearReward(const char* szCommand);
	void				OnCommandGetDailyReward(const char* szCommand);

	void				OnCommandMoveLeft(const char* szCommand);
	void				OnCommandMoveRight(const char* szCommand);

	void				OnEventMouseDownDateImg(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void				Update(int iServerTime);


private:

	int		GetCurMonth();			// 获取当前月
	int		GetCurDay();			// 获取当前日
	int		GetSignedInCount();		// 获取本月已签到次数
	int		GetRecvAwardCount();	// 获取本月已领奖次数
	int		GetCanLateSignInCount();// 获取本月可补签次数
	int		GetDaysInMonth();		// 获取一个月的天数
	bool	HasSignedInToday();		// 今天是否签到

	void	ShowMonth(int iMonth);									// 显示当前月份
	void	ShowSignedInDaysCount(int iSignedInDaysCount);			// 显示当前已签到天数
	void	ShowCanLateSignDaysCount(int iCanLateSignDaysCount);	// 显示当前可补签天数
	void	ShowAwardListDayRange();	// 显示领奖列表的范围
	void	ResetAwardListDayRange();	// 重设领奖列表范围，将可领奖的ID放在列表中
	void	SetListMoveBtnEnable();		// 设置列表移动按钮是否可用
	void	ShowAwardListItemsView();	// 显示当前的奖励物品界面
	bool	IsSignInEnoughThisMonth();	// 本月是否已满签（未签次数不超过3）
	bool	IsSignInCompeleteThisMonth();// 本月是否全勤（未签次数0）

	void	SetAwardBtn(int awardDay, AUIStillImageButton* pBtn, int signedInDays, int recvAwardDays, int curDay);
	void	SetAwardImg(AUIImagePicture* pImg, int itemID, int itemCount);

	void	GetAwardItemInfo(int month, int day, int &itemID, int &itemCount);

protected:

	virtual void		OnShowDialog();
	virtual bool		OnInitDialog();
	bool				SumUpMonthState(int year_state, int state);
	
	enum{
		IMG_MONTH_FRAME_ABSENT_MORE_THAN_3,
		IMG_MONTH_FRAME_ABSENT_LESS_OR_EQUAL_THAN_3,
		IMG_MONTH_FRAME_COMPLETE,
	};
	enum{
		IMG_DATE_FRAME_TODAY,
		IMG_DATE_FRAME_SIGNED_IN,
		IMG_DATE_FRAME_ABSENT,
		IMG_DATE_FRAME_FUTURE,
	};
	int					m_iCurrentSelection;
	int					m_iServerUpdateTime;
	int					m_iAwardListFirstDay;
	int					m_iAwardListLastDay;
	int					m_iEmptySlotsCount;
	SIGN_AWARD_CONFIG*	m_signAwardConfigs[MONTH_COUNT_IN_YEAR];
};

#endif // _DLGSIGN_IN_H_

