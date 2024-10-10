// DlgReverb.cpp : implementation file
//

#include "stdafx.h"
#include "FReverb.h"
#include "AudioEditor.h"
#include "DlgReverb.h"
#include "FReverbDataManager.h"
#include "Engine.h"
#include "Global.h"
#include "DlgNewReverb.h"

#include "PropListCtrl/BCGPPropertyListWrapper.h"
#include "PropListCtrl/BCGPPropertyObject.h"

static prop_range_template<float> s_rangeRoom;
static prop_range_template<int> s_rangeDecayTime;
static prop_range_template<float> s_rangeDecayRatio;
static prop_range_template<int> s_rangePreDelay;
static prop_range_template<int> s_rangeLateDelay;
static prop_range_template<float> s_rangeEarlyRef;
static prop_range_template<float> s_rangeLateRef;
static prop_range_template<float> s_rangePercent;
static prop_range_template<int> s_rangeHFCrossover;
static prop_range_template<int> s_rangeLFCrossover;

using AudioEngine::ReverbDataManager;

const int nBorderSize = 1;

// CDlgReverb dialog

IMPLEMENT_DYNAMIC(CDlgReverb, CBCGPDialog)

CDlgReverb::CDlgReverb(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgReverb::IDD, pParent)
	, m_bModified(false)
{
	m_pBCGPPropListWrapper = new CBCGPPropertyListWrapper;
	m_pPropertyListener = new CPropertyUpdateListener(this);
	m_pProperties = new CPropertyArray;
	m_pProperties->AddListener(m_pPropertyListener);
}

CDlgReverb::~CDlgReverb()
{
	delete m_pPropertyListener;
	m_pPropertyListener = 0;
	if (m_pProperties)
		m_pProperties->Clear();
	delete m_pProperties;
	m_pProperties = 0;
	delete m_pBCGPPropListWrapper;
	m_pBCGPPropListWrapper = 0;
}

void CDlgReverb::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REVERB, m_wndListBox);
}


