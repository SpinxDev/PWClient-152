/*
 * FILE: A3DRenderTarget.h
 *
 * DESCRIPTION: class representing a render target;
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DRENDERTARGET_H_
#define _A3DRENDERTARGET_H_


#include "A3DTypes.h"
#include "A3DPlatform.h"
#include "A3DDevObject.h"
#include "A3DSurface.h"

class A3DStream;
class A3DDevice;

class  A3DRenderTarget : public A3DDevObject
{
public:		//	Types

	//	Render target format
	struct RTFMT
	{
		/*	iWidth, iHeight: size of target. there are some pre-defined value:

				0: same as current device's size
				-2: 1/2 of current device's size
				-4: 1/4 of current device's size
		*/
		int			iWidth;
		int			iHeight;
		A3DFORMAT	fmtTarget;
		A3DFORMAT	fmtDepth;	//	A3DFMT_UNKNOWN means to use the format got from A3DDevice::GetDepthFormat()

		RTFMT()
		{
			iWidth		= 100;
			iHeight		= 100;
			fmtTarget	= A3DFMT_A8R8G8B8;
			fmtDepth	= A3DFMT_D16;
		}
	};

	friend class A3DRenderTargetMan;

public:

	/*	Check if specified color format is supported

		Return value:

			0: not supported
			0x01: can be created as a texture
			0x02: can be created as a surface
	*/
	static DWORD IsColorFormatSupported(A3DDevice* pA3DDevice, A3DFORMAT fmtColor);

	A3DRenderTarget();
	virtual ~A3DRenderTarget();

	//	Initialize object. This init() create a texture as render target
	bool Init(A3DDevice* pA3DDevice, const RTFMT& rtFmt, bool bUseColor, bool bUseDepth, bool bAutoMipMap=false);
	//	Initialize object. This init() create a surface as render target
	bool Init(A3DDevice* pA3DDevice, const RTFMT& rtFmt, bool bUseColor, bool bUseDepth, A3DMULTISAMPLE_TYPE mst, DWORD msq);
	//	Initialize object. This init() create a NULL render target that is supported by some hardware
	//	This function will return false if A3DDeviceCaps::SupportNullRT return false;
	bool InitAsNullRT(A3DDevice* pA3DDevice, const RTFMT& rtFmt, A3DMULTISAMPLE_TYPE ms, DWORD msq);
	//	Release object
	bool Release();

	//	Present content to device's current back buffer
	//	iWidth, iHeight: size on back buffer, 0 means use render target's own size
	bool PresentToBack(int x, int y, int iWidth=0, int iHeight=0);

	//	Export content to file
	bool ExportColorToFile(char * szFullpath);
	bool ExportDepthToFile(char * szFullpath);

	//	Apply as texture
	bool AppearAsTexture(int nLayer, bool bDepthBuffer = false);
	bool DisappearAsTexture(int nLayer);

	//	Apply view to device
	bool ApplyToDevice(int iRTIndex=0);
	//	Withdraw view from device
	void WithdrawFromDevice();

	//	Set/Get the flag that identify render target has been filled content
	bool HasFilled() const { return m_bHasFilled; }
	void SetHasFilled() { m_bHasFilled = true; }
	//	Get auto mipmap flag
	bool IsAutoMipMap() const { return m_bAutoMipMap; }
	//	Get render target format
	const RTFMT& GetFormat() const { return m_rtFmt; }
	//	Get render target current size
	int GetWidth() const { return m_iCurWidth; }
	int GetHeight() const { return m_iCurHeight; }
	//	Get current depth buffer format
	A3DFORMAT GetDepthFormat() const { return m_fmtCurDepth; }

	//	Get interfaces
	IDirect3DTexture9* GetTargetTexture() { return m_pColorTexture; }
	IDirect3DTexture9* GetDepthTexture() { return m_pDepthTexture; }
	A3DSurfaceProxy* GetTargetSurface() { return &m_ColorSurf; }
	A3DSurfaceProxy* GetDepthSurface() { return &m_DepthSurf; }

protected:

	A3DDevice*			m_pA3DDevice;
	A3DStream*			m_pA3DStream;

	IDirect3DTexture9*	m_pColorTexture;
	IDirect3DTexture9*	m_pDepthTexture;
	A3DSurfaceProxy		m_ColorSurf;
	A3DSurfaceProxy		m_DepthSurf;

	A3DMULTISAMPLE_TYPE	m_mst;		//	Multismaple type
	DWORD				m_msq;		//	Multismaple quality

	RTFMT		m_rtFmt;			//	Passed in format when render target is created
	int			m_iCurWidth;		//	Current width of render target
	int			m_iCurHeight;		//	Current height of render target
	A3DFORMAT	m_fmtCurDepth;		//	Current depth buffer format
	bool		m_bUseColor;		//	Create color buffer
	bool		m_bUseDepth;		//	Create depth buffer
	bool		m_bUsingTexture;	//	Create as RT texture
	bool		m_bAutoMipMap;		//	Generate mip map automatically
	bool		m_bHasFilled;
	bool		m_bIsNullRT;		//	true, is a NULL RT
	bool		m_bInitFinished;	//	true, initialization was finished
	DWORD		m_dwRentTime;		//	Time when the target is rented, 0 means never rented
	int			m_idAppied;			//	id got when render target applied to device

protected:

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

	//	Do some work to finish initialization
	void FinishInit();
	//	Create and Release surfaces
	bool CreatePlains();
	bool ReleasePlains();
	//	Get RT size in bytes
	int GetRenderTargetSize();
};

#endif