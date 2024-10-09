#ifndef _FWAUIMANAGER_H_
#define _FWAUIMANAGER_H_

#include "CodeTemplate.h"
#include <AUIManager.h>
#include "FWConfig.h"

class FWDialogBase;
typedef FWDialogBase * PFWDBASE;
class FWView;


// FWAUIManager is used to manage all dialogs
// used in fweditor
class FWAUIManager : public AUIManager  
{
	friend class FWView;
protected:
	// only FWView can create the manager
	FWAUIManager();
	virtual ~FWAUIManager();


	FWDialogBase * AUIDialogToFWDialog(AUIDialog *pAUIDlg);

	// let modal dialogs to "eat" messages first, if any
	// return true if there is at least one model dialog opend
	// or return false
	bool ProcessModelDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj);

	// process hotkey
	// return ture if the msg is recognized as a hotkey
	// or return false
	bool ProcessHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// this function will find any popup menu, and check it is pDlg
	// if not, close it
	// pDlg is the dialog who get the focus
	void ClosePopupMenu(PAUIDIALOG pDlg);

	// called per tick to update toolbar
	void UpdateAllToolbar();
	
	// dialog that has the capture
	FWDialogBase * m_pCaptured;

	FWView * m_pView;

	// all dialogs that managed by this class
	struct _DIALOG_INFO
	{
		const char * szClassName;
		const char * szAUIDlgName;
		FWDialogBase * pDlg;
	};
	const _DIALOG_INFO * m_dlgInfo;
public:
	// since AUIManager is owned by FWView, it could
	// access its master
	FWView * GetView();

	// match between AUIDialog and FWDialog's name
	bool MatchDialog(const AUIDialog *pDlg, const char *szClassName);

	// the szName is just the className of class FWDialogxxx
	FWDialogBase * GetFWDialog(const char *szName);
	
	// send event to a dialog
	void SendEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj);
	
	// set capture for specified dialog
	void SetCapture(FWDialogBase *pFWD);

	// relase any capture
	// we do not implement it as static, for that needs
	// global variable to get it work
	bool ReleaseCapture();

	// return the dialog which have the capture
	// can be NULL if not dialog have the capture
	FWDialogBase * GetCapture();

	// interfaces of AUIManager
	virtual bool Release (void);
	virtual bool DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual bool Tick (DWORD dwDeltaTime);
	enum { RENDER_PART_BOARD = 1, RENDER_PART_OTHER_THAN_BOARD = 2, RENDER_PART_ALL = 3};
	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszFilename = NULL);
	virtual bool OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj);
	virtual bool OnCommand(const char *pszCommand, PAUIDIALOG pDlg);
	
	// modified render method
	// client should never call base vesion of render
	// but call this one instead
	bool Render(int nRenderPart);
};

#endif
