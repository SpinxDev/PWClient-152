#pragma once

#include "DlgCustomizeBase.h"
#include "AUI_ImageGrid.h"

class AUIListBox;
class AUISlider;

class CDlgCustomizeNoseMouth : public CDlgCustomizeBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
protected:
	// ddx control
	AUI_ImageGrid m_ListBoxNoseTex;//��������list box
	AUI_ImageGrid m_ListBoxNoseTip;//��ͷ��״list box
	AUI_ImageGrid m_ListBoxNoseBridge;//������״list box
	
	AUI_ImageGrid m_ListBoxUpLipLine;//�ϴ���list box
	AUI_ImageGrid m_ListBoxMidLipLine;//����list box
	AUI_ImageGrid m_ListBoxDownLipLine;//�´���list box	

	typedef abase::vector<A2DSprite*>::iterator VecA2DSpriteIter_t;
	abase::vector<A2DSprite*> m_vecA2DSprite;

protected:	
	void DoCalculate();
	bool InitResource();
	void ResetNoseTip();
	void ResetNoseBridge();

	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual AUIStillImageButton * GetSwithButton();
	virtual void OnShowDialog();
	void ClearListCtrlAndA2DSprite();
	virtual void OnTick();
	
	void ResetUpLip();
	void ResetMidLip();
	void ResetDownLip();

public:
	void OnLButtonUpListboxNose(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void OnMouseWheelListboxNose(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	CDlgCustomizeNoseMouth();
	virtual ~CDlgCustomizeNoseMouth();
};

class AUICheckBox;

class CDlgCustomizeNoseMouth2 : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
protected:
	//DDX data
	bool m_bMouthScaleHSync;//��ˮƽ����ͬ��
	bool m_bMouthOffsetCornerSync;//���λ��ͬ��
	
	//DDX control
	AUISlider* m_pSliderThickUpLip; //���촽��ȵ���
	AUISlider* m_pSliderThickDownLip;//���촽��ȵ���
	AUISlider* m_pSliderScaleMouthH;//�첿����ˮƽ��������
	AUISlider* m_pSliderOffsetMouthV;//�첿���崹ֱλ�Ƶ���
	AUISlider* m_pSliderOffsetMouthZ;//�첿������Z���λ�Ƶ���
	AUISlider* m_pSliderOffsetCornerOfMouthSpecial;//��ǵĵ���
	AUISlider* m_pSliderScaleMouthH2;
	AUISlider* m_pSliderOffsetCornerOfMouthSpecial2;
	
	AUICheckBox* m_pCheckBoxScaleMouthH;
	AUICheckBox* m_pCheckBoxOffsetCorner;	
	
	AUISlider* m_pSliderScaleNoseTipH;//��ͷˮƽ��������
	AUISlider* m_pSliderScaleNoseTipV;//��ͷ��ֱ��������
	AUISlider* m_pSliderScaleNoseTipZ;//��ͷǰ���������
	AUISlider* m_pSliderOffsetNoseTipV;//��ͷ��ֱλ�Ƶ���
	AUISlider* m_pSliderScaleBridgeTipH;//����ˮƽ��������
	AUISlider* m_pSliderOffsetBridgeTipZ;//����ǰ���ƶ�����
	
protected:
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	
	void DoCalculate();
	bool InitResource();
	void SetSliderBound();	
	
	virtual void OnShowDialog();
	
public:
	CDlgCustomizeNoseMouth2();
	virtual AUIStillImageButton * GetSwithButton();
	
	void LoadMouthDefault();
	void LoadNoseDefault();
	void LoadDefault();
	
	void OnCommandMoveSlider(const char* szCommand);
	void OnCommandCheckSync(const char *szCommand);
	void OnCommandMoveScaleMouthHSlider(const char *szCommand);
	void OnCommandMoveScaleMouthH2Slider(const char *szCommand);
	void OnCommandMoveCornerOfMouthSlider(const char *szCommand);
	void OnCommandMoveCornerOfMouth2Slider(const char *szCommand);	
	void OnCommandReset(const char * szCommand);
	
};
