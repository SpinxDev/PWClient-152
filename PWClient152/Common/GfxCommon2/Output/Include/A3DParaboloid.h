#ifndef A3DPARABOLOID_H_
#define A3DPARABOLOID_H_

#include "A3DGFXElement.h"
#include "A3DGFXEditorInterface.h"

class A3DParaboloid : public A3DGFXElement
{
public:
	A3DParaboloid(A3DGFXEx* pGfx) : A3DGFXElement(pGfx)
	{
		m_nEleType		= ID_ELE_TYPE_PARABOLOID;
		m_fCoefficient	= 1.0f;
		m_fHeight		= 0;
		m_nVerts		= 0;
		m_pVerts		= NULL;
		m_nCircles		= 0;
	}
	~A3DParaboloid() {}

protected:
	float		m_fCoefficient;
	float		m_fHeight;
	int			m_nVerts;
	int			m_nCircles;
	A3DLVERTEX*	m_pVerts;

protected:
	void Build();
	void ReleaseVerts()
	{
		if (m_pVerts)
		{
			delete[] m_pVerts;
			m_pVerts = NULL;
			m_nVerts = 0;
		}
	}

public:
	A3DParaboloid& operator = (const A3DParaboloid& src);

	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual void Release();
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);
	virtual bool Render(A3DViewport*);
	virtual bool Play();

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch(nOp)
		{
		case ID_GFXOP_PARAB_COEFF:
			m_fCoefficient = prop;
			break;
		case ID_GFXOP_PARAB_HEIGHT:
			m_fHeight = prop;
			break;
		default:
			A3DGFXElement::SetProperty(nOp, prop);
		}

		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch(nOp)
		{
		case ID_GFXOP_PARAB_COEFF:
			return GFX_PROPERTY(m_fCoefficient);
		case ID_GFXOP_PARAB_HEIGHT:
			return GFX_PROPERTY(m_fHeight);
		}
		return A3DGFXElement::GetProperty(nOp);
	}	
};

inline bool A3DParaboloid::Play()
{
	if (!IsInit())
	{
		InitBaseData();
		Build();
		SetInit(true);
	}

	return true;
}

#endif
