#include "stdafx.h"
#include "WndEffect.h"
#include "resource.h"
#include "DlgEffectProp.h"
#include "FEffect.h"
#include "FEventLayerInstance.h"
#include "WndDefine.h"
#include "FEventLayer.h"
#include "Engine.h"
#include "Project.h"
#include "WndLayers.h"
#include "WndLayerRight.h"
#include "FEvent.h"
#include "../AudioEngine/xml/xmlcommon.h"
#include "Global.h"
#include "Render.h"

#include <AFileDialogWrapper.h>

#define EFFECT_BTN_CHECK	0x101
#define EFFECT_BTN_PROP		0x102
#define EFFECT_BTN_EDIT		0x103

#define MENU_DELETE_EFFECT	0x1003
#define MENU_SAVE_PRESET	0x1004
#define MENU_LOAD_PRESET	0x1005

BEGIN_MESSAGE_MAP(CWndEffect, CWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(EFFECT_BTN_PROP, OnClickProp)
	ON_BN_CLICKED(EFFECT_BTN_EDIT, OnClickEdit)
	ON_COMMAND(MENU_DELETE_EFFECT, OnDeleteEffect)
	ON_BN_CLICKED(EFFECT_BTN_CHECK, OnClickCheck)
	ON_COMMAND(MENU_SAVE_PRESET, OnSavePreset)
	ON_COMMAND(MENU_LOAD_PRESET, OnLoadPreset)
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CWndEffect::CWndEffect(void)
: m_btnCheckBox(IDB_BTN_UNCHECKED, IDB_BTN_CHECKED)
, m_pEffect(0)
, m_pEventLayerInstance(0)
, m_pEventLayer(0)
, m_bEnable(true)
, m_bEditing(false)
{
	m_clBK = RGB(42,46,50);
}

CWndEffect::~CWndEffect(void)
{
}

void CWndEffect::DrawRealize(CDC* pDC)
{
	if(!m_pEffect)
		return;
	CString strEffectType;	
	strEffectType = m_pEffect->GetName();
	CRect rc;
	rc.left = 20;
	rc.right = 120;
	rc.top = 0;
	rc.bottom = 20;
	DrawText(pDC, rc, strEffectType, RGB(118,129,140));
}

void CWndEffect::EndDraw()
{
	m_btnCheckBox.RedrawWindow();
	m_btnProp.RedrawWindow();
}

void CWndEffect::Reset()
{
	m_bEnable = true;
	m_btnCheckBox.SetButtonState(false);	
}

int CWndEffect::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWndBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_btnCheckBox.Create(0, WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|BS_OWNERDRAW, CRect(0,0,0,0), this, EFFECT_BTN_CHECK))
		return -1;

	if(!m_btnEdit.Create("编辑", WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON, CRect(0,0,0,0), this, EFFECT_BTN_EDIT))
		return -1;

	if(!m_btnProp.Create("属性", WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON, CRect(0,0,0,0), this, EFFECT_BTN_PROP))
		return -1;

	return 0;
}

void CWndEffect::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);

	CRect rc;
	rc.left = 0;
	rc.right = 14;
	rc.top = 0;
	rc.bottom = 14;
	m_btnCheckBox.MoveWindow(&rc);

	rc.left = 100;
	rc.right = 140;
	rc.top = 0;
	rc.bottom = 20;
	m_btnEdit.MoveWindow(&rc);

	rc.left = 140;
	rc.right = 180;
	rc.top = 0;
	rc.bottom = 20;
	m_btnProp.MoveWindow(&rc);
}

