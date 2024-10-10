#ifndef _DLG_SLIDER_H_
#define _DLG_SLIDER_H_
#include "afxcmn.h"
#include "..\Resource.h"
#include "BCGPSliderCtrl.h"
#include "afxwin.h"

#include "BCGPPropertyObject.h"

// CDlgSlider dialog

class CDlgSlider : public CBCGPDialog
{
public:

	class Listener
	{
	public:
		virtual void OnCommitChange(int iMin, int iMax, int iCur)		= 0;
		virtual void OnSliderPosChanged(int iMin, int iMax, int iCur)	= 0;
		virtual void OnGetValue(BOOL& bIsInteger, double& val_min, double& val_max, double& val_cur) const = 0;
	};


private:
	DECLARE_DYNAMIC(CDlgSlider)

public:
	CDlgSlider(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSlider();

// Dialog Data
	enum { IDD = IDD_SLIDER };

public:
	void SetListener(Listener* pListener) { m_pListener = pListener; }
	BOOL SetRange(int iMin, int iMax, int iCur, double step);
	int	 GetCurPos() {return m_iCur;}
	void SetUnit(const char* szUnit) { m_strUnit = szUnit; }
	void SetTipFunc(TipFunc func) { m_TipFunc = func; }
	void SetTitle(const char* szTitle) { m_strTitle = szTitle; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

private:
	CBCGPSliderCtrl m_Slider;
	CBCGPStatic m_StaticMin;
	CBCGPStatic m_StaticMax;
	CBCGPStatic m_StaticPos;
	CBCGPStatic m_wndStep;
	CBCGPStatic m_wndTip;

	int m_iMin, m_iMax, m_iCur;
	Listener* m_pListener;
	CString m_strTitle;
	CString m_strTip;
	CString m_strUnit;
	TipFunc m_TipFunc;
	double m_step;

protected:
	void updateText();
};


#endif