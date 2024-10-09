#pragma once

#include "DlgCustomizeBase.h"
#include "AUI_ImageGrid.h"

class AUIImagePicture;
class AUIListBox;
class A3DSurface;
class A2DSprite;
typedef DWORD A3DCOLOR;


class CPaletteControl
{
	AUIImagePicture * m_pPane;
	AUIImagePicture * m_pIndicator;
	AUIObject * m_pDisplay;
	A3DSurface * m_pSurface;
	A3DCOLOR m_color;
	AUIDialog* m_pDlg;
private:
	POINT GetPtFromColor();
	A3DCOLOR GetColorFromPt(POINT pt);
	void SetIndicatorPosition(POINT center);
	POINT ClampPointToPane(POINT &pt);
	void DialogToPane(POINT &pt);
	void PaneToDialog(POINT &pt);
	void UpdateDisplay();
	POINT SurfacePos2PanelPos(POINT& pt);
public:
	CPaletteControl(AUIImagePicture *pPane, AUIImagePicture * pIndicator, AUIObject *pDisplay, AUIDialog* pDlg);
	~CPaletteControl();
	void ResetContext(A3DSurface *pSurface, A2DSprite *pSprite);
	A3DCOLOR GetColor();
	void SetColor(A3DCOLOR color);
	void MoveIndicator(POINT center);
};


class CDlgCustomizePaint : public CDlgCustomizeBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
protected:
	int m_nSelPartIndex;
	CPaletteControl * m_pPallete;

	// ddx control
	AUIImagePicture * m_pPicPane;
	AUIImagePicture * m_pPicIndicator;
	AUIImagePicture * m_pPicDisplay;
	AUIListBox* m_pLbxFacePart;
	AUI_ImageGrid m_ListBoxEyeKohl;

	typedef abase::vector<A2DSprite*>::iterator VecA2DSpriteIter_t;
	abase::vector<A2DSprite*> m_vecA2DSprite;

protected:
	void ResetPaletteControl();
	void LoadDefault();
	void DoCalculate();
	bool InitResource();
	void UpdatePlayer(A3DCOLOR color);
	
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual AUIStillImageButton * GetSwithButton();
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	void ClearListCtrlAndA2DSprite();
	void ResetFaceAllColor();

public:
	void OnMouseMovePicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonDownPicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnLButtonUpListboxFacepart(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonUpListboxEyeKohl(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnKeyDownListboxFacepart(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseWheelListboxEyeKohl(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	void OnCommandReset(const char* szCommand);
	
	CDlgCustomizePaint();
	virtual ~CDlgCustomizePaint();
	virtual bool Render(void);
};

