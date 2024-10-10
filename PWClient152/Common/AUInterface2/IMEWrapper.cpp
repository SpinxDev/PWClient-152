//--------------------------------------------------------------------------------------
// File: IMEWrapper.cpp
//--------------------------------------------------------------------------------------
#include "AUI.h"
#include "stdio.h"
#include "IMEWrapper.h"
#include "assert.h"
//#pragma comment(lib,"version.lib")

#undef min // use __min instead
#undef max // use __max instead

#define UNISCRIBE_DLLNAME L"usp10.dll"

#define GETPROCADDRESS( Module, APIName, Temp ) \
	Temp = GetProcAddress( Module, #APIName ); \
	if( Temp ) \
	*(FARPROC*)&_##APIName = Temp

#define PLACEHOLDERPROC( APIName ) \
	_##APIName = Dummy_##APIName

#define IMM32_DLLNAME L"imm32.dll"
#define VER_DLLNAME L"version.dll"

#if defined(DEBUG) || defined(_DEBUG)
#define IMETRACE                  PWOutputDebugString
#else
#define IMETRACE                  
#endif

#ifdef UNICODE
#define PWOutputDebugString PWOutputDebugStringW
#else
#define PWOutputDebugString PWOutputDebugStringA
#endif
#define IME_MAX_EDITBOXLENGTH 0xFFFF

IMEWrapper   g_ImeWrapper;

HWND IMEWrapper::m_hWnd = NULL;
//--------------------------------------------------------------------------------------
// IMEWrapper class
//--------------------------------------------------------------------------------------
// IME constants

POINT                       IMEWrapper::s_ptCompString;      // Composition string position. Updated every frame.
int                         IMEWrapper::s_nFirstTargetConv;  // Index of the first target converted char in comp string.  If none, -1.
CXUniBuffer                 IMEWrapper::s_CompString = CXUniBuffer( 0 );
DWORD IMEWrapper::s_adwCompStringClause[MAX_COMPSTRING_SIZE];
WCHAR IMEWrapper::s_wszReadingString[32];
IMEWrapper::CCandList  IMEWrapper::s_CandList;       // Data relevant to the candidate list
bool                        IMEWrapper::s_bImeFlag = true;


#if defined(DEBUG) || defined(_DEBUG)
bool      IMEWrapper::m_bIMEStaticMsgProcCalled = false;
#endif

//--------------------------------------------------------------------------------------
// Outputs to the debug stream a formatted Unicode string with a variable-argument list.
//--------------------------------------------------------------------------------------
VOID WINAPI PWOutputDebugStringW( LPCWSTR strMsg, ... )
{
#if defined(DEBUG) || defined(_DEBUG)
	WCHAR strBuffer[512];

	va_list args;
	va_start(args, strMsg);
#if _MSC_VER >= 1300
	vswprintf_s( strBuffer, 512, strMsg, args );
#else
	vswprintf( strBuffer, strMsg, args );
#endif
	strBuffer[511] = L'\0';
	va_end(args);

	OutputDebugString( strBuffer );
#else
	UNREFERENCED_PARAMETER( strMsg );
#endif
}


//--------------------------------------------------------------------------------------
// Outputs to the debug stream a formatted MBCS string with a variable-argument list.
//--------------------------------------------------------------------------------------
VOID WINAPI PWOutputDebugStringA( LPCSTR strMsg, ... )
{
#if defined(DEBUG) || defined(_DEBUG)
	CHAR strBuffer[512];

	va_list args;
	va_start(args, strMsg);
#if _MSC_VER >= 1300
	vsprintf_s( strBuffer, 512, strMsg, args );
#else
	vsprintf( strBuffer, strMsg, args );
#endif
	strBuffer[511] = '\0';
	va_end(args);

	OutputDebugStringA( strBuffer );
#else
	UNREFERENCED_PARAMETER( strMsg );
#endif
}

//--------------------------------------------------------------------------------------
HRESULT IMEWrapper::CreateIMEWrapper()
{
    return S_OK;
}


//--------------------------------------------------------------------------------------
void IMEWrapper::InitDefaultElements( )
{
    //-------------------------------------
    // IMEWrapper
    //-------------------------------------
}


//--------------------------------------------------------------------------------------
IMEWrapper::IMEWrapper( )
{
	m_hWnd = NULL;
}


//--------------------------------------------------------------------------------------
IMEWrapper::~IMEWrapper()
{
}


//--------------------------------------------------------------------------------------
void IMEWrapper::SendKey( BYTE nVirtKey )
{
    keybd_event( nVirtKey, 0, 0, 0 );
    keybd_event( nVirtKey, 0, KEYEVENTF_KEYUP, 0 );
}


//--------------------------------------------------------------------------------------
//  GetImeId( UINT uIndex )
//      returns 
//  returned value:
//  0: In the following cases
//      - Non Chinese IME input locale
//      - Older Chinese IME
//      - Other error cases
//
//  Othewise:
//      When uIndex is 0 (default)
//          bit 31-24:  Major version
//          bit 23-16:  Minor version
//          bit 15-0:   Language ID
//      When uIndex is 1
//          pVerFixedInfo->dwFileVersionLS
//
//  Use IMEID_VER and IMEID_LANG macro to extract version and language information.
//  

// We define the locale-invariant ID ourselves since it doesn't exist prior to WinXP
// For more information, see the CompareString() reference.
#define LCID_INVARIANT MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT)
//--------------------------------------------------------------------------------------
// Enable/disable the entire IME system.  When disabled, the default IME handling
// kicks in.
void IMEWrapper::EnableImeSystem( bool bEnable )
{
    ImeUi_EnableIme( bEnable );
}


//--------------------------------------------------------------------------------------
// Resets the composition string.
void IMEWrapper::ResetCompositionString()
{
    s_CompString.SetText( L"" );
}


//--------------------------------------------------------------------------------------
// This function is used only briefly in CHT IME handling,
// so accelerator isn't processed.
void IMEWrapper::PumpMessage()
{
    MSG msg;

    while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
    {
        if( !GetMessageW( &msg, NULL, 0, 0 ) )
        {
            PostQuitMessage( ( int )msg.wParam );
            return;
        }
        TranslateMessage( &msg );
        DispatchMessageA( &msg );
    }
}


//--------------------------------------------------------------------------------------
void IMEWrapper::OnFocusIn(bool bCanChangeIME)
{
	extern DWORD g_dwTSFThreadId;
	if (g_dwTSFThreadId != GetCurrentThreadId())
	{
		assert(false);
		return;
	}
	s_bImeFlag = bCanChangeIME;
    ImeUi_EnableIme( s_bImeFlag );
}


//--------------------------------------------------------------------------------------
void IMEWrapper::OnFocusOut()
{
	extern DWORD g_dwTSFThreadId;
	if (g_dwTSFThreadId != GetCurrentThreadId())
	{
		return;
	}
    ImeUi_FinalizeString();
    ImeUi_EnableIme( false );
}


//--------------------------------------------------------------------------------------
bool IMEWrapper::StaticMsgProc( HWND hWnd, UINT uMsg, WPARAM& wParam, LPARAM& lParam )
{

    if( !ImeUi_IsEnabled() )
        return false;

#if defined(DEBUG) || defined(_DEBUG)
    m_bIMEStaticMsgProcCalled = true;
#endif

    switch( uMsg )
    {
        case WM_INPUTLANGCHANGE:
            IMETRACE( L"WM_INPUTLANGCHANGE\n" );
            {
            }
            return true;

        case WM_IME_SETCONTEXT:
            IMETRACE( L"WM_IME_SETCONTEXT\n" );
            //
            // We don't want anything to display, so we have to clear this
            //
            lParam = 0;
            return false;
            // Handle WM_IME_STARTCOMPOSITION here since
            // we do not want the default IME handler to see
            // this when our fullscreen app is running.
        case WM_IME_STARTCOMPOSITION:
            IMETRACE( L"WM_IME_STARTCOMPOSITION\n" );
            ResetCompositionString();
            return true;
        case WM_IME_ENDCOMPOSITION:
            IMETRACE( L"WM_IME_ENDCOMPOSITION\n" );
            return false;
        case WM_IME_COMPOSITION:
            IMETRACE( L"WM_IME_COMPOSITION\n" );
            return false;
    }

    return false;
}


//--------------------------------------------------------------------------------------
bool IMEWrapper::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	//Êó±ê²Ù×÷
	return false;
}


