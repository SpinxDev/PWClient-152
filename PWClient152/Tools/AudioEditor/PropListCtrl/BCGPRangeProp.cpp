#include "stdafx.h"
#include "BCGPRangeProp.h"
#include "DlgSlider.h"
#include "BCGPPropertyListWrapper.h"
#include "BCGPPropListEx.h"
#include "BCGPPropertyObject.h"
#include "AnyType.h"

class SliderListener : public CDlgSlider::Listener
{
public:
	SliderListener(CBCGPRangeProp* pRangeProp)
		: m_pRangeProp(pRangeProp) {}

	void OnCommitChange(int iMin, int iMax, int iCur) ;
	void OnSliderPosChanged(int iMin, int iMax, int iCur) ;
	void OnGetValue(BOOL& bIsInteger, double& val_min, double& val_max, double& val_cur) const;
private:
	void updatePropChanged(int iCur, bool bIsCommit) const;
private:
	CBCGPRangeProp* m_pRangeProp;
};

void SliderListener::OnGetValue(BOOL& bIsInteger, double& val_min, double& val_max, double& val_cur) const
{
	bIsInteger = m_pRangeProp->m_bIsInteger;
	if (bIsInteger) {
		val_min = (int)m_pRangeProp->GetMin();
		val_max = (int)m_pRangeProp->GetMax();
		val_cur = (int)m_pRangeProp->GetValue();
	}
	else {
		val_min = m_pRangeProp->GetMin();
		val_max = m_pRangeProp->GetMax();
		val_cur = m_pRangeProp->GetValue();
	}
}

void SliderListener::updatePropChanged(int iCur, bool bIsCommit) const
{
	_variant_t varOld = m_pRangeProp->GetValue();
	m_pRangeProp->SetValueByCurStep(iCur);
	if (!bIsCommit) {
		if (CBCGPPropListEx* pListEx = dynamic_cast<CBCGPPropListEx*>(m_pRangeProp->m_pWndList)) {
			pListEx->OnPropertyChangedNoCommit(m_pRangeProp);
		}
	}
	else if (varOld != m_pRangeProp->GetValue())
		m_pRangeProp->m_pWndList->OnPropertyChanged(m_pRangeProp);
	
}

void SliderListener::OnCommitChange(int iMin, int iMax, int iCur) 
{
	updatePropChanged(iCur, true);
}

void SliderListener::OnSliderPosChanged(int iMin, int iMax, int iCur) 
{
	updatePropChanged(iCur, false);
}

BOOL is_variant_integer(const _variant_t& val)
{
	switch(val.vt)
	{
	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_I8:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_UI8:
	case VT_INT:
	case VT_UINT:
		return TRUE;
	case VT_R4:
	case VT_R8:
		return FALSE;
	default:
	    throw bcgp_bad_prop("acquire for integer / real type.");
	}
}

CBCGPRangeProp::CBCGPRangeProp(const CString& strName
							   , const _variant_t& varValue
							   , const prop_range* pRange
							   , LPCTSTR lpszDescr /* = NULL */
							   , DWORD_PTR dwData /* = 0 */)
							   : CBCGPPropNoRedraw(strName
							   , varValue
							   , lpszDescr
							   , dwData)
							   , m_pRange(pRange)
							   , m_pPopup(NULL)
							   , m_pSliderDlg(new CDlgSlider)
							   , m_pSliderListener(new SliderListener(this))
							   , m_fStep(pRange->GetStep())

{
	try {
		m_bIsInteger = is_variant_integer(m_varValue);
	}
	catch (std::exception& e) {
		AfxMessageBox(e.what());
		throw;
	}

	if (m_bIsInteger) 
	{
		m_varValue = (int)varValue;
	}
	else 
	{
		m_varValue = (double)varValue;		
	}

	UpdateRange();
	m_dwFlags = PROP_HAS_BUTTON;
}

CBCGPRangeProp::~CBCGPRangeProp()
{
	delete m_pSliderListener;
	m_pSliderListener = NULL;
	delete m_pSliderDlg;
	m_pSliderDlg = NULL;
}