BEGIN_MESSAGE_MAP(CDlgReverb, CBCGPDialog)
	ON_LBN_SELCHANGE(IDC_LIST_REVERB, &CDlgReverb::OnLbnSelchangeListReverb)
	ON_BN_CLICKED(IDC_NEW_REVERB, &CDlgReverb::OnBnClickedNewReverb)
	ON_BN_CLICKED(IDC_DEL_REBERB, &CDlgReverb::OnBnClickedDelReberb)
	ON_BN_CLICKED(IDC_REVERB_SAVE, &CDlgReverb::OnBnClickedReverbSave)
	ON_BN_CLICKED(IDOK, &CDlgReverb::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgReverb message handlers

BOOL CDlgReverb::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();

	if(!load())
		return FALSE;

	updateListBox();

	CRect rc;
	GetDlgItem(IDC_FRM_REVERB_PROP)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	if (!m_pBCGPPropListWrapper->OnCreate(rc, this))
		return FALSE;	

	m_bModified = false;
	GetDlgItem(IDC_REVERB_SAVE)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReverb::updateListBox()
{
	ReverbDataManager* pReverbDataManager = Engine::GetInstance().GetEventSystem()->GetReverbDataManager();
	if(!pReverbDataManager)
		return;
	while(m_wndListBox.GetCount())
	{
		m_wndListBox.DeleteString(0);
	}
	int iReverbNum = pReverbDataManager->GetReverbNum();
	for (int i=0; i<iReverbNum; ++i)
	{
		REVERB* pReverb = pReverbDataManager->GetReverbByIndex(i);
		m_wndListBox.InsertString(i, pReverb->strName.c_str());
		m_wndListBox.SetItemData(i, (DWORD_PTR)pReverb);
	}
}

void CDlgReverb::UpdateProperty(bool bSave, bool bIsCommit /*= false*/)
{
	int iCurSel = m_wndListBox.GetCurSel();
	if(iCurSel == -1)
		return;

	REVERB* pReverb = (REVERB*)m_wndListBox.GetItemData(iCurSel);
	if(!pReverb)
		return;

	if(bSave)
	{
		int idx = 0;
		std::string strNewName = any_cast<std::string>(m_pProperties->GetPropVal(idx++));
		if(!GF_CheckName(strNewName.c_str()))
		{
			m_pProperties->SetPropVal(0, pReverb->strName);
			return;
		}
		pReverb->prop.Room = (int)(any_cast<float>(m_pProperties->GetPropVal(idx++))*100.0f);
		pReverb->prop.DecayTime = any_cast<int>(m_pProperties->GetPropVal(idx++))/1000.0f;
		pReverb->prop.DecayHFRatio = any_cast<float>(m_pProperties->GetPropVal(idx++));
		pReverb->prop.ReflectionsDelay = any_cast<int>(m_pProperties->GetPropVal(idx++))/1000.0f;
		pReverb->prop.ReverbDelay = any_cast<int>(m_pProperties->GetPropVal(idx++))/1000.0f;
		pReverb->prop.Reflections = (int)(any_cast<float>(m_pProperties->GetPropVal(idx++))*100.0f);
		pReverb->prop.Reverb = (int)(any_cast<float>(m_pProperties->GetPropVal(idx++))*100.0f);
		pReverb->prop.Diffusion = any_cast<float>(m_pProperties->GetPropVal(idx++));
		pReverb->prop.Density = any_cast<float>(m_pProperties->GetPropVal(idx++));
		pReverb->prop.RoomHF = (int)(any_cast<float>(m_pProperties->GetPropVal(idx++))*100.0f);
		pReverb->prop.RoomLF = (int)(any_cast<float>(m_pProperties->GetPropVal(idx++))*100.0f);
		pReverb->prop.HFReference = (float)any_cast<int>(m_pProperties->GetPropVal(idx++));
		pReverb->prop.LFReference = (float)any_cast<int>(m_pProperties->GetPropVal(idx++));
	}
	else
	{
		int idx = 0;
		m_pProperties->SetPropVal(idx++, pReverb->strName);
		m_pProperties->SetPropVal(idx++, pReverb->prop.Room/100.0f);
		m_pProperties->SetPropVal(idx++, (int)(pReverb->prop.DecayTime*1000));
		m_pProperties->SetPropVal(idx++, pReverb->prop.DecayHFRatio);
		m_pProperties->SetPropVal(idx++, (int)(pReverb->prop.ReflectionsDelay*1000));
		m_pProperties->SetPropVal(idx++, (int)(pReverb->prop.ReverbDelay*1000));
		m_pProperties->SetPropVal(idx++, pReverb->prop.Reflections/100.0f);
		m_pProperties->SetPropVal(idx++, pReverb->prop.Reverb/100.0f);
		m_pProperties->SetPropVal(idx++, pReverb->prop.Diffusion);
		m_pProperties->SetPropVal(idx++, pReverb->prop.Density);
		m_pProperties->SetPropVal(idx++, pReverb->prop.RoomHF/100.0f);
		m_pProperties->SetPropVal(idx++, pReverb->prop.RoomLF/100.0f);
		m_pProperties->SetPropVal(idx++, (int)pReverb->prop.HFReference);
		m_pProperties->SetPropVal(idx++, (int)pReverb->prop.LFReference);
	}

	m_bModified = true;
	GetDlgItem(IDC_REVERB_SAVE)->EnableWindow(TRUE);
}

void CDlgReverb::buildProperty()
{
	m_pProperties->Clear();

	int iCurSel = m_wndListBox.GetCurSel();
	if(iCurSel == -1)
		return;

	REVERB* pReverb = (REVERB*)m_wndListBox.GetItemData(iCurSel);
	if(!pReverb)
		return;

	m_pProperties->AddProperty(pReverb->strName, "Name");

	s_rangeRoom.SetRange(-100.0f, 0.0f);
	s_rangeRoom.SetStep(0.01);
	s_rangeRoom.SetUnit("dB");
	s_rangeRoom.SetTipFunc(GF_VolumeTip);
	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.Room/100.0f;
		pi.strName = "Room";
		pi.strPath = "单位dB\nroom effect level (at mid frequencies)";
		pi.pRange = &s_rangeRoom;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeDecayTime.SetRange(100, 20000);
	s_rangeDecayTime.SetStep(1);
	s_rangeDecayTime.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = (int)(pReverb->prop.DecayTime*1000);
		pi.strName = "Decay Time";
		pi.strPath = "单位ms\nreverberation decay time at mid frequencies";
		pi.pRange = &s_rangeDecayTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeDecayRatio.SetRange(0.1f, 2.0f);
	s_rangeDecayRatio.SetStep(0.01);
	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.DecayHFRatio;
		pi.strName = "Decay Ratio";
		pi.strPath = "high-frequency to mid-frequency decay time ratio";
		pi.pRange = &s_rangeDecayRatio;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangePreDelay.SetRange(0, 300);
	s_rangePreDelay.SetStep(1);
	s_rangePreDelay.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = (int)(pReverb->prop.ReflectionsDelay*1000);
		pi.strName = "Pre Delay";
		pi.strPath = "单位ms\ninitial reflection delay time";
		pi.pRange = &s_rangePreDelay;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeLateDelay.SetRange(0, 100);
	s_rangeLateDelay.SetStep(1);
	s_rangeLateDelay.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = (int)(pReverb->prop.ReverbDelay*1000);
		pi.strName = "Late Delay";
		pi.strPath = "单位ms\nlate reverberation delay time relative to initial reflection";
		pi.pRange = &s_rangeLateDelay;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeEarlyRef.SetRange(-100.0f, 10.0f);
	s_rangeEarlyRef.SetStep(0.01);
	s_rangeEarlyRef.SetUnit("dB");
	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.Reflections/100.0f;
		pi.strName = "Early Reflections";
		pi.strPath = "单位dB\nearly reflections level relative to room effect";
		pi.pRange = &s_rangeEarlyRef;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}
	
	s_rangeLateRef.SetRange(-100.0f, 20.0f);
	s_rangeLateRef.SetStep(0.01);
	s_rangeLateRef.SetUnit("dB");
	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.Reverb/100.0f;
		pi.strName = "Late Reflections";
		pi.strPath = "单位dB\nlate reverberation level relative to room effect";
		pi.pRange = &s_rangeLateRef;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangePercent.SetRange(0.0f, 100.0f);
	s_rangePercent.SetStep(0.01);
	s_rangePercent.SetUnit("%");
	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.Diffusion;
		pi.strName = "Diffusion";
		pi.strPath = "百分比\nValue that controls the echo density in the late reverberation decay";
		pi.pRange = &s_rangePercent;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.Density;
		pi.strName = "Density";
		pi.strPath = "百分比\nValue that controls the modal density in the late reverberation decay";
		pi.pRange = &s_rangePercent;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.RoomHF/100.0f;
		pi.strName = "RoomHF";
		pi.strPath = "单位dB\nrelative room effect level at high frequencies";
		pi.pRange = &s_rangeRoom;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = pReverb->prop.RoomLF/100.0f;
		pi.strName = "RoomLF";
		pi.strPath = "单位dB\nrelative room effect level at low frequencies";
		pi.pRange = &s_rangeRoom;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeHFCrossover.SetRange(1000, 20000);
	s_rangeHFCrossover.SetStep(1);
	s_rangeHFCrossover.SetUnit("Hz");
	{
		PROP_ITEM pi;
		pi.Val = (int)pReverb->prop.HFReference;
		pi.strName = "HF Crossover";
		pi.strPath = "单位Hz\nreference high frequency (hz)";
		pi.pRange = &s_rangeHFCrossover;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}
	
	s_rangeLFCrossover.SetRange(20, 1000);
	s_rangeLFCrossover.SetStep(1);
	s_rangeLFCrossover.SetUnit("Hz");
	{
		PROP_ITEM pi;
		pi.Val = (int)pReverb->prop.LFReference;
		pi.strName = "LF Crossover";
		pi.strPath = "单位Hz\reference low frequency (hz)";
		pi.pRange = &s_rangeLFCrossover;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	m_pBCGPPropListWrapper->AttachTo(m_pProperties);
}

void CDlgReverb::OnLbnSelchangeListReverb()
{
	buildProperty();
}
void CDlgReverb::OnBnClickedNewReverb()
{
	ReverbDataManager* pReverbDataManager = Engine::GetInstance().GetEventSystem()->GetReverbDataManager();
	if(!pReverbDataManager)
		return;

	CDlgNewReverb dlg;
	if(IDOK!=dlg.DoModal())
		return;
	REVERB* pReverbTemplate = dlg.GetSelectReverb();
	REVERB* pNewReverb = pReverbDataManager->CreateReverb(dlg.GetName());
	if(!pNewReverb)
		return;
	pNewReverb->prop = pReverbTemplate->prop;
	updateListBox();
	m_bModified = true;
	GetDlgItem(IDC_REVERB_SAVE)->EnableWindow(TRUE);
}

void CDlgReverb::OnBnClickedDelReberb()
{
	int iCurSel = m_wndListBox.GetCurSel();
	if(iCurSel == -1)
		return;	
	REVERB* pReverb = (REVERB*)m_wndListBox.GetItemData(iCurSel);
	if(!pReverb)
		return;

	CString csTips;
	csTips.Format("确定删除Reverb：“%s”吗？", pReverb->strName.c_str());
	if(IDYES != MessageBox(csTips, "删除Reverb", MB_ICONINFORMATION | MB_YESNO))
		return;

	ReverbDataManager* pReverbDataManager = Engine::GetInstance().GetEventSystem()->GetReverbDataManager();
	if(!pReverbDataManager)
		return;
	if(!pReverbDataManager->DeleteReverb(pReverb))
		return;
	updateListBox();
	m_bModified = true;
	GetDlgItem(IDC_REVERB_SAVE)->EnableWindow(TRUE);
}

void CDlgReverb::OnBnClickedReverbSave()
{
	if(!save())
		return;
	m_bModified = false;
	GetDlgItem(IDC_REVERB_SAVE)->EnableWindow(FALSE);
}

void CDlgReverb::OnBnClickedOk()
{
	if(m_bModified)
	{		
		if(IDYES == MessageBox("已修改，是否保存？", "保存", MB_ICONINFORMATION | MB_YESNO))
			OnBnClickedReverbSave();
	}
	CBCGPDialog::OnOK();
}

bool CDlgReverb::save()
{
	ReverbDataManager* pReverbDataManager = Engine::GetInstance().GetEventSystem()->GetReverbDataManager();
	if(!pReverbDataManager)
		return false;
	char szReverbPath[MAX_PATH] = {0};
	sprintf(szReverbPath, "%s\\Reverbs.xml", g_Configs.szReverbsPath);
	if(!pReverbDataManager->SaveXML(szReverbPath))
	{
		GF_Log(LOG_ERROR, "CDlgReverb::save Reverbs保存失败。");
		return false;
	}
	return true;
}

bool CDlgReverb::load()
{
	char szReverbPath[MAX_PATH] = {0};
	sprintf(szReverbPath, "%s\\Reverbs.xml", g_Configs.szReverbsPath);
	if(!Engine::GetInstance().GetEventSystem()->InitReverbDataManager(szReverbPath))
	{
		GF_Log(LOG_ERROR, "CDlgReverb::load 初始化ReverbDataManager失败");
		return false;
	}	
	return true;
}