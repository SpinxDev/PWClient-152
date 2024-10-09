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
	
	bool m_bEyeSync;						//同步
	int m_nEyeLeftOrRight;					// 0 : left, 1 : right	
	
	AUISlider* m_pSliderScaleEyeH;		//眼水平比例调节
	AUISlider* m_pSliderScaleEyeV;		//眼竖直比例调节
	AUISlider* m_pSliderRotateEye;		//眼旋转调解，绕Z轴旋转
	AUISlider* m_pSliderOffsetEyeH;		//眼水平移动
	AUISlider* m_pSliderOffsetEyeV;		//眼垂直移动
	AUISlider* m_pSliderOffsetEyeZ;		//眼上下移动
	AUISlider* m_pSliderScaleEyeBall;	//眼球缩放
	AUILabel* m_pLabelEye[7];			//标题
	
	AUICheckBox* m_pEyeCheckBoxSync;
	AUIStillImageButton* m_pEyeBtnLeft;
	AUIStillImageButton* m_pEyeBtnRight;

	
	// ddx control	
	AUISlider* m_pSliderScaleBrowH;	//眉毛的水平比例调节
	AUISlider* m_pSliderScaleBrowV; //眉毛的树枝比例调节
	AUISlider* m_pSliderRotateBrow; //眉毛旋转调节,绕Z轴旋转
	AUISlider* m_pSliderOffsetBrowH; //眉毛的水平位移
	AUISlider* m_pSliderOffsetBrowV; //眉毛的垂直位移
	AUISlider* m_pSliderOffsetBrowZ; //眉毛的前后位移
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
