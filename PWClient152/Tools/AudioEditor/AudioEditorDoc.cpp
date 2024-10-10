// AudioEditorDoc.cpp : implementation of the CAudioEditorDoc class
//

#include "stdafx.h"
#include "AudioEditor.h"

#include "AudioEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAudioEditorDoc

IMPLEMENT_DYNCREATE(CAudioEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CAudioEditorDoc, CDocument)
END_MESSAGE_MAP()


// CAudioEditorDoc construction/destruction

CAudioEditorDoc::CAudioEditorDoc()
{
	// TODO: add one-time construction code here

}

CAudioEditorDoc::~CAudioEditorDoc()
{
}

BOOL CAudioEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CAudioEditorDoc serialization

void CAudioEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CAudioEditorDoc diagnostics

#ifdef _DEBUG
void CAudioEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAudioEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAudioEditorDoc commands
