#ifndef _FWDIALOGTEXT_H_
#define _FWDIALOGTEXT_H_

#include "FWDialogGraph.h"

class AUIComboBox;
class AUIRadioButton;
class AUIListBox;
class FWArt;


class FWDialogText : public FWDialogGraph  
{
	FW_DECLARE_DYNCREATE(FWDialogText)
	DECLARE_EVENT_MAP_FW()
protected:
	AUIEditBox * m_pEditText;
	AUIComboBox * m_pCmbFont;
	AUIComboBox * m_pCmbPoint;
	AUIRadioButton * m_ppRadStyle[4];
	AUIListBox * m_pListArt;
	FWArt * m_pArt;
	int m_nLastSelIndexArt;
protected:
	virtual void OnShowDialog();

	void InitArtList();
	void SelectInPointCombo();
	void SelectInFontCombo();
	int Index2Style(int nIndex);
	int Style2Index(int nStyle);
	void InitPointCombo();
	void InitFontCombo();
	void OnLButtonDownListArt(WPARAM wParam, LPARAM lParam);
	void SelectInArtList();
	void OnSelChangeListArt(int nSelIndex);
	static void OnDoModalArtCallback(void *pData);
public:
	ACString m_strText;
	ACString m_strFont;
	int m_nPointSize;
	int m_nFontStyle;
public:
	void OnClickBtnArt(WPARAM wParam, LPARAM lParam);

	void SetArt(const FWArt *pArt);
	const FWArt * GetArt();
	
	FWDialogText();
	virtual ~FWDialogText();

	virtual bool Init(AUIDialog * pDlg);
	virtual void OnOK();
};

#endif 