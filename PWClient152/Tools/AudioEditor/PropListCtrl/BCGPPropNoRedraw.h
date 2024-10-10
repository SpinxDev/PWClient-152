#ifndef _BCGP_PROP_NOREDRAW_EX_H_
#define _BCGP_PROP_NOREDRAW_EX_H_

#include "BCGPPropList.h"

#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004

class CBCGPPropNoRedraw : public CBCGPProp
{
	typedef CBCGPProp base_class;
public:
	// Group constructor
	CBCGPPropNoRedraw(const CString& strGroupName, DWORD_PTR dwData = 0,
		BOOL bIsValueList = FALSE);

	// Simple property
	CBCGPPropNoRedraw(const CString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);

	// Override
	virtual void SetValue (const _variant_t& varValue);

private:
	void SetValueNoRedraw(_variant_t varValue);

protected:

	virtual void OnCloseCombo();
	virtual BOOL OnEndEdit ();
};

#endif