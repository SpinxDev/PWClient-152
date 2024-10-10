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
	// ��������
	AUI_ImageGrid m_ListBoxFaceShape;

	// ����
	AUI_ImageGrid m_ListBoxFalingTex;
	
	//	����
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
	AUISlider* m_pSliderOffsetForeheadH;	//��ͷ�ĺ���
	AUISlider* m_pSliderOffsetForeheadV;	//��ͷ������
	AUISlider* m_pSliderOffsetForeheadZ;	//��ͷ��ǰ����
	AUISlider* m_pSliderRotateForehead;		//��ͷ����ת
	AUISlider* m_pSliderScaleForehead;		//��ͷ�ı�������
	
	AUISlider* m_pSliderOffsetYokeBoneH;	//ȧ�ǵĺ���
	AUISlider* m_pSliderOffsetYokeBoneV;	//ȧ�ǵ�����
	AUISlider* m_pSliderOffsetYokeBoneZ;	//ȧ�ǵ�ǰ���ƶ�
	AUISlider* m_pSliderRotateYokeBone;		//ȧ�ǵ���ת
	AUISlider* m_pSliderScaleYokeBone;		//ȧ�ǵı�������
	
	AUISlider* m_pSliderOffsetCheekH;		//���յĺ���
	AUISlider* m_pSliderOffsetCheekV;		//���յ�����
	AUISlider* m_pSliderOffsetCheekZ;		//���յ�ǰ���ƶ�
	AUISlider* m_pSliderScaleCheek;			//���յı�������
	
	AUISlider* m_pSliderOffsetChainV;		//�°͵�����
	AUISlider* m_pSliderOffsetChainZ;		//�°͵�ǰ���ƶ�
	AUISlider* m_pSliderRotateChain;		//�°͵���?
	AUISlider* m_pSliderScaleChainH;		//�°͵����ұ�������
	
	AUISlider* m_pSliderOffsetJawH;			//�ǵĺ���
	AUISlider* m_pSliderOffsetJawV;			//�ǵ�����
	AUISlider* m_pSliderOffsetJawZ;			//�ǵ�ǰ���ƶ�
	AUISlider* m_pSliderScaleJawSpecial;	//�ǵ���������
	AUISlider* m_pSliderScaleJawH;			//�ǵ����ұ�������
	AUISlider* m_pSliderScaleJawV;			//�ǵ����±�������
	
	AUISlider * m_pSliderScaleEar;
	AUISlider * m_pSliderOffsetEarV;	
	
	//����3ͥ�Ĺ������ؼ�
	AUISlider* m_pSliderPartUp;
	AUISlider* m_pSliderPartMiddle;
	AUISlider* m_pSliderPartDown;
	
	//���̱�������
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
	void LoadDefault();//��Ҫ���ڸ�λ��ȱʡ���ݴ�������
	
	CDlgCustomizeFace2();
	virtual ~CDlgCustomizeFace2();
	
	virtual AUIStillImageButton * GetSwithButton();
	void OnCommandMoveSlider(const char* szCommand);
	void OnCommandReset(const char* szCommand);
};

#endif
