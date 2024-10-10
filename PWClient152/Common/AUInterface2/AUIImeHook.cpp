 /*
 *	Created by Zhang Yu June 24, 2004
 *  hook functions for non-standard ime such as Zi Guang
 */
#include "AUI.h"

#include <Windows.h>

#include "AUIImeHook.h"
#include "StdIo.h"
#include "hashmap.h"
#include "AString.h"

using namespace abase;

#define		MAX_CANDIDATELIST_SIZE	70000
#define		HIDE_IME_WINDOW		1

HHOOK _hook_handle = NULL;

enum IME_TYPE 
{
	enumImeNone,
	enumUNISPIM,
	enumPYJJ,
	enumPYJJ311,
	enumWBJJ26,
	enumZNCQ,
	enumZNKP,
	enumMSPY
};

struct IME_WNDS
{
	LPCSTR szCanWndClsName;
	LPCSTR szComWndClsName;
	LPCSTR szStsWndClsName;
	IME_TYPE enumType;
};

IME_WNDS Ime_Wnds[] = {
	{ "UNISPIM_CANDCLS", "UNISPIM_COMPCLS", "UNISPIM_STATUSCLS", enumUNISPIM }, // ×Ï¹â3.0
	{ "UIM20_CANDCLS", "UIM20_COMPCLS", "UNISPIM_STATUSCLS", enumUNISPIM }, // ×Ï¹â2.0
	{ "", "PYJJCOMPUIWND", "PYJJSTATUSWND", enumPYJJ }, // Æ´Òô¼Ó¼Ó
	{ "", "JJ_COMPUI_WND", "JJ_STATUS_WND", enumPYJJ311 }, // Æ´Òô¼Ó¼Ó3.11
	{ "", "WBJJCOMPUIWND", "WBJJSTATUSWND", enumWBJJ26 }, // Îå±Ê¼Ó¼Óplus2.6
	{ "CANDZNWB", "COMPZNWB", "STATZNWB", enumZNCQ }, // ÖÇÄÜ³ÂÇÅ5.6
	{ "CSIME_CandidateWnd", "CSIME_CompositionWnd", "CSIME_StatusWnd", enumZNKP }, // ÖÇÄÜ¿ñÆ´3.3
	{ "", "MSPY Component", "", enumMSPY }, // Î¢ÈíÆ´Òô
	{ "ReadPINTLGNT", "CompPINTLGNT", "StatusPINTLGNT", enumMSPY }, // Î¢ÈíÆ´Òô2.0
};

static char pCanBuf[MAX_CANDIDATELIST_SIZE];
hash_map<AString, IME_TYPE> WndClsMap;
static PLONGLONG TextOutA_Entry = 0;
static LONGLONG TextOutA_Entry_Content = 0;
static LONGLONG TextOutA_Entry_Content_New = 0;
static PLONGLONG ExtTextOutW_Entry = 0;
static LONGLONG ExtTextOutW_Entry_Content = 0;
static LONGLONG ExtTextOutW_Entry_Content_New = 0;
static PLONGLONG CreateCompatibleDC_Entry = 0;
static LONGLONG CreateCompatibleDC_Entry_Content = 0;
static LONGLONG CreateCompatibleDC_Entry_Content_New = 0;
static PLONGLONG SetWindowPos_Entry = 0;
static LONGLONG SetWindowPos_Entry_Content = 0;
static LONGLONG SetWindowPos_Entry_Content_New = 0;
static PLONGLONG CreateWindowExW_Entry = 0;
static LONGLONG CreateWindowExW_Entry_Content = 0;
static LONGLONG CreateWindowExW_Entry_Content_New = 0;
static PLONGLONG CreateWindowExA_Entry = 0;
static LONGLONG CreateWindowExA_Entry_Content = 0;
static LONGLONG CreateWindowExA_Entry_Content_New = 0;
static int _is_hook = 0;
static HWND hwnd_can_list = 0;
static HWND hwnd_com_list = 0;
static HWND hwnd_status = 0;
static HWND hwnd_parent = 0;
static HWND hwnd_parent_ime = 0;
static HDC hdc_can_list = 0;
static HDC hdc_com_list = 0;
static IME_TYPE ime_type = enumImeNone;
static ime_str_val* _ime_str_val = NULL;
static HDC hdc_zncq = 0;

