#pragma once

#include "DlgCustomizeBase.h"

class AUISlider;
class AUILabel;

class CDlgCustomizeStature : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	// ddx control
	AUISlider* m_pSliderHeadScale;			//ͷ��С
	AUISlider* m_pSliderUpScale;			//����׳��
	AUISlider* m_pSliderWaistScale;			//����Ϊ��
	AUISlider* m_pSliderArmWidth;			//��֫Χ��
	AUISlider* m_pSliderLegWidth;			//��֫Χ��

	AUISlider* m_pSliderBreastScale;		//�ز��ߵ�
	AUILabel* m_pLabelBreastScale;			//�ز��ߵͱ�ǩ


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

