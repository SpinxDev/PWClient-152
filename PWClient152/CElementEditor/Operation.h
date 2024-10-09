//----------------------------------------------------------
// Filename	: Operation.h
// Creator	: QingFeng Xin
// Date		: 2004.7.21
// Desc		: 对场景的基本操作类，希望对场景的所有操作都用
//			  此类做为基本类统一接口，可以添加成员函数，
//            成员变量，但不能减少成员函数，成员变量。
//-----------------------------------------------------------

#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "windows.h"
#include "ElementMap.h"



class COperation
{
public:
	COperation()
	{ 
	};
	virtual ~COperation(){};
	
	virtual bool OnEvent(UINT message, WPARAM wParam, LPARAM lParam){return true;};
	virtual void Tick(DWORD dwTimeDelta){};
	virtual void Render(A3DViewport* pA3DViewport){};

	//	Mouse action handler,
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags) { return true; }
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags) { return true; }
	virtual void Release(){}

protected:
	
private:
	
	
};

typedef COperation* POPERATION;

#endif //_OPERATION_H_