#pragma once

#include "DlgCustomizeBase.h"

class AUISlider;
class AUILabel;

class CDlgCustomizeStature : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	// ddx control
	AUISlider* m_pSliderHeadScale;			//头大小
	AUISlider* m_pSliderUpScale;			//上身健壮度
	AUISlider* m_pSliderWaistScale;			//腰部为度
	AUISlider* m_pSliderArmWidth;			//下肢围度
	AUISlider* m_pSliderLegWidth;			//下肢围度

	AUISlider* m_pSliderBreastScale;		//胸部高低
	AUILabel* m_pLabelBreastScale;			//胸部高低标签


protected:
	
	void DoCalculate();
	bool InitResource();
	void SetSliderBound();
	
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();

public:
	virtual AUIStillImageButton * GetSwithButton();

	void LoadDefault();
	void OnCommandMoveSlider(const char * szCommand);
	void OnCommandReset(const char * szCommand);

	CDlgCustomizeStature();
	virtual ~CDlgCustomizeStature();
};