void CWndEffect::OnClickProp()
{
	if(!m_pEffect)
		return;
	CDlgEffectProp dlg(m_pEffect);
	if(IDOK != dlg.DoModal())
		return;
	switch (m_pEffect->GetType())
	{
	case AudioEngine::EFFECT_TYPE_NONE:
		return;
	case AudioEngine::EFFECT_TYPE_PAN:
		m_pEffect->SetParameter(idPan, dlg.GetPanData().fPan);
		break;
	case AudioEngine::EFFECT_TYPE_VOLUME:
		m_pEffect->SetParameter(idVolume, dlg.GetVolumeData().fVolume);
		break;
	case AudioEngine::EFFECT_TYPE_PITCH:
		m_pEffect->SetParameter(idPitch, dlg.GetPitchData().fPitch);
		break;
	case AudioEngine::EFFECT_TYPE_SURROUND_PAN:
		m_pEffect->SetParameter(idLeftRightPan, dlg.GetSurroundPanData().fLeftRightPan);
		m_pEffect->SetParameter(idBackFrontPan, dlg.GetSurroundPanData().fBackFrontPan);
		m_pEffect->SetParameter(idLFE, dlg.GetSurroundPanData().fLFE);
		break;
	case AudioEngine::EFFECT_TYPE_PARAMEQ:
		m_pEffect->SetParameter(idCenterFreq, dlg.GetParamEQData().fCenterFreq);
		m_pEffect->SetParameter(idOctaveRange, dlg.GetParamEQData().fOctaveRange);
		m_pEffect->SetParameter(idFreqGain, dlg.GetParamEQData().fFreqGain);
		break;
	case AudioEngine::EFFECT_TYPE_COMPRESSOR:
		m_pEffect->SetParameter(idThreshold, dlg.GetCompressorData().fThreshold);
		m_pEffect->SetParameter(idAttack, dlg.GetCompressorData().fAttack);
		m_pEffect->SetParameter(idRelease, dlg.GetCompressorData().fRelease);
		m_pEffect->SetParameter(idMakeUpGain, dlg.GetCompressorData().fMakeUpGain);
		break;
	case AudioEngine::EFFECT_TYPE_ECHO:
		m_pEffect->SetParameter(idDelay, dlg.GetEchoData().fDelay);
		m_pEffect->SetParameter(idDecay, dlg.GetEchoData().fDecay);
		m_pEffect->SetParameter(idChannels, dlg.GetEchoData().fChannels);
		m_pEffect->SetParameter(idDryMix, dlg.GetEchoData().fDryMix);
		m_pEffect->SetParameter(idWetMix, dlg.GetEchoData().fWetMix);
		break;
	case AudioEngine::EFFECT_TYPE_OCCLUSION:
		m_pEffect->SetParameter(idDirect, dlg.GetOcclusionData().fDirect);
		m_pEffect->SetParameter(idReverb, dlg.GetOcclusionData().fReverb);
		break;
	}
	if(!m_pEventLayer || !m_pEffect)
		return;
	int idx = -1;
	Event* pEvent = m_pEventLayer->GetEvent();
	for (int i=0; i<pEvent->GetLayerNum(); ++i)
	{
		if(m_pEventLayer == pEvent->GetLayerByIndex(i))
		{
			idx = i;
			break;
		}
	}
	if(idx == -1)
		return;
	((CWndLayers*)GetParent()->GetParent())->GetWndLayerRight(idx)->UpdateCurves(false);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndEffect::OnClickEdit()
{
	if(!m_pEventLayer || !m_pEffect)
		return;
	int idx = -1;
	Event* pEvent = m_pEventLayer->GetEvent();
	for (int i=0; i<pEvent->GetLayerNum(); ++i)
	{
		if(m_pEventLayer == pEvent->GetLayerByIndex(i))
		{
			idx = i;
			break;
		}
	}
	if(idx == -1)
		return;
	m_bEditing = !m_bEditing;
	if(m_bEditing)
		((CWndLayers*)(GetParent()->GetParent()))->GetWndLayerRight(idx)->EditEffect(m_pEffect);
	else
		((CWndLayers*)(GetParent()->GetParent()))->GetWndLayerRight(idx)->EditEffect(0);
}

void CWndEffect::OnClickCheck()
{
	if(m_pEventLayerInstance && m_pEffect)
	{
		if(!m_bEnable)
		{
			if(!m_pEventLayerInstance->ApplyEffect(m_pEffect))
				return;
		}
		else
		{
			if(!m_pEventLayerInstance->RemoveEffect(m_pEffect))
				return;
		}
	}	
	m_bEnable = !m_bEnable;
	m_btnCheckBox.SetButtonState(!m_bEnable);
}

void CWndEffect::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, MENU_DELETE_EFFECT, "删除效果器");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, MENU_SAVE_PRESET, "保存预设...");
	menu.AppendMenu(MF_STRING, MENU_LOAD_PRESET, "加载预设...");
	menu.TrackPopupMenu(TPM_RIGHTALIGN, point.x, point.y, this);

	CWndBase::OnRButtonUp(nFlags, point);
}

