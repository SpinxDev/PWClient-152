#pragma once
#include "WndBase.h"
#include <vector>
#include "FAudioEngine.h"
#include "WndParam.h"

class CWndParam;
class CWndScale;
class CWndLayers;

using AudioEngine::Event;
using AudioEngine::EventInstance;

typedef std::vector<CWndParam*> ParamArray;

class CWndEvent : public CWndBase
{
public:
	CWndEvent(void);
	~CWndEvent(void);
public:
	void				SetEvent(Event* pEvent);
	bool				UpdateWindows();
	void				Reset();
	bool				GetLayerParamData(int idx, WND_PARAM_DATA& data);
	void				SetEventInstance(EventInstance* pEventInstance);
	bool				GetLayerCheckState(int idx) const;
	bool				GetEffectCheckState(int iLayerIdx, int iEffectIdx) const;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void		OnSize(UINT nType, int cx, int cy);			
	LRESULT				OnUpdateWindow(WPARAM wParam, LPARAM lParam);
	virtual BOOL		PreTranslateMessage(MSG* pMsg);
	afx_msg void		OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void		OnAddLayer();
protected:
	void				release();
protected:
	ParamArray			m_arrParam;
	CWndScale*			m_pWndScale;
	Event*				m_pEvent;
	CWndParam*			m_pActiveWndParam;	
	CWndLayers*			m_pWndLayers;
};