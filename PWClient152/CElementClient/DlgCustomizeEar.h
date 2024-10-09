#pragma once

#include "DlgCustomizeBase.h"
#include "AUI_ImageGrid.h"


class AUISlider;
class AUIListBox;

class CDlgCustomizeEar : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:
	// ddx control

	typedef abase::vector<A2DSprite*>::iterator VecA2DSpriteIter_t;
	abase::vector<A2DSprite*> m_vecA2DSprite;
	
protected:
	
	void DoCalculate();
	bool InitResource();
	void SetSliderBound();
	void ResetEarToNormal(void);

	
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual AUIStillImageButton * GetSwithButton();
	virtual void OnShowDialog();
	virtual void OnTick();

	void ClearListCtrlAndA2DSprite();

public:
	void LoadDefault();
	void OnCommandReset(const char * szCommand);
	void OnCommandMoveSlider(const char * szCommand);
	void OnCommandChooseEarShape(const char *pCommand);

	void OnLButtonUpListboxEarShape(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void OnMouseWheelListboxEarShape(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	CDlgCustomizeEar();
	virtual ~CDlgCustomizeEar();
	

};

