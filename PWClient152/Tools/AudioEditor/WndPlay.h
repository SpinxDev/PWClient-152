#pragma once

#include "WndBase.h"
#include "BounceButton.h"

class CWndPlay : public CWndBase
{
public:
	CWndPlay(void);
	~CWndPlay(void);
public:
	void SetPlayButtonState(bool bDown) { m_PlayButton.SetButtonState(bDown); }
	void SetPauseButtonState(bool bDown) { m_PauseButton.SetButtonState(bDown); }
	void ShowPlayingButton(bool bShow);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickPlay();
	afx_msg void OnClickPause();
	afx_msg void OnClickStop();
	afx_msg void OnClickPlaying();
protected:
	CBounceButton m_PlayButton;
	CBounceButton m_PauseButton;
	CBitmapButton m_StopButton;
	CBCGPButton m_PlayingButton;
};
