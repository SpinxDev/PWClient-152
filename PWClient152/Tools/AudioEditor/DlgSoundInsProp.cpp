// DlgSoundInsProp.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSoundInsProp.h"
#include "FCurve.h"

#include "PropListCtrl/BCGPPropertyListWrapper.h"
#include "PropListCtrl/BCGPPropertyObject.h"

static prop_range_template<float> s_rangeStart;
static prop_range_template<float> s_rangeLength;

static prop_set_template<int> s_setStartMode;
static prop_set_template<int> s_setStopMode;

static prop_range_template<int> s_rangeLoopCount;
static prop_set_template<int> s_setPlayNum;
static prop_range_template<int> s_rangeFadeTime;
static prop_set_template<int> s_setFadeInType;
static prop_set_template<int> s_setFadeOutType;
static prop_range_template<int> s_rangeIntervalTime;

using AudioEngine::START_MODE;
using AudioEngine::STOP_MODE;
using AudioEngine::PLAY_NUM;

enum
{
	idStartPosition,
	idLength,
	idStartMode,
	idStopMode,
	idLoopCount,
	idPlayNum,
	idFadeInTime,
	idFadeInCurveType,
	idFadeOutTime,
	idFadeOutCurveType,
	idIntervalTime,
	idIntervalTimeRandomization,
	idNote
};


// CDlgSoundInsProp 对话框

IMPLEMENT_DYNAMIC(CDlgSoundInsProp, CBCGPDialog)

CDlgSoundInsProp::CDlgSoundInsProp(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSoundInsProp::IDD, pParent)
{
	m_pBCGPPropListWrapper = new CBCGPPropertyListWrapper;
	m_pPropertyListener = new CPropertyUpdateListener(this);
	m_pProperties = new CPropertyArray;
	m_pProperties->AddListener(m_pPropertyListener);
}

CDlgSoundInsProp::~CDlgSoundInsProp()
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

void CDlgSoundInsProp::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSoundInsProp, CBCGPDialog)
END_MESSAGE_MAP()


// CDlgSoundInsProp 消息处理程序

BOOL CDlgSoundInsProp::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();

	CRect rc;
	GetDlgItem(IDC_PROP_FRM)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	if (!m_pBCGPPropListWrapper->OnCreate(rc, this))
		return FALSE;
	buildProperty();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSoundInsProp::SetProp(const SOUND_INSTANCE_PROPERTY& prop, float fMinValue, float fMaxValue, const char* szNote)
{
	m_Prop = prop;
	m_fMinValue = fMinValue;
	m_fMaxValue = fMaxValue;
	m_strNote = szNote;
}

