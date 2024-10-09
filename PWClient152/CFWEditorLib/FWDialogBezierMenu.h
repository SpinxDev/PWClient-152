#ifndef _FWDIALOGBEZIERMENU_H_
#define _FWDIALOGBEZIERMENU_H_

#include "FWDialogBase.h"

class FWDialogBezierMenu : public FWDialogBase  
{
	FW_DECLARE_DYNCREATE(FWDialogBezierMenu)
	DECLARE_COMMAND_MAP_FW()
protected:
	void InnerOnCommandBezierMenu(AString strCommand);
public:
	void OnCommandDuplicateHandle();
	void OnCommandRemoveHandle();
	void OnCommandBezierCorner();
	void OnCommandBezier();
	void OnCommandSmooth();
	void OnCommandSharp();
	void OnCommandBezierCornerHandle();
	void OnCommandBezierHandle();
	void OnCommandSmoothHandle();
	void OnCommandSharpHandle();
	
	FWDialogBezierMenu();
	virtual ~FWDialogBezierMenu();

	virtual void Show (bool bShow, bool bModal = false);
};

#endif 