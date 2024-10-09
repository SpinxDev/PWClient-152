/*
 * FILE: DlgCustomizeEye.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_CUSTOMIZE_EYE_H_
#define _DLG_CUSTOMIZE_EYE_H_

#ifdef _WIN32	
	#pragma once
#endif

#include "DlgCustomizeBase.h"
#include "AUI_ImageGrid.h"

class AUISlider;
class AUIListBox;
class AUILabel;
class AUICheckBox;
class AUIStillImageButton;

class CDlgCustomizeEye : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
	
protected:

	//DDX data
	//ddx control
	AUI_ImageGrid m_ListBoxEyeBaseTex;	
	AUI_ImageGrid m_ListBoxEyeShape;

	typedef abase::vector<A2DSprite*>::iterator VecA2DSpriteIter_t;
	abase::vector<A2DSprite*> m_vecA2DSprite;

	// the third eye
	AUI_ImageGrid m_LbxThirdEye;
	bool InitThirdEye();
	void ShowThirdEye(bool bShow);
	
	AUI_ImageGrid m_LbxBrowTex;
	AUI_ImageGrid m_LbxBrowShape;

protected:

	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnChangeLayoutEnd(bool bAllDone);
	
	void DoCalculate();
	bool InitResource();
	
	void Normal2Data();
	virtual void OnShowDialog();

	void ClearListCtrlAndA2DSprite();

public:
	CDlgCustomizeEye();
	virtual ~CDlgCustomizeEye();

	virtual AUIStillImageButton * GetSwithButton();
		
	virtual void OnTick();		

	// duplicated logic as CDlgCustomizeThirdEye
	void OnLButtonUpListBoxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseWheelListboxThirdEye(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnLButtonUpListbox(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnMouseWheelListbox(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
};


class CDlgCustomizeEye2 : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCustomizeEye2();

	virtual AUIStillImageButton * GetSwithButton();
	
	void OnCommandLeftEye(const char* szCommand);
	void OnCommandRightEye(const char* szCommand);
	void OnCommandEyeLeftRightHelper(int nCheckedIndex);
	void OnCommandEyeMoveSlider(const char* szCommand);
	void OnCommandEyeSync(const char* szCommand);
	
	void OnCommandBrowSwitchToLeft(const char *);
	void OnCommandBrowSwitchToRight(const char *);
	void OnCommandBrowSwithLeftRightHelper(int nCheckedIndex);
	void OnCommandBrowMoveSlider(const char * szCommand);
	void OnCommandBrowSync(const char* szCommand);
	
	void OnCommandReset(const char * szCommand);

	void LoadDefault();

protected:
	virtual bool OnInitContext();	
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);
	void SetSliderBound();
	void LoadEyeDefault();
	void DoCalculate();
	bool InitResource();
	void LoadBrowDefault();
		
protected:
	
	bool m_bEyeSync;						//ͬ��
	int m_nEyeLeftOrRight;					// 0 : left, 1 : right	
	
	AUISlider* m_pSliderScaleEyeH;		//��ˮƽ��������
	AUISlider* m_pSliderScaleEyeV;		//����ֱ��������
	AUISlider* m_pSliderRotateEye;		//����ת���⣬��Z����ת
	AUISlider* m_pSliderOffsetEyeH;		//��ˮƽ�ƶ�
	AUISlider* m_pSliderOffsetEyeV;		//�۴�ֱ�ƶ�
	AUISlider* m_pSliderOffsetEyeZ;		//�������ƶ�
	AUISlider* m_pSliderScaleEyeBall;	//��������
	AUILabel* m_pLabelEye[7];			//����
	
	AUICheckBox* m_pEyeCheckBoxSync;
	AUIStillImageButton* m_pEyeBtnLeft;
	AUIStillImageButton* m_pEyeBtnRight;

	
	// ddx control	
	AUISlider* m_pSliderScaleBrowH;	//üë��ˮƽ��������
	AUISlider* m_pSliderScaleBrowV; //üë����֦��������
	AUISlider* m_pSliderRotateBrow; //üë��ת����,��Z����ת
	AUISlider* m_pSliderOffsetBrowH; //üë��ˮƽλ��
	AUISlider* m_pSliderOffsetBrowV; //üë�Ĵ�ֱλ��
	AUISlider* m_pSliderOffsetBrowZ; //üë��ǰ��λ��
	AUILabel* m_pLabelBrow[6];
	
	AUICheckBox* m_pBrowCheckBoxSync;
	AUIStillImageButton* m_pBrowBtnLeft;
	AUIStillImageButton* m_pBrowBtnRight;
	
	// ddx data
	bool m_bBrowSync;
	// special : this one is not in DDX
	int m_nBrowLeftOrRight; // 1 == left, 0 == right
};

#endif
