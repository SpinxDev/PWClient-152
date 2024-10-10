/*
 * FILE: A3DGFXModel.h
 *
 * DESCRIPTION: Model
 *
 * CREATED BY: ZhangYu, 2004/12/25
 *
 * HISTORY:
 *
 */

#ifndef A3DGFXMODEL_H_
#define A3DGFXMODEL_H_

#include "A3DGFXElement.h"
#include "A3DGFXModelMan.h"
#include "A3DSkinModel.h"

class A3DGFXModel : public A3DGFXElement
{
public:
	A3DGFXModel(A3DGFXEx* pGfx);
	virtual ~A3DGFXModel() {}

protected:
	AString			m_strModelFile;
	A3DGFXModelRef*	m_pModelRef;
	AString			m_strActName;
	int				m_nLoops;
	bool			m_bAlphaCmp;
	bool			m_bZWriteEnable;
	bool			m_bStart;
	bool			m_bIsUse3DCamera;
	bool			m_bIsFacingDir;
	int				m_iMatMethod;
	A3DCOLORVALUE	m_MatScale;
	float			m_fTransparent;
	A3DMATRIX4		m_matTran;
	DWORD			m_dwTickTime;

	A3DVECTOR3		m_vOldPos;
	A3DVECTOR3		m_vOldDir;
	
	
	static bool			m_bOldZwriteEnable;
	static bool			m_bOldZTest;

protected:

	bool ChangeModel();
	void ReleaseModel();

	A3DGFXModel& operator = (const A3DGFXModel& src);
	void UpdateModel(const A3DMATRIX4& matTran, A3DCOLOR diffuse, DWORD dwTickTime);

	virtual bool IsForceUse3DCamera() const;

public:
	// interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual bool Render(A3DViewport*);
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual void DummyTick(DWORD dwTick);
	virtual bool Play();
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	virtual A3DSkinModel* GetSkinModel();
	virtual void PrepareRenderSkinModel();
	virtual void RestoreRenderSkinModel();
};

#endif
