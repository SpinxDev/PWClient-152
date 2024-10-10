/*
 * FILE: A3DRenderTargetMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/7/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DRENDERTARGETMAN_H_
#define _A3DRENDERTARGETMAN_H_

#include "A3DTypes.h"
#include "AStack.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;
class A3DAdditionalView;
class A3DRenderTarget;
class A3DCubeRenderTarget;
class A3DSurfaceProxy;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DRenderTargetMan
//	
///////////////////////////////////////////////////////////////////////////

class  A3DRenderTargetMan
{
public:		//	Types

	//	Render target surfaces info
	struct RT_SURFS
	{
		A3DSurfaceProxy*	pTargetBuffer;	//	Target frame buffer
		A3DSurfaceProxy*	pDepthBuffer;	//	Depth-Stencil buffer

		RT_SURFS() :
		pTargetBuffer(NULL),
		pDepthBuffer(NULL)
		{
		}
	};

	typedef AStack<RT_SURFS, RT_SURFS&> RTStack;

	friend class A3DDevice;
	friend class A3DRenderTarget;
	friend class A3DCubeRenderTarget;
	friend class A3DAdditionalView;

public:		//	Constructor and Destructor

	A3DRenderTargetMan();
	virtual ~A3DRenderTargetMan();

public:		//	Attributes

public:		//	Operations

	/*	Rent render target from manager

		iWid, iHei: size of target. there are some pre-defined value:
				0: same as current device's size
				-2: 1/2 of current device's size
				-4: 1/4 of current device's size
		iEdgeSize: edge size of cube target
		fmtColor: color buffer format
		fmtDepth: depth buffer format. A3DFMT_UNKNOWN means to use the format got from A3DDevice::GetDepthFormat()
		bAutoMipMap: true, automatically generate mipmap
	*/
	A3DRenderTarget* RentRenderTargetColor(int iWid, int iHei, A3DFORMAT fmtColor, bool bAutoMipMap=false);
	A3DRenderTarget* RentRenderTargetDepth(int iWid, int iHei, A3DFORMAT fmtDepth);
	A3DCubeRenderTarget* RentRenderTargetCube(int iEdgeSize, A3DFORMAT fmtColor);
	//	Return render target to manager
	void ReturnRenderTarget(A3DRenderTarget* pRT);
	void ReturnCubeRenderTarget(A3DCubeRenderTarget* pRT);
	//	Release all rentable render targets that are not rent. This method can be used to release
	//	redundant render targets resources.
	void ClearRentableRenderTargets();

	//	Get number of rentable render targets that have been ever created
	int GetTotalRentableRTNum() const { return m_iRentableRTNum; }

	//	Get current (the top most) render target's size
	//	bColorRT: true, get color render target's size; false, get depth render target's size
	void GetCurRTSize(bool bColorRT, int& iWidth, int& iHeight, int iRTIndex=0);

	inline void IncRenderTargetSize(int nSize) { m_nRenderTargetSize += nSize; }
	inline int GetRenderTargetSize() { return m_nRenderTargetSize; }

protected:	//	Attributes

	A3DDevice*		m_pA3DDevice;			//	Device object
	int				m_iRentableRTNum;		//	Total rentable render target number
	int				m_iRentableCubeRTNum;	//	Total cube rentable render target number

	APtrArray<RTStack*>				m_aRTStacks;		//	Render stacks
	APtrArray<A3DRenderTarget*>		m_aRentableRTs;		//	Rentable render targets
	APtrArray<A3DCubeRenderTarget*>	m_aRentableCubeRTs;	//	Rentable cube render targets
	int								m_nRenderTargetSize;

protected:	//	Operations

	//	Initialize object
	bool Init(A3DDevice* pA3DDevice);
	//	Release object
	void Release();

	//	Reset render targets
	bool Reset();

	//	Push render target
	bool PushRenderTarget(int iRTIndex, A3DSurfaceProxy* pTargetBuffer, A3DSurfaceProxy* pDepthBuffer);
	bool PushRenderTarget(int iRTIndex, A3DRenderTarget* pRenderTarget);
	bool PushRenderTarget(int iRTIndex, A3DAdditionalView* pAdditionalView);
	//	Pop render target
	bool PopRenderTarget(int iRTIndex);
	//	Peek render target but not pop it
	bool PeekRenderTarget(int iRTIndex, RT_SURFS& rtSurfs);
	//	Get render target depth
	int GetRTStackDepth(int iRTIndex);

	//	Remove render target that was long-time not rented
	void ReleaseIdleRenderTarget(bool bCubeRT);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DRENDERTARGETMAN_H_
