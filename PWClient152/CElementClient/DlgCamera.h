// Filename	: DlgCamera.h
// Creator	: Xiao Zhou
// Date		: 2005/11/18

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"
#include "AUIComboBox.h"

class CDlgCamera : public CDlgBase  
{
	friend class CECGameUIMan;

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgCamera();
	virtual ~CDlgCamera();
	
	void OnCommand_PrintScr(const char * szCommand);
	void OnCommand_Combo_Scale(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	
	int	PreparePrint();

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual void OnShowDialog();
	void	UpdateCaptureScale();

	PAUISTILLIMAGEBUTTON		m_pBtn_CameraMoveLeft;
	PAUISTILLIMAGEBUTTON		m_pBtn_CameraMoveRight;
	PAUISTILLIMAGEBUTTON		m_pBtn_CameraMoveUp;
	PAUISTILLIMAGEBUTTON		m_pBtn_CameraMoveDown;
	PAUISTILLIMAGEBUTTON		m_pBtn_CameraMoveFront;
	PAUISTILLIMAGEBUTTON		m_pBtn_CameraMoveBack;
	PAUISTILLIMAGEBUTTON		m_pBtn_CameraFarFocus;
	PAUISTILLIMAGEBUTTON		m_pBtn_CameraNearFocus;
	PAUICHECKBOX						m_pChk_HideUI;
	PAUICOMBOBOX						m_pCombo_Scale;
	bool							m_bPrinting;
	int							m_nPrintTimes;
	DWORD					m_dwLastTick;
};
