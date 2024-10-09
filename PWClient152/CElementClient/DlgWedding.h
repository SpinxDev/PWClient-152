// Filename	: DlgWedding.h
// Creator	: Xu Wenbin
// Date		: 2010/07/06

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;
class CECPlayer;

class CDlgWedding : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWedding();
	
	void OnCommand_PrevMonth(const char *szCommand);
	void OnCommand_NextMonth(const char *szCommand);
	void OnCommand_Rdo(const char *szCommand);
	void OnCommand_Book(const char *szCommand);
	void OnCommand_CancelBook(const char *szCommand);
	void OnCommand_Cancel(const char *szCommand);
	
	void OnEventLButtonDown_Img(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUP_Img(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Txt(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_List1(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void SetItem(int iSlot = -1);

	void OnWeddingBookList(void *pData);
	void OnWeddingBookSuccess(void *pData);

	void CancelBook(bool bConfirm);
	
	enum {GENERAL_BOOKCARD_ID = 28452};	//	ͨ������ƾ֤

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
		
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Release();


	PAUIIMAGEPICTURE m_pImg_Week[6][7];
	PAUILABEL m_pLab_Week[6][7];
	PAUILISTBOX m_pTxt_List1;	//	�������ڡ�����ʱ��Ԥ���б�
	PAUILABEL m_pTxt_Title;		//	��ǰ����
	PAUIIMAGEPICTURE m_pImg_Special;	//	����ƾ֤��Ʒ

	PAUISTILLIMAGEBUTTON m_pBtn_PrevMonth;
	PAUISTILLIMAGEBUTTON m_pBtn_NextMonth;
	PAUISTILLIMAGEBUTTON m_pBtn_Book;
	PAUISTILLIMAGEBUTTON m_pBtn_CancelBook;

private:

	//	ԤԼ�б����仯
	void OnBookListChange();

	//	ѡȡ�·ݷ����仯����ʼ��������Ϊѡ�񣬻�ʵ�����ڱ仯������ǰѡ�񲻺Ϸ���
	void OnMonthSelectChange();

	//	ѡ�����ӷ����仯����ʼ��������Ϊѡ�񣬻�ʵ�����ڱ仯������ǰѡ�񲻺Ϸ���
	void OnDaySelectChange();

	//	����ĳ���ѡ�����仯����ʼ��������Ϊѡ��
	void OnWeddingSceneSelectChange();

	//	���񳡴�ѡ�����仯����ʼ��������Ϊѡ��
	void OnWeddingSessionSelectChange();

	//	���µ�ǰѡ���µ����������ֵȣ�
	void UpdateCalendar();

	//	���µ�ǰ�����е�ͼƬ״̬��ѡ�С�Ԥ����
	void UpdateCalendarColor();

	//	���յ�ǰʱ�䣬���㵱ǰѡ���·����һ����ѡ�е�����
	int	 GetFirstSelectDay();

	//	���ҵ�ǰ�ɹ�ѡ��ĵ�һ������
	int  GetFirstScene();

	//	���ݵ�ǰ�����ж��Ƿ�������·�����ʾ������
	bool CanShowMonth(int year, int month);

	//	���ݵ�ǰ�����жϸ�������������ʾ����
	bool CanShowDay(int year, int month, int day);

	//	�жϵ�ǰ�����Ƿ��п���ʾ��Ԥ���б�
	bool HaveSessionDay(int year, int month, int day);

	//	�жϸ��������Ƿ�����������
	bool IsSpecialDay(int year, int month, int day);

	//	�ж��Ƿ���и����������ڵ���Ʒ�����Ͻ���Ϊ׼��
	bool HaveSpecialDayItem(int year, int month, int day);

	//	���ҵ�ǰ��Ʒ��Ӧ����������
	bool GetCurSpecialDay(int &year, int &month, int &day);

	//	���ҵ�ǰѡ�еĳ��Σ�����Ԥ���򷵻�-1��
	int	GetSelectedSession();

	//	�ж�ָ�������Ƿ��Ԥ��
	bool CanBookSession(int nSession);

	//	�ж�ָ�������Ƿ��ȡ��Ԥ��
	bool CanCancelBookSession(int nSession);

	//	�ж�ԤԼƾ֤�Ƿ�Ϸ�
	bool IsImageItemValid(int iSlot);

	//	��ѯԤ���б�
	void GetWeddingBookListFromServer();

	//	�������ɵ�ǰ���ڡ�������Ԥ���б�
	void RebuildSessionList();
	
	//	����Ԥ���б��������ʾ
	void UpdateSessionListHint(int nItem);

	//	������ҵ����Ƹ��½���
	void UpdatePlayerNames();

	//	����ֻ��Сʱ���ӵĳ����ַ���
	ACString GetSessionTimeString(int t);
	
	//	��գ��Ա��´���ʾʱ���¿�ʼ��
	void Reset();

	//	����������
	bool CheckTeamCondition(bool bBook);
	CECPlayer * GetTeamOtherPlayer();

	//	��ť��״̬����
	bool CanGoPrevMonth();
	bool CanGoNextMonth();
	bool CanBook();
	bool CanCancelBook();

	//	��������
	bool IsContinuousMonth(int year1, int month1, int year2, int month2);
	void GetPrevMonth(int year, int month, int &prevMonthYear, int &prevMonth);
	void GetNextMonth(int year, int month, int &nextMonthYear, int &nextMonth);

	//	ʱ���ѯ�ͼ���
	void GetCurServerTime(int &year, int &month, int &day, char &hour, char &minute, char &second);
	tm	 GetServerLocalTime(int t);
	void GetServerLocalTime(int t, int &year, int &month, int &day);
	void GetYearMonthDay(const tm &t1, int &year, int &month, int &day);

	//	��Ա����
	int m_curYear;	//	��ǰ���
	int m_curMonth;	//	��ǰ�·�
	int m_curDay;	//	��ǰ����

	int m_selYear;	//	ѡ�е���ݣ�Ĭ��Ϊ��ǰ��ݣ������û�ѡ���ǰʱ�䣩
	int m_selMonth;	//	ѡ�е��·ݣ�ͬ�ϣ�
	int m_selDay;	//	ѡ�е����ӣ�ͬ�ϣ�

	int m_selScene;	//	ѡ�еĻ��񳡾�
	int m_iSlot;	//	ԤԼƾ֤�±�

	struct wedding_book 
	{
		int start_time;
		int end_time;
		int groom;
		int bride;
		int scene;
		char status;
		char special;
	};
	typedef abase::vector<wedding_book> wedding_book_list;
	wedding_book_list m_bookList;

	//	�жϰ������Ƿ����ض����ڵ�����ƾ֤
	bool HaveInviteCard(const wedding_book &session);

	A2DSprite * m_pSprite_Icon1;
	A2DSprite * m_pSprite_Icon2;
	bool		m_bSpriteInited;
	void SetIcon(PAUIIMAGEPICTURE pImage, bool bWithSessionList);
	A2DSprite * LoadSprite(AString strFile);
};