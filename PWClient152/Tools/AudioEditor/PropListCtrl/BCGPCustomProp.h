#ifndef _BCGP_CUSTOM_PROP_H_
#define _BCGP_CUSTOM_PROP_H_

#include "BCGPPropNoRedraw.h"

struct prop_custom_funcs;
class CBCGPCustomProp : public CBCGPPropNoRedraw
{
public:
	// Constructor
	CBCGPCustomProp(const CString& strName, prop_custom_funcs* pCustomFuncs,
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);

	// Public operations
public:
	any GetCustomValue() const;
	void SetCustomValue(any val);

	// Override operations
protected:
	virtual void OnDrawValue (CDC* pDC, CRect rect);
	virtual void SetValue (const _variant_t& varValue);
	virtual const _variant_t& GetValue () const;
	virtual CString FormatProperty ();
	virtual void OnClickButton (CPoint point);

private:
	prop_custom_funcs* m_pCustomFuncs;
};

#endif