//--------------------------------------------------------------------------------------
bool IMEWrapper::MsgProc( UINT uMsg, WPARAM& wParam, LPARAM& lParam )
{
#if defined(DEBUG) || defined(_DEBUG)
    // DXUT.cpp used to call IMEWrapper::StaticMsgProc() so that, but now
    // this is the application's responsiblity.  To do this, call 
    // CDXUTDialogResourceManager::MsgProc() before calling this function.
    assert( m_bIMEStaticMsgProcCalled && L"To fix, call CDXUTDialogResourceManager::MsgProc() first" );
#endif
    switch( uMsg )
    {
	case WM_DESTROY:
		//ImeUi_Uninitialize();
		break;
	case WM_QUIT:
		ImeUi_Uninitialize();
		break;
    }	

    bool trappedData;
    bool* trapped = &trappedData;

    *trapped = false;
    if( !ImeUi_IsEnabled() )
	{
        /*return CDXUTEditBox::MsgProc( uMsg, wParam, lParam );*/
	}

    ImeUi_ProcessMessage(GetHWnd(), uMsg, wParam, lParam, trapped );
    if( *trapped == false )
	{
        /*CDXUTEditBox::MsgProc( uMsg, wParam, lParam );*/
	}

    return *trapped;
}

HWND IMEWrapper::SetHWnd(HWND hwnd)
{
	HWND xhwnd = m_hWnd;
	m_hWnd = hwnd;
	ImeUi_SetWindow(m_hWnd);
	return xhwnd;
}
HWND IMEWrapper::GetHWnd()
{
	return m_hWnd;
}
//--------------------------------------------------------------------------------------
void IMEWrapper::RenderCandidateReadingWindow( float fElapsedTime, bool bReading )
{
	return;
}


