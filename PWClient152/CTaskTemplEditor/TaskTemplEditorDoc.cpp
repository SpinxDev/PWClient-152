// TaskTemplEditorDoc.cpp : implementation of the CTaskTemplEditorDoc class
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTemplEditorDoc.h"
#include "TaskTempl.h"
#include "TaskTemplEditorView.h"
#include "AMemory.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorDoc

IMPLEMENT_DYNCREATE(CTaskTemplEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CTaskTemplEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CTaskTemplEditorDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorDoc construction/destruction

CTaskTemplEditorDoc::CTaskTemplEditorDoc()
{
}

CTaskTemplEditorDoc::~CTaskTemplEditorDoc()
{
}

BOOL CTaskTemplEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorDoc serialization

void CTaskTemplEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorDoc diagnostics

#ifdef _DEBUG
void CTaskTemplEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTaskTemplEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorDoc commands

BOOL CTaskTemplEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	/*
	if (GetFileAttributes(lpszPathName) & FILE_ATTRIBUTE_READONLY)
	{
		_edit_mode = FALSE;
		AfxMessageBox(_T("只读模式，只能查看，请先Check Out再编辑"), MB_ICONINFORMATION);
	}
	else
		_edit_mode = TRUE;

	ATaskTempl* pTask = new ATaskTempl;
	if (!pTask->LoadFromTextFile(CSafeString(lpszPathName)))
	{
		delete pTask;
		return FALSE;
	}

//	AfxGetEditorView()->CreateHisTempStartFile(lpszPathName);
*/

	return TRUE;
}

BOOL CTaskTemplEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	/*
	DWORD dwAttr = GetFileAttributes(lpszPathName);
	if (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_READONLY))
		_edit_mode = FALSE;
	else
		_edit_mode = TRUE;

	if (!_edit_mode)
	{
		AfxMessageBox(_T("只读模式，不能保存"), MB_ICONINFORMATION);
		SetModifiedFlag(FALSE);
		return TRUE;
	}

	AfxGetEditorView()->OnApply();
//	if (!g_pTaskTempl->SaveToTextFile(CSafeString(lpszPathName))) return FALSE;
	SetModifiedFlag(FALSE);
	*/
	return TRUE;
}
