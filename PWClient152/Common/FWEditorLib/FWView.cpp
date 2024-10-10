// FWView.cpp: implementation of the FWView class.
//
//////////////////////////////////////////////////////////////////////

#include "FWView.h"
#include "FWStateSelect.h"
#include "FWDoc.h"
#include "FWDialogBoard.h"
#include "FWFlatCollector.h"
#include "FWAUIManager.h"
#include "FWDialogEdit.h"
#include "FWPlayer.h"
#include "FWDialogBezierMenu.h"
#include "FWGlyphList.h"
#include "FWGlyphBezier.h"
#include "FWDialogPropBase.h"
#include "FWCommandList.h"
#include "FWCommandSetGlyphProp.h"
#include "FWStream.h"
#include "AF.h"
#include "Global.h"
#include <A3DViewport.h>
#include <A3DCamera.h>
#include <A3DDevice.h>
#include <A3DMacros.h>
#include <A3DEngine.h>

#include "FWGlyphFirework.h"
#include "FWFireworkLanch.h"
#include "FWOutterProperty.h"

#define new A_DEBUG_NEW





FWView::FWView() :
	m_pCurrentState(NULL),
	m_pAUIManager(NULL),
	m_uClipboardFormat(0),
	m_bShowGrid(true),
	m_pDoc(NULL),
	m_pA3DDevice(NULL),
	m_pA3DEngine(NULL),
	m_pA3DCameraPreview(NULL),
	m_pA3DViewportPreview(NULL),
	m_pFlatCollecter(NULL),
	m_pOldViewport(NULL),
	m_pStream(NULL),
	m_pPlayer(NULL),
	m_bVisible(false),
	m_bInited(false)
{
}

FWView::~FWView()
{
	Release();
}

FWDoc * FWView::GetDocument()
{
	ASSERT(m_pDoc);
	
	return m_pDoc;
}


void FWView::ChangeState(FWState* pNewState)
{
	delete m_pCurrentState;
	m_pCurrentState = pNewState;
}

UINT FWView::SetTimer(UINT nIDEvent, UINT nElapse)
{
	return ::SetTimer(
		GetHwnd(),
		nIDEvent,
		nElapse, 
		NULL);
}

bool FWView::KillTimer(int nIDEvent)
{
	return ::KillTimer(
		GetHwnd(),
		nIDEvent) != 0;
}

ARectI FWView::GetClientRect(void)
{
	return GetBoard()->GetClientRect();
}

void FWView::OnDraw(A3DGFXExMan * pGfxExMan)
{
	if (!IsVisible() || !m_bInited) return;

	GetFWAUIManager()->Render(FWAUIManager::RENDER_PART_BOARD);

	if (m_pPlayer->IsActive())
	{
		GetFWAUIManager()->Render(FWAUIManager::RENDER_PART_OTHER_THAN_BOARD);
	
		SwitchToPreviewViewport();

		A3DViewport *pViewport = GetActiveViewport();
		pViewport->Active();
		pViewport->ClearDevice();

		m_pPlayer->Render(pGfxExMan);

		SwitchToMainViewport();
	}
	else
	{
		m_pCurrentState->OnDraw();
	}

	GetFWAUIManager()->Render(FWAUIManager::RENDER_PART_OTHER_THAN_BOARD);
}

void FWView::OnRButtonUp(UINT nFlags, APointI point)
{
	DPtoLP(&point);
	m_pCurrentState->OnRButtonUpImpl(nFlags, point);
}

void FWView::OnRButtonDown(UINT nFlags, APointI point)
{
	DPtoLP(&point);
	m_pCurrentState->OnRButtonDownImpl(nFlags, point);
}

void FWView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_pCurrentState->OnKeyUp(nChar, nRepCnt, nFlags);
}

void FWView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_pCurrentState->OnKeyDown(nChar, nRepCnt, nFlags);
}

void FWView::OnLButtonDown(UINT nFlags, APointI point)
{
	DPtoLP(&point);
	m_pCurrentState->OnLButtonDownImpl(nFlags, point);
}

void FWView::OnLButtonUp(UINT nFlags, APointI point)
{
	DPtoLP(&point);
	m_pCurrentState->OnLButtonUpImpl(nFlags, point);
}

