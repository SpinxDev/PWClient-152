 // Filename	: AUIConsole.cpp
// Creator	: Tom Zhou
// Date		: October 20, 2004
// Desc		: AUIConsole is the class of control console.

#include "AUI.h"

#include "A3DMacros.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"

#include "CSplit.h"

#include "AUIDef.h"
#include "AUIDialog.h"
#include "AUIConsole.h"
#include "AUIManager.h"
#include "AUICommon.h"
#include "AUICTranslate.h"

AUIConsole::AUIConsole() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_CONSOLE;
	m_pEdit = NULL;
	m_nCurHistory = -1;
	m_nBlink = GetTickCount();
	m_nHistoryQueueBegin = m_nNumHistories = 0;
	m_nShowCaret = m_nCaretPos = m_nMaxCmdLen = 0;
	m_nLinesPerPage = m_nNumDisplay = m_nQueueBegin = m_nFirstLine = 0;
}

AUIConsole::~AUIConsole()
{
}

bool AUIConsole::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{
	/*	Label property line format:
	 *
	 *		CONSOLE				Keyword
	 *		Title				Name
	 *		""					Command.
	 *		""					Sound.
	 *		0 0 200 20			x y Width Height
	 *		3					String ID of font name
	 *		20					Font height
	 */

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIConsole::Init(), failed to call base AUIObject::Init()");

	if( !pASF ) return true;

	BEGIN_FAKE_WHILE2;

	CHECK_BREAK2(pASF->GetNextToken(true));
	int idString = atoi(pASF->m_szToken);
	
	ACString strFontName = m_pParent->GetStringFromTable(idString);
	if( strFontName.GetLength() == 0 ) strFontName = _AL("方正细黑一简体");
	CSplit s(strFontName);
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("###"));
	m_szFontName = vec[0];
	if( vec.size() > 1 ) m_nShadow = a_atoi(vec[1]);
	if( vec.size() > 2 ) m_nOutline = a_atoi(vec[2]);
	if( vec.size() > 3 ) m_bBold = a_atoi(vec[3]) ? true : false;
	if( vec.size() > 4 ) m_bItalic = a_atoi(vec[4]) ? true : false;

	CHECK_BREAK2(pASF->GetNextToken(true));
	m_nFontHeight = atoi(pASF->m_szToken);

	if (!SetFontAndColor(m_szFontName, m_nFontHeight,
		m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic))
		return AUI_ReportError(__LINE__, 1, "AUIConsole::Init(), failed to call SetFontAndColor()");

	FitScreen();

	END_FAKE_WHILE2;

	BEGIN_ON_FAIL_FAKE_WHILE2;

	AUI_ReportError(DEFAULT_2_PARAM, "AUIConsole::Init(), failed to read from file");

	END_ON_FAIL_FAKE_WHILE2;

	RETURN_FAKE_WHILE_RESULT2;
}

