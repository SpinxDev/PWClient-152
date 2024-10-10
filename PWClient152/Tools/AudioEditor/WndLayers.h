#pragma once
#include "wndbase.h"
#include <vector>

using AudioEngine::Event;
using AudioEngine::EventInstance;

class CWndLayerLeft;
class CWndLayerRight;
class CWndEvent;

typedef std::vector<CWndLayerLeft*> LayerLeftArray;
typedef std::vector<CWndLayerRight*> LayerRightArray;

class CWndLayers : public CWndBase
{
public:
	CWndLayers(void);
	~CWndLayers(void);
public:
	virtual BOOL		Create(CWnd* pParentWnd, const RECT& rect);
	void				SetEvent(Event* pEvent);
	void				SetEventWindow(CWndEvent* pWndEvent);
	bool				UpdateWindows();
	void				Reset();
	void				SetEventInstance(EventInstance* pEventInstance);
	CWndLayerLeft*		GetWndLayerLeft(int idx) const;
	CWndLayerRight*		GetWndLayerRight(int idx) const;
protected:
	DECLARE_MESSAGE_MAP()	
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);	
protected:
	void				release();
	void				updateVScrollBar();
	int					getTotalHeight();
protected:
	LayerLeftArray		m_arrLayerLeft;
	LayerRightArray		m_arrLayerRight;
	Event*				m_pEvent;
	CWndEvent*			m_pWndEvent;
	int					m_iVScrollPos;
	int					m_iVScrollPageSize;
public:
	afx_msg void		OnAddLayer();
};
