#include "stdafx.h"
#include "WndParam.h"
#include "FEventParameter.h"
#include "DlgSetParamValue.h"
#include "Global.h"
#include "WndDefine.h"
#include "DlgParamProp.h"
#include "FEvent.h"
#include "Engine.h"
#include "Project.h"

using AudioEngine::EVENT_PARAM_PROPERTY;
using AudioEngine::Event;
using AudioEngine::VELOCIY_LOOP_MODE;

#define COLOR_ACTIVE	RGB(118,129,140)
#define COLOR_UNACTIVE	RGB(33,37,40)

#define LINE_COLOR_ACTIVE	RGB(255,255,0)
#define LINE_COLOR_UNACTIVE	RGB(160,160,40)

#define MENU_SET_VALUE	0X1001
#define MENU_ADD_PARAM	0X1002
#define MENU_DEL_PARAM	0X1003
#define MENU_SET_PROP	0X1004

BEGIN_MESSAGE_MAP(CWndParam, CWndBase)
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
	ON_COMMAND(MENU_SET_VALUE, &CWndParam::OnSetValue)
	ON_COMMAND(MENU_ADD_PARAM, &CWndParam::OnAddParam)
	ON_COMMAND(MENU_DEL_PARAM, &CWndParam::OnDelParam)
	ON_COMMAND(MENU_SET_PROP, &CWndParam::OnSetProp)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CWndParam::CWndParam(void)
{
	m_clBK = COLOR_UNACTIVE;
	m_pEventParam = 0;
	m_pEventInstanceParam = 0;
	m_bActive = false;
	m_bLButtonDown = false;
}

CWndParam::~CWndParam(void)
{
	if(m_pEventInstanceParam)	
		m_pEventInstanceParam->RemoveListener(this);
}

void CWndParam::DrawRealize(CDC* pDC)
{
	if(!m_pEventParam)
		return;
	COLORREF cl;
	if(m_bActive)
		cl = LINE_COLOR_ACTIVE;
	else
		cl = LINE_COLOR_UNACTIVE;
	CRect rc;
	GetClientRect(&rc);
	float fValue = 0.0f;
	if(m_pEventInstanceParam)
		fValue = m_pEventInstanceParam->GetValue();
	else
		fValue = m_pEventParam->GetValue();
	int x = (int)((fValue - m_ParamData.fMinValue)/m_ParamData.fPerPixelValue + 0.5f);	
	DrawLine(pDC, x, 0, x, rc.Height(), cl, 2);

	CRect rcText;
	rcText.left = 15;
	rcText.right = 100;
	rcText.top = rc.top;
	rcText.bottom = rc.bottom;
	
	DrawText(pDC, rcText, m_pEventParam->GetName(), cl);
}

void CWndParam::SetEventParem(EventParameter* pEventParam, EventInstanceParameter* pEventInstanceParam)
{
	if(m_pEventInstanceParam && m_pEventInstanceParam != pEventInstanceParam)	
		m_pEventInstanceParam->RemoveListener(this);
	if(pEventInstanceParam && m_pEventInstanceParam != pEventInstanceParam)
		pEventInstanceParam->AddListener(this);

	m_pEventParam = pEventParam;
	m_pEventInstanceParam = pEventInstanceParam;	
	Update();
}

void CWndParam::SetActive(bool bActive)
{
	if(m_bActive == bActive)
		return;
	m_bActive = bActive;
	if(m_bActive)
		m_clBK = COLOR_ACTIVE;
	else
		m_clBK = COLOR_UNACTIVE;
	Update();
}

bool CWndParam::OnValueChanged(EventParameter* pEventParameter)
{
	m_fValue = pEventParameter->GetValue();
	Update();
	return true;
}

void CWndParam::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_bActive)
		return;
	if(!m_pEventParam)
		return;
	float fValue = m_ParamData.fMinValue + point.x * m_ParamData.fPerPixelValue;
	fValue = GF_Clamp(m_ParamData.fMinValue, m_ParamData.fMaxValue, fValue);
	if(m_pEventInstanceParam)
		m_pEventInstanceParam->SetValue(fValue, true);
	else
		m_pEventParam->SetValue(fValue, false);
	Update();
	m_bLButtonDown = true;
	SetCapture();

	CWndBase::OnLButtonDown(nFlags, point);
}

void CWndParam::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
	if(!m_pEventParam)
		return;

	CRect rc;
	GetClientRect(&rc);
	EVENT_PARAM_PROPERTY prop;
	m_pEventParam->GetProperty(prop);
	m_ParamData.fMinValue = prop.fMinValue;
	m_ParamData.fMaxValue = prop.fMaxValue;
	int iWidth = rc.Width();
	m_ParamData.fInterval = (iWidth - 30)/100.0f;
	if(m_ParamData.fInterval == 0)
		m_ParamData.fInterval = 1;
	float fValueStep = (m_ParamData.fMaxValue-m_ParamData.fMinValue)/100.0f;
	m_ParamData.fPerPixelValue = fValueStep / m_ParamData.fInterval;
}

