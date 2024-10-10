// Filename	: AUIConsole.h
// Creator	: Tom Zhou
// Date		: October 20, 2004
// Desc		: AUIConsole is the class of control console.

#ifndef _AUICONCOLE_H_
#define _AUICONCOLE_H_

#include "AUIObject.h"
#include "AUIFrame.h"

#define AUICONSOLE_MAX_DISPLAY		100
#define AUICONSOLE_MAX_HISTORY		20
#define AUICONSOLE_BLINK_INTERVAL	500

class AUIConsole : public AUIObject
{
public:
	AUIConsole();
	virtual ~AUIConsole();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	/* support properties
	"Font Name"		Property->s
	"Font Height"	Property->i
	And AUIObject.Properties
	*/
	virtual bool GetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();

	void FitScreen();
	void SetEditBox(PAUIOBJECT pEdit);
	const ACHAR * GetLastCommandLine();
	const ACHAR * GetCommandLine();
	void GetHistory(abase::vector<ACString>& vecHistory);
	void SetCommandLine(const ACHAR *pszCmd);
	void SetCommandLineColor(A3DCOLOR color);
	void AddLine(const ACHAR *pszLine, A3DCOLOR color);

	void ClearAll();
	void ClearDisplay();
	void ClearHistory();

protected:
	PAUIOBJECT m_pEdit;
	int m_nLinesPerPage;
	ACString m_strDisplay[AUICONSOLE_MAX_DISPLAY];
	A3DCOLOR m_colorDisplay[AUICONSOLE_MAX_DISPLAY];
	int m_nNumDisplay, m_nQueueBegin, m_nFirstLine;

	ACString m_strCmdLine;
	ACString m_strHistory[AUICONSOLE_MAX_HISTORY];
	int m_nBlink, m_nShowCaret, m_nCaretPos, m_nMaxCmdLen;
	int m_nCurHistory, m_nHistoryQueueBegin, m_nNumHistories;
};

typedef AUIConsole * PAUICONSOLE;

#endif	//	_AUICONCOLE_H_