#define GET_SEQU(n) ((n) >= 9 ? '0' : (n) + '1')

inline BOOL is_num_char(char x)
{
	return (x >= '0' && x <= '9');
}

inline BOOL is_num_char(wchar_t x)
{
	return (x >= L'0' && x <= L'9');
}

inline BOOL is_eng_char(char x)
{
	return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z');
}

inline BOOL is_eng_char(wchar_t x)
{
	return (x >= L'a' && x <= L'z') || (x >= L'A' && x <= L'Z');
}

static void _GetNormComVal(HIMC h, ime_str_val* pVal)
{
	LONG lComSz = ImmGetCompositionString(h, GCS_COMPREADSTR, NULL, 0);
	if (lComSz >= 0)
	{
		char* pComBuf = new char[lComSz+2];
		ImmGetCompositionString(h, GCS_COMPREADSTR, pComBuf, lComSz);
		*(wchar_t*)&pComBuf[lComSz] = L'\0';
		if (strcmp(pComBuf, "") != 0)
			_ime_str_val->set_com_wstr((LPWSTR)pComBuf, wcslen((LPWSTR)pComBuf));
		delete[] pComBuf;
	}
}

static void _GetNormalComCanVal(ime_str_val* pVal)
{
	HIMC hIMC = ImmGetContext(hwnd_parent);

	if (ime_type != enumMSPY)
		_GetNormComVal(hIMC, pVal);
	else
		pVal->SetComPartReady(TRUE);

//	DWORD dwSize, dwCount = 1;
//	dwSize = ImmGetCandidateList(hIMC, 0, NULL, 0);
//	dwSize = ImmGetCandidateListCount(hIMC, &dwCount);
//	dwSize = 10000;

//	if (dwSize && dwCount)
//	{

		DWORD dwSize;
		CANDIDATELIST* pCand;
//		char* pCanBuf = new char[dwSize];
		pCand = (CANDIDATELIST*)pCanBuf;
		dwSize = ImmGetCandidateList(hIMC, 0, pCand, MAX_CANDIDATELIST_SIZE);

		if (dwSize)
		{
			pCand = (CANDIDATELIST*)pCanBuf;
			if (pCand->dwStyle == IME_CAND_READ)
			{
				DWORD start = pCand->dwPageStart;
				DWORD pagesize = pCand->dwPageSize;
				if( pCand->dwSelection < start )
					start -= pCand->dwPageSize;
				if( pCand->dwSelection >= start + pCand->dwPageSize )
					start += pCand->dwPageSize;
				if( pCand->dwSelection >= start && pCand->dwSelection < start + pCand->dwPageSize )
					_ime_str_val->set_can_sel(pCand->dwSelection - start);
				else
					_ime_str_val->set_can_sel(-1);
				for (DWORD i = 0; start + i < pCand->dwCount && i < pCand->dwPageSize; i++)
				{
					LPWSTR str = (LPWSTR)((char*)pCand + pCand->dwOffset[start + i]);

					_ime_str_val->push_can_wstr(str, wcslen(str));
				}
			}
		}

//		delete[] pCanBuf;
//	}
	ImmReleaseContext(hwnd_parent, hIMC);
}

static BOOL _ProcessCurComStr(LPCSTR szVal, int cb, BOOL bAddBlank)
{
	char* p = (char*)szVal;
	if (_ime_str_val->get_com_str() == NULL)
	{
		_ime_str_val->set_com_str(szVal, cb);
		return TRUE;
	}
	else if(!_ime_str_val->IsComPartReady())
	{
		if (is_eng_char(szVal[0]))
		{
			if (bAddBlank)
				_ime_str_val->append_com_str(" ", 1);
			_ime_str_val->append_com_str(szVal, cb);
			return TRUE;
		}
		else if(bAddBlank && cb > 1 && szVal[0] == ' ' && is_eng_char(szVal[1]))
		{
			_ime_str_val->append_com_str(szVal, cb);
			return TRUE;
		}
		else
			_ime_str_val->SetComPartReady(TRUE);
	}
	return FALSE;
}

