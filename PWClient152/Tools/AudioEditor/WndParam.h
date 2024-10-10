#pragma once
#include "WndBase.h"
#include "FAudioEngine.h"
#include "FEventParameter.h"

using AudioEngine::EventParameter;
using AudioEngine::EventInstanceParameter;

struct WND_PARAM_DATA 
{
	float fMinValue;
	float fMaxValue;
	float fInterval;
	float fPerPixelValue;
};

class CWndParam : public CWndBase, public EventParameter::EventParameterListener
{
public:
	CWndParam(void);
	~CWndParam(void);
public:
	void					SetEventParem(EventParameter* pEventParam, EventInstanceParameter* pEventInstanceParam);
	EventParameter*			GetEventParam() const { return m_pEventParam; }
	void					SetActive(bool bActive);
	void					GetParamData(WND_PARAM_DATA& data) const { data = m_ParamData; }
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetValue();
	afx_msg void OnAddParam();
	afx_msg void OnDelParam();
	afx_msg void OnSetProp();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	virtual void			DrawRealize(CDC* pDC);
	virtual bool			OnValueChanged(EventParameter* pEventParameter);
protected:
	float					m_fValue;
	EventParameter*			m_pEventParam;
	EventParameter*			m_pEventInstanceParam;
	bool					m_bActive;
	WND_PARAM_DATA			m_ParamData;
	bool					m_bLButtonDown;	
};
