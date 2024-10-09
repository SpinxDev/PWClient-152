/*
 * FILE: DlgCustomizeFace.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef _DLG_CUSTOMZIE_FACE_H_
#define _DLG_CUSTOMIZE_FACE_H_

#ifdef _WIN32
	#pragma once
#endif

#include "DlgCustomizeBase.h"
#include "AUI_ImageGrid.h"

class AUIImagePicture;
class AUISlider;
class AUIListBox;
class AUIImagePicture;

class CDlgCustomizeFace : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:
	// 基本脸型
	AUI_ImageGrid m_ListBoxFaceShape;

	// 法令
	AUI_ImageGrid m_ListBoxFalingTex;
	
	//	耳型
	AUI_ImageGrid m_ListBoxEar;

	typedef abase::vector<A2DSprite*>::iterator VecA2DSpriteIter_t;
	abase::vector<A2DSprite*> m_vecA2DSprite;

protected:
	
	void DoCalculate();
	bool InitResource();

	virtual bool OnInitContext();
	virtual AUIStillImageButton * GetSwithButton();
	virtual void OnShowDialog();
	virtual void OnTick();

	void ClearListCtrlAndA2DSprite();
public:	
	void OnCommandSwitchDlg(const char* szCommand);
	
	void OnLButtonUpListbox(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseWheelListbox(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	CDlgCustomizeFace();
	virtual ~CDlgCustomizeFace();
};



class CDlgCustomizeFace2 : public CDlgCustomizeBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
protected:
	//ddx control
	AUISlider* m_pSliderOffsetForeheadH;	//额头的横移
	AUISlider* m_pSliderOffsetForeheadV;	//额头的纵移
	AUISlider* m_pSliderOffsetForeheadZ;	//额头的前后移
	AUISlider* m_pSliderRotateForehead;		//额头的旋转
	AUISlider* m_pSliderScaleForehead;		//额头的比例缩放
	
	AUISlider* m_pSliderOffsetYokeBoneH;	//颧骨的横移
	AUISlider* m_pSliderOffsetYokeBoneV;	//颧骨的纵移
	AUISlider* m_pSliderOffsetYokeBoneZ;	//颧骨的前后移动
	AUISlider* m_pSliderRotateYokeBone;		//颧骨的旋转
	AUISlider* m_pSliderScaleYokeBone;		//颧骨的比例缩放
	
	AUISlider* m_pSliderOffsetCheekH;		//脸颊的横移
	AUISlider* m_pSliderOffsetCheekV;		//脸颊的纵移
	AUISlider* m_pSliderOffsetCheekZ;		//脸颊的前后移动
	AUISlider* m_pSliderScaleCheek;			//脸颊的比例缩放
	
	AUISlider* m_pSliderOffsetChainV;		//下巴的纵移
	AUISlider* m_pSliderOffsetChainZ;		//下巴的前后移动
	AUISlider* m_pSliderRotateChain;		//下巴的旋?
	AUISlider* m_pSliderScaleChainH;		//下巴的左右比例缩放
	
	AUISlider* m_pSliderOffsetJawH;			//颌骨的横移
	AUISlider* m_pSliderOffsetJawV;			//颌骨的纵移
	AUISlider* m_pSliderOffsetJawZ;			//颌骨的前后移动
	AUISlider* m_pSliderScaleJawSpecial;	//颌骨的特殊缩放
	AUISlider* m_pSliderScaleJawH;			//颌骨的左右比例缩放
	AUISlider* m_pSliderScaleJawV;			//颌骨的上下比例缩放
	
	AUISlider * m_pSliderScaleEar;
	AUISlider * m_pSliderOffsetEarV;	
	
	//调整3庭的滚动条控件
	AUISlider* m_pSliderPartUp;
	AUISlider* m_pSliderPartMiddle;
	AUISlider* m_pSliderPartDown;
	
	//脸盘比例调节
	AUISlider* m_pSliderScaleFaceH;
	AUISlider* m_pSliderScaleFaceV;
	
protected:
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	
	void DoCalculate();
	bool InitResource();
	void SetSliderBound();
	
	
	void DefaultForehead();
	void DefaultYokeBone();
	void DefaultCheek();
	void DefaultChain();
	void DefaultJaw();
	void DefaultEar();
	void DefaultFace();
	
	virtual void OnShowDialog();
	
public:
	void LoadDefault();//主要用于复位把缺省数据传给数据
	
	CDlgCustomizeFace2();
	virtual ~CDlgCustomizeFace2();
	
	virtual AUIStillImageButton * GetSwithButton();
	void OnCommandMoveSlider(const char* szCommand);
	void OnCommandReset(const char* szCommand);
};

#endif
