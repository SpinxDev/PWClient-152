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
	AUI_ImageGrid m_ListBoxNoseTex;//鼻子纹理list box
	AUI_ImageGrid m_ListBoxNoseTip;//鼻头形状list box
	AUI_ImageGrid m_ListBoxNoseBridge;//鼻梁形状list box
	
	AUI_ImageGrid m_ListBoxUpLipLine;//上唇线list box
	AUI_ImageGrid m_ListBoxMidLipLine;//唇沟list box
	AUI_ImageGrid m_ListBoxDownLipLine;//下唇线list box	

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
	bool m_bMouthScaleHSync;//嘴水平比例同步
	bool m_bMouthOffsetCornerSync;//嘴角位移同步
	
	//DDX control
	AUISlider* m_pSliderThickUpLip; //上嘴唇厚度调节
	AUISlider* m_pSliderThickDownLip;//下嘴唇厚度调节
	AUISlider* m_pSliderScaleMouthH;//嘴部整体水平比例调节
	AUISlider* m_pSliderOffsetMouthV;//嘴部整体垂直位移调节
	AUISlider* m_pSliderOffsetMouthZ;//嘴部整体沿Z轴的位移调节
	AUISlider* m_pSliderOffsetCornerOfMouthSpecial;//嘴角的调节
	AUISlider* m_pSliderScaleMouthH2;
	AUISlider* m_pSliderOffsetCornerOfMouthSpecial2;
	
	AUICheckBox* m_pCheckBoxScaleMouthH;
	AUICheckBox* m_pCheckBoxOffsetCorner;	
	
	AUISlider* m_pSliderScaleNoseTipH;//鼻头水平比例调节
	AUISlider* m_pSliderScaleNoseTipV;//鼻头垂直比例调节
	AUISlider* m_pSliderScaleNoseTipZ;//鼻头前后比例调节
	AUISlider* m_pSliderOffsetNoseTipV;//鼻头垂直位移调节
	AUISlider* m_pSliderScaleBridgeTipH;//鼻梁水平比例调节
	AUISlider* m_pSliderOffsetBridgeTipZ;//鼻梁前后移动调节
	
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
