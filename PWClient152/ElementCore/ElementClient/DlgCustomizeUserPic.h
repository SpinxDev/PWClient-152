/*
 * FILE: DlgCustomizeUserPic.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_CUSTOMIZE_USER_PIC_H_
#define _DLG_CUSTOMIZE_USER_PIC_H_

#ifdef _WIN32
	#pragma once
#endif

#include "DlgCustomizeBase.h"

class AUIImagePicture;
class A2DSprite;
class AUIStillImageButton;
class AUISlider;
class AUILabel;

class CDlgCustomizeUserPic:public CDlgCustomizeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
	
	friend class CDlgCustomizePre;
	
protected:
	void ResetLayout();

	void ScaleDialogTo(int nRight, int nButtom);
	void ResetDialogSize(int nWinWidth, int nWinHeight);

	void RefreshUserPic();
	void MoveUserPicture(int left, int top);

	float m_fA2DSpriteUserPicScale;//A2DSprite的比例
	float GetA2DSpriteUserPicScale();

	float m_fA2DSpriteUserPicOriScale;//A2DSprite的原始比例

	float m_fPicUserPicScale;//控件的比例
	float GetPicUserPicScale();

	int m_nPicUserPicOriWidth;//控件原始尺寸
	int m_nPicUserPicOriHeight;

	int m_nA2DSPriteUserPicItemPosX;//图片item的位置
	int m_nA2DSPriteUserPicItemPosY;

	//鼠标在item上面的位置
	int m_nMouseOnItemX, m_nMouseOnItemY;

	//鼠标在图片上面的位置
	int m_nMouseOnA2DSpriteX,m_nMouseOnA2DSpriteY;

	AUIImagePicture* m_pPicUserPic;
	A2DSprite* m_pA2DSpriteUserPic;
	AUIStillImageButton* m_pBtnUserPicDrag;
	AUIStillImageButton* m_pBtnUserPicZoomIn;
	AUIStillImageButton* m_pBtnUserPicZoomOut;

	AUISlider* m_pSliderUserPicAlpha;
	AUILabel* m_pLabelUserPicAlpha;

	AUILabel* m_pLabelUserPic;

	//ddx data
	int m_nUserPicAlpha;

protected:
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual void OnTick();

	void Default2Data(); 

	//放大图片
	void ZoomOutUserPic();

	//缩小图片
	void ZoomInUserPic();

public:
	CDlgCustomizeUserPic();
	virtual ~CDlgCustomizeUserPic();

	//主要用于点击按钮后自动放大缩小图片
	bool Tick_WinUserPic();

	//放大图片
	void OnCommandZoomOutUserPic(const char* szCommand);

	//缩小图片
	void OnCommandZoomInUserPic(const char* szCommand);

	//Move slider
	void OnCommandMoveSlider(const char* szCommand);

	//滚动鼠标放大缩小图片
	void OnMouseWheelZoom(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	//拖动图片
	void OnLButtonDownPicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonUpPicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnMouseMovePicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	//缩放窗口
	void OnLButtonDownBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonUpBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseMoveBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
};

#endif
