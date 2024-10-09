#define _WIN32_WINDOWS	0x0500

#include "EC_Global.h"
#include "EC_AutoHomeInputFilter.h"
#include "EC_InputCtrl.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "A3DCamera.h"
#include "EC_Viewport.h"
#include "A3DFuncs.h"
#include "EC_Utility.h"
#include "A3DDevice.h"
#include "AC.h"
#include <windowsx.h>
#include <winuser.h>
#include "AutoScene.h"
#include "AutoHomeMan.h"

inline void _set_cam_pos(A3DCamera* pCam, const A3DVECTOR3& pos)
{
	CAutoHomeMan* pAutoHomeMan = g_pGame->GetGameRun()->GetWorld()->GetAutoScene()->GetAutoHomeMan();
	if (pAutoHomeMan)
	{
		if (!pAutoHomeMan->IsInCameraRect(pos))
			return;
	}
	
	A3DVECTOR3 vecCamPos = pos;
	float fHeight = g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(pos);
	if (vecCamPos.y < fHeight + 0.3f) 
	{
		vecCamPos.y = fHeight + 0.3f;
	}
	a_Clamp(vecCamPos.x, -800.0f, 800.0f);
	a_Clamp(vecCamPos.z, -800.0f, 800.0f);
	a_Clamp(vecCamPos.y, 0.0f, 1000.0f);
	pCam->SetPos(vecCamPos);
}

CECAutoHomeInputFilter::CECAutoHomeInputFilter(CECInputCtrl* pInputCtrl)
: CECInputFilter(pInputCtrl)
{
	m_LBPress.bPressed		= false;
	m_LBPress.bTurnCamera	= false;
	m_RBPress.bPressed		= false;
	m_RBPress.bTurnCamera	= false;	
}

CECAutoHomeInputFilter::~CECAutoHomeInputFilter()
{
}

bool CECAutoHomeInputFilter::TranslateInput(int iInput)
{
	if (iInput == IT_KEYBOARD)
		return TranslateKeyboard();
	else if (iInput == IT_MOUSE)
		return TranslateMouse();

	return false;
}

//	Translate keyboard input
bool CECAutoHomeInputFilter::TranslateKeyboard()
{
	bool bRet = false;
	int i, iNumMsg = m_pInputCtrl->GetWinKeyMsgNum();

	for (i = 0; i < iNumMsg; i++)
	{
		const CECInputCtrl::WINMSG& Msg = m_pInputCtrl->GetWinKeyMsg(i);

		if (Msg.uMsg == WM_KEYDOWN)
		{
			if (Msg.wParam == 'U' && m_pInputCtrl->IsCtrlPressed(Msg.dwCtrlkeys))
			{
				g_pGame->GetGameRun()->PostMessage(111, -1, 0);
				return true;
			}
		}

	/*	//	test code added by dyx
	 	if (Msg.uMsg == WM_KEYDOWN)
		{
			if (Msg.wParam == 'C' && m_pInputCtrl->IsCtrlPressed(Msg.dwCtrlkeys))
			{
				g_pGame->GetGameRun()->PostMessage(112, -1, 0);
				return true;
			}
		}	*/
	}

	A3DCamera* pCam = g_pGame->GetViewport()->GetA3DCamera();
	A3DVECTOR3 pos = pCam->GetPos();
	A3DVECTOR3 dir = pCam->GetDir();
	A3DVECTOR3 up = pCam->GetUp();
	A3DVECTOR3 r = pCam->GetRight();

	static const float _pos_delta = 1.f;
	A3DVECTOR3 vPosDelta;
	CAutoScene* pAutoScene = g_pGame->GetGameRun()->GetWorld()->GetAutoScene();
	ASSERT(pAutoScene);
	const float POS_DELTA = 0.5f;
	float fCameraSpeed = pAutoScene->GetCameraSpeed() / 100.0f;

	if (m_pInputCtrl->KeyIsBeingPressed('A'))
	{
		vPosDelta = -r * _pos_delta * fCameraSpeed;
		vPosDelta -= r * POS_DELTA;
//		if (!pAutoScene->CheckCameraCollision(pos, vPosDelta))
		{
			pos += vPosDelta;
			_set_cam_pos(pCam, pos);
		}
		bRet = true;
	}
	else if (m_pInputCtrl->KeyIsBeingPressed('D'))
	{
		vPosDelta = r * _pos_delta * fCameraSpeed;
		vPosDelta += r * POS_DELTA;
//		if (!pAutoScene->CheckCameraCollision(pos, vPosDelta))
		{
			pos += vPosDelta;
			_set_cam_pos(pCam, pos);
		}
		bRet = true;
	}
	else if (m_pInputCtrl->KeyIsBeingPressed('W'))
	{
		vPosDelta = dir * _pos_delta * fCameraSpeed;
		vPosDelta += dir * POS_DELTA;
//		if (!pAutoScene->CheckCameraCollision(pos, vPosDelta))
		{
			pos += vPosDelta;
			_set_cam_pos(pCam, pos);
		}
		bRet = true;
	}
	else if (m_pInputCtrl->KeyIsBeingPressed('S'))
	{
		vPosDelta = -dir * _pos_delta * fCameraSpeed;
		vPosDelta -= dir * POS_DELTA;
//		if (!pAutoScene->CheckCameraCollision(pos, vPosDelta))
		{
			pos += vPosDelta;
			_set_cam_pos(pCam, pos);
		}
		bRet = true;
	}
	// Added by jdl for easy control
	else if (m_pInputCtrl->KeyIsBeingPressed(VK_SPACE))
	{
		vPosDelta = A3DVECTOR3(0.0f, 1.0f, 0.0f) * _pos_delta * fCameraSpeed;
		vPosDelta += A3DVECTOR3(0.0f, 1.0f, 0.0f) * POS_DELTA;
//		if (!pAutoScene->CheckCameraCollision(pos, vPosDelta))
		{
			pos += vPosDelta;
			_set_cam_pos(pCam, pos);
		}
		bRet = true;
	}
	else if (m_pInputCtrl->KeyIsBeingPressed('Z'))
	{
		vPosDelta = A3DVECTOR3(0.0f, -1.0f, 0.0f) * _pos_delta * fCameraSpeed;
		vPosDelta += A3DVECTOR3(0.0f, -1.0f, 0.0f) * POS_DELTA;
//		if (!pAutoScene->CheckCameraCollision(pos, vPosDelta))
		{
			pos += vPosDelta;
			_set_cam_pos(pCam, pos);
		}
		bRet = true;
	}

	return bRet;
}

