// Filename	: AUICustomize.h
// Creator	: Xiao Zhou
// Date		: May 23, 2006
// Desc		: AUICustomize is the class of custiomize control.

#ifndef _AUICUSTOMIZE_H_
#define _AUICUSTOMIZE_H_

#include "AUIObject.h"

class AUICustomize : public AUIObject
{
public:
	AUICustomize();
	virtual ~AUICustomize();

	virtual bool Save(FILE *file);
	virtual bool Render(void);
	virtual bool GetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);

protected:
	AString m_szObjectType;

};

typedef AUICustomize * PAUICUSTOMIZE;

#endif //_AUICUSTOMIZE_H_