static void _ProcessCurComWStr(LPCWSTR szVal, int cb)
{
	if (_ime_str_val->get_com_str() == NULL && is_eng_char(szVal[0]))
		_ime_str_val->set_com_wstr(szVal, cb);
}

static void _ProcessCurCanStr(LPCSTR szVal, int cb)
{
	char* p = (char*)szVal;
	if (_ime_str_val->IsFoundSequ())
	{
		_ime_str_val->push_can_str(szVal, cb);
		_ime_str_val->SetFoundSequ(FALSE);
	}
	else
	{
		char sequ = GET_SEQU(_ime_str_val->get_size());
		BOOL bFound = FALSE;
		char* buf = new char[cb+1];
		buf[0] = '\0';
		while (p < szVal+cb)
		{
			if (*p == sequ)
			{
				if (bFound)
				{
					_ime_str_val->push_can_str(buf, strlen(buf));
					buf[0] = '\0';
					sequ = GET_SEQU(_ime_str_val->get_size());
				}
				else
					bFound = TRUE;
			}
			else if (bFound)
			{
				if (IsDBCSLeadByte(*p))
				{
					strncat(buf, p, 2);
					p += 2;
					continue;
				}
				else if (buf[0] != '\0')
				{
					_ime_str_val->push_can_str(buf, strlen(buf));
					buf[0] = '\0';
					sequ = GET_SEQU(_ime_str_val->get_size());
					bFound = FALSE;
				}
			}
			p ++;
		}
		if (bFound)
		{
			if (buf[0] != '\0')
				_ime_str_val->push_can_str(buf, strlen(buf));
			else
				_ime_str_val->SetFoundSequ(TRUE);
		}
		delete[] buf;
	}
}

static void _ProcessImeStr(HDC hDC, LPCSTR szVal, int cb, BOOL bAddBlank)
{
	if (hDC == hdc_com_list && _ProcessCurComStr(szVal, cb, bAddBlank))
		return;
	_ProcessCurCanStr(szVal, cb);
}

static void _ProcessUNISPIM(HDC hDC, LPCSTR szVal, int cb)
{
	_ProcessImeStr(hDC, szVal, cb, FALSE);
}

static void _ProcessPYJJ(HDC hDC, LPCSTR szVal, int cb)
{
	_ProcessImeStr(hDC, szVal, cb, TRUE);
}

static void _ProcessZNKP(HDC hDC, LPCSTR szVal, int cb)
{
	_ProcessImeStr(hDC, szVal, cb, TRUE);
}

static void _ProcessWBJJ26(HDC hDC, LPCSTR szVal, int cb)
{
	BOOL bComStr = TRUE;
	for(int i = 0 ; i < cb ; i ++)
		if(!is_eng_char(szVal[i]))
		{
			bComStr = FALSE;
			break;
		}
	if( bComStr && _ime_str_val->get_com_str() == NULL)
		_ime_str_val->set_com_str(szVal, cb);
	else if(is_num_char(szVal[0]))
	{
		_ime_str_val->ClearCanLst();
		char* buf = new char[cb+1];
		strncpy(buf, szVal, cb);
		buf[cb] = '\0';
		char* p = (char*)buf;
		char* q = NULL;
		do
		{
			if(!is_num_char(p[0]))
				break;
			q = strchr(p,int(' '));
			if(q == NULL)
				_ime_str_val->push_can_str(p + 2, strlen(p) - 2);
			else
			{
				_ime_str_val->push_can_str(p + 2, q - p - 2);
				p = q + 2;
				if( p - buf > cb)
					break;
			}
		}while(q != NULL);
		delete []buf;
	}
}

