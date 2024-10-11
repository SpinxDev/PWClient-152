/*
 *	Created by Zhang Yu June 24, 2004
 *  hook functions for non-standard ime such as Zi Guang
 */

#ifndef	_AUIIMEHOOK_H__
#define _AUIIMEHOOK_H__

#include <assert.h>
#include "AVariant.h"

#define		MAX_CAN_LIST_COUNT	10

#define		CODE_PAGE			CP_ACP

class ime_str_val
{
protected:
	LPWSTR m_com_val;
	LPWSTR m_can_lst_val[MAX_CAN_LIST_COUNT];
	int m_cal_sel;
	int can_lst_size;
	BOOL com_part_ready;
	BOOL found_sequ;
public:
	ime_str_val()
	{
		m_com_val = NULL;
		can_lst_size = 0;
		com_part_ready = FALSE;
		found_sequ = FALSE;
		m_cal_sel = 0;
	}
	virtual ~ime_str_val() { Clear(); }
	int get_size() const { return can_lst_size;	}
	void push_can_str(LPCSTR szStr, int nLen)
	{
		if (can_lst_size == MAX_CAN_LIST_COUNT) return;
		CSafeString wsz(szStr, nLen);
		m_can_lst_val[can_lst_size] = new wchar_t[wcslen(wsz)+1];
		wcscpy(m_can_lst_val[can_lst_size], wsz);
		can_lst_size ++;
	}
	void push_can_wstr(LPCWSTR wszStr, int nLen)
	{
		if (can_lst_size == MAX_CAN_LIST_COUNT) return;
		m_can_lst_val[can_lst_size] = new wchar_t[nLen+1];
		wcsncpy(m_can_lst_val[can_lst_size], wszStr, nLen);
		m_can_lst_val[can_lst_size][nLen] = L'\0';
		can_lst_size ++;
	}
	void set_can_sel(int nSel)
	{
		m_cal_sel = nSel;
	}
	void set_com_wstr(LPCWSTR wszStr, int nLen)
	{
		assert(m_com_val == NULL);
		m_com_val = new wchar_t[nLen+1];
		wcsncpy(m_com_val, wszStr, nLen);
		m_com_val[nLen] = L'\0';
	}
	void append_com_wstr(LPCWSTR szStr, int nLen)
	{
		if (m_com_val == NULL)
			set_com_wstr(szStr, nLen);
		else
		{
			int nOldLen = wcslen(m_com_val);
			int nNewLen = nOldLen + nLen;
			wchar_t* p = new wchar_t[nNewLen+1];
			wcscpy(p, m_com_val);
			wcsncpy(p + nOldLen, szStr, nLen);
			p[nNewLen] = L'\0';
			delete[] m_com_val;
			m_com_val = p;
		}
	}
	void set_com_str(LPCSTR szStr, int nLen)
	{
		assert(m_com_val == NULL);
		CSafeString wsz(szStr, nLen);
		m_com_val = new wchar_t[wcslen(wsz)+1];
		wcscpy(m_com_val, wsz);
	}
	void append_com_str(LPCSTR szStr, int nLen)
	{
		if (m_com_val == NULL)
			set_com_str(szStr, nLen);
		else
		{
			int nOldLen = wcslen(m_com_val);
			CSafeString wsz(szStr, nLen);
			int nNewLen = nOldLen + wcslen(wsz);
			wchar_t* p = new wchar_t[nNewLen+1];
			wcscpy(p, m_com_val);
			wcscpy(p+nOldLen, wsz);
			delete[] m_com_val;
			m_com_val = p;
		}
	}
	void Clear() 
	{
		for (int i = 0; i < can_lst_size; i++)
			delete[] m_can_lst_val[i];
		can_lst_size = 0;
		delete m_com_val;
		m_com_val = NULL;
		com_part_ready = FALSE;
		found_sequ = FALSE;
	}
	void ClearComLst()
	{
		delete m_com_val;
		m_com_val = NULL;
		com_part_ready = FALSE;
	}
	void ClearCanLst()
	{
		for (int i = 0; i < can_lst_size; i++)
			delete[] m_can_lst_val[i];
		can_lst_size = 0;
		found_sequ = FALSE;
	}
	void SetComPartReady(BOOL b) { com_part_ready = b; }
	void SetFoundSequ(BOOL b) { found_sequ = b;	}
	BOOL IsFull() { return can_lst_size == MAX_CAN_LIST_COUNT; }
	BOOL IsComPartReady() const { return com_part_ready; }
	BOOL IsFoundSequ() const { return found_sequ; }
	LPWSTR get_com_str() { return m_com_val; }
	LPWSTR get_can_str(int nIndex) { return m_can_lst_val[nIndex]; }
	int get_can_sel() { return m_cal_sel; }
};

void StartImeHook();
void EndImeHook();
void OnChangeIme(HWND hParent);
void GetComCanVal(ime_str_val* pVal, BOOL bNormMode);

#endif