// Filename	: DlgGMQueryItem.h
// Creator	: Feng Ning
// Date		: 2011/03/25

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUIListbox.h"
#include "AUISubDialog.h"
#include "AUIImagePicture.h"

class CDlgGMQueryItemSub : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	
public:
	void OnEventLButtonDown_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDBCLK_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
};

class CDlgGMQueryItem : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	enum
	{
		FILTER_MAX = 7,
	};

	CDlgGMQueryItem();
	virtual ~CDlgGMQueryItem();

	void OnCommand_ShowType(const char * szCommand);
	void OnCommand_DeleteItems(const char * szCommand);
	void OnEventLButtonDBCLK_Result(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDBCLK_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_Items(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal);
	virtual void OnGMQueryResult(int iType, const void* pData, DWORD dwSize);

	virtual void OnChangeLayoutEnd(bool bAllDone);

	// calling by the number select dialog
	void ConfirmCount(int count);

	// set the target player and item
	void UpdateTarget(int idPlayer, int idItem);

protected:
	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void RefreshResult(int type);	

	enum 
	{
		STATE_WAIT,
		STATE_DELETING,
	};
	int m_State;

	AString m_IconFile;
	int m_ItemID;
	int m_PlayerID;
	int m_FilterType;
	struct ItemInfo
	{
		unsigned char where;
		unsigned char index;
		unsigned int count;
	};
	typedef abase::vector<ItemInfo> ItemInfos;
	ItemInfos m_Infos;
	ACString GetItemDesc(const ItemInfo& info);

	ItemInfos m_Deleteing;
	void TryToDelete(const ItemInfos& infos);
	void ConfirmDelete(const ItemInfos& infos);

private:
	PAUISTILLIMAGEBUTTON m_pFilters[FILTER_MAX+1];
	PAUISTILLIMAGEBUTTON m_pBtnDelete;

	PAUILISTBOX m_pLstResult;
	PAUISUBDIALOG m_pSubItems;

	typedef abase::vector<PAUIIMAGEPICTURE> ItemIcons;
	ItemIcons m_pIcons;

	PAUIOBJECT m_pQuerying;
};