static void _ProcessPYJJ311(HDC hDC, LPCWSTR szVal, int cb)
{
	if (!_ime_str_val->IsFoundSequ() && is_eng_char(szVal[0]))
	{
		_ime_str_val->append_com_wstr(szVal, cb);
		_ime_str_val->append_com_wstr(L" ", 1);
	}
	else if( !( cb == 1 && szVal[0] == L'.' ))
	{
		_ime_str_val->ClearComLst();
		wchar_t* p = (wchar_t*)szVal;
		if (_ime_str_val->IsFoundSequ())
		{
			_ime_str_val->push_can_wstr(szVal, cb);
			_ime_str_val->SetFoundSequ(FALSE);
		}
		else
		{
			wchar_t sequ = GET_SEQU(_ime_str_val->get_size());
			BOOL bFound = FALSE;
			wchar_t* buf = new wchar_t[cb+1];
			buf[0] = L'\0';
			while (p < szVal+cb)
			{
				if (*p == sequ)
				{
					if (bFound)
					{
						_ime_str_val->push_can_wstr(buf, wcslen(buf));
						buf[0] = L'\0';
						sequ = GET_SEQU(_ime_str_val->get_size());
					}
					else
						bFound = TRUE;
				}
				else if (bFound)
				{
					if(*p != L'.')
						wcsncat(buf, p, 1);
				}
				p ++;
			}
			if (bFound)
			{
				if (buf[0] != L'\0')
					_ime_str_val->push_can_wstr(buf, wcslen(buf));
				else
					_ime_str_val->SetFoundSequ(TRUE);
			}
			delete[] buf;
		}
	}
}

static void _ProcessZNCQ(HDC hDC, LPCSTR szVal, int cb)
{
	BOOL bComStr = TRUE;
	for(int i = 0 ; i < cb ; i ++)
		if(!is_eng_char(szVal[i]))
		{
			bComStr = FALSE;
			break;
		}
	if( bComStr && _ime_str_val->get_com_str() == NULL)
		_ime_str_val->set_com_str(szVal, cb);
	else if(is_num_char(szVal[0]))
	{
		_ime_str_val->ClearCanLst();
		char* buf = new char[cb+1];
		strncpy(buf, szVal, cb);
		buf[cb] = '\0';
		char* p = (char*)buf;
		char* q = NULL;
		do
		{
			if(!is_num_char(p[0]))
				break;
			q = strchr(p,int(' '));
			if(q == NULL)
				_ime_str_val->push_can_str(p + 2, strlen(p) - 2);
			else
			{
				_ime_str_val->push_can_str(p + 2, q - p - 2);
				p = q + 1;
			}
		}while(q != NULL);
		delete []buf;
	}
}

void _GetComCanLstVal(HDC hDC, LPCSTR szVal, int cb)
{
	assert(_ime_str_val);
	switch (ime_type)
	{
	case enumUNISPIM:
		_ProcessUNISPIM(hDC, szVal, cb);
		break;
	case enumPYJJ:
		_ProcessPYJJ(hDC, szVal, cb);
		break;
	case enumZNKP:
		_ProcessZNKP(hDC, szVal, cb);
		break;
	case enumWBJJ26:
		_ProcessWBJJ26(hDC, szVal, cb);
		break;
	default:
		break;
	}
}

extern bool g_bHideIme;

