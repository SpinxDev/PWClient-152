/*
* FILE: A3DDeviceCaps.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: Liyi, 2012/2/29
*
* HISTORY:
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _A3D_DEVICE_CAPS_H_
#define _A3D_DEVICE_CAPS_H_

#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	class A3DDeviceCaps
//	
///////////////////////////////////////////////////////////////////////////

class A3DDeviceCaps
{
public:		//	Types

	//	Direct DepthBuffer Access type
	enum DDBA_TYPE
	{
		DDBA_NONE = 0,	//	Can't access depth buffer
		DDBA_RAWZ,		//	Support RAW-Z
		DDBA_INTZ,		//	Support INT-Z
	};

	friend class A3DDevice;

public:		//	Constructors and Destructors

	A3DDeviceCaps();
	~A3DDeviceCaps();

public:		//	Attributes

public:		//	Operations

	//	Check caps
	void CheckCaps(A3DDevice* pA3DDevice);

	//	Get caps
	bool SupportR16F_RT() const { return m_bSupRT_R16F; }
	bool SupportR32F_RT() const { return m_bSupRT_R32F; }
	bool SupportGR16F_RT() const { return m_bSupRT_GR16F; }
	bool SupportGR32F_RT() const { return m_bSupRT_GR32F; }
	bool SupportABGR16F_RT() const { return m_bSupRT_ABGR16F; }
	bool SupportABGR32F_RT() const { return m_bSupRT_ABGR32F; }

	bool SupportHWShadowPCF() const { return m_bSupHWShadowPCF; }
	bool SupportInstance() const { return m_bSupInstances; }
	bool SupportAnisoFilter() const { return m_bSupAnisoFilter; }
	bool SupportMinAnisoFilter() const { return m_bSupMinAnisoFilter; }
	bool SupportMagAnisoFilter() const { return m_bSupMagAnisoFilter; }
	bool SupportsRGBRead() const { return m_bSupsRGBRead; }
	bool SupportNullRT() const { return m_bSupNullRT; }
	bool SupportLPPGBuf() const { return m_bSupLPPGBuf; }
	DDBA_TYPE GetDDBAType() const { return m_typeDDBA; }
	bool SupportMRTs() const { return m_bSupMRTs; }
	bool SupportDiffBitMRTs() const { return m_bSupDiffBitMRT; }
	bool SupportSimuBloom() const { return m_bSupSimuBloom; }
	bool SupportStreamOffset() const { return m_bSupStreamOffset; }

protected:	//	Attributes

	A3DDevice*	m_pA3DDevice;		//	A3D device object

	bool		m_bSupRT_R16F;			//	true, support D3DFMT_R16F render target
	bool		m_bSupRT_R32F;			//	true, support D3DFMT_R32F render target
	bool		m_bSupRT_GR16F;			//	true, support D3DFMT_G16R16F render target
	bool		m_bSupRT_GR32F;			//	true, support D3DFMT_G32R32F render target
	bool		m_bSupRT_ABGR16F;		//	true, support D3DFMT_A16B16G16R16F render target
	bool		m_bSupRT_ABGR32F;		//	true, support D3DFMT_A32B32G32R32F render target

	bool		m_bSupHWShadowPCF;		//	true, support Hardware PCF for shadow map
	bool		m_bSupInstances;		//	true, support instance rendering
	bool		m_bSupAnisoFilter;		//	true, support anisotropic for one of min or mag texture filter
	//	m_bSupAnisoFilter = m_bSupMinAnisoFilter | m_bSupMagAnisoFilter
	bool		m_bSupMinAnisoFilter;	//	true, support anisotropic min texture filter
	bool		m_bSupMagAnisoFilter;	//	true, support anisotropic mag texture filter
	bool		m_bSupsRGBRead;			//	true, support sRGB read
	bool		m_bSupNullRT;			//	true, support NULL render target
	bool		m_bSupLPPGBuf;			//	true, support LPP G-Buffer render target
	DDBA_TYPE	m_typeDDBA;				//	Direct DepthBuffer Access type
	bool		m_bSupMRTs;				//	true, support Multiple Render Targets
	bool		m_bSupDiffBitMRT;		//	true, support different bit depth MRT
	bool		m_bSupSimuBloom;		//	true, support simultaneous bloom rendering
	bool		m_bSupStreamOffset;		//	true, support D3DDEVCAPS2::D3DDEVCAPS2_STREAMOFFSET

protected:	//	Operations

};

#endif		//	_A3D_DEVICE_CAPS_H_