void FWView::OnMouseMove(UINT nFlags, APointI point)
{
	DPtoLP(&point);
	m_pCurrentState->OnMouseMove(nFlags, point);
}

void FWView::OnLButtonDblClk(UINT nFlags, APointI point)
{
	DPtoLP(&point);
	m_pCurrentState->OnLButtonDblClk(nFlags, point);
}

void FWView::OnTimer(UINT nIDEvent)
{
	m_pCurrentState->OnTimerImpl(nIDEvent);
}

void FWView::SetCapture()
{
	GetBoard()->SetCapture();	
}

bool FWView::ReleaseCapture()
{
	return GetBoard()->ReleaseCapture();
}

FWView * FWView::GetCapture()
{
	if (GetBoard()->GetCapture())
		return this;
	else
		return NULL;
}

FWDialogBoard * FWView::GetBoard()
{
	return GetFWDialog2(FWDialogBoard);
}

FWAUIManager * FWView::GetFWAUIManager()
{
	ASSERT(m_pAUIManager);

	return m_pAUIManager;
}

HDC FWView::GetDC()
{
	return ::GetDC(GetHwnd());
}


void FWView::Tick(DWORD dwTickTime)
{
	if (!IsVisible() || !m_bInited) return;

	GetFWAUIManager()->Tick(dwTickTime);

	m_pPlayer->Tick(dwTickTime);
}


HWND FWView::GetHwnd()
{
	return m_pA3DDevice->GetDeviceWnd();
}

int FWView::GetClipboardFormat()
{
	if (m_uClipboardFormat == 0)
	{
		m_uClipboardFormat = ::RegisterClipboardFormat(_AL("FW_EDITOR_GLYPH"));
	}
	return m_uClipboardFormat;
}

void FWView::DPtoLP(APointI * lpPoint, int nCount)
{
	APointI center = GetClientRect().CenterPoint();
	while (nCount--)
	{
		lpPoint->x -= center.x;
		lpPoint->y -= center.y;
		lpPoint++;
	}
}

void FWView::DPtoLP(ARectI * lpRect)
{
	APointI center = GetClientRect().CenterPoint();
	lpRect->left -= center.x;
	lpRect->right -= center.x;
	lpRect->top -= center.y;
	lpRect->bottom -= center.y;
}


void FWView::LPtoDP(APointI * lpPoint, int nCount)
{
	APointI center = GetClientRect().CenterPoint();
	while (nCount--)
	{
		lpPoint->x += center.x;
		lpPoint->y += center.y;
		lpPoint++;
	}
}

void FWView::LPtoDP(ARectI * lpRect)
{
	APointI center = GetClientRect().CenterPoint();
	lpRect->left += center.x;
	lpRect->right += center.x;
	lpRect->top += center.y;
	lpRect->bottom += center.y;
}

void FWView::DPtoLP(float &x, float &y)
{
	APointI center = GetClientRect().CenterPoint();
	x -= center.x;
	y -= center.y;
}

void FWView::LPtoDP(float &x, float &y)
{
	APointI center = GetClientRect().CenterPoint();
	x += center.x;
	y += center.y;
}

ARectI FWView::GetClientRectLP(void)
{
	ARectI rect = GetClientRect();
	DPtoLP(&rect);
	return rect;
}

bool FWView::IsShowGrid()
{
	return m_bShowGrid;
}

void FWView::SetShowGrid(bool bShow)
{
	m_bShowGrid = bShow;
}

void FWView::ShowPropDlg(FWGlyph *pGlyph)
{
	ASSERT(pGlyph);


	FWDialogPropBase *pDlg = static_cast<FWDialogPropBase *>(GetFWDialog(pGlyph->GetDlgToShow()));
	if (!pDlg) return;
	
	pGlyph->PrepareDlg(pDlg);

	InnerShowPropDlg(pDlg, pGlyph);
}

struct PropDlgDoModelParam
{
	FWGlyph *pGlyph;
	FWDialogPropBase *pDlg;
	FWView *pThis;
};