bool AUIConsole::InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem)
{
	ASSERT(pA3DEngine && pA3DDevice);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DDevice;
	m_szFontName = m_pParent->GetAUIManager()->GetDefaultFontName();
	m_nFontHeight = m_pParent->GetAUIManager()->GetDefaultFontSize();
	
	XMLGET_STRING_VALUE(pItem, _AL("Name"), m_szName)
	XMLGET_INT_VALUE(pItem, _AL("x"), m_x)
	XMLGET_INT_VALUE(pItem, _AL("y"), m_y)
	XMLGET_INT_VALUE(pItem, _AL("Width"), m_nWidth)
	XMLGET_INT_VALUE(pItem, _AL("Height"), m_nHeight)

	XMLGET_STRING_VALUE(pItem, _AL("Command"), m_strCommand)
	XMLGET_STRING_VALUE(pItem, _AL("Template"), m_strTemplate)

	m_nDefaultX = m_x;
	m_nDefaultY = m_y;
	m_nDefaultWidth = m_nWidth;
	m_nDefaultHeight = m_nHeight;

	AXMLItem *pChildItem = pItem->GetFirstChildItem();
	while( pChildItem )
	{
		if( a_stricmp(pChildItem->GetName(), _AL("Resource")) == 0 )
		{
			AXMLItem *pResouceChildItem = pChildItem->GetFirstChildItem();
			while( pResouceChildItem )
			{
				if( a_stricmp(pResouceChildItem->GetName(), _AL("FrameImage")) == 0 )
				{
					AString strFileName;
					XMLGET_STRING_VALUE(pResouceChildItem, _AL("FileName"), strFileName)
					InitIndirect(strFileName, 0);
				}
				pResouceChildItem = pResouceChildItem->GetNextItem();
			}
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Text")) == 0 )
		{
			XMLGET_WSTRING_VALUE(pChildItem, _AL("FontName"), m_szFontName)
			XMLGET_INT_VALUE(pChildItem, _AL("FontSize"), m_nFontHeight)
			XMLGET_INT_VALUE(pChildItem, _AL("Shadow"), m_nShadow)
			m_nShadow = A3DCOLORRGBA(0, 0, 0, m_nShadow);
			XMLGET_COLOR_VALUE(pChildItem, _AL("ShadowColor"), m_nShadow)
			XMLGET_INT_VALUE(pChildItem, _AL("Outline"), m_nOutline)
			XMLGET_BOOL_VALUE(pChildItem, _AL("Bold"), m_bBold)
			XMLGET_BOOL_VALUE(pChildItem, _AL("Italic"), m_bItalic)
			XMLGET_BOOL_VALUE(pChildItem, _AL("FreeType"), m_bFreeType)
			XMLGET_COLOR_VALUE(pChildItem, _AL("Color"), m_color)
		}
		else if( a_stricmp(pChildItem->GetName(), _AL("Hint")) == 0)
		{
			ACString strHint;
			XMLGET_WSTRING_VALUE(pChildItem, _AL("String"), strHint);
			AUICTranslate trans;
			m_szHint = trans.Translate(strHint);
		}
		pChildItem = pChildItem->GetNextItem();
	}
	
	if (!SetFontAndColor(m_szFontName, m_nFontHeight,
		m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType))
		return AUI_ReportError(__LINE__, 1, "AUIConsole::InitXML(), failed to call SetFontAndColor()");

	FitScreen();
	
	return true;
}

bool AUIConsole::Release(void)
{
	return AUIObject::Release();
}

bool AUIConsole::Save(FILE *file)
{
	fprintf(file, "CONSOLE");

	if( !AUIObject::Save(file) ) return false;

	int idStrFont = m_pParent->AddStringToTable(m_szFontName);
	if( idStrFont < 0 ) return false;

	fprintf(file, " %d %d\n", idStrFont, m_nFontHeight);

	return true;
}

bool AUIConsole::SaveXML(AXMLItem *pXMLItemDialog)
{
	AXMLItem *pXMLItemConsole = new AXMLItem;
	pXMLItemConsole->InsertItem(pXMLItemDialog);
	pXMLItemConsole->SetName(_AL("CONSOLE"));
	XMLSET_STRING_VALUE(pXMLItemConsole, _AL("Name"), AS2AC(m_szName), _AL(""))
	XMLSET_INT_VALUE(pXMLItemConsole, _AL("x"), m_x, 0)
	XMLSET_INT_VALUE(pXMLItemConsole, _AL("y"), m_y, 0)
	XMLSET_INT_VALUE(pXMLItemConsole, _AL("Width"), m_nWidth, 0)
	XMLSET_INT_VALUE(pXMLItemConsole, _AL("Height"), m_nHeight, 0)

	AXMLItem *pXMLItemHint = new AXMLItem;
	pXMLItemHint->InsertItem(pXMLItemConsole);
	pXMLItemHint->SetName(_AL("Hint"));
	AUICTranslate trans;
	XMLSET_STRING_VALUE(pXMLItemHint, _AL("String"), trans.ReverseTranslate(m_szHint), _AL(""))

	XMLSET_STRING_VALUE(pXMLItemConsole, _AL("Command"), AS2AC(m_strCommand), _AL(""))
	XMLSET_STRING_VALUE(pXMLItemConsole, _AL("Template"), AS2AC(m_strTemplate), _AL(""))

	AXMLItem *pXMLItemText = new AXMLItem;
	pXMLItemText->InsertItem(pXMLItemConsole);
	pXMLItemText->SetName(_AL("Text"));
	XMLSET_STRING_VALUE(pXMLItemText, _AL("FontName"), m_szFontName, _AL(""))
	XMLSET_INT_VALUE(pXMLItemText, _AL("FontSize"), m_nFontHeight, 0)
	XMLSET_COLOR_VALUE(pXMLItemText, _AL("ShadowColor"), m_nShadow, _AL("0,0,0,0"));
	XMLSET_INT_VALUE(pXMLItemText, _AL("Outline"), m_nOutline, 0)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Bold"), m_bBold, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("Italic"), m_bItalic, false)
	XMLSET_BOOL_VALUE(pXMLItemText, _AL("FreeType"), m_bFreeType, true)
	
	return true;
}

bool AUIConsole::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_KEYDOWN )
	{
		int nVirtKey = int(wParam);
		int nCmdLen = m_strCmdLine.GetLength();

		if( nVirtKey == VK_ESCAPE )
		{
			m_pParent->Show(false);
		}
		// Command line key buffer control.
		else if( nVirtKey == VK_DELETE && m_nCaretPos < nCmdLen )
		{
			m_strCmdLine = m_strCmdLine.Left(m_nCaretPos)
				+ m_strCmdLine.Right(nCmdLen - m_nCaretPos - 1);
		}
		else if( nVirtKey == VK_HOME && m_nCaretPos > 0 )
		{
			m_nCaretPos = 0;
		}
		else if( nVirtKey == VK_END && m_nCaretPos < nCmdLen )
		{
			m_nCaretPos = nCmdLen;
		}
		else if( nVirtKey == VK_LEFT && m_nCaretPos > 0 )
		{
			m_nCaretPos--;
		}
		else if( nVirtKey == VK_RIGHT && m_nCaretPos < nCmdLen )
		{
			m_nCaretPos++;
		}
		// Command line history control.
		else if( nVirtKey == VK_UP && m_nCurHistory != m_nHistoryQueueBegin )
		{
			if( -1 == m_nCurHistory )
			{
				if( m_nNumHistories <= AUICONSOLE_MAX_HISTORY )
					m_nCurHistory = m_nNumHistories - 1;
				else
				{
					m_nCurHistory = m_nHistoryQueueBegin - 1;
					if( m_nCurHistory < 0 )
						m_nCurHistory += AUICONSOLE_MAX_HISTORY;
				}
			}
			else
			{
				m_nCurHistory--;
				if( m_nCurHistory < 0 )
					m_nCurHistory += AUICONSOLE_MAX_HISTORY;
			}

			m_strCmdLine = m_strHistory[m_nCurHistory];
			m_nCaretPos = m_strCmdLine.GetLength();
		}
		else if( nVirtKey == VK_DOWN && m_nCurHistory != -1	)
		{
			if( m_nNumHistories <= AUICONSOLE_MAX_HISTORY )
			{
				if( m_nCurHistory < m_nNumHistories - 1 )
					m_nCurHistory++;
				else
					m_nCurHistory = -1;
			}
			else
			{
				m_nCurHistory++;
				m_nCurHistory %= AUICONSOLE_MAX_HISTORY;
				if( m_nCurHistory == m_nHistoryQueueBegin )
					m_nCurHistory = -1;
			}
			
			if( m_nCurHistory >= 0 )
				m_strCmdLine = m_strHistory[m_nCurHistory];
			else
				m_strCmdLine = _AL("");
			m_nCaretPos = m_strCmdLine.GetLength();
		}
		// Result display area hisroty control.
		else if( nVirtKey == VK_PRIOR )
		{
			if( m_nNumDisplay > m_nLinesPerPage )
			{
				if( m_nFirstLine % AUICONSOLE_MAX_DISPLAY != m_nQueueBegin )
				{
					m_nFirstLine--;
					if( m_nFirstLine < 0 )
						m_nFirstLine += AUICONSOLE_MAX_DISPLAY;
				}
			}
		}
		else if( nVirtKey == VK_NEXT )
		{
			if( m_nNumDisplay > m_nLinesPerPage )
			{
				if( (m_nNumDisplay <= AUICONSOLE_MAX_DISPLAY &&
					m_nFirstLine + m_nLinesPerPage < m_nNumDisplay) ||
					(m_nNumDisplay > AUICONSOLE_MAX_DISPLAY &&
					(m_nFirstLine + m_nLinesPerPage) % AUICONSOLE_MAX_DISPLAY != m_nQueueBegin) )
				{
					m_nFirstLine++;
					m_nFirstLine %= AUICONSOLE_MAX_DISPLAY;
				}
			}
		}
		else if( nVirtKey != VK_RETURN )
			return false;

		return true;
	}
	else if( msg == WM_CHAR )
	{
		int nCmdLen = m_strCmdLine.GetLength();

		if( ACHAR(wParam) == ACHAR('~') )
		{
			m_pParent->Show(false);
		}
		else if( ACHAR(wParam) == ACHAR('\r') && nCmdLen > 0 )
		{
			AddLine(_AL(">") + m_strCmdLine, m_color);

			if( m_nNumHistories < AUICONSOLE_MAX_HISTORY )
			{
				m_strHistory[m_nNumHistories] = m_strCmdLine;
				m_nCurHistory = -1;
			}
			else
			{
				m_strHistory[m_nHistoryQueueBegin] = m_strCmdLine;
				m_nHistoryQueueBegin++;
				m_nHistoryQueueBegin %= AUICONSOLE_MAX_HISTORY;
				m_nCurHistory = -1;
			}
			m_nCaretPos = 0;
			m_nNumHistories++;
			m_szText = m_strCmdLine;
			m_strCmdLine = _AL("");
			if( m_strCommand != "" )
				m_pParent->SetCommand(m_strCommand);
			else
				m_pParent->SetCommand(m_szName);
		}
		else if( ACHAR(wParam) == ACHAR('\b') && m_nCaretPos > 0 )
		{
			m_strCmdLine = m_strCmdLine.Left(m_nCaretPos - 1)
				+ m_strCmdLine.Right(nCmdLen - m_nCaretPos);
			m_nCaretPos--;
		}
		else if( ACHAR(wParam) >= ACHAR(0x20) )
		{
			ACHAR szInput[2];
			szInput[0] = (ACHAR)wParam;
			szInput[1] = 0;
			if( int(strlen(AC2AS(m_strCmdLine))) < m_nMaxCmdLen )
			{
				m_strCmdLine = m_strCmdLine.Left(m_nCaretPos)
					+ szInput + m_strCmdLine.Right(nCmdLen - m_nCaretPos);
				m_nCaretPos++;
			}
		}

		return true;
	}
	else if( WM_MOUSEWHEEL == msg )
	{
		int zDelta = (short)HIWORD(wParam);
		if( zDelta > 0 )
			return OnDlgItemMessage(WM_KEYDOWN, VK_PRIOR, 0);
		else if( zDelta < 0 )
			return OnDlgItemMessage(WM_KEYDOWN, VK_NEXT, 0);
	}

	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIConsole::Tick(void)
{
	return AUIObject::Tick();
}

