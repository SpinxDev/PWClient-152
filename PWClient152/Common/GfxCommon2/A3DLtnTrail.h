#ifndef A3DLTNTRAIL_H_
#define A3DLTNTRAIL_H_

#include "A3DGFXElement.h"
#include "A3DGFXEditorInterface.h"
#include "RotList.h"

#define LTNTRAIL_BUF_SIZE 256

struct LTNTRAIL_DATA
{
	A3DVECTOR3		m_vPos1;
	A3DVECTOR3		m_vPos2;
	A3DQUATERNION	m_qDir;
	A3DVECTOR3		m_vCenter;
	int				m_nSegLife;
	float			m_fAlpha;
	float			m_fU;

	LTNTRAIL_DATA()
	{
		m_nSegLife = 0;
		m_fAlpha = 0;
		m_fU = 0;
	}
	LTNTRAIL_DATA(
		const A3DVECTOR3& vPos1,
		const A3DVECTOR3& vPos2,
		const A3DQUATERNION& qDir,
		const A3DVECTOR3& vCenter,
		int nSegLife,
		float fAlpha) :
		m_vPos1(vPos1),
		m_vPos2(vPos2),
		m_qDir(qDir),
		m_vCenter(vCenter),
		m_nSegLife(nSegLife),
		m_fAlpha(fAlpha)
		{
			m_fU = 0;
		}
};

class A3DLtnTrail : public A3DGFXElement
{
public:
	A3DLtnTrail(A3DGFXEx* pGfx, int nBufSize = LTNTRAIL_BUF_SIZE);
	virtual ~A3DLtnTrail() {}

protected:
	A3DVECTOR3	m_vPos1;
	A3DVECTOR3	m_vPos2;
	int			m_nSegLife;
	float		m_fMinAmp;
	float		m_fMaxAmp;
	bool		m_bBind;
	bool		m_bMatrixEnable;
	bool		m_bPos1Enable;
	bool		m_bPos2Enable;

	RotList<LTNTRAIL_DATA> m_TrailLst;
	bool		m_bPos1Set;
	bool		m_bPos2Set;

protected:
	void AddPoint(const A3DMATRIX4& matTran);
	bool InnerTick(DWORD dwTickTime);

public:
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual bool Render(A3DViewport*);
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);
	virtual bool Play();
	virtual bool TickAnimation(DWORD dwTickTime);
	virtual bool Stop();
	bool Init(A3DDevice* pDevice);
	A3DLtnTrail& operator = (const A3DLtnTrail& src);

public:
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	virtual bool IsParamEnable(int nParamId) const;
	virtual void EnableParam(int nParamId, bool bEnable);
	virtual bool NeedUpdateParam(int nParamId) const;
	virtual void UpdateParam(int nParamId, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetParam(int nParamId) const;
	virtual GfxValueType GetParamType(int nParamId) const;
};

#endif
