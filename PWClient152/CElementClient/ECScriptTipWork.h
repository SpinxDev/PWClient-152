// Filename	: ECScriptTipWork.h
// Creator	: zhangyitian
// Date		: 2014/07/24

#ifndef _ELEMENTCLIENT_ECSCRIPTTIPWORK_H_
#define _ELEMENTCLIENT_ECSCRIPTTIPWORK_H_

#include "AAssist.h"

class CDlgScriptTip;

class CECScriptTipWork {
public:
	CECScriptTipWork(CDlgScriptTip* pDlgScriptTip);
	virtual ~CECScriptTipWork();
	void Tick();
	bool IsFinished();
protected:
	virtual void OnFirstTick() = 0;
	virtual void OnTick() = 0;
	void SetFinished(bool bFinished);
protected:
	CDlgScriptTip* m_pDlgScriptTip;	
private:
	bool m_bFirstTick;
	bool m_bFinished;
};

//////////////////////////////////////////

class CECScriptTipWorkSetTitleAndContent : public CECScriptTipWork {
public:
	CECScriptTipWorkSetTitleAndContent(CDlgScriptTip* pDlgScriptTip, 
		const ACString& strTitle, const ACString& strContent);
protected:
	virtual void OnFirstTick();
	virtual void OnTick();
private:
	ACString m_strTitle;
	ACString m_strContent;	
};

///////////////////////////////////////////

class CECScriptTipWorkMove : public CECScriptTipWork {
public:
	CECScriptTipWorkMove(CDlgScriptTip* pDlgScriptTip, float fStart, float fEnd, unsigned int uiTotalTime, bool bStopGfx = false);
protected:
	virtual void OnFirstTick();
	virtual void OnTick();
private:
	float m_fStart;
	float m_fEnd;
	bool m_bStopGfx;
	unsigned int m_uiTotalTime;
	DWORD m_dwStartTime;
};

////////////////////////////////////////////

class CECScriptTipWorkGfx : public CECScriptTipWork {
public:
	CECScriptTipWorkGfx(CDlgScriptTip* pDlgScriptTip, unsigned int uiTotalTime);
protected:
	virtual void OnFirstTick();
	virtual void OnTick();
private:
	unsigned int m_uiTotalTime;
	DWORD m_dwStartTime;
};

#endif