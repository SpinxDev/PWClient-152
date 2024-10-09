//----------------------------------------------------------
// Filename	: Brush.h
// Creator	: QingFeng Xin
// Date		: 2004.7.21
// Desc		: 画刷基本类，不能直接对象化，用的时候需要重载
//-----------------------------------------------------------

#ifndef _ELEMENT_BRUSH_H_
#define _ELEMENT_BRUSH_H_

#include "ElementMap.h"
#include "A3DMacros.h"

class A3DViewport;
class A3DVector;

class CElementBrush
{
public:
	CElementBrush()
	{ 
		m_dwColor = A3DCOLORRGB(255,255,0); 
	};
	virtual ~CElementBrush(){ };
	
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD dwTimeDelta);

	virtual void SetCenter(float x, float z);
	void GetCenter(float &x,float &z){ x = m_fCenterX; z = m_fCenterY; };
	void SetBrushColor( DWORD clr) { m_dwColor = clr; };

protected:
	virtual void DrawRect();
	virtual void DrawCircle();


	void DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum);	

private:	
	float m_fCenterX;
	float m_fCenterY;
	DWORD m_dwColor;
};

#endif// _ELEMENT_BRUSH_H_