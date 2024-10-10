#pragma once
#include "wndbase.h"

using AudioEngine::SoundInstanceTemplate;

class CWndSoundInstance : public CWndBase
{
public:
	CWndSoundInstance(void);
	~CWndSoundInstance(void);
public:
	void SetSoundInstance(SoundInstanceTemplate* pSoundInstanceTemplate);
	SoundInstanceTemplate* GetSoundInstance() const { return m_pSoundInstanceTemplate; }
protected:
	virtual void DrawRealize(CDC* pDC);
protected:
	SoundInstanceTemplate* m_pSoundInstanceTemplate;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSoundInsProp();
	afx_msg void OnDeleteSoundIns();
	afx_msg void OnSavePreset();
	afx_msg void OnLoadPreset();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	bool m_bLButtonDown;
	CPoint m_ptLast;
	HCURSOR m_hResizeCursor;
	HCURSOR m_hMoveCursor;
	UINT m_msg;
};
