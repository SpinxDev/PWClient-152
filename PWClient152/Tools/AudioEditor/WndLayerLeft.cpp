#include "stdafx.h"
#include "WndLayerLeft.h"
#include "resource.h"
#include "WndDefine.h"
#include "FEvent.h"
#include "FEventLayer.h"
#include "DlgLayerProp.h"
#include "FEventParameter.h"
#include "WndDefine.h"
#include "DlgAddEffect.h"
#include "WndEffect.h"
#include "DlgSelectSoundDef.h"
#include "DlgSoundInsProp.h"
#include "FSoundInstance.h"
#include "FEventParameter.h"
#include "FSoundDef.h"
#include "FEventLayerInstance.h"
#include "FSoundInstanceTemplate.h"
#include "Engine.h"
#include "Project.h"
#include "Global.h"
#include "EventView.h"
#include "AudioEditorView.h"

using AudioEngine::EVENT_PROPERTY;
using AudioEngine::EventParameter;
using AudioEngine::EVENT_PARAM_PROPERTY;

#define IDC_BTN_CHECK	1000

#define MENU_LAYER_PROP		0x1001
#define MENU_ADD_EFFECT		0x1002
#define MENU_DELETE_LAYER	0x1004
#define MENU_ADD_SOUNDDEF	0x1005

#define EFFECT_WIDTH	180
#define EFFECT_HEIGHT	20
#define EFFECT_V_SPACE	5
#define EFFECT_LEFT		25
#define EFFECT_TOP		15

CWndLayerLeft::CWndLayerLeft(void)
: m_btnCheckBox(IDB_BTN_UNCHECKED, IDB_BTN_CHECKED)
{
	m_clBK = RGB(118,129,140);
	m_pEventLayer = 0;
	m_pEventLayerInstance = 0;
	m_bEnable = true;
}

CWndLayerLeft::~CWndLayerLeft(void)
{
	release();
}

BEGIN_MESSAGE_MAP(CWndLayerLeft, CWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_CHECK, OnClickCheck)
	ON_WM_RBUTTONUP()
	ON_COMMAND(MENU_LAYER_PROP, OnLayerProp)
	ON_COMMAND(MENU_ADD_EFFECT, OnAddEffect)	
	ON_COMMAND(MENU_DELETE_LAYER, OnDeleteLayer)
	ON_COMMAND(MENU_ADD_SOUNDDEF, OnAddSoundDef)
END_MESSAGE_MAP()

int CWndLayerLeft::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(!m_btnCheckBox.Create(0, WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|BS_OWNERDRAW, CRect(0,0,0,0), this, IDC_BTN_CHECK))
		return -1;

	return 0;
}

void CWndLayerLeft::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
	CRect rc;
	rc.left = 15;
	rc.right = 29;
	rc.top = 0;
	rc.bottom = 14;
	m_btnCheckBox.MoveWindow(&rc, TRUE);

	int y=EFFECT_TOP;
	for (size_t i=0; i<m_arrWndEffect.size(); ++i)
	{
		rc.top = y;
		rc.bottom = rc.top + EFFECT_HEIGHT;
		rc.left = EFFECT_LEFT;
		rc.right = rc.left + EFFECT_WIDTH;
		y = rc.bottom + EFFECT_V_SPACE;
		m_arrWndEffect[i]->MoveWindow(&rc);
	}
}

void CWndLayerLeft::SetEventLayer(EventLayer* pEventLayer)
{
	if(!pEventLayer)
		return;
	m_pEventLayer = pEventLayer;
	UpdateWindows();
}

void CWndLayerLeft::OnClickCheck()
{
	if(m_pEventLayerInstance)
	{
		if(!m_pEventLayerInstance->SetMute(m_bEnable))
			return;
	}

	m_bEnable = !m_bEnable;
	m_btnCheckBox.SetButtonState(!m_bEnable);
}

void CWndLayerLeft::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, MENU_ADD_EFFECT, "添加效果器");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, MENU_LAYER_PROP, "层属性");	
	menu.AppendMenu(MF_STRING, MENU_DELETE_LAYER, "删除层");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, MENU_ADD_SOUNDDEF, "添加声音定义实例");
	menu.TrackPopupMenu(TPM_RIGHTALIGN, point.x, point.y, this);

	CWndBase::OnRButtonUp(nFlags, point);
}

