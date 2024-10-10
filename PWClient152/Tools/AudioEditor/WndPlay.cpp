#include "StdAfx.h"
#include "WndPlay.h"
#include "resource.h"
#include "WndDefine.h"

#define BTN_PLAY	1001
#define BTN_STOP	1002
#define BTN_PLAYING	1003
#define BTN_PAUSE	1004

CWndPlay::CWndPlay(void)
: m_PlayButton(IDB_BTN_PLAY_DOWN, IDB_BTN_PLAY_UP)
, m_PauseButton(IDB_BTN_PAUSE_DOWN, IDB_BTN_PAUSE_UP)
{
}

CWndPlay::~CWndPlay(void)
{
}
BEGIN_MESSAGE_MAP(CWndPlay, CWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(BTN_PLAY, OnClickPlay)
	ON_BN_CLICKED(BTN_PAUSE, OnClickPause)
	ON_BN_CLICKED(BTN_STOP, OnClickStop)
	ON_BN_CLICKED(BTN_PLAYING, OnClickPlaying)
END_MESSAGE_MAP()

int CWndPlay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rtClient;
	GetClientRect(&rtClient);

	if(!m_PlayButton.Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, CRect(0,0,0,0), this ,BTN_PLAY))
		return -1;

	if(!m_PauseButton.Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, CRect(0,0,0,0), this ,BTN_PAUSE))
		return -1;

	if(!m_StopButton.Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, CRect(0,0,0,0), this ,BTN_STOP))
		return -1;

	if(!m_StopButton.LoadBitmaps(IDB_BTN_STOP))
		return -1;

	if(!m_PlayingButton.Create("ÕýÔÚ²¥·Å", WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON, CRect(0,0,0,0), this, BTN_PLAYING))
		return -1;

	m_PlayingButton.ShowWindow(SW_HIDE);

	return 0;
}

void CWndPlay::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
	CRect rc;
	rc.left = 15;
	rc.right = 47;
	rc.top = 15;
	rc.bottom = 47;
	m_PlayButton.MoveWindow(&rc, TRUE);

	rc.left += 50;
	rc.right += 50;
	m_PauseButton.MoveWindow(&rc, TRUE);

	rc.left += 50;
	rc.right += 50;
	m_StopButton.MoveWindow(&rc, TRUE);

	rc.left += 50;
	rc.right += 80;
	m_PlayingButton.MoveWindow(&rc, TRUE);
}

void CWndPlay::OnClickPlay()
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_PLAY_PLAY, 0, 0);
}

void CWndPlay::OnClickPause()
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_PLAY_PAUSE, 0, 0);
}

void CWndPlay::OnClickStop()
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_PLAY_STOP, 0, 0);
}

void CWndPlay::OnClickPlaying()
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_PLAY_PLAYING, 0, 0);
}

void CWndPlay::ShowPlayingButton(bool bShow)
{
	if(bShow)
		m_PlayingButton.ShowWindow(SW_SHOWNORMAL);
	else
		m_PlayingButton.ShowWindow(SW_HIDE);
}