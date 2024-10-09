// Filename	: DlgGeneralCard.h
// Creator	: Han Guanghui
// Date		: 2013/09/06

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "EC_Counter.h"
#include "EC_IvtrTypes.h"
#include <vector>

class ATaskTempl;
class CECControlAnimControl;
class CECIvtrGeneralCard;
//////////////////////////////////////////////////////////////////////////

class CDlgGeneralCard : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

	friend class CECControlAnimControl;

public:
	CDlgGeneralCard();
	virtual ~CDlgGeneralCard();

	void				OnCommandRight(const char* szCommand);
	void				OnCommandLeft(const char* szCommand);

	void				OnCommandBuyCard(const char* szCommand);
	void				OnCommandStorage(const char* szCommand);
	void				OnCommandConfirm(const char* szCommand);
	void				OnCommandCancel(const char* szCommand);
	void				OnCommandSwallow(const char* szCommand);
	void				OnCommandVolume(const char* szCommand); // 图鉴
	void				OnCommandMonsterSpirit(const char* szCommand);

	void				OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void				OnEventLButtonDblclkCard(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void				OnEventLButtonDown_Left(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void				OnEventLButtonDown_Right(WPARAM wParam, LPARAM lParam, AUIObject *pObj);


	void				OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	bool				SwallowCard(unsigned char is_inv,					// 1-普通包裹, 0-卡牌仓库
									unsigned char swallowed_card_index,		// 被吞噬卡牌在包裹中的索引
									unsigned int count,						// 数量
									bool bBatchSwallow);					// 是否用于批量吞噬
	bool				IsSwallowing() { return m_bSwallowing; }
	void				SetSwallowing(bool swallow);
	void				Update();
	void				SendSwallowCardCmd();
	void				SetSelectCard(int index);
	void				SetCardImage(CECIvtrItem* pItem);
	static const char*	GetCardFrameFile(unsigned int rank);
	static void			SetImgCover(PAUIIMAGEPICTURE pImg, const char* path_img);
	static void			SetCardImage(CECIvtrItem *pItem, PAUIIMAGEPICTURE pImgCard, PAUIIMAGEPICTURE pImgFrame, PAUIIMAGEPICTURE pImgGfx);
	static void			SetCardImage(int tid, PAUIIMAGEPICTURE pImgCard, PAUIIMAGEPICTURE pImgFrame, PAUIIMAGEPICTURE pImgGfx);
	CECIvtrGeneralCard*	GetCardByIndex(int i);
	bool				CanAnimateCard();

protected:
	enum{
		UN_SELECTED,
		SELECTED,
	};
	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();
	virtual void		OnHideDialog();
	virtual void		OnTick();
	virtual bool		Release();

	virtual bool		OnChangeLayoutBegin();
	virtual void		OnChangeLayoutEnd(bool bAllDone);

	void				UpdateDisplay();
	void				UpdateEquipment();
	void				SetImagePath(const char* path_card, const char* path_frame, const char* path_gfx);
	bool				IsCardChanged();
	unsigned char		GetSelectedCardIndex();

	DWORD				m_dwDataToDetectChange[SIZE_GENERALCARD_EQUIPIVTR];
	PAUIIMAGEPICTURE	m_pImgEquip[SIZE_GENERALCARD_EQUIPIVTR];
	PAUIIMAGEPICTURE	m_pImgCard;
	PAUIIMAGEPICTURE	m_pImgGfx;
	PAUIIMAGEPICTURE	m_pImgFrame;
	bool				m_bSwallowing;
	unsigned char		m_ucParamForSwallow_isinv;
	unsigned char		m_ucParamForSwallow_swallowed_card_index;
	unsigned int		m_uiParamForSwallow_count;
	enum DIALOG_TYPE{
		CARD_EQUIP,
		CARD_CHAT_LINK_ITEM,
		CARD_MOUSE_HOVER,
	};
	DIALOG_TYPE			m_enumDlgType;

	PAUIIMAGEPICTURE	m_pImgCardLeft;
	PAUIIMAGEPICTURE	m_pImgCardRight;
	PAUIIMAGEPICTURE	m_pImgCardLeftFrame;
	PAUIIMAGEPICTURE	m_pImgCardRightFrame;

	PAUIIMAGEPICTURE	m_pImgCardLeftOut;
	PAUIIMAGEPICTURE	m_pImgCardRightOut;
	PAUIIMAGEPICTURE	m_pImgCardLeftFrameOut;
	PAUIIMAGEPICTURE	m_pImgCardRightFrameOut;

	CECControlAnimControl* m_ctrl;

	CECCounter m_launchAnimCounterNext;
	CECCounter m_launchAnimCounterPrev;
};

//////////////////////////////////////////////////////////////////////////

struct POINT_f
{
	float x;
	float y;
	POINT_f& operator=(const POINT& t);
	POINT_f& operator=(const SIZE& t);
};

class CECControlAnimation
{
public:
	enum
	{
		ANIM_MOVE_ACC=0,
		ANIM_MOVE_EXP,
		ANIM_MOVE_FAST,
	};
public:
	CECControlAnimation(int moveType, DWORD dwTimeCost=400);
	void AddControl(PAUIOBJECT src,PAUIOBJECT des);
	void AddControl(PAUIOBJECT src, const POINT &desPos, const SIZE &desSize);
	void ChangeMoveType(int newMoveType);
	int  GetMoveType()const{ return m_moveType; }
	
	bool Tick(DWORD dwDelta);

	bool IsFinished() const { return m_bFinished;}
	PAUIOBJECT GetSrcControl()const{ return m_pSrc; }

protected:
	bool CanFinish();
	DWORD GetElapsed(DWORD dwDelta)const;
	DWORD GetElapsed(int moveType, DWORD timeElapsed, DWORD dwDelta)const;
	float GetMoveRatio()const;
	float GetMoveRatio(int moveType, DWORD timeElapsed, DWORD timeCost)const;
private:
	PAUIOBJECT m_pSrc;

	POINT_f m_delta;
	POINT_f m_start;
	POINT_f m_end;
	POINT_f m_cur;

	POINT_f	m_deltaSize;
	POINT_f	m_curSize;
	POINT_f	m_startSize;
	POINT_f	m_endSize;

	bool m_bFinished;
	int m_moveType;
	DWORD m_timeElapsed;
	DWORD m_timeCost;
};


class CECUIControlData
{
public:
	void Register(PAUIOBJECT obj);
	void UpdateUI();
protected:	
	
	abase::hash_map<PAUIOBJECT,POINT> m_ControlPos;
	abase::hash_map<PAUIOBJECT,SIZE> m_ControlSize;
};

class CECCardIndexSelector
{
public:
	CECCardIndexSelector():m_curIndex(-1) { memset(m_realdCardIndexMap,0,sizeof(m_realdCardIndexMap));};
	bool IsValid() const { return m_curIndex >=0;};
	bool MovePrev();
	bool MoveNext();
	int GetCurCardIndex() const ;
	int GetCardPrevSib(int card);
	int GetCardNextSib(int card);
	bool ComputeMoveTimes(int destIndex,int& times);
	void UpdateRealCardIndex(CDlgGeneralCard* pDlg);
protected:
	int m_curIndex;
	abase::vector<int> m_realCardIndex;
	int m_realdCardIndexMap[SIZE_GENERALCARD_EQUIPIVTR];
};

class CECControlAnimControl
{
public:
	CECControlAnimControl(CDlgGeneralCard* pDlg);

	void Init();
	void Release();
	void ReleaseAnim();
	void RecoverControl(); // 恢复控件位置和Size
	void Reset();

	bool HasAnimRunning() const { return m_pAnimVec.size()>0;}

	void Prev();
	void Next();
	void JumpTo(int dest);

	void Tick(DWORD dt);

	void ApplyCardImgToControl(bool bSel); // 设置控件的卡牌图片

protected:
	void SetImgCover(int cur,int sib, PAUIIMAGEPICTURE cardCover,PAUIIMAGEPICTURE cardFrame);

protected:
	CDlgGeneralCard* m_pDlg;
	abase::vector<CECControlAnimation*> m_pAnimVec;
	A3DViewport* m_pNewViewport;
	CECUIControlData m_UIData;
	CECCardIndexSelector m_cardIndexSelector;

	bool m_bMoveForwards;
	int m_iLoopTimes;
};