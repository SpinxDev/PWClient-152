// Filename	: ECScriptTipWorkMan.cpp
// Creator	: zhangyitian
// Date		: 2014/07/24

#include "ECScriptTipWorkMan.h"
#include "ECScriptTipWork.h"

CECScriptTipWorkMan::CECScriptTipWorkMan() {
	m_pCurWork = NULL;
}

CECScriptTipWorkMan::~CECScriptTipWorkMan() {
	if (m_pCurWork) {
		delete m_pCurWork;
	}
	std::deque<CECScriptTipWork*>::iterator itr;
	for (itr = m_workList.begin(); itr != m_workList.end(); ++itr) {
		delete *itr;
	}
}

void CECScriptTipWorkMan::AddWork(CECScriptTipWork* pWork) {
	m_workList.push_back(pWork);
}

void CECScriptTipWorkMan::Tick() {
	if (m_pCurWork == NULL && m_workList.empty()) {
		return;
	}
	if (m_pCurWork == NULL) {
		m_pCurWork = m_workList.front();
		m_workList.pop_front();
	}
	m_pCurWork->Tick();
	if (m_pCurWork->IsFinished()) {
		delete m_pCurWork;
		m_pCurWork = NULL;
	}
}

bool CECScriptTipWorkMan::IsFree() {
	return m_pCurWork == NULL && m_workList.empty();
}