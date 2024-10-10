/*
 * FILE: A3DGridDecalEx.h
 *
 * DESCRIPTION: 3D Animation Decal with Changeable Grid
 *
 * CREATED BY: Zhangyachuan, 2008/8/18
 *
 * HISTORY:
 *
 */

#ifndef _A3DGRIDDECALEX_H_
#define _A3DGRIDDECALEX_H_

#include "A3DGFXElement.h"
#include "A3DTypes.h"
#include <algorithm>

class A3DGFXEx;
class A3DGfxGridAnimation;

class A3DGridDecalEx : public A3DGFXElement
{
public:
	typedef struct GRID_VERTEX_DATA {
		GRID_VERTEX_DATA() : m_vPos(0), m_dwColor(A3DCOLORRGBA(255, 255, 255, 255)) {}
		A3DVECTOR3	m_vPos;
		A3DCOLOR	m_dwColor;
	} GridData;

	A3DGridDecalEx(A3DGFXEx* pGfx, int nType);

	virtual ~A3DGridDecalEx();
	

protected:
	
	// need to save
	// self data members
	// width / height vertex number
	int			m_nWidthVertNum;
	int			m_nHeightVertNum;
	float		m_fGridSize;
	float		m_fZOffset;

	// vertice buffer, indice buffer(indice buf not used now)
	// vertice are in local space
	GridData*	m_pVerts;

	bool		m_bAffectedByScale;
	bool		m_bRotFromView;
	float		m_fOffsetHeight;
	bool		m_bAlwaysOnGround;		//总是贴地
		
	//
	A3DGfxGridAnimation*	m_pGridAnimation;

private:

	// no need to save, Runtime data
	int			m_nRectNum;
	float*		m_pUVBuf;
	A3DVECTOR3* m_p2DVertBuf;
	A3DCOLOR*	m_pColorBuf;
	A3DTLWARPVERTEX* m_pWarp2DBuf;
	static const float s_fGridSize;

	//used by m_bRotFromView
	A3DVECTOR3		m_vPos;
	A3DVECTOR3		m_vDir;
	A3DVECTOR3		m_vView;
	A3DVECTOR3		m_vUp;

public:

	// overload interfaces of A3DGFXElement
	virtual bool Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual A3DGFXElement* Clone(A3DGFXEx* pGfx) const;
	virtual bool Init(A3DDevice* pDevice);
	virtual bool Play();
	virtual bool Render(A3DViewport*);
	virtual int GetVertsCount();
	virtual int FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView);
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual GFX_PROPERTY GetProperty(int nOp) const;
	inline int GetVertRealNumber() const { return m_nWidthVertNum * m_nHeightVertNum; }

	virtual bool TickAnimation(DWORD dwTickTime);

protected:
	
	A3DGridDecalEx& operator = (const A3DGridDecalEx& src);
	virtual int DrawToBuffer(A3DViewport* pView, A3DTLWARPVERTEX* pVerts, int nMaxVerts, float rw, float rh);
	virtual int DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts);
	bool Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex);
	bool Fill_Verts_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex, DWORD diffuse, DWORD specular);
	bool Fill_Verts_3D(const KEY_POINT& kp, A3DViewport* pView, A3DWARPVERTEX* pVerts, float fAlpha);
	bool Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts);
	bool Fill_Verts_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts, A3DCOLOR diffuse, A3DCOLOR specular, float& fWidth, float& fHeight);
	bool Fill_Verts_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLWARPVERTEX* pVerts, float& fWidth, float& fHeight);
	bool Update(A3DViewport* pView);

public:
	
#ifdef GFX_EDITOR
	// editor interface
	GridData * GetData() const { return m_pVerts; }
	GridData * GetRow(int i) const { return &m_pVerts[i * m_nWidthVertNum]; }
	bool AddRowAt(int iRow, bool bAbove);
	bool AddColAt(int iCol, bool bLeft);
	bool RemoveRowAt(int iRow);
	bool RemoveColAt(int iCol);
	void SetVertPos(int iRow, int iCol, const A3DVECTOR3& vPos) { m_pVerts[iRow * m_nWidthVertNum + iCol].m_vPos = vPos; }
	void SetVertColor(int iRow, int iCol, A3DCOLOR dwColor) { m_pVerts[iRow * m_nWidthVertNum + iCol].m_dwColor = dwColor; }
	float GetGridSize() const { return m_fGridSize; }
	int GetWidthVertNum() const { return m_nWidthVertNum; }
	int GetHeightVertNum() const { return m_nHeightVertNum; }
	A3DGfxGridAnimation*	GetGridAnimation() const { return m_pGridAnimation; }
#endif


