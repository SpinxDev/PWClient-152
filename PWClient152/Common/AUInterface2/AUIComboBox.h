// Filename	: AUIComboBox.h
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIComboBox is the class of combo box control.

#ifndef _AUICOMBOBOX_H_
#define _AUICOMBOBOX_H_

#include "vector.h"

#include "AAssist.h"
#include "AUIObject.h"
#include "AUIFrame.h"
#include "AUICommon.h"

#define AUICOMBOBOX_ERROR			(-1)

#define AUICOMBOBOX_STATE_NORMAL	0
#define AUICOMBOBOX_STATE_POPUP		1

enum
{
	AUICOMBOBOX_SORT_ASCENT = 0,
	AUICOMBOBOX_SORT_DESCENT
};

enum
{
	AUICOMBOBOX_FRAME_NORMAL,
	AUICOMBOBOX_FRAME_EXPAND,
	AUICOMBOBOX_FRAME_PUSHED,
	AUICOMBOBOX_FRAME_HILIGHT
};

enum
{
	AUICOMBOBOX_DIR_UP = 0,
	AUICOMBOBOX_DIR_DOWN
};

typedef struct _AUICOMBOBOX_ITEM
{
	ACString strText;
	ACString strTextSrc;
	ACString strHint;
	DWORD dwData;
	void *pvData;
	A3DCOLOR color;
	AString strDataName;
	AString strDataPtrName;
	UINT64 uiData64;
	UINT64 uiDataPtr64;
	AString strData64Name;
	AString strDataPtr64Name;
	EditBoxItemsSet itemsSet;
} AUICOMBOBOX_ITEM, * P_AUICOMBOBOX_ITEM;

class AUIListBox;

class AUIComboBox : public AUIObject
{
public:
	AUIComboBox();
	virtual ~AUIComboBox();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool InitIndirect(const char *pszFileName, int idType);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	/* support properties
	"Frame mode"			Property->i
	"Frame Image"			Property->fn
	"Frame Expand Image"	Property->fn
	"Frame Pushed Image"	Property->fn
	"Hilight Image"			Property->fn
	"List Box Name"			Property->fn
	"Align"					Property->i

	And AUIObject.Properties
	*/
	virtual bool GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();
	virtual A3DRECT GetRect(void);
	virtual void SetFocus(bool bFocus);
	virtual bool Resize();
	virtual void Show(bool bShow);
	virtual void OptimizeSpriteResource(bool bLoad);

	void Command();
	int AddString(const ACHAR *pszString);
	int InsertString(int nIndex, const ACHAR *pszString);
	int DeleteString(int nIndex);
	int GetCount(void);
	int GetCurSel(void);
	int SetCurSel(int nIndex);
	const ACHAR * GetText(int nIndex);
	int SetText(int nIndex, const ACHAR *pszText);
	A3DCOLOR GetItemTextColor(int nIndex);
	int SetItemTextColor(int nIndex, A3DCOLOR color);
	void ResetContent(void);
	void * GetItemDataPtr(int nIndex, AString strName = "");
	int SetItemDataPtr(int nIndex, void *pData, AString strName = "");
	DWORD GetItemData(int nIndex, AString strName = "");
	int SetItemData(int nIndex, DWORD dwItemData, AString strName = "");
	UINT64 GetItemDataPtr64(int nIndex, AString strName = "");
	int SetItemDataPtr64(int nIndex, UINT64 uiDataPtr64, AString strName = "");
	UINT64 GetItemData64(int nIndex, AString strName = "");
	int SetItemData64(int nIndex, UINT64 uiData64, AString strName = "");
	const ACHAR * GetItemHint(int nIndex);
	int SetItemHint(int nIndex, const ACHAR *pszHint);
	int GetActiveItem();
	bool SortItems(int nMode);
	void SetDefaultDir(int nDir);
	void SetListBox(AUIListBox* pList);
	const AUIListBox* GetListBox() const;
	void SetItemMask(int nMask)	{ m_nItemMask = nMask; }
	void SetImageList(abase::vector<A2DSprite*> *pvecImageList) { m_pvecImageList = pvecImageList; }

protected:
	int m_nDir;
	int m_nDefaultDir;
	int m_nCurSel;
	int m_nActiveItem;
	bool m_bJustPopup;
	int m_nFrameMode;
	int m_nItemMask;
	PAUIFRAME m_pAUIFrame, m_pAUIFrameExpand, m_pAUIFramePushed;
	AUIListBox *m_pListBox;
	A2DSprite *m_pA2DSpriteHilight;
	abase::vector<AUICOMBOBOX_ITEM> m_Item;
	abase::vector<A2DSprite*> *m_pvecImageList;
};

typedef AUIComboBox * PAUICOMBOBOX;

#endif //_AUICOMBOBOX_H_