// Filename	: DlgChangeSkirt.h
// Creator	: Xu Wenbin
// Date		: 2009/06/01

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include "AUIScroll.h"
#include "AUIImagePicture.h"

class CECIvtrFashion;
class CDlgChangeSkirt : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgChangeSkirt();
	virtual ~CDlgChangeSkirt();

	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_BeginChange(const char * szCommand);
	void OnCommand_StopChange(const char * szCommand);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual bool Tick(void);

	void Store(PAUIOBJECT pItem, CECIvtrFashion *pFashion);
	void Replace(PAUIOBJECT pSrcItem, PAUIOBJECT pDstItem);
	void Discard(PAUIOBJECT pItem);

	AString GetIconFile(int iSuite);

protected:
	virtual bool OnInitDialog();

private:
	bool IsFashionShortCutChangeable();
	int  GetFashionShortCutOffset(PAUIOBJECT pItem);
	int  GetFashionShortCutOffsetType(int offset);
	void SetFashionShortCut(PAUIOBJECT pItem, CECIvtrFashion *pFashion);
	bool IsFashionShortCutExchangeable(PAUIOBJECT pItem1, PAUIOBJECT pItem2);
	CECIvtrFashion * GetFashion(PAUIOBJECT pItem);
	bool IsFashionMatch(PAUIOBJECT pItem, CECIvtrFashion *pFashion);

	static int GetMinAutoFashionTime();
	static int GetMaxAutoFashionTime();

private:
	PAUIOBJECT m_pEdit_Hour;
	PAUIOBJECT m_pEdit_Minute;
	PAUISTILLIMAGEBUTTON m_pButton_BeginChange;
	PAUISTILLIMAGEBUTTON m_pButton_StopChange;
	PAUISCROLL m_pScroll_Suite;

	enum {SuiteSizeWidth = 6};
	enum {SuiteSizeHeight = 3};
	enum {SuiteSizeShown = SuiteSizeWidth * SuiteSizeHeight};
	PAUIIMAGEPICTURE m_pImage_Suite[SuiteSizeHeight];
};
