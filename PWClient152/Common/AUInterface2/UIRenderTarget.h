/*
* FILE: UIRenderTarget.h
*
* DESCRIPTION: Class for respresenting the terrain in A3D Engine
*
* CREATED BY: Liyi, 2009/8/24
*
* HISTORY:
*
* Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _UI_RENDER_TARGET_H_
#define _UI_RENDER_TARGET_H_

#include "A3DDevObject.h"

class A3DRenderTarget;
class A3DTexture;
class A3DDevice;
class A3DStream;
class A3DSurface;
class A3DPixelShader;


class UIRenderTarget : public A3DDevObject
{
public:
	UIRenderTarget();
	~UIRenderTarget();

	bool Init(A3DDevice* pA3DDevice, int nWidth, int nHeight);
	void Release();

	//   (F*(1-a) + A*a) *(1-b) + B*b = Ff
	//   F*(1-a)*(1-b) + a*A*(1-b) + B*b = Ff
	//	 RTalpha = (1-a)*(1-b)
	//	 RTalpha A pass = 1 * (1-a) + a * 0;
	//   RTalpah B pass = (1-a)(1-b) + b * 0;
	//   RTColor = a*A *(1-b) + B *b 
	//   RTColor A pass = 0 * (1-a) + aA   
	//   RTColor B pass = aA*(1-b) + B * b

	bool BeginRenderRTAlpha();	//1
	bool CopyRTR2A();			//2 dx8 nv's driver bug 
	bool BeginRenderRT();		//3
	bool EndRenderRT();			//4
	
	
	bool Render(int nX, int nY);

	bool NeedRender();
	void SetNeedRender(bool bNeedRender);

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();
	
	void SaveRenderTarget(const char* szFileName);

	A3DRenderTarget* GetUIRenderTarget() { return m_pRenderTarget;}

	A3DRenderTarget* GetAlphaRT() {return m_pAlphaRT;}

	int GetWidth() const { return m_nWidth; }
	int GetHeight() const { return m_nHeight; }

protected:
	bool CreateStream();
	bool CreateRenderTarget();
	void FillStream(int nX, int nY);

protected:
	A3DDevice*			m_pA3DDevice;

	A3DRenderTarget*	m_pRenderTarget;
	A3DRenderTarget*	m_pAlphaRT;
	A3DPixelShader*		m_pCopyAlphaShader;

	A3DStream*			m_pStream;

	int					m_nWidth;
	int					m_nHeight;
	bool				m_bNeedRender;

};

#endif // _UI_RENDER_TARGET_H_