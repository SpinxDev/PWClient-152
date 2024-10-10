/*
 * FILE: DlgAutoChooseColor.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/12/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#include "DlgHomeBase.h"

#ifndef _DLG_AUTO_CHOOSE_COLOR_H_
#define _DLG_AUTO_CHOOSE_COLOR_H_


class AUIImagePicture;

class CColorControl
{
public:
	CColorControl(AUIImagePicture* pColorPic, AUIImagePicture* pCurPos, AUIImagePicture* pCurColor, AUIDialog * pDlg, A3DSurface* pSurface);
	~CColorControl();

	A3DCOLOR GetColor();
	void SetColor(A3DCOLOR color);
	void MoveCurPos(POINT ptCenter);

protected:
	POINT GetPosFromColorPic();
	void SetCurPos(POINT ptCenter);
	POINT MapToScrPos(POINT pt);
	void UpdateCurColor();
	POINT ClampCurPosToColorPic(POINT& pt);
	A3DCOLOR GetColorFromPos(POINT pt);
protected:

	AUIImagePicture* m_pImagePicColorPic;
	AUIImagePicture* m_pImagePicCurPos;
	AUIImagePicture* m_pImagePicCurColor;
	A3DSurface * m_pSurface;
	A3DCOLOR m_Color;
	AUIDialog* m_pDlg;
};

//-------------------------------------------------------------------------------

class CDlgAutoChooseColor : public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgAutoChooseColor();
	~CDlgAutoChooseColor();
	enum ColorType_e
	{
		CT_NULL,
		CT_DAYSUN,
		CT_DAYENV,
		CT_DAYFOG,
		CT_NIGHTSUN,
		CT_NIGHTENV,
		CT_NIGHTFOG,
		CT_LIGHT,
		CT_WATERFOG
	};
	void InitColorDlg(ColorType_e ct, A3DCOLOR color);

protected:
	virtual bool OnInitContext();
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();

	void OnCommandOK(const char* szCommand);
	void OnCommandCancel(const char* szCommand);
	void OnLButtonDownColorPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseMoveColorPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
protected:

	AUIImagePicture* m_pImagePicColorPic;
	AUIImagePicture* m_pImagePicCurColor;
	AUIImagePicture* m_pImagePicCurPos;
	ColorType_e m_ColorType;

	A3DCOLOR m_clrInit;
	CColorControl* m_pColorCtrl;

};

#endif