void FWView::InnerShowPropDlg(FWDialogPropBase *pDlg, FWGlyph *pGlyph)
{
	PropDlgDoModelParam *pParam = new PropDlgDoModelParam;
	pParam->pDlg = pDlg;
	pParam->pGlyph = pGlyph;
	pParam->pThis = this;
	
	pDlg->DoModal(OnClosePropDlg, pParam);
}

void FWView::OnClosePropDlg(void *pData)
{
	PropDlgDoModelParam *pParam = (PropDlgDoModelParam *)pData;
	
	if (pParam->pDlg->GetCloseID() == FWDialogBase::FW_IDOK)
	{
#define BUFFER_SIZE 1024
		BYTE *pDataOld = new BYTE[BUFFER_SIZE];
		BYTE *pDataNew = new BYTE[BUFFER_SIZE];

		AMemFile fileOld;
		fileOld.Attach(pDataOld, BUFFER_SIZE, 512);
		FWArchive arOld(&fileOld, FWArchive::MODE_STORE);
		pParam->pGlyph->Serialize(arOld);
		arOld.Flush();
		int nSizeOld = fileOld.GetPos();
		fileOld.Detach();

		pParam->pGlyph->UpdateFromDlg(pParam->pDlg);

		AMemFile fileNew;
		fileNew.Attach(pDataNew, BUFFER_SIZE, 512);
		FWArchive arNew(&fileNew, FWArchive::MODE_STORE);
		pParam->pGlyph->Serialize(arNew);
		arNew.Flush();
		int nSizeNew = fileNew.GetPos();
		fileNew.Detach();

		pParam->pThis->GetDocument()->GetCommandList()->ExecuteCommand(
			new FWCommandSetGlyphProp(pParam->pThis->GetDocument(), pParam->pGlyph, pDataOld, nSizeOld, pDataNew, nSizeNew));
	}
	
	SAFE_DELETE(pParam);
}

FWDialogBase * FWView::GetFWDialog(const char *szDlgName)
{
	return GetFWAUIManager()->GetFWDialog(szDlgName);
}

bool FWView::Init(A3DEngine *pEngine, A3DDevice *pDevice)
{
	BEGIN_FAKE_WHILE;

	// record device
	m_pA3DDevice = pDevice;
	m_pA3DEngine = pEngine;

	// init auimanager
	m_pAUIManager = new FWAUIManager;
	
	CHECK_BREAK(m_pAUIManager->Init(
		m_pA3DEngine, 
		m_pA3DDevice,
		"FWDialogs.dcf"));

	// create stream
	m_pStream = new FWStream;
	CHECK_BREAK(m_pStream->Init(GetA3DDevice()));

	// create flat collector
	m_pFlatCollecter = new FWFlatCollector;
	CHECK_BREAK(m_pFlatCollecter->Init(GetA3DEngine()));

	//	Create preview camera
	CHECK_BREAK(m_pA3DCameraPreview = new A3DCamera());
	CHECK_BREAK(m_pA3DCameraPreview->Init(GetA3DDevice(), DEG2RAD(65.0f), 0.1f, 2000.0f));
	m_pA3DCameraPreview->SetPos(g_vOrigin + g_vAxisY * 0.f - g_vAxisZ * 60.0f);
	m_pA3DCameraPreview->SetDirAndUp(g_vAxisY * .5f + g_vAxisZ, g_vAxisY + -g_vAxisZ * .5f);
	
	//	Create preview Viewport
	CHECK_BREAK(GetA3DDevice()->CreateViewport(
		&m_pA3DViewportPreview, 0, 0, 1, 1,	0.0f, 1.0f, true, true, 0xff404040));
	CHECK_BREAK(m_pA3DViewportPreview->SetCamera(m_pA3DCameraPreview));

	// prepare viewport
	ARectI rect = GetClientRect();
	PreparePreviewViewport(rect);

	// create player
	m_pPlayer = new FWPlayer;
   	CHECK_BREAK(m_pPlayer->Init(m_pA3DViewportPreview));

	m_pAUIManager->m_pView = this;


	ChangeState(new FWStateSelect(this));

	m_bInited = true;

	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;

	Release();

	END_ON_FAIL_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;
}