private:

	// Util func
	inline void _setSize(int nWidth, int nHeight)
	{
		if (m_nWidthVertNum != nWidth || m_nHeightVertNum != nHeight)
		{
			m_nWidthVertNum = nWidth;
			m_nHeightVertNum = nHeight;
			m_nRectNum = _calcCurRectNum();
			_resetVertBuffer(nWidth * nHeight);
			_setInitVertPos();
		}
	}
	inline void _resetVertBuffer(int nVertNum)
	{
		if (m_pVerts) delete [] m_pVerts;
		m_pVerts = new GridData[nVertNum];

		// 每一次顶点数目变动，也同样需要变动 UV缓存和2D顶点缓存 (这两个缓存仅在局部使用，每次使用时自行赋值)
		_resetUVBuffer(nVertNum);
		_reset2DVertBuffer(nVertNum);
		_resetColorBuffer(nVertNum);
	}
	inline void _resetUVBuffer(int nVertNum)
	{
		delete [] m_pUVBuf;
		m_pUVBuf = new float[nVertNum << 1];
	}
	inline void _reset2DVertBuffer(int nVertNum)
	{
		delete [] m_p2DVertBuf;
		m_p2DVertBuf = new A3DVECTOR3[nVertNum];
	}
	inline void _resetColorBuffer(int nVertNum)
	{
		delete [] m_pColorBuf;
		m_pColorBuf = new A3DCOLOR[nVertNum];
	}
	inline int _calcCurRectNum() { return (m_nHeightVertNum - 1) * (m_nWidthVertNum - 1); }	
	inline void _fillGfxVertex(A3DGFXVERTEX& vert, const A3DVECTOR3& vPos, DWORD dwMatIndex, DWORD dwDiffuse, DWORD dwSpecular, float u, float v)
	{ vert.x = vPos.x, vert.y = vPos.y, vert.z = vPos.z, vert.dwMatIndices = dwMatIndex, vert.diffuse = dwDiffuse, vert.specular = dwSpecular, vert.tu = u, vert.tv = v; }
	void _scaleVertPos(float fScale)
	{
		int nVertNum = GetVertRealNumber();
		for (int i = 0 ; i < nVertNum ; ++i) {
			m_pVerts[i].m_vPos *= fScale;
		}
	}
	void _setInitVertPos();
	void _setUVCoord(float *_pBufU, float *_pBufV);
	void _setColorComposed(A3DCOLOR diffuse, int nVertNum);

	inline A3DVECTOR3	getVertRotFromView(const A3DVECTOR3& v, const A3DMATRIX4& matScale)
	{
		A3DVECTOR3 newPos = m_vPos + (- v.y * m_vDir + v.x * m_vUp + v.z * m_vView) * matScale;
		return newPos;
	}

	inline void _fillGfxVertexMatchGround( A3DGFXVERTEX& vert, const A3DVECTOR3& vPos, DWORD dwMatIndex, DWORD dwDiffuse, DWORD dwSpecular, float u, float v, const A3DMATRIX4& matWorld )
	{
		A3DVECTOR3	vNewPos;
		vNewPos = vPos * matWorld;

		float fHeight = AfxGetGrndNorm(vNewPos,NULL);
		vNewPos.y = fHeight + m_fOffsetHeight;

		_fillGfxVertex(vert, vNewPos, dwMatIndex, dwDiffuse, dwSpecular, u, v);
	}
};


inline void A3DGridDecalEx::_setInitVertPos()
{
	float fHalfWid = (m_nWidthVertNum - 1) / 2.f;
	float fHalfHei = (m_nHeightVertNum - 1) / 2.f;
	fHalfWid = (fHalfWid < 1.f ? 1.f : fHalfWid) * m_fGridSize;
	fHalfHei = (fHalfHei < 1.f ? 1.f : fHalfHei) * m_fGridSize;
	float fStartX = -fHalfWid / 2;
	float fStartY = fHalfHei / 2;
	float fEndX = fHalfWid / 2;
	float fEndY = -fHalfHei / 2;

	int nVertsIndexBase = 0;
	const float fStepX = (fEndX - fStartX) / (m_nWidthVertNum - 1);
	const float fStepY = (fEndY - fStartY) / (m_nHeightVertNum - 1);
	GridData * pVertsPosBase = NULL;
	
	float fCurY = fStartY;
	for (int iRow = 0 ; iRow < m_nHeightVertNum ; ++iRow) {
		pVertsPosBase = m_pVerts + nVertsIndexBase;
		
		float fCurX = fStartX;
		for (int iCol = 0 ; iCol < m_nWidthVertNum ; ++iCol) {
			pVertsPosBase[iCol].m_vPos.x = fCurX;
			pVertsPosBase[iCol].m_vPos.y = fCurY;
			pVertsPosBase[iCol].m_vPos.z = 0;

			fCurX += fStepX;
		}

		fCurY += fStepY;
		nVertsIndexBase += m_nWidthVertNum;
	}
}

inline void A3DGridDecalEx::_setColorComposed(A3DCOLOR diffuse, int nVertNum)
{
	A3DCOLOR * _pColorBuf = m_pColorBuf;
	GridData * _pVertsBuf = m_pVerts;
	for (int i = 0 ; i < nVertNum ; ++i) {
		A3DCOLOR& dwColor = _pVertsBuf[i].m_dwColor;
		A3DCOLOR& dwDestColor = _pColorBuf[i];

		A3DCOLOR dwA = ((((((dwColor & 0xff000000) >> 24)) * ((diffuse & 0xff000000) >> 24)) & 0x0000ff00) <<16);
		A3DCOLOR dwR = ((((((dwColor & 0x00ff0000) >> 16)) * ((diffuse & 0x00ff0000) >> 16)) & 0x0000ff00) << 8);
		A3DCOLOR dwG = ((((((dwColor & 0x0000ff00) >> 8 )) * ((diffuse & 0x0000ff00) >> 8 )) & 0x0000ff00)     );
		A3DCOLOR dwB = ((((((dwColor & 0x000000ff)      )) * ((diffuse & 0x000000ff)      )) & 0x0000ff00) >> 8);

		dwDestColor = dwA | dwR | dwG | dwB;
	}
}

#endif