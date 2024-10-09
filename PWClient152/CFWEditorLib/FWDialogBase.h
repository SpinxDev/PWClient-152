#ifndef _FWDIALOGBASE_H_
#define _FWDIALOGBASE_H_

#include "CodeTemplate.h"
#include "AUIObject.h"
#include "AUIDialog.h"
#include "FWDefinedMsg.h"
#include "FWConfig.h"
#include "FWObject.h"

class AUIDialog;
class AUIObject;
typedef AUIObject* PAUIOBJECT;

class FWView;
class FWDoc;
class FWAUIManager;
class AUIListBox;

class FWDialogBase;


// callback helper for DoModal()
typedef void (*FuncDoModalCallBack)(void *pData);

// type of function that called when user select a different
// item in a listbox
typedef void (FWDialogBase::*FuncOnSelChangeListbox)(int nSelIndex);

// represent a dialog
// this is a wrapper of AUIDialog
class FWDialogBase : public FWObject
{
	friend class FWAUIManager;
	FW_DECLARE_DYNAMIC(FWDialogBase)
	DECLARE_EVENT_MAP_FW()
	DECLARE_COMMAND_MAP_FW()
protected:

	// pointer to AUIDialog
	AUIDialog * m_pDlg;
	
	// used for generate WM_LCLICK event
	PAUIOBJECT m_pObjWhenLButtonDown;

	// ID indicate the clase method
	// see enum FW_xxx
	int m_nCloseID;

	// callback used by DoModal()
	FuncDoModalCallBack m_pDoModalCallback;
	// parameter of callback m_pDoModalCallback
	void * m_pDoModalCallbackData;

	// implement template that will call OnSelChange function if needed
	// you can call the function in your OnLButtonDown handler of a listbox
	void OnLButtonDownListboxTemplate(WPARAM wParam, LPARAM lParam, AUIListBox *pListbox, int *pnLastSelIndex, FuncOnSelChangeListbox pFuncOnSel);

	// this is the base/intermedia class that should not be instantiation
	FWDialogBase();
	virtual ~FWDialogBase() {};
public:

	enum {FW_IDOK, FW_IDCANCEL};
	int GetCloseID() { return m_nCloseID; }

	FWDialogBase * GetFWDialog(const char *szClassName);

	// show dialog
	virtual void Show (bool bShow, bool bModal = false);
	// whether the dialog is visible now
	bool IsShow();

	// init dialog, internal used by FWAUIManger
	virtual bool Init(AUIDialog * pDlg)
	{ 
		ASSERT(pDlg);
		m_pDlg = pDlg; 
		return true; 
	}
	virtual void Release() {}

	// get pointer to AUIDialog 
	AUIDialog * GetAUIDialog() { return m_pDlg; }

	// set capture for me
	void SetCapture();
	// release capture for me
	bool ReleaseCapture();
	// return the dialog who have capture
	FWDialogBase * GetCapture();

	FWView * GetView();
	FWDoc * GetDocument();
	
	// get pointer to manager
	FWAUIManager * GetFWAUIManager();

	// functin used as part of EVENT_MAP mechanism
	// find an entry in event map, and call the handler if any
	bool ProcessEventMap(const EVENT_MAP_ENTRY_FW *pMapEntry, UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	// functin used as part of EVENT_MAP mechanism
	// in general, derived class should not override this function
	// unless you want change the ENVET_MAP mechanism
	virtual bool OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	
	// functin used as part of COMMAND_MAP mechanism
	// find an entry in command map, and call the handler if any
	bool ProcessCommandMap(const COMMAND_MAP_ENTRY_FW *pMapEntry, const char * szCommand);
	// functin used as part of COMMAND_MAP mechanism
	// in general, derived class should not override this function
	// unless you want change the COMMAND_MAP mechanism
	virtual bool OnCommand(const char *szCommand);

	// send event to me
	void SendEvent(UINT uMsg, WPARAM wParam = NULL, LPARAM lParam = NULL, PAUIOBJECT pObj = NULL);

	// similar to MFC. 
	// different from MFC :: these base version will not close dialog
	// since in FW case, most dialog should not be closed by press Enter/Esc key
	// but these base version must be called in derived version
	virtual void OnOK();
	virtual void OnCancel();

	// show the dialog as a modal dialog
	// provide a callback that will be triggered when 
	// the dialog is closed
	// pData is the parameter passed to callback
	void DoModal(FuncDoModalCallBack pCallback, void *pData);

	// enable/disable dialog
	void SetEnable(bool bEnable);
	// to get the enable state
	bool IsEnabled();

	// default handler to BTN_OK and BTN_CANCEL
	// the default implement will call OnOK() or OnCancel()
	void OnClickBtnCancel(WPARAM wParam, LPARAM lParam);
	void OnClickBtnOK(WPARAM wParam, LPARAM lParam);

	// propxy method that appear in AUIDialog
	void EnableMenuItem(const char *pszCommand, bool bEnable = true);
	void SetPos(int x, int y, int alignModeX = AUIDialog::alignNone, int alignModeY = AUIDialog::alignNone, FWDialogBase *pParentDlg = NULL);
	void SetCanMove (bool bCanMove);
	POINT GetPos (void);
	PAUIOBJECT GetDlgItem (const char *pszItemName);
	void CheckMenuItem(const char *pszCommand, bool bCheck);
	SIZE GetSize(void);
	virtual bool Render(void);
	bool IsModal (void);
	A3DRECT GetRect (void);
};



#endif 