bool AUIConsole::Render(void)
{
	if( m_pParent->GetRenderState() == AUIDIALOG_RENDERSTATE_STATIC )
		return true;

	int nAlpha = int(m_pParent->GetWholeAlpha() * m_nAlpha / 255);

	POINT ptPos = GetPos();

	FitFont();
	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	int nTextW = 0, nTextH = 0;
	ACString strText = _AL(">") + m_strCmdLine.Left(m_nCaretPos);
	A3DPOINT2 ptSize;
	ptSize = pFont->GetTextExtent(strText);
	nTextW = ptSize.x;
	nTextH = ptSize.y;

	// Render display area.
	for( int i = 0; i < min(m_nLinesPerPage, m_nNumDisplay); i++ )
	{
		int y = ptPos.y + i * nTextH;
		A3DRECT rc(ptPos.x, y, ptPos.x + m_nWidth, y + nTextH + 1);

		int nIndex = m_nFirstLine + i;
		nIndex %= AUICONSOLE_MAX_DISPLAY;

		pFont->DrawText(ptPos.x, y, m_strDisplay[nIndex], (nAlpha << 24) | (0xFFFFFF & m_colorDisplay[nIndex]), &rc);
	}

	// Render command input area.
	int y = ptPos.y + (m_nLinesPerPage + 1) * nTextH;
	pFont->TextOut(ptPos.x, y, m_pEdit ? _AL(">") : _AL(">") + m_strCmdLine, (nAlpha << 24) | (0xFFFFFF & m_color));

	DWORD dwTick = GetTickCount();
	if( dwTick - m_nBlink > AUICONSOLE_BLINK_INTERVAL )
	{
		m_nBlink = dwTick;
		m_nShowCaret++;
	}
	if( m_nShowCaret % 2 && !m_pEdit )
	{
		pFont->TextOut(ptPos.x + nTextW, y + 1, _AL("_"), (nAlpha << 24) | (0xFFFFFF & m_color));
	}

	return true;
}

