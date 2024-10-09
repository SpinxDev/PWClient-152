// SetVernierOperation.h: interface for the CSetVernierOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETVERNIEROPERATION_H__5D9AF368_609A_4E76_A675_325637AEC1EC__INCLUDED_)
#define AFX_SETVERNIEROPERATION_H__5D9AF368_609A_4E76_A675_325637AEC1EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Operation.h"
class CSetVernierOperation :public COperation     
{

public:
	CSetVernierOperation();
	virtual ~CSetVernierOperation();
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

};

#endif // !defined(AFX_SETVERNIEROPERATION_H__5D9AF368_609A_4E76_A675_325637AEC1EC__INCLUDED_)
