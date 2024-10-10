#ifndef _PROPCUSTOMFUNCS_H_
#define _PROPCUSTOMFUNCS_H_

#include "BoostAny.h"

struct prop_custom_funcs
{
	virtual ~prop_custom_funcs() = 0 {}

	//	callback when button clicked
	virtual BOOL CALLBACK OnBtnClicked(void) = 0;
	//	callback when need the content be shown
	virtual LPCTSTR CALLBACK OnGetShowString(void) const = 0;
	
	//	Carefully implement below two functions!
	//	If you do not want return an actual value, then return NULL is good
	//	Do not return an pointer pointing to an empty any() object which will make the BCGPCustomProp work abnormally
	//	callback when update data from proplist to data
	virtual any* CALLBACK OnGetValue(void) { return NULL; }
	virtual const any* CALLBACK OnGetValue(void) const { return NULL; }

	//	callback when update data from data to proplist
	virtual void CALLBACK OnSetValue(any var) {}

	//	callback when draw custom value
	virtual void CALLBACK OnDrawValue(CDC* pDC, CRect rect) {}
};

#endif