//--------------------------------------------------------------------------------------
void IMEWrapper::RenderComposition( float fElapsedTime )
{
    return;
}


//--------------------------------------------------------------------------------------
void IMEWrapper::RenderIndicator( float fElapsedTime )
{
	return;
}


//--------------------------------------------------------------------------------------
void IMEWrapper::Render( float fElapsedTime )
{
	return;
}


//--------------------------------------------------------------------------------------
void IMEWrapper::SetImeEnableFlag( bool bFlag )
{
    s_bImeFlag = bFlag;
}


LONG lFirstIME = 0;
//--------------------------------------------------------------------------------------
void IMEWrapper::Initialize( HWND hWnd )
{
	extern DWORD g_dwTSFThreadId;
	if (g_dwTSFThreadId != GetCurrentThreadId())
	{
		assert(false);
		return;
	}
	LONG x = InterlockedIncrement(&lFirstIME);
	if(x == 1 )
	{
		ImeUiCallback_DrawRect = NULL;
		ImeUiCallback_Malloc = malloc;
		ImeUiCallback_Free = free;
		ImeUiCallback_DrawFans = NULL;
		ImeUi_Initialize( hWnd );
	}
	s_CompString.SetBufferSize( MAX_COMPSTRING_SIZE );
	//ImeUi_EnableIme( true );
}


//--------------------------------------------------------------------------------------
void IMEWrapper::Uninitialize()
{
	extern DWORD g_dwTSFThreadId;
	if (g_dwTSFThreadId != GetCurrentThreadId())
	{
		assert(false);
		return;
	}

    ImeUi_EnableIme( false );
    ImeUi_Uninitialize( );
}


//--------------------------------------------------------------------------------------
void CXUniBuffer::Initialize()
{

}


//--------------------------------------------------------------------------------------
void CXUniBuffer::Uninitialize()
{

}


//--------------------------------------------------------------------------------------
bool CXUniBuffer::SetBufferSize( int nNewSize )
{
	// If the current size is already the maximum allowed,
	// we can't possibly allocate more.
	if( m_nBufferSize == IME_MAX_EDITBOXLENGTH )
		return false;

	int nAllocateSize = ( nNewSize == -1 || nNewSize < m_nBufferSize * 2 ) ? ( m_nBufferSize ? m_nBufferSize *
		2 : 256 ) : nNewSize * 2;

	// Cap the buffer size at the maximum allowed.
	if( nAllocateSize > IME_MAX_EDITBOXLENGTH )
		nAllocateSize = IME_MAX_EDITBOXLENGTH;

	WCHAR* pTempBuffer = new WCHAR[nAllocateSize];
	if( !pTempBuffer )
		return false;

	ZeroMemory( pTempBuffer, sizeof( WCHAR ) * nAllocateSize );

	if( m_pwszBuffer )
	{
		CopyMemory( pTempBuffer, m_pwszBuffer, m_nBufferSize * sizeof( WCHAR ) );
		delete[] m_pwszBuffer;
	}

	m_pwszBuffer = pTempBuffer;
	m_nBufferSize = nAllocateSize;
	return true;
}





