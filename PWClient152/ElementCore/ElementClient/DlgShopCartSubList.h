// Filename	: DlgShopCartSubList.h
// Creator	: Xu Wenbin
// Date		: 2014/5/20

#ifndef _ELEMENTCLIENT_DLGSHOPCARTSUBLIST_H_
#define _ELEMENTCLIENT_DLGSHOPCARTSUBLIST_H_

#include "DlgBase.h"

#include <AUISubDialog.h>

class CECShoppingCart;
class CECObservableChange;
typedef int	ItemShoppingCartID;
class CDlgShopCartSub;

class CDlgShopCartSubList : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgShopCartSubList();
	
	void OnCommand_CANCEL(const char *szCommand);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);

	void SetShopCart(CECShoppingCart *pShoppingCart);

	//	由父对话框转发，来自 CECShoppingCartObserver
	virtual void OnModelChange(const CECShoppingCart *p, const CECObservableChange *q);

protected:
	virtual bool OnInitDialog();
	virtual bool Release();
	
	void ClearItems();
	void UpdateItems();
	void Append(ItemShoppingCartID itemCartID);

	CECShoppingCart * m_pShoppingCart;

	PAUISUBDIALOG	m_pSub;					//	SUBDIALOG 控件模板
	AString			m_strDialogName;		//	子对话框名称
	AString			m_strDialogFile;		//	子对话框文件

	typedef abase::vector<PAUISUBDIALOG> SubDialogArray;
	SubDialogArray	m_subs;					//	依次包含所有商品的控件列表
	int				m_nNextSubDialogID;		//	下次添加子控件时使用的下标

	bool			m_inRelease;			//	当前正在释放中

	int				AllocSubDialogID();				//	分配一个新的子对话框ID（用于添加新商品）
	int				SubDialogCount()const;			//	当前子对话框的个数（对应商品个数）
	AString			MakeSubDialogName(int index);	//	查询给定下标的子对话框名称
	PAUISUBDIALOG	GetSubDialog(int index);		//	查询给定下标的子对话框
	PAUISUBDIALOG	AppendSubDialog();				//	增加一个SubDialog
	void			RemoveSubDialog(PAUISUBDIALOG pSub);
	SIZE			CurrentDialogDefaultSize();
	SIZE			SubDialogOrgSize();
	void			FitSize();						//	使当前对话框大小适配所有SUBDIALOG
	void			SubDialogResetScroll();			//	恢复滚动条到默认位置（将要进行位置调整）

	CDlgShopCartSub * FindSubDialogFor(ItemShoppingCartID itemCartID, SubDialogArray::iterator &it);
	void			RemoveSubDialogAndAdjust(SubDialogArray::iterator it);
};

#endif	//	_ELEMENTCLIENT_DLGSHOPCARTSUBLIST_H_