// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "ElementData.h"
#include "ChildFrm.h"
#include "A3D.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

#include "ExtendDataTempl.h"
#include "BaseDataIDMan.h"
#include "ExpTypes.h"
#include "BaseDataExp.h"

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	/*
	BaseDataTempl tmpl;
	tmpl.Load("BaseData\\×°±¸\\µ¯Ò©\\·Éµ¯\\·Éµ¯.tmpl");
	PROJECTILE_ESSENCE ws;
	Fill_PROJECTILE_ESSENCE(tmpl, &ws);
	tmpl.Release();
	*/

	/*
	ExtendDataTempl templ;
	templ.Load("BaseData\\×°±¸\\ÎäÆ÷\\¸½¼ÓÊôÐÔ\\vvv.ext", false);
	EQUIPMENT_ADDON ea;
	Fill_EQUIPMENT_ADDON(templ, &ea);
	templ.Release();
	*/

	/*
	BaseDataIDManBase man;
	man.Load("BaseData\\TemplID.dat");
	man.CreateID("debug");
	man.Release();
	*/
	/*
	ExtDataIDMan man;
	man.Load("BaseData\\ExtDataID.dat");
	man.CreateID("fff");
	man.Release();
	*/
	/*
	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	
	PROCESS_INFORMATION pi;
	BOOL bRet = CreateProcess(
		"F:\\Programs\\VSS6\\win32\\ss.exe",
		"checkin TemplID.dat",
		NULL,
		NULL,
		FALSE,
		CREATE_NO_WINDOW,
		NULL,
		"E:\\OnlineGm\\Bin\\BaseData",
		&si,
		&pi);
	DWORD dw = GetLastError();
	*/
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
