//--------------------------------------------------------------------------------------
// File: IMEWrapper.h
//--------------------------------------------------------------------------------------
#pragma once
#ifndef IME_WRAPPER_H
#define IME_WRAPPER_H

#include <usp10.h>
#if _MSC_VER < 1300
#include <msctf.h>
#else
#include <dimm.h>
#endif
#include "ImeUi.h"


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class IMEWrapper;


//-----------------------------------------------------------------------------
// CXUniBuffer class for the edit control
//-----------------------------------------------------------------------------
class CXUniBuffer
{
public:
	CXUniBuffer( int nInitialSize = 1 );
	~CXUniBuffer();

	static void WINAPI      Initialize();
	static void WINAPI      Uninitialize();

	int                     GetBufferSize()
	{
		return m_nBufferSize;
	}
	bool                    SetBufferSize( int nSize );
	int                     GetTextSize()
	{
		return lstrlenW( m_pwszBuffer );
	}
	const WCHAR* GetBuffer()
	{
		return m_pwszBuffer;
	}
	const WCHAR& operator[]( int n ) const
	{
		return m_pwszBuffer[n];
	}
	WCHAR& operator[]( int n );
	void                    Clear();
	bool                    InsertChar( int nIndex, WCHAR wChar ); // Inserts the char at specified index. If nIndex == -1, insert to the end.
	bool                    RemoveChar( int nIndex );  // Removes the char at specified index. If nIndex == -1, remove the last char.
	bool                    InsertString( int nIndex, const WCHAR* pStr, int nCount = -1 );  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
	bool                    SetText( LPCWSTR wszText );

private:

	WCHAR* m_pwszBuffer;    // Buffer to hold text
	int m_nBufferSize;      // Size of the buffer allocated, in characters



};

//-----------------------------------------------------------------------------
// IME-enabled EditBox control
//-----------------------------------------------------------------------------
#define MAX_COMPSTRING_SIZE 256


class IMEWrapper 
{
public:
	enum
	{
		INDICATOR_NON_IME,
		INDICATOR_CHS,
		INDICATOR_CHT,
		INDICATOR_KOREAN,
		INDICATOR_JAPANESE
	};
	struct CCandList
	{
		CXUniBuffer HoriCand; // Candidate list string (for horizontal candidate window)
		int nFirstSelected;   // First character position of the selected string in HoriCand
		int nHoriSelectedLen; // Length of the selected string in HoriCand
		RECT rcCandidate;     // Candidate rectangle computed and filled each time before rendered
	};
public:
	IMEWrapper();
	virtual                 ~IMEWrapper();

    static HRESULT          CreateIMEWrapper();
    static void             InitDefaultElements();
    static void WINAPI      Initialize( HWND hWnd );
    static void WINAPI      Uninitialize();
    static  HRESULT WINAPI  StaticOnCreateDevice();
    static  bool    WINAPI  StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM& wParam, LPARAM& lParam );
    static  void WINAPI     SetImeEnableFlag( bool bFlag );
	static  HWND            SetHWnd(HWND hwnd);
	static  HWND            GetHWnd();

    bool                    MsgProc( UINT uMsg, WPARAM& wParam, LPARAM& lParam );
    static void				OnFocusIn(bool bCanChangeIME);
    static void             OnFocusOut();
    void                    PumpMessage();
    virtual void            Render( float fElapsedTime );
    virtual void            RenderCandidateReadingWindow( float fElapsedTime, bool bReading );
    virtual void            RenderComposition( float fElapsedTime );
    virtual void            RenderIndicator( float fElapsedTime );
    virtual bool            HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	static CXUniBuffer s_CompString;       // Buffer to hold the composition string (we fix its length)
	static DWORD            s_adwCompStringClause[MAX_COMPSTRING_SIZE];
	static CCandList s_CandList;          // Data relevant to the candidate list
	static WCHAR            s_wszReadingString[32];// Used only with horizontal reading window (why?)
	static void WINAPI      EnableImeSystem( bool bEnable );
protected:

    static WORD WINAPI      GetLanguage()
    {
        return ImeUi_GetLanguage();
    }
    static WORD WINAPI      GetPrimaryLanguage()
    {
        return ImeUi_GetPrimaryLanguage();
    }
    static void WINAPI      SendKey( BYTE nVirtKey );
    static DWORD WINAPI     GetImeId( UINT uIndex = 0 )
    {
        return ImeUi_GetImeId( uIndex );
    };
    static void WINAPI      CheckInputLocale();
    static void WINAPI      CheckToggleState();
    static void WINAPI      SetupImeApi();
    static void WINAPI      ResetCompositionString();
    static void             SetupImeUiCallback();

protected:

    static POINT s_ptCompString;        // Composition string position. Updated every frame.
    static int s_nFirstTargetConv;      // Index of the first target converted char in comp string.  If none, -1.
    static bool s_bImeFlag;			  // Is ime enabled 
	static HWND m_hWnd;
#if defined(DEBUG) || defined(_DEBUG)
    static bool    m_bIMEStaticMsgProcCalled;
#endif
};

extern IMEWrapper   g_ImeWrapper;

#endif // IME_WRAPPER_H
