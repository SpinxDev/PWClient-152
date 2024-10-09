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
	
	enum {GENERAL_BOOKCARD_ID = 28452};	//	通用排期凭证

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
		
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Release();


	PAUIIMAGEPICTURE m_pImg_Week[6][7];
	PAUILABEL m_pLab_Week[6][7];
	PAUILISTBOX m_pTxt_List1;	//	给定日期、场景时的预订列表
	PAUILABEL m_pTxt_Title;		//	当前日期
	PAUIIMAGEPICTURE m_pImg_Special;	//	特殊凭证物品

	PAUISTILLIMAGEBUTTON m_pBtn_PrevMonth;
	PAUISTILLIMAGEBUTTON m_pBtn_NextMonth;
	PAUISTILLIMAGEBUTTON m_pBtn_Book;
	PAUISTILLIMAGEBUTTON m_pBtn_CancelBook;

private:

	//	预约列表发生变化
	void OnBookListChange();

	//	选取月份发生变化（初始化，或人为选择，或实际日期变化导致以前选择不合法）
	void OnMonthSelectChange();

	//	选择日子发生变化（初始化，或人为选择，或实际日期变化导致以前选择不合法）
	void OnDaySelectChange();

	//	婚礼的场景选择发生变化（初始化，或人为选择）
	void OnWeddingSceneSelectChange();

	//	婚礼场次选择发生变化（初始化，或人为选择）
	void OnWeddingSessionSelectChange();

	//	更新当前选中月的月历（文字等）
	void UpdateCalendar();

	//	更新当前日历中的图片状态（选中、预订）
	void UpdateCalendarColor();

	//	参照当前时间，计算当前选中月份里第一个可选中的日子
	int	 GetFirstSelectDay();

	//	查找当前可供选择的第一个场景
	int  GetFirstScene();

	//	根据当前日期判断是否给定的月份能显示日历）
	bool CanShowMonth(int year, int month);

	//	根据当前日期判断给定的日子能显示场次
	bool CanShowDay(int year, int month, int day);

	//	判断当前日期是否有可显示的预订列表
	bool HaveSessionDay(int year, int month, int day);

	//	判断给定日期是否是特殊日期
	bool IsSpecialDay(int year, int month, int day);

	//	判断是否具有给定特殊日期的物品（拖上界面为准）
	bool HaveSpecialDayItem(int year, int month, int day);

	//	查找当前物品对应的特殊日期
	bool GetCurSpecialDay(int &year, int &month, int &day);

	//	查找当前选中的场次（不可预订则返回-1）
	int	GetSelectedSession();

	//	判断指定场次是否可预定
	bool CanBookSession(int nSession);

	//	判断指定场次是否可取消预定
	bool CanCancelBookSession(int nSession);

	//	判断预约凭证是否合法
	bool IsImageItemValid(int iSlot);

	//	查询预订列表
	void GetWeddingBookListFromServer();

	//	重新生成当前日期、场景的预订列表
	void RebuildSessionList();
	
	//	更新预订列表的悬浮提示
	void UpdateSessionListHint(int nItem);

	//	根据玩家的名称更新界面
	void UpdatePlayerNames();

	//	生成只含小时分钟的场次字符串
	ACString GetSessionTimeString(int t);
	
	//	清空（以便下次显示时重新开始）
	void Reset();

	//	检查组队条件
	bool CheckTeamCondition(bool bBook);
	CECPlayer * GetTeamOtherPlayer();

	//	按钮的状态控制
	bool CanGoPrevMonth();
	bool CanGoNextMonth();
	bool CanBook();
	bool CanCancelBook();

	//	辅助函数
	bool IsContinuousMonth(int year1, int month1, int year2, int month2);
	void GetPrevMonth(int year, int month, int &prevMonthYear, int &prevMonth);
	void GetNextMonth(int year, int month, int &nextMonthYear, int &nextMonth);

	//	时间查询和计算
	void GetCurServerTime(int &year, int &month, int &day, char &hour, char &minute, char &second);
	tm	 GetServerLocalTime(int t);
	void GetServerLocalTime(int t, int &year, int &month, int &day);
	void GetYearMonthDay(const tm &t1, int &year, int &month, int &day);

	//	成员变量
	int m_curYear;	//	当前年份
	int m_curMonth;	//	当前月份
	int m_curDay;	//	当前日子

	int m_selYear;	//	选中的年份（默认为当前年份，但随用户选择或当前时间）
	int m_selMonth;	//	选中的月份（同上）
	int m_selDay;	//	选中的日子（同上）

	int m_selScene;	//	选中的婚礼场景
	int m_iSlot;	//	预约凭证下标

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

	//	判断包裹中是否有特定日期的排期凭证
	bool HaveInviteCard(const wedding_book &session);

	A2DSprite * m_pSprite_Icon1;
	A2DSprite * m_pSprite_Icon2;
	bool		m_bSpriteInited;
	void SetIcon(PAUIIMAGEPICTURE pImage, bool bWithSessionList);
	A2DSprite * LoadSprite(AString strFile);
};