//	Translate mouse input
bool CECAutoHomeInputFilter::TranslateMouse()
{
	CECGameRun* pGameRun = g_pGame->GetGameRun();
	int i, iNumMsg = m_pInputCtrl->GetWinMouseMsgNum();

	for (i=0; i < iNumMsg; i++)
	{
		const CECInputCtrl::WINMSG& Msg = m_pInputCtrl->GetWinMouseMsg(i);
		int x = GET_X_LPARAM(Msg.lParam);
		int y = GET_Y_LPARAM(Msg.lParam);
		DWORD dwFlags = Msg.dwCtrlkeys;

		switch (Msg.uMsg)
		{
		case WM_LBUTTONDOWN:	OnLButtonDown(x, y, dwFlags);	break;
		case WM_LBUTTONDBLCLK:	OnLButtonDblClk(x, y, dwFlags);	break;
		case WM_LBUTTONUP:		OnLButtonUp(x, y, dwFlags);		break;
		case WM_RBUTTONDOWN:	OnRButtonDown(x, y, dwFlags);	break;
		case WM_RBUTTONDBLCLK:	OnRButtonDblClk(x, y, dwFlags);	break;
		case WM_RBUTTONUP:		OnRButtonUp(x, y, dwFlags);		break;
		case WM_MOUSEMOVE:		OnMouseMove(x, y, dwFlags);		break;

			/*
		case WM_MOUSEWHEEL:
		{
			int iDelta = ((short)HIWORD(Msg.wParam));
			dwFlags = LOWORD(Msg.wParam);
			OnMouseWheel(x, y, iDelta, dwFlags);
			break;
		}
		*/
		}
	}

	if (m_LBPress.bTurnCamera || m_RBPress.bTurnCamera)
	{
		if( g_pGame->GetA3DDevice()->GetShowCursor() )
			g_pGame->ShowCursor(false);
	}
	else
	{
		if( !g_pGame->GetA3DDevice()->GetShowCursor() )
			g_pGame->ShowCursor(true);
	}

	return true;
}

void CECAutoHomeInputFilter::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_LBPress.bPressed		= true;
	m_LBPress.dwPressTime	= a_GetTime();
	m_LBPress.bTurnCamera	= false;
	m_LBPress.ptPress.Set(x, y);
}

void CECAutoHomeInputFilter::OnLButtonDblClk( int x, int y, DWORD dwFlags)
{
	m_LBPress.bPressed		= true;
	m_LBPress.dwPressTime	= a_GetTime();
	m_LBPress.bTurnCamera	= false;
	m_LBPress.ptPress.Set(x, y);
}

void CECAutoHomeInputFilter::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	m_LBPress.bPressed = false;

	if (m_LBPress.bTurnCamera)
	{
		m_LBPress.bTurnCamera = false;
		m_pInputCtrl->CaptureMouse(false);
	}
}

void CECAutoHomeInputFilter::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	m_pInputCtrl->CaptureMouse(true);
	m_RBPress.bPressed = true;
	m_RBPress.bTurnCamera = true;
	m_RBPress.ptPress.Set(x, y);
}

void CECAutoHomeInputFilter::OnRButtonDblClk(int x, int y, DWORD dwFlags)
{
	OnRButtonDown(x, y, dwFlags);
}

void CECAutoHomeInputFilter::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_RBPress.bTurnCamera)
	{
		m_pInputCtrl->CaptureMouse(false);
		m_RBPress.bTurnCamera = false;
	}
}

void CECAutoHomeInputFilter::OnMouseMove(int x, int y, DWORD dwFlags)
{
	static const float _rot_delta = 0.1f;

	if (m_RBPress.bTurnCamera)
	{
		POINT ptOff;
		ptOff.x = x - m_RBPress.ptPress.x;
		ptOff.y = y - m_RBPress.ptPress.y;

		A3DCamera* pCam = g_pGame->GetViewport()->GetA3DCamera();
		if (ptOff.x) pCam->DegDelta(ptOff.x * _rot_delta);
		if (ptOff.y) pCam->PitchDelta(-ptOff.y * _rot_delta);

		POINT pt = { m_RBPress.ptPress.x, m_RBPress.ptPress.y };
		::ClientToScreen(g_pGame->GetGameInit().hWnd, &pt);
		::SetCursorPos(pt.x, pt.y);
	}
}

void CECAutoHomeInputFilter::OnMouseWheel(int x, int y, int iDelta, DWORD dwFlags)
{
	static float fSpeed = 1.0f;
	A3DCamera* pCam = g_pGame->GetViewport()->GetA3DCamera();
	_set_cam_pos(pCam, pCam->GetPos() + pCam->GetDir() * (iDelta > 0 ? fSpeed : -fSpeed));
}
