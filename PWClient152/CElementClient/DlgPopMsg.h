// Filename	: DlgPopMsg.h
// Creator	: XuWenbin
// Date		: 2012/8/3

#pragma once

#include "DlgBase.h"

class CDlgPopMsg : public CDlgBase
{
public:
	CDlgPopMsg();

	void Add(const ACHAR* pszMsg);	
    void Align();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
	virtual void OnChangeLayoutEnd(bool bAllDone);

private:

	PAUIOBJECT	m_pTxt_Message;

	enum State
	{
		STATE_EMPTY = 0,
		STATE_ENTER,
		STATE_KEEPING,
		STATE_LEAVE,
	};
    State	m_state;

    DWORD	m_dwNPCSpeakTime;
    POINT	m_Center;
    SIZE	m_Size;
};