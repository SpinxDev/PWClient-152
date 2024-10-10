/*
 * FILE: A3DParticleSystemEx.h
 *
 * DESCRIPTION: Particle System
 *
 * CREATED BY: ZhangYu, 2004/8/3
 *
 * HISTORY:
 *
 */

#ifndef _A3DPARTICLESYSTEMEX_H_
#define _A3DPARTICLESYSTEMEX_H_

#include "A3DGFXElement.h"
#include "A3DParticle.h"
#include "A3DParticleEmitter.h"
#include "A3DGFXKeyPoint.h"

typedef APtrList<A3DParticle*> ParticleList;

class A3DParticleSystemEx : public A3DGFXElement
{
	typedef abase::vector<size_t> PropIdxArray;
public:
	A3DParticleSystemEx(A3DGFXEx* pGfx, int nType);
	virtual ~A3DParticleSystemEx() {}

protected:
	A3DParticleEmitter* m_pEmitter;
	abase::vector<A3DGFXKeyPointCtrlBase*> m_Affectors;
	abase::vector<A3DParticle*> m_Particles;
	ParticleList m_LstAct;
	ParticleList m_LstFree;

	int				m_nQuota;
	int				m_nEmissionCount;
	float			m_fParticleWidth;
	float			m_fParticleHeight;
	bool			m_b3DParticle;
	bool			m_bFacing;
	A3DMATRIX4		m_matSelfScale;
	bool			m_bScaleNoOffset;
	bool			m_bNoWidthScale;
	bool			m_bNoHeightScale;
	float			m_fOrgPtWidth;
	float			m_fOrgPtHeight;
	float			m_fOrgPtWidCo;
	float			m_fOrgPtHeiCo;
	float			m_fZOffset;

	// Emitter Interp Info
	bool			m_bFirstTick;
	A3DVECTOR3		m_vOldPos;
	A3DQUATERNION	m_qOldDir;
	float			m_fOldScl;
	A3DVECTOR3		m_vParAxisOff;
	bool			m_bApplyMotion;
	
	// Particle use UV from itself
	bool			m_bIsUseParUV;
	bool			m_bIsStartWithGrndHeight;
	bool			m_bInitRandom;

	// Particle emitter started
	bool			m_bTriggerEmitter;
	
	// Stop emit when fade out
	bool			m_bStopEmitWhenFadeOut;

protected:
	void Expire(float fTimeSpan);
	void TriggerEmitter(float fTimeSpan);
	void TriggerEmitter(float fTimeSpan, const A3DVECTOR3& vEmit, const A3DQUATERNION& qEmit);
	void ApplyMotion(A3DParticle* p, float fTimeSpan);
	inline void ApplyUV(A3DParticle* p, DWORD dwTick) { p->CalcParticlesUV(dwTick, m_dwTexInterval, m_nTexRow, m_nTexCol, m_fTexWid, m_fTexHei); }
	void TickDummy(DWORD dwTick);
	void TriggerAffectors(float fTimeSpan, DWORD dwTick);
	A3DParticle* AddParticle();
	void InitParticle(A3DParticle* p);
	int GetEmissionCount(float fTimeSpan);
	int Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex);
	int Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts);
	void Fill_Vert_3D(
		const A3DParticle* p,
		const KEY_POINT& kp,
		const A3DVECTOR3& vCam,
		A3DVECTOR3* verts,
		const A3DMATRIX4& matTran,
		const A3DQUATERNION& qParent,
		float fParentScale) const;
	bool Fill_Vert_2D(
		const A3DParticle* p,
		const KEY_POINT& kp,
		A3DViewport* pView,
		A3DCameraBase* pCamera,
		A3DVECTOR4* verts,
		const A3DMATRIX4& matTran,
		const A3DMATRIX4& matVPs,
		float fParentScale,
		float& fWidth,
		float& fHeight) const;

	A3DMATRIX4 GetBindTranMat(const KEY_POINT& kp) const
	{
		A3DMATRIX4 matTran;

		if (m_bScaleNoOffset)
			matTran = m_matSelfScale * GetTranMatrix(kp) * GetParentTM();
		else
			matTran = GetTranMatrix(kp) * m_matSelfScale * GetParentTM();

		return matTran;
	}
	float GetCurAlpha(const KEY_POINT& kp) const;
	bool IsTranInLocal() const { return !m_pEmitter->IsParticlePosAbs() && !m_bFacing; }
	bool IsParticlePosAbs() const { return m_pEmitter->IsParticlePosAbs(); }
	void Render_Dummy(A3DViewport* pView);
	void Resume();
	virtual void ResumeLoop();	// Override base class' interface
	void ReleasePool();
	

public:
	virtual void SetPoolSize(int nSize);
	int GetPoolSize() const { return (int)m_Particles.size(); }
	bool Init(A3DDevice* pDevice);
	A3DParticleSystemEx& operator = (const A3DParticleSystemEx& src);
	A3DParticleEmitter* GetEmitter() { return m_pEmitter; }
	const A3DParticleEmitter* GetEmitter() const { return m_pEmitter; }
	void SetQuota (int nQuota) { m_nQuota = nQuota; }
	int GetQuota() const { return m_nQuota; }


	int DrawToBuffer(A3DViewport* pView, A3DTLWARPVERTEX* pVerts, int nMaxVerts, float rw, float rh);
	int DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts);
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);

protected:
	
	const ParticleList& GetActParticleList() const { return m_LstAct; }
	void ReleaseAffectors();
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual bool Render(A3DViewport*);
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool Stop();
	virtual bool Play();
	virtual bool StopParticleEmit();
	virtual void DoFadeOut();

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;

	virtual size_t GetSubProtertyCount(int nSubId);
	virtual bool AddSubProterty(int nSubId);
	virtual bool RemoveSubProterty(int nSubId, size_t nSubIdx);
	virtual bool SetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId) const;

	const PropIdxArray& GetCtrlListByType(int nType, PropIdxArray& lst) const;
};

#endif