void CDlgSoundInsProp::buildProperty()
{
	m_pProperties->Clear();

	s_rangeStart.SetRange(m_fMinValue, m_fMaxValue);
	s_rangeStart.SetStep(0.01);
	{
		PROP_ITEM pi;
		pi.Val = m_Prop.fStartPosition;
		pi.strName = "开始点";
		pi.strDesc = "";
		pi.pRange = &s_rangeStart;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	s_rangeLength.SetRange(0.0f, m_fMaxValue-m_fMinValue);
	s_rangeLength.SetStep(0.01);
	{
		PROP_ITEM pi;
		pi.Val = m_Prop.fLength;
		pi.strName = "长度";
		pi.strDesc = "";
		pi.pRange = &s_rangeLength;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}	

	if(0 == s_setStartMode.GetCount())
	{
		s_setStartMode.AddElement((int)AudioEngine::START_MODE_IMMEDIATE, "立即开始");
		s_setStartMode.AddElement((int)AudioEngine::START_MODE_WAIT_FOR_PREVIOUS, "等待上一个播完");
	}
	{
		PROP_ITEM pi;
		pi.Val = (int)m_Prop.startMode;
		pi.strName = "开始模式";
		pi.strDesc = "触发播放命令时的行为方式";
		pi.pOptions = &s_setStartMode;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	if(0 == s_setStopMode.GetCount())
	{
		s_setStopMode.AddElement((int)AudioEngine::STOP_MODE_CUTOFF, "立即停止");
		s_setStopMode.AddElement((int)AudioEngine::STOP_MODE_PLAYTOEND, "播放完当前文件");
	}
	{
		PROP_ITEM pi;
		pi.Val = (int)m_Prop.stopMode;
		pi.strName = "停止模式";
		pi.strDesc = "触发停止命令时的行为方式";
		pi.pOptions = &s_setStopMode;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	s_rangeLoopCount.SetRange(0, 100);
	s_rangeLoopCount.SetStep(1);
	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iLoopCount;
		pi.strName = "循环次数";
		pi.strDesc = "0为无限循环";
		pi.pRange = &s_rangeLoopCount;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}
	
	if(0 == s_setPlayNum.GetCount())
	{
		s_setPlayNum.AddElement((int)AudioEngine::PLAY_NUM_ONE_AT_A_TIME, "一次播放一个");
		s_setPlayNum.AddElement((int)AudioEngine::PLAY_NUM_ALL, "播放全部");
	}
	{
		PROP_ITEM pi;
		pi.Val = (int)m_Prop.playNum;
		pi.strName = "播放数量";
		pi.strDesc = "";
		pi.pOptions = &s_setPlayNum;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}	

	s_rangeFadeTime.SetRange(0, 60000);
	s_rangeFadeTime.SetStep(1);
	s_rangeFadeTime.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iFadeInTime;
		pi.strName = "淡入时间";
		pi.strDesc = "单位毫秒";
		pi.pRange = &s_rangeFadeTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	if(0 == s_setFadeInType.GetCount())
	{
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_LINEAR, "线性");
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_SIN, "S形上升");
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_COS, "凸形上升");
		s_setFadeInType.AddElement((int)AudioEngine::CURVE_POW, "凹形上升");
	}
	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iFadeInCurveType;
		pi.strName = "淡入曲线";
		pi.strDesc = "淡入曲线形状";
		pi.pOptions = &s_setFadeInType;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iFadeOutTime;
		pi.strName = "淡出时间";
		pi.strDesc = "单位毫秒";
		pi.pRange = &s_rangeFadeTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	if(0 == s_setFadeOutType.GetCount())
	{
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_LINEAR, "线性");
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_SIN, "S形下降");
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_COS, "凸形下降");
		s_setFadeOutType.AddElement((int)AudioEngine::CURVE_POW, "凹形下降");
	}
	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iFadeOutCurveType;
		pi.strName = "淡出曲线";
		pi.strDesc = "淡出曲线形状";
		pi.pOptions = &s_setFadeOutType;
		pi.iWay = BCGP_PROP_WAY_OPTIONS;
		m_pProperties->AddProperty(pi);
	}

	s_rangeIntervalTime.SetRange(0, 1200000);
	s_rangeIntervalTime.SetStep(1);
	s_rangeIntervalTime.SetUnit("ms");
	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iPreIntervalTime;
		pi.strName = "前置时间间隔";
		pi.strDesc = "单位毫秒\n第一次播放时有效，循环播放时不与后置时间间隔叠加";
		pi.pRange = &s_rangeIntervalTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iPreIntervalTimeRandomization;
		pi.strName = "前置时间间隔随机";
		pi.strDesc = "单位毫秒\n第一次播放时有效，循环播放时不与后置时间间隔叠加";
		pi.pRange = &s_rangeIntervalTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iIntervalTime;
		pi.strName = "后置时间间隔";
		pi.strDesc = "单位毫秒";
		pi.pRange = &s_rangeIntervalTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	{
		PROP_ITEM pi;
		pi.Val = m_Prop.iIntervalTimeRandomization;
		pi.strName = "后置时间间隔随机";
		pi.strDesc = "单位毫秒";
		pi.pRange = &s_rangeIntervalTime;
		pi.iWay = BCGP_PROP_WAY_RANGE;
		m_pProperties->AddProperty(pi);
	}

	m_pProperties->AddProperty(m_strNote, "标签");
	m_pBCGPPropListWrapper->AttachTo(m_pProperties);
}

void CDlgSoundInsProp::UpdateProperty(bool bSave, bool bIsCommit /*= false*/)
{
	if(bSave)
	{
		int idx = 0;
		m_Prop.fStartPosition = any_cast<float>(m_pProperties->GetPropVal(idx++));
		m_Prop.fLength = any_cast<float>(m_pProperties->GetPropVal(idx++));
		m_Prop.startMode = (START_MODE)any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.stopMode = (STOP_MODE)any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iLoopCount = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.playNum = (PLAY_NUM)any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iFadeInTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iFadeInCurveType = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iFadeOutTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iFadeOutCurveType = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iPreIntervalTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iPreIntervalTimeRandomization = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iIntervalTime = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_Prop.iIntervalTimeRandomization = any_cast<int>(m_pProperties->GetPropVal(idx++));
		m_strNote = any_cast<std::string>(m_pProperties->GetPropVal(idx++));
	}
	else
	{
		int idx = 0;
		m_pProperties->SetPropVal(idx++, m_Prop.fStartPosition);
		m_pProperties->SetPropVal(idx++, m_Prop.fLength);
		m_pProperties->SetPropVal(idx++, (int)m_Prop.startMode);
		m_pProperties->SetPropVal(idx++, (int)m_Prop.stopMode);
		m_pProperties->SetPropVal(idx++, m_Prop.iLoopCount);
		m_pProperties->SetPropVal(idx++, (int)m_Prop.playNum);
		m_pProperties->SetPropVal(idx++, m_Prop.iFadeInTime);
		m_pProperties->SetPropVal(idx++, m_Prop.iFadeInCurveType);
		m_pProperties->SetPropVal(idx++, m_Prop.iFadeOutTime);
		m_pProperties->SetPropVal(idx++, m_Prop.iFadeOutCurveType);
		m_pProperties->SetPropVal(idx++, m_Prop.iPreIntervalTime);
		m_pProperties->SetPropVal(idx++, m_Prop.iPreIntervalTimeRandomization);
		m_pProperties->SetPropVal(idx++, m_Prop.iIntervalTime);
		m_pProperties->SetPropVal(idx++, m_Prop.iIntervalTimeRandomization);
		m_pProperties->SetPropVal(idx++, m_strNote);
	}	
}
