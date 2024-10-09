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

	int		GetCurMonth();			// ��ȡ��ǰ��
	int		GetCurDay();			// ��ȡ��ǰ��
	int		GetSignedInCount();		// ��ȡ������ǩ������
	int		GetRecvAwardCount();	// ��ȡ�������콱����
	int		GetCanLateSignInCount();// ��ȡ���¿ɲ�ǩ����
	int		GetDaysInMonth();		// ��ȡһ���µ�����
	bool	HasSignedInToday();		// �����Ƿ�ǩ��

	void	ShowMonth(int iMonth);									// ��ʾ��ǰ�·�
	void	ShowSignedInDaysCount(int iSignedInDaysCount);			// ��ʾ��ǰ��ǩ������
	void	ShowCanLateSignDaysCount(int iCanLateSignDaysCount);	// ��ʾ��ǰ�ɲ�ǩ����
	void	ShowAwardListDayRange();	// ��ʾ�콱�б�ķ�Χ
	void	ResetAwardListDayRange();	// �����콱�б�Χ�������콱��ID�����б���
	void	SetListMoveBtnEnable();		// �����б��ƶ���ť�Ƿ����
	void	ShowAwardListItemsView();	// ��ʾ��ǰ�Ľ�����Ʒ����
	bool	IsSignInEnoughThisMonth();	// �����Ƿ�����ǩ��δǩ����������3��
	bool	IsSignInCompeleteThisMonth();// �����Ƿ�ȫ�ڣ�δǩ����0��

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

