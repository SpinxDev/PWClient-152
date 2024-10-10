// FWAUIManager.cpp: implementation of the FWAUIManager class.
//
//////////////////////////////////////////////////////////////////////

#include "FWAUIManager.h"
#include "FWDialogMain.h"
#include "CodeTemplate.h"
#include "FWDialogGraphMenu.h"
#include "FWDialogBoard.h"
#include "FWDialogEdit.h"
#include "FWView.h"
#include "FWDialogPlay.h"
#include "FWDialogScale.h"
#include "FWDialogSimple.h"
#include "FWDialogCustom.h"
#include "FWDialogGraph.h"
#include "FWDialogText.h"
#include "FWDialogParam.h"
#include "FWDialogLanch.h"
#include "FWDialogBezierMenu.h"
#include "FWDialogTool.h"
#include "FWDialogPolygonMenu.h"
#include "FWLog.h"
#include "A3DFTFontMan.h"
#include <A3DEngine.h>

#define new A_DEBUG_NEW









FWAUIManager::FWAUIManager()
{
	m_pCaptured = NULL;
	m_pView = NULL;
	m_dlgInfo = NULL;
}

FWAUIManager::~FWAUIManager()
{
	Release();
}

#define DLG_INSTANCE(className) \
((FWDialogBase *)FW_RUNTIME_CLASS(className)->CreateObject())

bool FWAUIManager::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszFilename)
{
	BEGIN_FAKE_WHILE;

	// init base manager
	CHECK_BREAK(AUIManager::Init(pA3DEngine, pA3DDevice, pszFilename));
	
	// init dialog info array

	// used to init m_dlgInfo, just for convenience
	const _DIALOG_INFO dlgInfo[] = 
	{
		{ "FWDialogMain",			"DLG_FW_MAIN",			DLG_INSTANCE(FWDialogMain) },
		{ "FWDialogGraphMenu",		"DLG_FW_GRAPH_MENU",	DLG_INSTANCE(FWDialogGraphMenu) },
		{ "FWDialogBoard",			"DLG_FW_BOARD",			DLG_INSTANCE(FWDialogBoard) },
		{ "FWDialogEdit",			"DLG_FW_EDIT",			DLG_INSTANCE(FWDialogEdit) },
		{ "FWDialogPlay",			"DLG_FW_PLAY",			DLG_INSTANCE(FWDialogPlay) },
		{ "FWDialogScale",			"DLG_FW_SCALE",			DLG_INSTANCE(FWDialogScale) },
		{ "FWDialogSimple",			"DLG_FW_SIMPLE",		DLG_INSTANCE(FWDialogSimple) },
		{ "FWDialogCustom",			"DLG_FW_CUSTOM",		DLG_INSTANCE(FWDialogCustom) },
		{ "FWDialogGraph",			"DLG_FW_GRAPH",			DLG_INSTANCE(FWDialogGraph) },
		{ "FWDialogText",			"DLG_FW_TEXT",			DLG_INSTANCE(FWDialogText) },
		{ "FWDialogParam",			"DLG_FW_PARAM",			DLG_INSTANCE(FWDialogParam) },
		{ "FWDialogLanch",			"DLG_FW_LANCH",			DLG_INSTANCE(FWDialogLanch) },
		{ "FWDialogBezierMenu",		"DLG_FW_BEZIER_MENU",	DLG_INSTANCE(FWDialogBezierMenu) },
		{ "FWDialogTool",			"DLG_FW_TOOL",			DLG_INSTANCE(FWDialogTool) },
		{ "FWDialogPolygonMenu",	"DLG_FW_POLYGON_MENU",	DLG_INSTANCE(FWDialogPolygonMenu) },
		{ NULL, NULL, NULL}
	};
	const int dlg_info_size = sizeof(dlgInfo) / sizeof(dlgInfo[0]);

	// copy data to m_dlgInfo
	m_dlgInfo = new _DIALOG_INFO[dlg_info_size];
	memcpy(const_cast<_DIALOG_INFO *>(m_dlgInfo), dlgInfo, sizeof(dlgInfo));

	// init fwdialogs
	const _DIALOG_INFO * pInfo = m_dlgInfo;
	while (pInfo->szClassName)
	{
		PAUIDIALOG pAUIDlg = GetDialog(pInfo->szAUIDlgName);
		CHECK_BREAK_LOG1(pAUIDlg,
			"FWAUIManager::Init(), can not load AUIDialog \"%s\"", 
			pInfo->szAUIDlgName);

		CHECK_BREAK_LOG1((pInfo++)->pDlg->Init(pAUIDlg),
			"FWAUIManager::Init(), error occur in %s::Init()", 
			pInfo->szClassName);
	}
	CHECK_BREAK(pInfo->szClassName == NULL);

	// drawing board can not be moved
	// note : this is used to disable the default moveing method.
	// user can only move board by drag its frame, which will be
	// implemented in FWDialogBoard
	GetFWDialog2(FWDialogBoard)->SetCanMove(false);

	END_FAKE_WHILE;


	RETURN_FAKE_WHILE_RESULT;
}