void CBCGPRangeProp::SetValueByCurStep(int nStep)
{
	if (nStep < 0) return;
	double valCur = nStep * m_fStep;
	_variant_t val;
	if (m_bIsInteger)
	{
		int iStart = m_varMin;
		val = static_cast<int>(iStart + valCur);
	}
	else
	{
		double fStart = m_varMin;
		val = fStart + valCur;
	}
	SetValue(val);
}

void CBCGPRangeProp::SetValue (const _variant_t& varValue)
{
	if (m_bIsInteger) {
		CBCGPPropNoRedraw::SetValue((int)varValue);
	}
	else
		CBCGPPropNoRedraw::SetValue((double)varValue);
	Redraw();
}

void CBCGPRangeProp::OnClickButton(CPoint point)
{
	UpdateRange();

	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);

	m_bButtonIsDown = TRUE;
	Redraw ();

	CRect rcMainFrm;
	AfxGetMainWnd()->GetWindowRect(&rcMainFrm);
	CPoint pt (rcMainFrm.left + rcMainFrm.Width()/2, rcMainFrm.top + rcMainFrm.Height()/2);
	
	int nSliderCur = 0;
	int nSliderMin = 0;
	int nSliderMax = 0;
	if (m_bIsInteger) {
		int nMin = m_varMin;
		int nMax = m_varMax;
		int nCur = m_varValue;
		nSliderCur = static_cast<int>((nCur - nMin) / m_fStep);
		nSliderMax = static_cast<int>((nMax - nMin) / m_fStep);
	}
	else {
		double fMin = m_varMin;
		double fMax = m_varMax;
		double fCur = m_varValue;
		nSliderCur = static_cast<int>((fCur - fMin) / m_fStep);
		nSliderMax = static_cast<int>((fMax - fMin) / m_fStep);
	}

	m_pSliderDlg->SetRange(nSliderMin, nSliderMax, nSliderCur, m_pRange->GetStep());
	m_pSliderDlg->SetListener(m_pSliderListener);
	m_pSliderDlg->SetUnit(m_pRange->GetUnit());
	m_pSliderDlg->SetTipFunc(m_pRange->GetTipFunc());
	m_pSliderDlg->SetTitle(m_strName);
	if (!m_pSliderDlg->GetSafeHwnd() && !m_pSliderDlg->Create(IDD_SLIDER, m_pWndList))
		return;
	
	CRect sliderDlgRc;
	m_pSliderDlg->GetWindowRect(&sliderDlgRc);
	pt.x -= sliderDlgRc.Width()/2;
	pt.y -= sliderDlgRc.Height()/2;
	sliderDlgRc.MoveToXY(pt);
	m_pSliderDlg->MoveWindow(&sliderDlgRc);
	m_pSliderDlg->ShowWindow(SW_SHOW);
}

void CBCGPRangeProp::OnKillSelection (CBCGPProp* /*pNewSel*/) 
{
	if (!IsInPlaceEditing () && m_pSliderDlg->GetSafeHwnd())
		m_pSliderDlg->DestroyWindow();
}

void CBCGPRangeProp::UpdateRange()
{		
	// cast the type to int / double
	if (m_bIsInteger) 
	{
		m_varMin = (int)Any2VT(m_pRange->GetMin());
		m_varMax = (int)Any2VT(m_pRange->GetMax());
	}
	else 
	{
		m_varMin = (double)Any2VT(m_pRange->GetMin());
		m_varMax = (double)Any2VT(m_pRange->GetMax());
	}
	m_fStep = m_pRange->GetStep();
}

BOOL CBCGPRangeProp::OnEndEdit ()
{
	if (m_bIsInteger) {
		int nMin = m_varMin;
		int nMax = m_varMax;
		int nCur = m_varValue;
		nCur = GF_Clamp(nMin, nMax, nCur);
		m_varValue = nCur;
	}
	else {
		double fMin = m_varMin;
		double fMax = m_varMax;
		double fCur = m_varValue;
		fCur = GF_Clamp(fMin, fMax, fCur);
		m_varValue = fCur;
	}
	return CBCGPPropNoRedraw::OnEndEdit();
}