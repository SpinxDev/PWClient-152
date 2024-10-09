/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   13:04
	file base:	DlgHelp
	file ext:	h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgBase.h"
#include <vector>

class AUIStillImageButton;
class AUITreeView;
class AUITextArea;
class AUIImagePicture;
class A2DSprite;

class CDlgHelp : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:
	int m_nCurHelpGroup;
	abase::vector<AString> m_vecFile;
	abase::vector<AString> m_vecImage;
	A2DSprite *m_pA2DSpriteImage;

	// control variable
	AUIStillImageButton *	m_ppHelpGroupButton[5];
	AUITreeView *			m_pTree;
	AUITextArea *			m_pTextArea;
	AUIImagePicture *		m_pImage;
protected:
	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();

	bool BuildHelp(int nGroup, int nDoc);
public:
	void OnCommandIDCancel(const char *szCommand);
	void OnCommandHelpMain(const char * szCommand);
	void OnCommandWizard(const char * szCommand);
	void OnLButtonDownTree(WPARAM, LPARAM, AUIObject *);

	CDlgHelp();
	virtual ~CDlgHelp();
};

