/********************************************************************
	created:	2005/09/27
	created:	27:9:2005   18:07
	file name:	DlgGMParam.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "DlgBase.h"
#include "Param.h"

class AUILabel;
class AUIComboBox;
class AUICheckBox;
class AUIListBox;
class AUIEditBox;
class CGMCommand;
class CParam;
class CParamArray;
class CGMCommandListBox;

class CDlgGMParam : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
private:
	AUIListBox * m_pLbxParam;
	PAUIOBJECT m_pEbxValue;
	AUICheckBox * m_pChkValue;
	AUILabel * m_pLblChkValueForChk;
	AUIComboBox * m_pCbxValue;
	CGMCommand * m_pGMCommand;
	CGMCommandListBox * m_pGMCommandListBox;
	CParamArray m_paramArray;
	int m_nCurSel;
private:
	void ShowValueControl(int nType);
	void FillListWithDisplayValues(const CParamArray &displayValues);
	void SelectInValueList(const CParam &param);
	void LoadValue();
	void SaveValue();
protected:
	virtual void DoDataExchange(bool bSave);
public:
	CDlgGMParam();
	virtual ~CDlgGMParam();

	void SetAssocData(CGMCommand *pGMCommand, CGMCommandListBox *pGMCommandListBox);
	void OnLButtonDownListBoxParam(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnCommandBtnOK(const char * szCommand);
	void OnCommandBtnCancel(const char * szCommand);
};

