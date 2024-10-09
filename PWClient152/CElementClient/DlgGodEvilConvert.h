// File		: DlgGodEvilConvert.h
// Creator	: Xu Wenbin
// Date		: 2010/3/16

#pragma once

#include "DlgBase.h"

class AUIImagePicture;
class AUIObject;
class CDlgGodEvilConvert : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	void OnCommandNormalConfirm(const char * szCommand);
	
	void OnCommandAdvancedConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);

protected:
	void Update();
	void UpdateConvertUI( AUIImagePicture * pImgNeed1, AUIObject * pLbelNeed1, int itemID);
	void ConfirmConvert(int convertType, int itemID);
	bool HasItem(int itemID);
	bool MeetTaoistRank(int converType);

	virtual void OnShowDialog();
};