bool AUIConsole::GetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Font Name") )
		a_strncpy(Property->s, m_szFontName, AFILE_LINEMAXLEN - 1);
	else if( 0 == strcmpi(pszPropertyName, "Font Height") )
		Property->i = m_nFontHeight;
	else
		return AUIObject::GetProperty(pszPropertyName, Property);

	return true;
}

bool AUIConsole::SetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Font Name") )
		return SetFontAndColor(Property->s, m_nFontHeight, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType);
	else if( 0 == strcmpi(pszPropertyName, "Font Height") )
		return SetFontAndColor(m_szFontName, Property->i, m_color, m_nShadow, m_nOutline, m_bBold, m_bItalic, m_bFreeType);
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUIConsole::RegisterPropertyType()
{
	return	SetAUIObjectPropertyType("Font Name", AUIOBJECT_PROPERTY_VALUE_ACHAR) &&
			SetAUIObjectPropertyType("Font Height", AUIOBJECT_PROPERTY_VALUE_INT);
}

void AUIConsole::SetCommandLineColor(A3DCOLOR color)
{
	m_color = color;
}

void AUIConsole::AddLine(const ACHAR *pszLine, A3DCOLOR color)
{
	if( m_nNumDisplay < AUICONSOLE_MAX_DISPLAY )
	{
		m_strDisplay[m_nNumDisplay] = pszLine;
		m_colorDisplay[m_nNumDisplay] = color;
	}
	else
	{
		m_strDisplay[m_nQueueBegin] = pszLine;
		m_colorDisplay[m_nQueueBegin] = color;
		m_nQueueBegin++;
		m_nQueueBegin %= AUICONSOLE_MAX_DISPLAY;
	}
	m_nNumDisplay++;
	if( m_nNumDisplay > m_nLinesPerPage )
		m_nFirstLine = m_nNumDisplay - m_nLinesPerPage;
}

