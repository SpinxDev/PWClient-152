// SpeciallyAddOperation.h: interface for the CSpeciallyAddOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPECIALLYADDOPERATION_H__F358CBFC_0F85_414B_AE30_03C0CF426119__INCLUDED_)
#define AFX_SPECIALLYADDOPERATION_H__F358CBFC_0F85_414B_AE30_03C0CF426119__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Operation.h"
#include "SceneSpeciallyObject.h"

class CSpeciallyAddOperation  :public COperation     
{
public:
	CSpeciallyAddOperation();
	virtual ~CSpeciallyAddOperation();
	
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);

	void CreateSpecially();

private:
	CSceneSpeciallyObject m_Specially;
};

#endif // !defined(AFX_SPECIALLYADDOPERATION_H__F358CBFC_0F85_414B_AE30_03C0CF426119__INCLUDED_)
