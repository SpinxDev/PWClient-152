// Filename	: DlgFortressExchange.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIEditbox.h"
#include "AUIComboBox.h"
#include "AUIStillImageButton.h"

class CDlgFortressExchange : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressExchange();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Exchange(const char *szCommand);
	void OnCommand_Combo_M01(const char *szCommand);
	void OnCommand_Combo_M02(const char *szCommand);
	
	void OnEventKeyDown_Edit_01(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void UpdateInfo();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	
private:

	int GetSrcMaterial();
	int GetDstMaterial();
	int GetMaterialCntToConvert();
	int	GetMaterialCntConverted();
	bool IsCanConvert();
	void ShowConverted(int nConverted);

	enum {BUILDING_MATERIAL_COUNT = 5};	//	可以处理的材料总数

	PAUICOMBOBOX	m_pCombo_M01;	//	被转换材料类型
	PAUIOBJECT		m_pEdit_01;		//	数量输入框
	
	PAUICOMBOBOX	m_pCombo_M02;	//	目标材料类型
	PAUIOBJECT		m_pEdit_02;		//	目标材料转换个数

	PAUISTILLIMAGEBUTTON	m_pBtn_Exchange;	//	兑换按钮
};