/*
 * FILE: A3DPhysEmitter.h
 *
 * DESCRIPTION: Physical Emitter supports Particles' direction and Dummy Element
 *
 * CREATED BY: ZhangYachuan, 2008/11/21
 *
 * HISTORY:
 *
 */

#include "A3DPhysFluidEmitter.h"

#ifndef _A3DPHYSEMITTER_H_
#define _A3DPHYSEMITTER_H_


class A3DGFXEx;
class A3DGFXElement;
class A3DPhysParticle
{
	// Constructor / destructor
public:
	A3DPhysParticle();
	virtual ~A3DPhysParticle();

	// Public operations
public:
	float Get2DRot() const { return m_f2DRot; }
	float GetScale() const { return m_fScale; }
	float GetScaleNoise() const { return m_fScaleNoise; }
	float GetAge() const { return m_fAge; }
	float GetWidth() const { return m_fWidth; }
	float GetHeight() const { return m_fHeight; }
	bool IsActived() const { return m_bIsActived; }
	A3DGFXElement* GetDummy() const { return m_pDummy; }
	const A3DVECTOR3& GetPos() const { return m_vPos; }
	const A3DQUATERNION& GetDir() const { return m_qDir; }
	A3DQUATERNION& GetDirRef() { return m_qDir; }
	A3DCOLOR GetColor() const { return m_dwColor; }
	A3DMATRIX4 GetMatrix() const;

	void SetWidth(float fWidth) { m_fWidth = fWidth; }
	void SetHeight(float fHeight) { m_fHeight = fHeight; }
	void SetDir(const A3DQUATERNION& q) { m_qDir = q; }
	void Set2DRot(float f2DRot);
	void SetScale(float fSize) { m_fScale = fSize; }
	void SetScaleNoise(float fScaleNoise) { m_fScaleNoise = fScaleNoise; }
	void SetDummy(const A3DGFXElement* pEle, A3DGFXEx* pGfx);
	void SetActive(bool bActive) { m_bIsActived = bActive; }
	void SetPos(const A3DVECTOR3& vPos) { m_vPos = vPos; }
	void SetColor(A3DCOLOR dwColor) { m_dwColor = dwColor; }
	void SetAge(float fAge) { m_fAge = fAge; }
	
	void Update(DWORD dwTickTime);

	void Reset()
	{
		if (m_pDummy)
		{
			m_pDummy->Release();
			delete m_pDummy;
			m_pDummy = NULL;
		}
	}
	// Attributes
private:
	bool m_bIsActived;
	float m_fWidth;
	float m_fHeight;
	float m_f2DRot;
	float m_fScale;
	float m_fScaleNoise;
	float m_fAge;
	A3DQUATERNION m_qDir;
	A3DCOLOR m_dwColor;
	A3DVECTOR3 m_vPos;
	A3DGFXElement* m_pDummy;

};

typedef APtrList<int> PhysParList;
typedef abase::vector<A3DPhysParticle*> PhysParPool;

class A3DPhysEmitter : public A3DPhysFluidEmitter
{
	typedef abase::vector<size_t> PropIdxArray;
public:

	static unsigned int s_MaxSupportParticleNum;

	enum 
	{
		PHYSPAR_NORMAL = 0,
		PHYSPAR_FACETOVIEWER,
		PHYSPAR_PHYSORIENT,
	};

	// Constructor / destructor
public:
	explicit A3DPhysEmitter(A3DGFXEx* pGfx);
	virtual ~A3DPhysEmitter();

	// Public operation
protected:
	virtual A3DGFXElement* Clone(A3DGFXEx*) const;
	A3DPhysEmitter& operator = (const A3DPhysEmitter& rhs);

	// this pair of function is used for saving / loading the gfx properties
	virtual bool Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);

	// GFX element tick state
	virtual bool TickAnimation(DWORD dwTickTime);

	virtual bool Play();
	virtual bool Stop();

	// GFX element render (register render)
	virtual bool Render(A3DViewport*);
	// GFX element render (real buffer filling)
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);

	virtual void DoFadeOut();
	// Private operations
private:

	// Set / Get property
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;	

	virtual size_t GetSubProtertyCount(int nSubId);
	virtual bool AddSubProterty(int nSubId);
	virtual bool RemoveSubProterty(int nSubId, size_t nSubIdx);
	virtual bool SetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetSubProperty(int nSubId, size_t nSubIdx, int nSubOpId) const;
	const PropIdxArray& GetCtrlListByType(int nType, PropIdxArray& lst) const;

	void ReleaseParticlePool();
	void ReleaseAffectors();
	void InitParticle(A3DPhysParticle& pPar);
	void Render_Dummy(A3DViewport* pView);
	void TriggerAffectors(float, const unsigned int*, int);
	void UpdateParticleSize();
	int Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex, const unsigned int * pIDs, int nParticleNum);
	int Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts, const unsigned int * pIDs, int nParticleNum);

	//Attributes
private:

	// For init particle
	float m_fParticleWidth;
	float m_fParticleHeight;
	// Phys Particle scale
	float m_fScaleMax;
	float m_fScaleMin;
	// Particle Rotation min - max
	float m_fRotMax;
	float m_fRotMin;
	// Particle Color min - max
	A3DCOLOR m_dwColorMax;
	A3DCOLOR m_dwColorMin;

// 	// Is face to view
// 	bool m_bIsFaceToView;
	
	// Use orientation
	int m_iPhysParType;

	// Stop emit when fade out
	bool			m_bStopEmitWhenFadeOut;

	A3DPhysParticle* m_poolPhysParticles;
	abase::vector<A3DGFXKeyPointCtrlBase*> m_Affectors;
};


//////////////////////////////////////////////////////////////////////////
// Help Functions
//////////////////////////////////////////////////////////////////////////

A3DCOLOR genRandColor(A3DCOLOR dwColorMin, A3DCOLOR dwColorMax);
float genRandFloat(float fMin, float fMax);
void exchangeUV(float& fU0, float& fU1, float& fV0, float& fV1, bool bIsUReverse, bool bIsVReverse, bool bIsUVInterchange);

#endif