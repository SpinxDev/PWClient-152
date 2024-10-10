#ifndef _BCGP_FILE_PROP_H_
#define _BCGP_FILE_PROP_H_

#include <AFileDialogWrapper.h>
#include "BCGPPropNoRedraw.h"

class AFileDialogWrapper;

class CBCGPAFileProp : public CBCGPPropNoRedraw
{
public:
	CBCGPAFileProp(const CString& strName, 
		const CString& strFileName, 
		BOOL bOpenFileDialog = TRUE,
		LPCTSTR lpszRelativePath = _T(""),
		LPCTSTR lpszDefExt = _T(""),
		int iListStyle = AFileDialogWrapper::AFD_USEDEFAULT,
		LPCTSTR lpszDescr = _T(""),
		DWORD_PTR dwData = 0);
	virtual ~CBCGPAFileProp();

	// Overrides
public:
	virtual void OnClickButton (CPoint point);

private:
	BOOL m_bIsOpenFileDialog;
	CString m_strRelativePath;
	CString m_strDefExt;
	int m_iListStyle;
};

#endif