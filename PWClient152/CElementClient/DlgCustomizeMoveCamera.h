/*
 * FILE: DlgCustomizeMoveCamera.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DLG_CUSTOMIZE_MOVECAMERA_H_
#define _DLG_CUSTOMIZE_MOVECAMERA_H_

#ifdef _WIN32
	#pragma once
#endif

class AUIStillImageButton;

#include "DlgCustomizeBase.h"

class CDlgCustomizeMoveCamera : public CDlgCustomizeBase
{
protected:
	//ddx control
	AUIStillImageButton* m_pBtnCameraMoveFront;
	AUIStillImageButton* m_pBtnCameraMoveBack;
	AUIStillImageButton* m_pBtnCameraMoveLeft;
	AUIStillImageButton* m_pBtnCameraMoveRight;
	
	AUIStillImageButton* m_pBtnCameraTurnLeft;
	AUIStillImageButton* m_pBtnCameraTurnRight;

	AUIStillImageButton* m_pBtnCameraMoveUp;
	AUIStillImageButton* m_pBtnCameraMoveDown;
	
protected:

	virtual bool OnInitContext();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual void OnTick();

public:
	CDlgCustomizeMoveCamera();
	virtual ~CDlgCustomizeMoveCamera();


};

#endif