/********************************************************************
	created:	2005/09/03
	created:	3:9:2005   11:48
	file name:	ECHelpDialog.h
	author:		yaojun
	
	purpose:	interface for help dialog
*********************************************************************/

#pragma once


#include <AString.h>

class AUIObject;
class CECScriptContext;

class IECScriptUI
{
public:
	virtual ~IECScriptUI() {}
	virtual void ShowHelpDialog() = 0;
	virtual void CloseHelpDialog(bool bCloseHistory = true) = 0;
	virtual void SetHasNextTip(bool bHasNext) = 0;
	virtual void SetHelpDialogContent(AString strContent) = 0;
	virtual void SetHelpDialogImage(AString strFile) = 0;
	virtual void AddToSpirit(int nScriptID, AString strName) = 0;
	virtual void SetTipDialogTitleAndContent(const AString& strTitle, const AString& strContent) = 0;
	virtual void ShowTip(int iGfxTime, int iInTime, int iStayTime, int iOutTime) = 0;
};