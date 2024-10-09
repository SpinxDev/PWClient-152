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

	float m_fA2DSpriteUserPicScale;//A2DSprite�ı���
	float GetA2DSpriteUserPicScale();

	float m_fA2DSpriteUserPicOriScale;//A2DSprite��ԭʼ����

	float m_fPicUserPicScale;//�ؼ��ı���
	float GetPicUserPicScale();

	int m_nPicUserPicOriWidth;//�ؼ�ԭʼ�ߴ�
	int m_nPicUserPicOriHeight;

	int m_nA2DSPriteUserPicItemPosX;//ͼƬitem��λ��
	int m_nA2DSPriteUserPicItemPosY;

	//�����item�����λ��
	int m_nMouseOnItemX, m_nMouseOnItemY;

	//�����ͼƬ�����λ��
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

	//�Ŵ�ͼƬ
	void ZoomOutUserPic();

	//��СͼƬ
	void ZoomInUserPic();

public:
	CDlgCustomizeUserPic();
	virtual ~CDlgCustomizeUserPic();

	//��Ҫ���ڵ����ť���Զ��Ŵ���СͼƬ
	bool Tick_WinUserPic();

	//�Ŵ�ͼƬ
	void OnCommandZoomOutUserPic(const char* szCommand);

	//��СͼƬ
	void OnCommandZoomInUserPic(const char* szCommand);

	//Move slider
	void OnCommandMoveSlider(const char* szCommand);

	//�������Ŵ���СͼƬ
	void OnMouseWheelZoom(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
	//�϶�ͼƬ
	void OnLButtonDownPicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonUpPicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void OnMouseMovePicUserPic(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	//���Ŵ���
	void OnLButtonDownBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonUpBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseMoveBtnDrag(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
};

#endif
