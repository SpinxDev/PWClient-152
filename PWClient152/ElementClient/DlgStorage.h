/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   19:38
	file name:	DlgStorage.h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIScroll.h"
#include "EC_Counter.h"

class CECIvtrItem;
class CECInventory;
class CDlgStorage : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgStorage();
	virtual ~CDlgStorage();
	virtual bool Tick(void);

	void OnLButtonDownItem(WPARAM, LPARAM, AUIObject *pObj);
	void OnLButtonDbClickItem(WPARAM, LPARAM, AUIObject *pObj);
	void OnRButtonDownItem(WPARAM, LPARAM, AUIObject *pObj);
	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnLButtonDownCardDice(WPARAM, LPARAM, AUIObject *pObj);

	void OnCommandChooseMoney(const char * szCommand);
	void OnCommandStorage(const char * szCommand);
	void OnCommandStorage1(const char * szCommand);
	void OnCommandStorage2(const char * szCommand);
	void OnCommandIDCANCEL(const char * szCommand);
	void OnCommandChangeSkirt(const char *szCommand);
	void OnCommand_arrange(const char *szCommand);
	void OnCommand_arrange_fashion_bypart(const char *szCommand);
	void OnCommand_arrange_fashion_bysuite(const char *szCommand);
	void OnCommand_Btn_All(const char *szCommand);
	void OnCommandBatchSwallow(const char *szCommand);
	void OnCommmandAutoDice(const char *szCommand);

	int  GetMoneyCnt();
	bool IsFashionStorage();
	bool IsGeneralCardStorage();

	PAUIOBJECT GetEmptySlot(CECIvtrItem* pIvtrSrc);
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void AutoDice();
	bool UpdateStorage();
protected:
	abase::vector<PAUIIMAGEPICTURE> m_vecImgCell;
	abase::vector<PAUIIMAGEPICTURE> m_vecImgBg;
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool OnChangeLayoutBegin();
	virtual bool Render();

	int GetHighlightedFashionEquipMask();

	void StartBatchSwallow();
	void EndBatchSwallow();
	void BatchSwallow();
	void StartAutoDice();
	void EndAutoDice();
	bool OnDiceDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjOver);
	void UpdateDiceImg();
	CECInventory* GetInventory();

	enum {FASHION_COUNT = 6};
	PAUIOBJECT m_pTxt_Fashion[FASHION_COUNT];
	PAUIOBJECT m_pTxt_FashionAll;

	enum {FASHION_PER_LINE = 8};
	PAUISCROLL m_pScl_Right;
	CECCounter m_BatchSwallowCounter;
	int		   m_iSwallowIndex;
	bool	   m_bSwallowing;
	bool	   m_bAutoDice;
};