void FWView::Release()
{
	SAFE_DELETE(m_pAUIManager);
	SAFE_DELETE(m_pCurrentState);
	A3DRELEASE(m_pA3DCameraPreview);
	A3DRELEASE(m_pA3DViewportPreview);
	A3DRELEASE(m_pFlatCollecter);
	A3DRELEASE(m_pStream);
	SAFE_DELETE(m_pPlayer);

	m_uClipboardFormat = 0;
	m_bShowGrid = true;
	m_pDoc = NULL;
	m_pA3DDevice = NULL;
	m_pA3DEngine = NULL;
	m_pOldViewport = NULL;

	m_bInited = false;
}


void FWView::SwitchToPreviewViewport()
{
	ASSERT(m_pOldViewport == NULL);

	m_pOldViewport = GetActiveViewport();
	GetA3DEngine()->SetActiveViewport(m_pA3DViewportPreview);
}

void FWView::SwitchToMainViewport()
{
	ASSERT(m_pOldViewport);

	GetA3DEngine()->SetActiveViewport(m_pOldViewport);
	m_pOldViewport = NULL;
}

A3DViewport * FWView::GetActiveViewport()
{
	return GetA3DEngine()->GetActiveViewport();
}

void FWView::PreparePreviewViewport(ARectI rect)
{
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	
	A3DVIEWPORTPARAM param;
	param.X = rect.left;
	param.Y = rect.top;
	param.Width = nWidth;
	param.Height = nHeight;
	param.MinZ = 0.f;
	param.MaxZ = 1.f;
	m_pA3DViewportPreview->SetParam(&param);
	
	// place camera
	float fRatio = (float)nWidth / nHeight;
	m_pA3DCameraPreview->SetProjectionParam(DEG2RAD(65.0f), 0.1f, 2000.0f, fRatio);
	
	if (m_pDoc)
	{
		m_pA3DCameraPreview->SetPos(g_vOrigin + g_vAxisY * 0.f - g_vAxisZ * 120.0f);
		m_pA3DCameraPreview->SetDirAndUp(g_vAxisY * .6f + g_vAxisZ, g_vAxisY + -g_vAxisZ * .6f);

		// get the blast pos 
// 		const FWFireworkLanch *pFirework = static_cast<const FWFireworkLanch *>(GetDocument()->GetGlyphLanch()->GetFirework());
// 		const FWAssemblyProfile & profile = pFirework->GetAssemblyProfile();
// 		const FWOutterPropertyList *pPropList = profile.GetOutterPropertyList();
// 		const FWOutterProperty *pProp = pPropList->FindByName("ÉÏÉý¾àÀë");
// 		A3DVECTOR3 blastPos(0, pProp->Data, 0);
// 		m_pA3DCameraPreview->SetPos(g_vOrigin + g_vAxisY * blastPos.y + g_vAxisZ * 60.0f);
// 		m_pA3DCameraPreview->SetDirAndUp(g_vAxisZ, g_vAxisY);
	}
}



bool FWView::DealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return GetFWAUIManager()->DealWindowsMessage(uMsg, wParam, lParam);
}

void FWView::ShowContextMenu(FWGlyph *pGlyph, APointI point)
{
	LPCSTR szMenu = pGlyph->GetContextMenu();
	if (!szMenu) return;

	FWDialogBase *pMenu = GetFWAUIManager()->GetFWDialog(szMenu);
	
	LPtoDP(&point);
	pMenu->SetPos(point.x, point.y);

	pMenu->Show(true);
}

void FWView::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

bool FWView::IsVisible()
{
	return m_bVisible;
}

void FWView::MoveCamera(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	A3DVECTOR3 pos = m_pA3DCameraPreview->GetPos();
	static const float STEP = 2.f;

	switch (nChar)
	{
	case VK_UP:
		pos.z += STEP;
		break;
	case VK_DOWN:
		pos.z -= STEP;
		break;
	case VK_HOME:
		pos = g_vOrigin + g_vAxisY * 0.f - g_vAxisZ * 10.0f;
		break;
	case VK_END:
		pos = g_vOrigin + g_vAxisY * 0.f - g_vAxisZ * 120.0f;
		break;
	}

	m_pA3DCameraPreview->SetPos(pos);
}