//--------------------------------------------------------------------------------------
CXUniBuffer::CXUniBuffer( int nInitialSize )
{
	CXUniBuffer::Initialize();  // ensure static vars are properly init'ed first

	m_nBufferSize = 0;
	m_pwszBuffer = NULL;

	if( nInitialSize > 0 )
		SetBufferSize( nInitialSize );
}


//--------------------------------------------------------------------------------------
CXUniBuffer::~CXUniBuffer()
{
	delete[] m_pwszBuffer;
}


//--------------------------------------------------------------------------------------
WCHAR& CXUniBuffer::operator[]( int n )  // No param checking
{
	// This version of operator[] is called only
	// if we are asking for write access, so
	// re-analysis is required.

	return m_pwszBuffer[n];
}


//--------------------------------------------------------------------------------------
void CXUniBuffer::Clear()
{
	*m_pwszBuffer = L'\0';
}


//--------------------------------------------------------------------------------------
// Inserts the char at specified index.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CXUniBuffer::InsertChar( int nIndex, WCHAR wChar )
{
	assert( nIndex >= 0 );

	if( nIndex < 0 || nIndex > lstrlenW( m_pwszBuffer ) )
		return false;  // invalid index

	// Check for maximum length allowed
	if( GetTextSize() + 1 >= IME_MAX_EDITBOXLENGTH )
		return false;

	if( lstrlenW( m_pwszBuffer ) + 1 >= m_nBufferSize )
	{
		if( !SetBufferSize( -1 ) )
			return false;  // out of memory
	}

	assert( m_nBufferSize >= 2 );

	// Shift the characters after the index, start by copying the null terminator
	WCHAR* dest = m_pwszBuffer + lstrlenW( m_pwszBuffer ) + 1;
	WCHAR* stop = m_pwszBuffer + nIndex;
	WCHAR* src = dest - 1;

	while( dest > stop )
	{
		*dest-- = *src--;
	}

	// Set new character
	m_pwszBuffer[ nIndex ] = wChar;

	return true;
}


//--------------------------------------------------------------------------------------
// Removes the char at specified index.
// If nIndex == -1, remove the last char.
//--------------------------------------------------------------------------------------
bool CXUniBuffer::RemoveChar( int nIndex )
{
	if( !lstrlenW( m_pwszBuffer ) || nIndex < 0 || nIndex >= lstrlenW( m_pwszBuffer ) )
		return false;  // Invalid index

	MoveMemory( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof( WCHAR ) *
		( lstrlenW( m_pwszBuffer ) - nIndex ) );
	return true;
}


//--------------------------------------------------------------------------------------
// Inserts the first nCount characters of the string pStr at specified index.
// If nCount == -1, the entire string is inserted.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CXUniBuffer::InsertString( int nIndex, const WCHAR* pStr, int nCount )
{
	assert( nIndex >= 0 );
	if( nIndex < 0 )
		return false;

	if( nIndex > lstrlenW( m_pwszBuffer ) )
		return false;  // invalid index

	if( -1 == nCount )
		nCount = lstrlenW( pStr );

	// Check for maximum length allowed
	if( GetTextSize() + nCount >= IME_MAX_EDITBOXLENGTH )
		return false;

	if( lstrlenW( m_pwszBuffer ) + nCount >= m_nBufferSize )
	{
		if( !SetBufferSize( lstrlenW( m_pwszBuffer ) + nCount + 1 ) )
			return false;  // out of memory
	}

	MoveMemory( m_pwszBuffer + nIndex + nCount, m_pwszBuffer + nIndex, sizeof( WCHAR ) *
		( lstrlenW( m_pwszBuffer ) - nIndex + 1 ) );
	CopyMemory( m_pwszBuffer + nIndex, pStr, nCount * sizeof( WCHAR ) );

	return true;
}


//--------------------------------------------------------------------------------------
bool CXUniBuffer::SetText( LPCWSTR wszText )
{
	assert( wszText != NULL );

	int nRequired = int( wcslen( wszText ) + 1 );

	// Check for maximum length allowed
	if( nRequired >= IME_MAX_EDITBOXLENGTH )
		return false;

	while( GetBufferSize() < nRequired )
		if( !SetBufferSize( -1 ) )
			break;
	// Check again in case out of memory occurred inside while loop.
	if( GetBufferSize() >= nRequired )
	{
#if _MSC_VER >= 1300
		wcscpy_s( m_pwszBuffer, GetBufferSize(), wszText );
#else
		wcscpy( m_pwszBuffer, wszText );
#endif
		return true;
	}
	else
		return false;
}







