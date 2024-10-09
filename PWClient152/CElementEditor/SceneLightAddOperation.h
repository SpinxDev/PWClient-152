///----------------------------------------------------------
// Filename	: SceneLightAddOperation.h
// Creator	: QingFeng Xin
// Date		: 2004.8.16
// Desc		: ²Ù×÷ÍØÕ¹Àà¡£
//-----------------------------------------------------------

#if !defined(AFX_SCENELIGHTADDOPERATION_H__A19057EE_9238_44A8_B660_5C03D6B9750A__INCLUDED_)
#define AFX_SCENELIGHTADDOPERATION_H__A19057EE_9238_44A8_B660_5C03D6B9750A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Operation.h"
#include "SceneLightObject.h"

class CSceneLightAddOperation :public COperation 
{
public:
	CSceneLightAddOperation();
	virtual ~CSceneLightAddOperation();

	virtual bool OnEvent(UINT message, WPARAM wParam, LPARAM lParam){ return true;};
	virtual void Tick(DWORD dwTimeDelta){};
	virtual void Render(A3DViewport* pA3DViewport);

	//	Mouse action handler,
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags) { return true; }

	void CreateLight();
	
private:
	CSceneLightObject m_TempLightObject;

};

#endif // !defined(AFX_SCENELIGHTADDOPERATION_H__A19057EE_9238_44A8_B660_5C03D6B9750A__INCLUDED_)
