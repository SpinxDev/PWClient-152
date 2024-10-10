// Filename	: AUIPicture.h
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIPicture is the class of static picture control.

#ifndef _AUIPICTURE_H_
#define _AUIPICTURE_H_

#include "AUIObject.h"

class AUIPicture : public AUIObject
{
public:
	AUIPicture();
	virtual ~AUIPicture();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool Tick(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);

protected:

};

typedef AUIPicture * PAUIPICTURE;

#endif //_AUIPICTURE_H_