#include "stdafx.h"
#include "resource.h"
#include "pagedialog.h"
#include "direct.h"
#include "SelectPath.h"

bool BrowseFile(CString & str2 , LPCSTR filter,LPCSTR folder)
{
	CString text;
	CSelectPath path;
	char curr[MAX_PATH + 1];
	GetCurrentDirectoryA(MAX_PATH, curr);
	CString str = curr;
	if(folder){
		SetCurrentDirectoryA(folder);
		char curr2[MAX_PATH+1];
		GetCurrentDirectoryA(MAX_PATH, curr2);
		str = curr2;
	}
	path.SetPath(str,CString(filter));
	if(path.DoModal() == IDOK)
	{
		SetCurrentDirectoryA(curr);
		str2 = path.GetPathName();
		return true;
	}
	SetCurrentDirectoryA(curr);
	return false;
}

void BrowseFile(CEdit * editbox, LPCSTR filter,LPCSTR folder)
{
	CString text;
	CSelectPath path;
	char curr[MAX_PATH + 1];
	GetCurrentDirectoryA(MAX_PATH, curr);
	CString str = curr;
	if(folder){
		SetCurrentDirectoryA(folder);
		char curr2[MAX_PATH+1];
		GetCurrentDirectoryA(MAX_PATH, curr2);
		str = curr2;
	}
	path.SetPath(str,CString(filter));
	if(path.DoModal() == IDOK)
	{
		editbox->SetWindowText(path.GetPathName());
	}
	SetCurrentDirectoryA(curr);
}
