#ifndef _BCGP_RANGE_PROP_H_
#define _BCGP_RANGE_PROP_H_

#include "BCGPToolbar.h"
#include "BCGPPropNoRedraw.h"

class prop_range;
class CDlgSlider;
class SliderListener;

class CBCGPRangeProp : public CBCGPPropNoRedraw
{
public:
	friend class SliderListener;

	//	Simple property
	CBCGPRangeProp(const CString& strName, const _variant_t& varValue, const prop_range* pRange,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	~CBCGPRangeProp();

	//	public operations
public:

	_variant_t GetMin() const { return m_varMin; }
	_variant_t GetMax() const { return m_varMax; }
	BOOL IsInteger() const { return m_bIsInteger; }
	void SetValueByCurStep(int nSteps);

	//	Override operations
protected:

	virtual void SetValue (const _variant_t& varValue);
	virtual void OnClickButton(CPoint point);
	virtual void OnKillSelection (CBCGPProp* /*pNewSel*/);
	virtual BOOL OnEndEdit ();

	void UpdateRange();

private:
	
	const prop_range* m_pRange;

	_variant_t m_varMin;
	_variant_t m_varMax;

	BOOL m_bIsInteger;
	double m_fStep;

	CBCGPPopupMenu* m_pPopup;
	CDlgSlider* m_pSliderDlg;
	SliderListener* m_pSliderListener;
};

#endif