bool FWAUIManager::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj)
{
	// close popupmenu-like dialog if any
	if (WM_LBUTTONDOWN == uMsg)	ClosePopupMenu(pDlg);

	// let model dialog "eat" msg first, if any
	if (ProcessModelDialog(uMsg, wParam, lParam, pDlg, pObj))
		return true;

	// process hotkey if any
	// note : when any model dialog is open, the hotkey 
	// will not be valid
	if (ProcessHotkey(uMsg, wParam, lParam))
		return true;

	// dispath message to dialogs
	FWDialogBase *pFWDlg = AUIDialogToFWDialog(pDlg);
	if (!pFWDlg) return false;
	return pFWDlg->OnEvent(uMsg, wParam, lParam, pObj);
}

void FWAUIManager::ClosePopupMenu(PAUIDIALOG pDlg)
{
	// close graph menu
	FWDialogGraphMenu *pFWDlgGraphMenu = 
		static_cast<FWDialogGraphMenu *>(GetFWDialog2(FWDialogGraphMenu));
	if (pDlg != pFWDlgGraphMenu->GetAUIDialog())
		pFWDlgGraphMenu->Show(false);
}

void FWAUIManager::SetCapture(FWDialogBase *pFWD)
{
	// let the AUIDialog do actual work
	m_pCaptured = pFWD;
	pFWD->GetAUIDialog()->SetCapture(true);
}

bool FWAUIManager::ReleaseCapture()
{
	// let the AUIDialog do actual work
	if (m_pCaptured)
	{
		m_pCaptured->GetAUIDialog()->SetCapture(false);
		m_pCaptured = NULL;
	}
	return true;
}

FWDialogBase * FWAUIManager::GetCapture()
{
	return m_pCaptured;
}



bool FWAUIManager::DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// AUI will throw WM_TIMER away, so we process it here
	if (uMsg == WM_TIMER)
	{
		// note : by now, only the board use timer
		return OnEvent(uMsg, wParam, lParam, GetFWDialog2(FWDialogBoard)->GetAUIDialog(), NULL);
	}
	else
	{
		return AUIManager::DealWindowsMessage(uMsg, wParam, lParam);
	}
}

void FWAUIManager::SendEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj)
{
	OnEvent(uMsg, wParam, lParam, pDlg, pObj);
}

void FWAUIManager::UpdateAllToolbar()
{
	const _DIALOG_INFO *pInfo = m_dlgInfo;
	while (pInfo->szClassName)
		(pInfo++)->pDlg->SendEvent(WM_UPDATE_TOOLBAR);
}

bool FWAUIManager::Tick(DWORD dwDeltaTime)
{
	UpdateAllToolbar();
	return AUIManager::Tick(dwDeltaTime);
}

bool FWAUIManager::Render(int nRenderPart)
{
	// quite the same as base version Render
	// but can specify the part to render

	bool bval;
	PAUIDIALOG pDlg;
	ALISTPOSITION pos;
	
	pos = m_DlgZOrderBack.GetTailPosition();
	while( pos )
	{
		pDlg = m_DlgZOrderBack.GetPrev(pos);
		
		if (nRenderPart & RENDER_PART_BOARD)
		{
			if (!MatchDialog(pDlg, TO_STRING(FWDialogBoard)))
				continue;
		}
		else if (nRenderPart & RENDER_PART_OTHER_THAN_BOARD)
		{
			if (MatchDialog(pDlg, TO_STRING(FWDialogBoard)))
				continue;
		}

		if( pDlg && pDlg->IsShow() )
		{
			FWDialogBase *pFWDlg = AUIDialogToFWDialog(pDlg);
			if (pFWDlg) 
				bval = pFWDlg->Render();
			else 
				bval = pDlg->Render();
			if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		}
	}
	
	pos = m_DlgZOrder.GetTailPosition();
	while( pos )
	{
		pDlg = m_DlgZOrder.GetPrev(pos);
		
		if (nRenderPart & RENDER_PART_BOARD)
		{
			if (!MatchDialog(pDlg, TO_STRING(FWDialogBoard)))
				continue;
		}
		else if (nRenderPart & RENDER_PART_OTHER_THAN_BOARD)
		{
			if (MatchDialog(pDlg, TO_STRING(FWDialogBoard)))
				continue;
		}

		if( pDlg && pDlg->IsShow() )
		{
			FWDialogBase *pFWDlg = AUIDialogToFWDialog(pDlg);
			if (pFWDlg) 
				bval = pFWDlg->Render();
			else 
				bval = pDlg->Render();
			if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
		}
	}
	
	bval = RenderHint();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	
	bval = m_pA3DEngine->FlushInternalSpriteBuffer();
	if( !bval ) return AUI_ReportError(__LINE__, __FILE__);
	
	m_pFontMan->Flush();
	
	return true;
}