void CWndParam::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, MENU_SET_VALUE, "设定参数值");
	menu.AppendMenu(MF_STRING, MENU_ADD_PARAM, "添加参数");
	menu.AppendMenu(MF_STRING, MENU_DEL_PARAM, "删除参数");
	menu.AppendMenu(MF_STRING, MENU_SET_PROP, "设置参数属性");
	menu.TrackPopupMenu(TPM_RIGHTALIGN, point.x, point.y, this);

	CWndBase::OnRButtonUp(nFlags, point);
}

void CWndParam::OnSetValue()
{
	if(!m_pEventParam)
		return;
	float fValue;
	if(m_pEventInstanceParam)
		fValue = m_pEventInstanceParam->GetValue();
	else
		fValue = m_pEventParam->GetValue();
	CDlgSetParamValue dlg(fValue);
	if(IDOK != dlg.DoModal())
		return;
	float value = GF_Clamp(m_ParamData.fMinValue, m_ParamData.fMaxValue, dlg.GetValue());
	if(m_pEventInstanceParam)
		m_pEventInstanceParam->SetValue(value, true);
	else
		m_pEventParam->SetValue(value, false);
	Update();
}

void CWndParam::OnAddParam()
{
	if(!m_pEventParam)
		return;
	Event* pEvent = m_pEventParam->GetEvent();
	if(!pEvent)
		return;
	int iParamNum = pEvent->GetParameterNum();
	CString strName;
	while (true)
	{
		strName.Format("Param_%d", iParamNum++);
		if(!pEvent->IsParameterExist(strName))
			break;
	}
	CDlgParamProp dlg;
	dlg.SetProp(strName, 0, 1, 0, 0, 0);
_Edit:
	if(IDOK != dlg.DoModal())
		return;
	float fMinValue, fMaxValue, fVelocity, fSeekSpeed;
	int iLoopMode;
	dlg.GetProp(strName, fMinValue, fMaxValue, fVelocity, iLoopMode, fSeekSpeed);
	if(pEvent->IsParameterExist(strName))
	{
		MessageBox("已存在同名参数", "警告", MB_ICONWARNING);
		goto _Edit;
	}
	EventParameter* pEventParameter = pEvent->AddParameter(strName);
	pEventParameter->SetEvent(pEvent);
	EVENT_PARAM_PROPERTY prop;
	prop.fMinValue = fMinValue;
	prop.fMaxValue = fMaxValue;
	prop.fVelocity = fVelocity;
	prop.velocityLoopMode = (VELOCIY_LOOP_MODE)iLoopMode;
	prop.fSeekSpeed = fSeekSpeed;
	pEventParameter->SetProperty(prop);
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndParam::OnDelParam()
{
	if(!m_pEventParam)
		return;
	CString strTip;
	strTip.Format("确定删除参数“%s”吗？", m_pEventParam->GetName());
	if(IDYES != MessageBox(strTip, "警告", MB_ICONWARNING|MB_YESNO))
		return;
	Event* pEvent = m_pEventParam->GetEvent();
	if(!pEvent)
		return;
	if(pEvent->IsParameterUsed(m_pEventParam->GetName()))
	{
		MessageBox("有事件层引用了该参数，不能删除", "警告", MB_ICONWARNING);
		return;
	}
	if(pEvent->GetParameterNum() == 1)
	{
		MessageBox("事件需要至少一个参数，不能删除", "警告", MB_ICONWARNING);
		return;
	}
	if(!pEvent->DeleteParameter(m_pEventParam))
		return;
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndParam::OnSetProp()
{
	if(!m_pEventParam)
		return;
	Event* pEvent = m_pEventParam->GetEvent();
	if(!pEvent)
		return;
	CString strName = m_pEventParam->GetName();
	EVENT_PARAM_PROPERTY prop;
	m_pEventParam->GetProperty(prop);
	CDlgParamProp dlg;
	dlg.SetProp(strName, m_ParamData.fMinValue, m_ParamData.fMaxValue, prop.fVelocity, prop.velocityLoopMode, prop.fSeekSpeed);
	if(IDOK != dlg.DoModal())
		return;
	float fMinValue, fMaxValue, fVelocity, fSeekSpeed;
	int iLoopMode;
	dlg.GetProp(strName, fMinValue, fMaxValue, fVelocity, iLoopMode, fSeekSpeed);
	m_pEventParam->SetName(strName);	
	prop.fMinValue = fMinValue;
	prop.fMaxValue = fMaxValue;
	prop.fVelocity = fVelocity;
	prop.velocityLoopMode = (VELOCIY_LOOP_MODE)iLoopMode;
	prop.fSeekSpeed = fSeekSpeed;
	m_pEventParam->SetProperty(prop);
	if(m_pEventInstanceParam)
		m_pEventInstanceParam->SetProperty(prop);
	::SendMessage(this->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndParam::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bLButtonDown)
		return;
	if(!m_bActive)
		return;
	if(!m_pEventParam)
		return;
	float fValue = m_ParamData.fMinValue + point.x * m_ParamData.fPerPixelValue;
	fValue = GF_Clamp(m_ParamData.fMinValue, m_ParamData.fMaxValue, fValue);
	if(m_pEventInstanceParam)
		m_pEventInstanceParam->SetValue(fValue, true);
	else
		m_pEventParam->SetValue(fValue, false);
	Update();
	CWndBase::OnMouseMove(nFlags, point);
}

void CWndParam::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = false;
	ReleaseCapture();
	CWndBase::OnLButtonUp(nFlags, point);
}