void CWndEffect::OnDeleteEffect()
{
	if(!m_pEffect || !m_pEventLayer)
		return;
	CString strEffectType = m_pEffect->GetName();
	CString strTip;
	strTip.Format("确定删除效果“%s”吗？", strEffectType);
	if(IDYES != MessageBox(strTip, "删除效果", MB_ICONWARNING | MB_YESNO))
		return;
	if(!m_pEventLayer->DeleteEffect(m_pEffect))
		return;
	::SendMessage(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndEffect::SetEffect(Effect* pEffect, EventLayer* pEventLayer)
{
	if(!pEffect || !pEventLayer)
		return;
	m_pEffect = pEffect;
	m_pEventLayer = pEventLayer;
}

void CWndEffect::SetEventLayerInstance(EventLayerInstance* pEventLayerInstance)
{
	m_pEventLayerInstance = pEventLayerInstance;
}

void CWndEffect::OnSavePreset()
{
	if(!m_pEffect || !m_pEventLayer)
		return;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),FALSE, g_Configs.szEffectPresetPath, "保存Effect预设文件","",this);
	if(IDOK != dlg.DoModal())
		return;

	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("EffectPreset");
	doc.LinkEndChild(root);
	AddElement(root, "Type", (int)m_pEffect->GetType());
	m_pEffect->SaveXML(root);
	TiXmlPrinter printer;
	doc.Accept(&printer);

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, dlg.GetFullPathName(), "w"))
	{
		return;
	}
	fwrite((LPVOID)printer.CStr(), printer.Size(), 1, pFile);
	fclose(pFile);
	GF_Log(LOG_NORMAL, "成功保存预设%s", dlg.GetFileName());
}

void CWndEffect::OnLoadPreset()
{
	if(!m_pEffect || !m_pEventLayer)
		return;

	AFileDialogWrapper dlg(g_Render.GetA3DDevice(),TRUE, g_Configs.szEffectPresetPath, "选择Effect预设文件","",this);	
	if(IDOK != dlg.DoModal())
		return;

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, dlg.GetFullPathName(), "r"))
	{
		return;
	}

	fseek(pFile, 0, SEEK_END);
	long lLength = ftell(pFile);
	char* pBuf = new char[lLength + 1];
	memset(pBuf, 0, lLength + 1);
	fseek(pFile, 0, SEEK_SET);
	fread(pBuf, lLength, 1, pFile);	
	fclose(pFile);
	pBuf[lLength] = 0;

	TiXmlDocument doc;
	doc.Parse(pBuf);
	delete [] pBuf;

	TiXmlNode* pNode = doc.FirstChild("EffectPreset");
	if (!pNode)
	{
		GF_Log(LOG_ERROR, "%s不是Effect预设文件", dlg.GetFileName());
		return;
	}

	TiXmlElement* root = pNode->ToElement();
	int iType;
	QueryElement(root, "Type", iType);
	if(iType != m_pEffect->GetType())
	{
		GF_Log(LOG_ERROR, "Effect类型不同，不能加载预设");
		return;
	}
	m_pEffect->LoadXML(root->FirstChild("Effect")->ToElement());
	::SendMessage(this->GetParent()->GetParent()->GetParent()->GetSafeHwnd(), WM_UPDATE_WINDOW, 0, 0);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	GF_Log(LOG_NORMAL, "成功加载预设%s", dlg.GetFileName());
}