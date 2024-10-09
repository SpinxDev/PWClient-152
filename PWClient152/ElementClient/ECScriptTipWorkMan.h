// Filename	: ECScriptTipWorkMan.h
// Creator	: zhangyitian
// Date		: 2014/07/24

#ifndef _ELEMENTCLIENT_ECSCRIPTTIPWORKMAN_H_
#define _ELEMENTCLIENT_ECSCRIPTTIPWORKMAN_H_

#include <deque>

class CECScriptTipWork;

class CECScriptTipWorkMan {
public:
	CECScriptTipWorkMan();
	~CECScriptTipWorkMan();
	void AddWork(CECScriptTipWork* pWork);
	void Tick();
	bool IsFree();
private:
	std::deque<CECScriptTipWork*> m_workList;
	CECScriptTipWork* m_pCurWork;
};

#endif