void CWndLayerLeft::OnLayerProp()
{
	if(!m_pEventLayer)
		return;
	CDlgLayerProp dlg(m_pEventLayer->GetEvent());
	dlg.SetEventLayer(m_pEventLayer);
	if(IDOK != dlg.DoModal())
		return;	
	if(!m_pEventLayer->AttachParameter(dlg.GetParamName()))
		return;
	if(0 != strcmp(dlg.GetName(), m_pEventLayer->GetName()))
	{
		m_pEventLayer->SetName(dlg.GetName());
		Update();
	}
	Update();
	GF_GetView()->GetEventView()->RecreateEventInstance();
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndLayerLeft::OnAddEffect()
{
	CDlgAddEffect dlg;
	if(IDOK!=dlg.DoModal())
		return;
	switch (dlg.GetEffectType())
	{
	case AudioEngine::EFFECT_TYPE_NONE:
		return;
		break;
	case AudioEngine::EFFECT_TYPE_PAN:		
		{
			if(m_pEventLayer->HasEffectType(AudioEngine::EFFECT_TYPE_PAN))
			{
				MessageBox("已经有Pan效果，不可再添加");
				return;
			}
			if(m_pEventLayer->HasEffectType(AudioEngine::EFFECT_TYPE_SURROUND_PAN))
			{
				MessageBox("已经有Surround Pan效果，不可再添加");
				return;
			}
			Event* pEvent = m_pEventLayer->GetEvent();
			if(!pEvent)
				return;
			EVENT_PROPERTY prop;
			pEvent->GetProperty(prop);
			if(prop.mode == AudioEngine::MODE_3D)
			{
				MessageBox("Pan效果只对2D事件有效");
				return;
			}
		}		
		break;
	case AudioEngine::EFFECT_TYPE_SURROUND_PAN:		
		{
			if(m_pEventLayer->HasEffectType(AudioEngine::EFFECT_TYPE_PAN))
			{
				MessageBox("已经有Pan效果，不可再添加");
				return;
			}
			if(m_pEventLayer->HasEffectType(AudioEngine::EFFECT_TYPE_SURROUND_PAN))
			{
				MessageBox("已经有Surround Pan效果，不可再添加");
				return;
			}
			Event* pEvent = m_pEventLayer->GetEvent();
			if(!pEvent)
				return;
			EVENT_PROPERTY prop;
			pEvent->GetProperty(prop);
			if(prop.mode == AudioEngine::MODE_3D)
			{
				MessageBox("Surround Pan效果只对2D事件有效");
				return;
			}
		}
		break;
	case AudioEngine::EFFECT_TYPE_VOLUME:
		break;
	case AudioEngine::EFFECT_TYPE_PITCH:
		break;
	case AudioEngine::EFFECT_TYPE_PARAMEQ:
		break;
	case AudioEngine::EFFECT_TYPE_COMPRESSOR:
		break;
	case AudioEngine::EFFECT_TYPE_ECHO:
		break;
	case AudioEngine::EFFECT_TYPE_OCCLUSION:
		{
			if(m_pEventLayer->HasEffectType(AudioEngine::EFFECT_TYPE_OCCLUSION))
			{
				MessageBox("已经有Occlusion效果，不可再添加");
				return;
			}
			Event* pEvent = m_pEventLayer->GetEvent();
			if(!pEvent)
				return;
			EVENT_PROPERTY prop;
			pEvent->GetProperty(prop);
			if(prop.mode == AudioEngine::MODE_2D)
			{
				MessageBox("Occlusion效果只对3D事件有效");
				return;
			}
		}
		break;
	}
	if(!m_pEventLayer->AddEffect(dlg.GetEffectType()))
		return;
	::SendMessage(this->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndLayerLeft::OnDeleteLayer()
{
	if(!m_pEventLayer)
		return;
	CString strTip;
	strTip.Format("确定删除层“%s”吗？", m_pEventLayer->GetName());
	if(IDYES != MessageBox(strTip, "警告", MB_ICONWARNING|MB_YESNO))
		return;
	m_pEventLayer->GetEvent()->DeleteLayer(m_pEventLayer);
	::SendMessage(this->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	GF_GetView()->GetEventView()->RecreateEventInstance();
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndLayerLeft::DrawRealize(CDC* pDC)
{
	if(!m_pEventLayer)
		return;
	CRect rc;
	GetClientRect(&rc);
	int x = 45;//(int)(rc.Width()*0.7f);
	CRect rcText;
	rcText.left = x;
	rcText.right = rc.right;
	rcText.top = rc.top;
	rcText.bottom = rcText.top + 20;
	CString strText;
	strText.Format("%s   (%s)", m_pEventLayer->GetName(), m_pEventLayer->GetParameterName());
	DrawText(pDC, rcText, strText, 0);
}

void CWndLayerLeft::EndDraw()
{
	m_btnCheckBox.RedrawWindow();
	for (size_t i=0; i<m_arrWndEffect.size(); ++i)
	{
		m_arrWndEffect[i]->RedrawWindow();
	}
}

bool CWndLayerLeft::UpdateWindows()
{
	release();
	if(!m_pEventLayer)
		return false;
	int iEffectNum = m_pEventLayer->GetEffectNum();
	for (int i=0; i<iEffectNum; ++i)
	{
		CWndEffect* pWndEffect = new CWndEffect;
		if(!pWndEffect->Create(this, CRect(0,0,0,0)))
			return false;
		pWndEffect->SetEffect(m_pEventLayer->GetEffectByIndex(i), m_pEventLayer);
		m_arrWndEffect.push_back(pWndEffect);
	}
	SendMessage(WM_SIZE);
	return true;
}

void CWndLayerLeft::Reset()
{
	m_bEnable = true;
	m_btnCheckBox.SetButtonState(false);
	for (size_t i=0; i<m_arrWndEffect.size(); ++i)
	{
		m_arrWndEffect[i]->Reset();
	}
}

void CWndLayerLeft::release()
{
	for (size_t i=0; i<m_arrWndEffect.size(); ++i)
	{
		m_arrWndEffect[i]->DestroyWindow();
		delete m_arrWndEffect[i];
	}
	m_arrWndEffect.clear();
}

void CWndLayerLeft::OnAddSoundDef()
{
	if(!m_pEventLayer)
		return;
	CDlgSelectSoundDef dlgSelectSoundDef(false);
	if(IDOK != dlgSelectSoundDef.DoModal())
		return;
	if(dlgSelectSoundDef.GetSelectSoundDefList().size() != 1)
		return;
	SoundDef* pSoundDef = dlgSelectSoundDef.GetSelectSoundDefList().front();
	if(!pSoundDef)
		return;
	EventParameter* pEventParameter = m_pEventLayer->GetParameter();
	EVENT_PARAM_PROPERTY paramProp;
	pEventParameter->GetProperty(paramProp);
	SOUND_INSTANCE_PROPERTY soundInsOrop;
	soundInsOrop.fStartPosition = paramProp.fMinValue;
	soundInsOrop.fLength = paramProp.fMaxValue - paramProp.fMinValue;
	CDlgSoundInsProp dlgSoundInsProp;
	dlgSoundInsProp.SetProp(soundInsOrop, paramProp.fMinValue, paramProp.fMaxValue, "");
_EditProp:
	if(IDOK != dlgSoundInsProp.DoModal())
		return;
	SOUND_INSTANCE_PROPERTY& newSoundInsOrop = dlgSoundInsProp.GetProp();
	newSoundInsOrop.fStartPosition = (newSoundInsOrop.fStartPosition - paramProp.fMinValue)/(paramProp.fMaxValue-paramProp.fMinValue);
	newSoundInsOrop.fLength = newSoundInsOrop.fLength/(paramProp.fMaxValue-paramProp.fMinValue);
	if(newSoundInsOrop.fStartPosition + newSoundInsOrop.fLength - 1.0f > 1e-5)
	{
		MessageBox("超过参数范围", "警告", MB_ICONWARNING);
		goto _EditProp;
	}
	int iSoundInstanceNum = m_pEventLayer->GetSoundInstanceTemplateNum();
	for (int i=0; i<iSoundInstanceNum; ++i)
	{
		SOUND_INSTANCE_PROPERTY insProp;
		SoundInstanceTemplate* pSoundInstanceTemplate = m_pEventLayer->GetSoundInstanceTemplateByIndex(i);
		pSoundInstanceTemplate->GetProperty(insProp);
		float fMinStart = newSoundInsOrop.fStartPosition;
		if(insProp.fStartPosition < fMinStart)
			fMinStart = insProp.fStartPosition;
		float fMaxEnd = newSoundInsOrop.fStartPosition + newSoundInsOrop.fLength;
		if(insProp.fStartPosition + insProp.fLength > fMaxEnd)
			fMaxEnd = insProp.fStartPosition + insProp.fLength;
		if(newSoundInsOrop.fLength + insProp.fLength - (fMaxEnd - fMinStart) > 1e-5)
		{
			CString strTip;
			strTip.Format("与实例%s有重叠", pSoundInstanceTemplate->GetSoundDef()->GetName());
			MessageBox(strTip, "警告", MB_ICONWARNING);
			goto _EditProp;
		}
	}

	SoundInstanceTemplate* pSoundInstanceTemplate = m_pEventLayer->AddSoundInstanceTemplate(pSoundDef);
	if(!pSoundInstanceTemplate)
		return;
	pSoundInstanceTemplate->SetProperty(newSoundInsOrop);
	pSoundInstanceTemplate->SetNote(dlgSoundInsProp.GetNote());
	::SendMessage(this->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	GF_GetView()->GetEventView()->RecreateEventInstance();
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndLayerLeft::SetEventLayerInstance(EventLayerInstance* pEventLayerInstance)
{
	m_pEventLayerInstance = pEventLayerInstance;
	for (size_t i=0; i<m_arrWndEffect.size(); ++i)
	{
		m_arrWndEffect[i]->SetEventLayerInstance(pEventLayerInstance);
	}
}

bool CWndLayerLeft::GetEffectCheckState(int idx) const
{
	return m_arrWndEffect[idx]->GetEffectCheckState();
}