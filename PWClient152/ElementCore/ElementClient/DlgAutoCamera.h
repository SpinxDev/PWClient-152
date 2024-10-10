/*
 * FILE: DlgAutoCamera.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/5/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_AUTO_CAMERA_H_
#define _DLG_AUTO_CAMERA_H_

#include "DlgHomeBase.h"

class AUIStillImageButton;
class AUISlider;

class CDlgAutoCamera:public CDlgHomeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgAutoCamera();
	virtual ~CDlgAutoCamera();

	class CameraPosAndDir_t
	{
	public:
		A3DVECTOR3 vPos;
		A3DVECTOR3 vDir;
		
		void Reset() 
		{
			vPos.Clear();
			vDir.Clear();
		}

		CameraPosAndDir_t& operator = ( const CameraPosAndDir_t &i_Val)
		{
			vPos = i_Val.vPos;
			vDir = i_Val.vDir;
		}
	};

	void SaveCameraPosAndDir(int i_nIdx, const A3DVECTOR3 &i_vPos, const A3DVECTOR3 &i_vDir);
	void DealWithMouseWheelChangeCameraSpeed(UINT uMsg, WPARAM wParam, LPARAM lParam);


protected:
	virtual void OnShowDialog();
	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);

	void LoadDefault();
	bool InitResource();
	void InitVars();

	void SetLabelsText();
	void SetSliderBound();

	void OnCommandMoveSlider(const char* szCommand);
	void OnCommandCamera(const char* szCommand);
	void AddCameraSpeed(int i_nDelta);

	enum BTN_TYPE
	{
		BTN_CAMERA_0 = 0,
		BTN_CAMERA_1,
		BTN_CAMERA_2,
		BTN_CAMERA_3,
		BTN_CAMERA_4,
		BTN_CAMERA_5,
		BTN_CAMERA_6,
		BTN_CAMERA_7,
		BTN_CAMERA_8,
		BTN_CAMERA_9,
		BTN_NUM
	};

	AUIStillImageButton* m_pBtn[BTN_NUM];
	CameraPosAndDir_t m_CameraPosAndDir[BTN_NUM];
	
	AUISlider* m_pSliderCameraSpeed;
	int m_nSliderCameraSpeed;
};

#endif