bool FWAUIManager::OnCommand(const char *pszCommand, PAUIDIALOG pDlg)
{
	// note : the base version processed IDOK, IDCANCEL to close dialog
	// we do not call the base to disable this mechanism since most dialog
	// in fweditor should not closed by such commands

	FWDialogBase *pFWDlg = AUIDialogToFWDialog(pDlg);
	if (!pFWDlg) return false;

	return pFWDlg->OnCommand(pszCommand);
}


#define PROCESS_EDIT_HOTKEY(func) \
{\
	FWDialogEdit *pDlg = static_cast<FWDialogEdit *>(GetFWDialog2(FWDialogEdit)); \
	if (pDlg->IsEnabled() && pDlg->IsShow()) \
	{ \
		pDlg->func(NULL, NULL); \
		return true; \
	}\
}

#define PROCESS_EDIT_HOTKEY_WITH_CTRL(func) \
{\
	SHORT state = GetKeyState(VK_CONTROL); \
	if (!HIWORD(state)) break; \
	FWDialogEdit *pDlg = static_cast<FWDialogEdit *>(GetFWDialog2(FWDialogEdit)); \
	if (pDlg->IsEnabled() && pDlg->IsShow()) \
	{ \
		pDlg->func(NULL, NULL); \
		return true; \
	}\
}
bool FWAUIManager::ProcessHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg != WM_KEYDOWN)
		return false;

	switch(wParam) 
	{
	case VK_DELETE:
		PROCESS_EDIT_HOTKEY(OnClickBtnDelete);
		break;
	case 'Z':
		PROCESS_EDIT_HOTKEY_WITH_CTRL(OnClickBtnUndo);
		break;
	case 'Y':
		PROCESS_EDIT_HOTKEY_WITH_CTRL(OnClickBtnRedo);
		break;	
	case 'C':
		PROCESS_EDIT_HOTKEY_WITH_CTRL(OnClickBtnCopy);
		break;
	case 'X':
		PROCESS_EDIT_HOTKEY_WITH_CTRL(OnClickBtnCut);
		break;
	case 'V':
		PROCESS_EDIT_HOTKEY_WITH_CTRL(OnClickBtnPaste);
		break;
	case 'A':
		PROCESS_EDIT_HOTKEY_WITH_CTRL(OnClickBtnSelectAll);
		break;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_HOME:
	case VK_END:
		{
			// move glyph in board
			FWDialogEdit *pDlg = static_cast<FWDialogEdit *>(GetFWDialog2(FWDialogEdit));
			if (pDlg->IsEnabled() && pDlg->IsShow())
			{
				GetView()->OnKeyDown(wParam, LOWORD(lParam), 0);
				return true;
			}
			// control camera
			else
			{
				GetView()->MoveCamera(wParam, LOWORD(lParam), 0);
				return true;
			}
		}
		break;

	}

	return false;
}

FWView * FWAUIManager::GetView()
{
	ASSERT(m_pView);

	return m_pView;
}

bool FWAUIManager::ProcessModelDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj)
{
	bool bFound = false;
	const _DIALOG_INFO *pInfo = m_dlgInfo;
	while (pInfo->szClassName)
	{
		if (pInfo->pDlg->IsModal())
		{
			bFound = true;
			if (pDlg == pInfo->pDlg->GetAUIDialog())
				pInfo->pDlg->OnEvent(uMsg, wParam,lParam, pObj);
		}
		pInfo++;
	}

	return bFound;
}

FWDialogBase * FWAUIManager::AUIDialogToFWDialog(AUIDialog *pAUIDlg)
{
	const _DIALOG_INFO *pInfo = m_dlgInfo;
	while (pInfo->szClassName)
	{
		if (pInfo->pDlg->GetAUIDialog() == pAUIDlg)
			return pInfo->pDlg;
		pInfo++;
	}

	return NULL;
}

FWDialogBase * FWAUIManager::GetFWDialog(const char *szName)
{
	AString strName = szName;

	const _DIALOG_INFO * pInfo = m_dlgInfo;
	while (pInfo->szClassName)
	{
		if (strName == pInfo->szClassName)
			return pInfo->pDlg;
		pInfo++;
	}

	// illege name
	ASSERT(false);
	return NULL;
}

bool FWAUIManager::MatchDialog(const AUIDialog *pDlg, const char *szClassName)
{
	FWDialogBase *pFWDlg = GetFWDialog(szClassName);
	if (!pFWDlg) return false;
	
	return (pFWDlg->GetAUIDialog() == pDlg);
}

bool FWAUIManager::Release(void)
{
	// delete dialog
	_DIALOG_INFO *pInfo = const_cast<_DIALOG_INFO *>(m_dlgInfo);
	if (pInfo)
	{
		while (pInfo->szClassName)
		{
			SAFE_DELETE(pInfo->pDlg);
			pInfo++;
		}
		
		// delete info array
		pInfo = const_cast<_DIALOG_INFO *>(m_dlgInfo);
		SAFE_DELETE_ARRAY(pInfo);

		m_dlgInfo = NULL;
	}

	// call base version
	return AUIManager::Release();
}