const ACHAR * AUIConsole::GetLastCommandLine()
{
	if( m_nNumHistories <= 0 )
		return _AL("");
	else if( m_nNumHistories <= AUICONSOLE_MAX_HISTORY )
		return m_strHistory[m_nNumHistories - 1];
	else
	{
		int nHistoryQueueEnd = m_nHistoryQueueBegin - 1;
		if( nHistoryQueueEnd < 0 )
			nHistoryQueueEnd += AUICONSOLE_MAX_HISTORY;
		return m_strHistory[nHistoryQueueEnd];
	}
}

void AUIConsole::GetHistory(abase::vector<ACString>& vecHistory)
{
	vecHistory.clear();
	if( m_nNumHistories <= 0 )
	{
		return ;
	}else if( m_nNumHistories <= AUICONSOLE_MAX_HISTORY )
	{
		for (unsigned int i=0; i<m_nNumHistories; ++i)
		{
			vecHistory.push_back(m_strHistory[i]);
		}
	}else
	{
		for (unsigned int i=0; i<AUICONSOLE_MAX_HISTORY; ++i)
		{
			int index = (m_nHistoryQueueBegin + i + AUICONSOLE_MAX_HISTORY)%AUICONSOLE_MAX_HISTORY;
			vecHistory.push_back(m_strHistory[index]);
		}
	}
}

void AUIConsole::ClearAll()
{
	ClearDisplay();
	ClearHistory();
}

void AUIConsole::ClearDisplay()
{
	m_nNumDisplay = m_nQueueBegin = m_nFirstLine = 0;
}

void AUIConsole::ClearHistory()
{
	m_nCurHistory = -1;
	m_nHistoryQueueBegin = m_nNumHistories = m_nCaretPos = 0;
}

void AUIConsole::FitScreen()
{
//	float fWindowScale = m_pParent->GetAUIManager()->GetWindowScale() * m_pParent->GetScale();
//	// If I am console, resize widths of parent dialog and me to fit screen.
//	int nWndW = m_pA3DDevice->GetDevFormat().nWidth;
//	m_pParent->SetSize(nWndW, m_pParent->GetSize().cy);
//
//	POINT ptPos = m_pParent->GetPos();
//
//	//	old : m_pParent->SetPos(-1, ptPos.y);
//	m_pParent->SetPosEx(0, ptPos.y, AUIDialog::alignCenter);
//
//	m_nWidth = nWndW - 2 * m_x;

	A3DPOINT2 ptSize;
	int nTextW = 0, nTextH = 0;
	
	FitFont();

	A3DFTFont *pFont = m_pFont ? m_pFont : m_pParent->GetAUIManager()->GetDefaultFont();
	ptSize = pFont->GetTextExtent(_AL("W"));
	nTextW = ptSize.x;
	nTextH = ptSize.y;

	m_nMaxCmdLen = m_nWidth / nTextW - 2;
	m_nLinesPerPage = m_nHeight / nTextH - 2;
	m_color = A3DCOLORRGB(255, 255, 255);

	if( m_pEdit )
	{
		SIZE s = m_pEdit->GetSize();
		m_pEdit->SetSize(m_nWidth - nTextW, s.cy);
		m_pEdit->SetPos(m_x + nTextW, m_y + (m_nLinesPerPage + 1) * nTextH);
	}
}

void AUIConsole::SetEditBox(PAUIOBJECT pEdit)
{
	m_pEdit = pEdit;
}

const ACHAR * AUIConsole::GetCommandLine()
{
	return m_strCmdLine;
}

void AUIConsole::SetCommandLine(const ACHAR *pszCmd)
{
	m_strCmdLine = pszCmd;
}