void GetComCanVal(ime_str_val* pVal, BOOL bNormMode)
{
	__try
	{
		if (!hwnd_parent) return;
		_ime_str_val = pVal;
		if (ime_type == enumImeNone || ime_type	== enumMSPY)
		{
			if (bNormMode)
			{
				pVal->Clear();
				_GetNormalComCanVal(pVal);
			}
			else if (ime_type == enumImeNone)
			{
				pVal->ClearComLst();
				HIMC hIMC = ImmGetContext(hwnd_parent);
				_GetNormComVal(hIMC, pVal);
				ImmReleaseContext(hwnd_parent, hIMC);
			}
		}
		else if (!bNormMode)
		{
			pVal->Clear();
			if (hwnd_can_list && IsWindowVisible(hwnd_can_list)) ::InvalidateRect(hwnd_can_list, NULL, TRUE);
			if (hwnd_com_list && IsWindowVisible(hwnd_com_list)) ::InvalidateRect(hwnd_com_list, NULL, TRUE);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
//		pVal->Clear();
//		g_bHideIme = false;
	}
}

inline void CHECK_CREATE_WINDOW(LPCSTR szClassName, HWND hWndParent, int& X, int& Y, int nWidth, int nHeight)
{
	BOOL bHide = FALSE;
	hash_map<AString, IME_TYPE>::iterator it = WndClsMap.find(AString(szClassName));
	if (it != WndClsMap.end())
	{
		ime_type = it.value()->second;
#ifdef HIDE_WINDOW
		bHide = TRUE;
#endif
	}
#ifdef HIDE_WINDOW
	else if (GetParent(hWndParent) == hwnd_parent_ime)
		bHide = TRUE;
#endif

	if (bHide)
	{
		X = 1 - nWidth;
		Y = 1 - nHeight;
	}
}

inline void CHECK_CREATE_WINDOW(LPCWSTR szClassName, HWND hWndParent, int& X, int& Y, int nWidth, int nHeight)
{
	CHECK_CREATE_WINDOW(CSafeString(szClassName).GetAnsi(), hWndParent, X, Y, nWidth, nHeight);
}

HWND WINAPI MyCreateWindowExA(
    DWORD dwExStyle,
    LPCSTR lpClassName,
    LPCSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent ,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
	HWND hRet = 0;

	__try
	{
		if( stricmp(lpClassName, "MSCandUIWindow_Candidate") != 0 )
		{
			if (HIWORD(lpClassName) != 0)
				CHECK_CREATE_WINDOW(lpClassName, hWndParent, X, Y, nWidth, nHeight);
		}

		*CreateWindowExA_Entry = CreateWindowExA_Entry_Content;
		hRet = CreateWindowExA(
			dwExStyle,
			lpClassName,
			lpWindowName,
			dwStyle,
			X,
			Y,
			nWidth,
			nHeight,
			hWndParent,
			hMenu,
			hInstance,
			lpParam);
		*CreateWindowExA_Entry = CreateWindowExA_Entry_Content_New;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return hRet;
}

HWND WINAPI MyCreateWindowExW(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X,
    int Y,
    int nWidth,
    int nHeight,
    HWND hWndParent ,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam)
{
	HWND hRet = 0;

	__try
	{
		if( a_stricmp(lpClassName, _AL("MSCandUIWindow_Candidate")) != 0 )
		{
			if (HIWORD(lpClassName) != 0)
				CHECK_CREATE_WINDOW(lpClassName, hWndParent, X, Y, nWidth, nHeight);
		}

		*CreateWindowExW_Entry = CreateWindowExW_Entry_Content;
		hRet = CreateWindowExW(
			dwExStyle,
			lpClassName,
			lpWindowName,
			dwStyle,
			X,
			Y,
			nWidth,
			nHeight,
			hWndParent,
			hMenu,
			hInstance,
			lpParam);

		*CreateWindowExW_Entry = CreateWindowExW_Entry_Content_New;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return hRet;
}

BOOL WINAPI MySetWindowPos(
    HWND hWnd,
    HWND hWndInsertAfter,
    int X,
    int Y,
    int cx,
    int cy,
    UINT uFlags)
{
	BOOL bRet = FALSE;

	__try
	{
		char szClassName[50];
		::GetClassNameA(hWnd, szClassName, 50);
		if (GetParent(GetParent(hWnd)) == hwnd_parent_ime || strcmp(szClassName, "GPYHORIZONTAL") == 0 || strcmp(szClassName, "GPYSTATUS") == 0)
		{
			if (strcmp("MSPY Component", szClassName) == 0
			 || strcmp("ReadPINTLGNT", szClassName) == 0)
			{
				hwnd_com_list = hWnd;
				ime_type = enumMSPY;
			}

	#if HIDE_IME_WINDOW
			if (uFlags & SWP_NOSIZE)
			{
				RECT rc;
				GetWindowRect(hWnd, &rc);
				X = 1 - rc.right + rc.left;
				Y = 1 - rc.bottom + rc.top;
			}
			else
			{
				X = 1 - cx;
				Y = 1 - cy;
			}
	#endif
		}

		*SetWindowPos_Entry = SetWindowPos_Entry_Content;
		bRet = SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
		*SetWindowPos_Entry = SetWindowPos_Entry_Content_New;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return bRet;
}

BOOL WINAPI MyTextOutA(HDC hDC, int nx, int ny, LPCSTR lp, int cb) 
{
	BOOL bShowText = TRUE, bRet = TRUE;
	__try
	{
		if (hDC == hdc_can_list || hDC == hdc_com_list)
		{
#if HIDE_IME_WINDOW
			bShowText = FALSE;
#endif
			if (_ime_str_val)
				_GetComCanLstVal(hDC, lp, cb);
		}
		if (hDC == hdc_zncq && ime_type == enumZNCQ)
			_ProcessZNCQ(hDC, lp, cb);
		if (bShowText)
		{
			*TextOutA_Entry = TextOutA_Entry_Content;
			bRet = TextOutA(hDC, nx, ny, lp, cb);
			*TextOutA_Entry = TextOutA_Entry_Content_New;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return bRet;
}

BOOL WINAPI MyExtTextOutW(
	HDC hDC,
	int x,
	int y,
	UINT op,
	CONST RECT * lprc,
	LPCWSTR lp,
	UINT c,
	CONST INT *dx)
{
	BOOL bShowText = TRUE, bRet = TRUE;
	__try
	{
		if (hDC == hdc_can_list || hDC == hdc_com_list)
		{
#if HIDE_IME_WINDOW
			bShowText = FALSE;
#endif
			if (_ime_str_val && !_ime_str_val->IsComPartReady())
			{
				switch(ime_type)
				{
					case enumPYJJ311:
						_ProcessPYJJ311(hDC, lp, c);
						break;
					default:
						_ime_str_val->ClearComLst();
						_ProcessCurComWStr(lp, c);
				}
			}
		}
		if (bShowText)
		{
			*ExtTextOutW_Entry = ExtTextOutW_Entry_Content;
			bRet = ExtTextOutW(hDC, x, y, op, lprc, lp, c, dx);
			*ExtTextOutW_Entry = ExtTextOutW_Entry_Content_New;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return bRet;
}

HDC WINAPI MyCreateCompatibleDC(HDC hDC)
{
	HDC hRet = 0;

	__try
	{
		*CreateCompatibleDC_Entry = CreateCompatibleDC_Entry_Content;
		hRet = CreateCompatibleDC(hDC);
		*CreateCompatibleDC_Entry = CreateCompatibleDC_Entry_Content_New;
		HWND hWndIme = ::WindowFromDC(hDC);
		if (hWndIme)
		{
			if (hWndIme == hwnd_can_list)
				hdc_can_list = hRet;
			if (hWndIme == hwnd_com_list)
				hdc_com_list = hRet;
			if (ime_type == enumZNCQ && hWndIme == hwnd_status)
				hdc_zncq = hDC;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return hRet;
}

#define HookGdiFunc(h, f_name, f_entry, f_content, f_content_new) \
{ \
	DWORD dwTemp; \
	f_entry = (PLONGLONG)GetProcAddress(h, #f_name); \
	if (VirtualProtect(f_entry, 16, PAGE_EXECUTE_READWRITE, &dwTemp)) { \
		f_content = f_content_new = *f_entry; \
		unsigned char* p = (unsigned char*)&f_content_new; \
		*p++ = (unsigned char)0xe9; \
		*(DWORD*)p = DWORD((DWORD)&My##f_name - 5 - (DWORD)f_entry); \
		*f_entry = f_content_new; } \
}

BOOL CALLBACK VerifyImeWndProc(HWND hWnd, LPARAM)
{
	char szClassName[50];
	::GetClassNameA(hWnd, szClassName, 50);
	if (hwnd_parent_ime == 0 && strcmp(szClassName, "IME") == 0)
	{
		hwnd_parent_ime = hWnd;
		return FALSE;
	}

	int nSize = sizeof(Ime_Wnds) / sizeof(IME_WNDS);
	for (int i = 0; i < nSize; i++)
	{
		BOOL bFind = FALSE;
		if (strcmp(szClassName, Ime_Wnds[i].szCanWndClsName) == 0)
		{
			hwnd_can_list = hWnd;
			bFind = TRUE;
		}
		else if (strcmp(szClassName, Ime_Wnds[i].szComWndClsName) == 0)
		{
			hwnd_com_list = hWnd;
			ime_type = Ime_Wnds[i].enumType;
			bFind = TRUE;
		}
		else if (strcmp(szClassName, Ime_Wnds[i].szStsWndClsName) == 0)
		{
			hwnd_status = hWnd;
			bFind = TRUE;
		}
		if (bFind)
		{
			RECT rc;
			GetWindowRect(hWnd, &rc);
			SetWindowPos(hWnd, 
				NULL,
				1 - rc.right + rc.left,
				1 - rc.bottom + rc.top,
				0,
				0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
	if (hwnd_status && hwnd_can_list && hwnd_com_list) return FALSE;
	return TRUE;
}

void OnChangeIme(HWND hParent)
{
	hwnd_parent = hParent;
	hwnd_can_list = 0;
	hwnd_com_list = 0;
	hdc_zncq = 0;
	ime_type = enumImeNone;
	EnumThreadWindows(GetCurrentThreadId(), VerifyImeWndProc, NULL);
	hdc_can_list = 0;
	hdc_com_list = 0;
	hdc_zncq = 0;
}

void StartImeHook()
{
	if (_is_hook == 0)
	{
		HMODULE h = ::LoadLibraryA("gdi32.dll");
		HookGdiFunc(h, CreateCompatibleDC, CreateCompatibleDC_Entry, CreateCompatibleDC_Entry_Content, CreateCompatibleDC_Entry_Content_New)
		HookGdiFunc(h, TextOutA, TextOutA_Entry, TextOutA_Entry_Content, TextOutA_Entry_Content_New)
		HookGdiFunc(h, ExtTextOutW, ExtTextOutW_Entry, ExtTextOutW_Entry_Content, ExtTextOutW_Entry_Content_New)
		::FreeLibrary(h);

		h = ::LoadLibraryA("user32.dll");
		HookGdiFunc(h, SetWindowPos, SetWindowPos_Entry, SetWindowPos_Entry_Content, SetWindowPos_Entry_Content_New)
		HookGdiFunc(h, CreateWindowExW, CreateWindowExW_Entry, CreateWindowExW_Entry_Content, CreateWindowExW_Entry_Content_New)
		HookGdiFunc(h, CreateWindowExA, CreateWindowExA_Entry, CreateWindowExA_Entry_Content, CreateWindowExA_Entry_Content_New)
		::FreeLibrary(h);

		WndClsMap.clear();
		for (int i = 0; i < sizeof(Ime_Wnds)/sizeof(IME_WNDS); i++)
		{
			if (strcmp(Ime_Wnds[i].szCanWndClsName, "") != 0)
				WndClsMap[Ime_Wnds[i].szCanWndClsName] = Ime_Wnds[i].enumType;
			if (strcmp(Ime_Wnds[i].szComWndClsName, "") != 0)
				WndClsMap[Ime_Wnds[i].szComWndClsName] = Ime_Wnds[i].enumType;
			if (strcmp(Ime_Wnds[i].szStsWndClsName, "") != 0)
				WndClsMap[Ime_Wnds[i].szStsWndClsName] = Ime_Wnds[i].enumType;
		}
		EnumThreadWindows(GetCurrentThreadId(), VerifyImeWndProc, NULL);
	}
	_is_hook ++;
}

void EndImeHook()
{
	_is_hook --;
	if (_is_hook == 0)
	{
		if (TextOutA_Entry && TextOutA_Entry_Content)
			*TextOutA_Entry = TextOutA_Entry_Content;
		if (ExtTextOutW_Entry && ExtTextOutW_Entry_Content)
			*ExtTextOutW_Entry = ExtTextOutW_Entry_Content;
		if (CreateCompatibleDC_Entry && CreateCompatibleDC_Entry_Content)
			*CreateCompatibleDC_Entry = CreateCompatibleDC_Entry_Content;
		if (SetWindowPos_Entry && SetWindowPos_Entry_Content)
			*SetWindowPos_Entry = SetWindowPos_Entry_Content;
		if (CreateWindowExW_Entry && CreateWindowExW_Entry_Content)
			*CreateWindowExW_Entry = CreateWindowExW_Entry_Content;
		if (CreateWindowExA_Entry && CreateWindowExA_Entry_Content)
			*CreateWindowExA_Entry = CreateWindowExA_Entry_Content;
	}
}
