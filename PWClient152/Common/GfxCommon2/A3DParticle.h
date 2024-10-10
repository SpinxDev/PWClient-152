/*
 * FILE: A3DParticle.h
 *
 * DESCRIPTION: Particle Object
 *
 * CREATED BY: ZhangYu, 2004/8/3
 *
 * HISTORY:
 *
 */

#ifndef _A3DPARTICLE_H_
#define _A3DPARTICLE_H_

#include "A3DTypes.h"
#include "A3DMacros.h"
#include "A3DGFXElement.h"

class A3DParticle
{
public:
	A3DParticle() : m_vDir(_unit_dir), m_pDummy(0) 
	{
		m_fTTL = 0;
		m_fTotalTTL = 0;
		m_fRotAngle = 0;
		m_fVelAlongAcc = 0;
		m_fSelfVel = 0;
		m_fWidth = 0;
		m_fHeight = 0;
		m_fScale = 0;
		m_fScaleNoise = 0;
		m_fTexU = 0;
		m_fTexV = 0;
		m_dwTickCounter = 0;
		m_nCurTexCol = 0;
		m_nCurTexRow = 0;
	}
	~A3DParticle() { Reset(); }

public:
	// 粒子剩余的Time to live
	float			m_fTTL;
	// 总共的生命期
	float			m_fTotalTTL;
	// 旋转速度
	float			m_fRotAngle;
	// 色彩
	A3DCOLOR		m_color;
	// 位置
	A3DVECTOR3		m_vPos;
	A3DVECTOR3		m_vOldPos;
	A3DVECTOR3		m_vOrgOffset;
	// 方向
	A3DQUATERNION	m_vDir;
	A3DVECTOR3		m_vMoveDir;
	A3DVECTOR3		m_vAxisOff;
	mutable A3DVECTOR3		m_vOldMoveDir;

	// 沿加速度方向的分速度
	float			m_fVelAlongAcc;
	// 自身速度大小
	float			m_fSelfVel;
	// 大小
	float			m_fWidth;
	float			m_fHeight;
	float			m_fScale;
	float			m_fScaleNoise;
	A3DGFXElement*	m_pDummy;

	// Particle UV animation support
	float			m_fTexU;
	float			m_fTexV;
	DWORD			m_dwTickCounter;
	int				m_nCurTexCol;
	int				m_nCurTexRow;

public:
	void Reset()
	{
		if (m_pDummy)
		{
			m_pDummy->Release();
			delete m_pDummy;
			m_pDummy = 0;
		}
	}

	// When particle is Add to the active list, call this function
	inline void InitParticleUV()
	{
		m_dwTickCounter = 0;
		m_nCurTexCol = m_nCurTexRow = 0;
	}

	inline void InitParticleUV(int nTexRow, int nTexCol, float fTexWidth, float fTexHeight)
	{
		m_dwTickCounter = 0;
		m_nCurTexRow = nTexRow;
		m_nCurTexCol = nTexCol;

		m_fTexU = fTexWidth * m_nCurTexCol;
		m_fTexV = fTexHeight * m_nCurTexRow;
	}

	// this function is the same as the A3DGFXElement::TexAnimation's None-Tile Mode
	inline void CalcParticlesUV(DWORD dwTickTime, DWORD dwTexInterval, int nRowNum, int nColNum, float fTexWidth, float fTexHeight)
	{
		m_dwTickCounter += dwTickTime;
		if (m_dwTickCounter < dwTexInterval)
			return;
		m_dwTickCounter -= dwTexInterval;
				
		if (++m_nCurTexCol >= nColNum)
		{
			if (++m_nCurTexRow >= nRowNum)
			{
				m_nCurTexRow = 0;
				m_nCurTexCol = 0;
			}
			else
				m_nCurTexCol = 0;
		}
		m_fTexU = fTexWidth * m_nCurTexCol;
		m_fTexV = fTexHeight * m_nCurTexRow;
	}
};

#endif
