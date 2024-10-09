// Filename	: DlgBlackScreen.h
// Creator	: zhangyitian
// Date		: 2014/8/6

#ifndef _ELEMENTCLIENT_DLGBLACKSCREEN_H_
#define _ELEMENTCLIENT_DLGBLACKSCREEN_H_

#include "DlgBase.h"

class CDlgBlackScreen : public CDlgBase {
public:
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnChangeLayoutEnd(bool bAllDone);
private:
	void SetView();
};

#endif