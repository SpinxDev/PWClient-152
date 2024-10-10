#include "StdAfx.h"
#include "A3DParaboloid.h"
#include "A3DConfig.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DGFXExMan.h"
#include "A3DGFXStreamMan.h"

static const char* _format_co	= "Coeff: %f";
static const char* _format_hei	= "height: %f";

static const float _grid_sz		= .1f;
static const int _max_circles	= 8;
static const int _segs			= 360 / 15;
static const A3DCOLOR _specu	= A3DCOLORRGBA(0, 0, 0, 255);

bool A3DParaboloid::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_fCoefficient, sizeof(m_fCoefficient), &dwRead);
		pFile->Read(&m_fHeight, sizeof(m_fHeight), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_co, &m_fCoefficient);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_hei, &m_fHeight);
	}

	return true;
}

bool A3DParaboloid::Save(AFile* pFile)
{
	DWORD dwWrite;

	if (pFile->IsBinary())
	{
		pFile->Write(&m_fCoefficient, sizeof(m_fCoefficient), &dwWrite);
		pFile->Write(&m_fHeight, sizeof(m_fHeight), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		
		sprintf(szLine, _format_co, m_fCoefficient);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_hei, m_fHeight);
		pFile->WriteLine(szLine);
	}

	return true;
}

A3DParaboloid& A3DParaboloid::operator = (const A3DParaboloid& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);

	m_fCoefficient	= src.m_fCoefficient;
	m_fHeight		= src.m_fHeight;
	m_nVerts		= src.m_nVerts;
	m_nCircles		= src.m_nCircles;

	return *this;
}

A3DGFXElement* A3DParaboloid::Clone(A3DGFXEx* pGfx) const
{
	A3DParaboloid* p = new A3DParaboloid(pGfx);
	return &(*p = *this);
}

void A3DParaboloid::Release()
{
	A3DGFXElement::Release();
	ReleaseVerts();
}

bool A3DParaboloid::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

void A3DParaboloid::Build()
{
	if (m_pVerts)
		delete[] m_pVerts;

	m_nCircles = int(m_fHeight / _grid_sz);

	if (m_nCircles < 1)
		m_nCircles = 1;
	else if (m_nCircles > _max_circles)
		m_nCircles = _max_circles;

	float fGrdSz = m_fHeight / m_nCircles;
	m_nVerts = (m_nCircles + 1) * (_segs + 1);
	m_pVerts = new A3DLVERTEX[m_nVerts];

	float fUC = m_fTexWid * .5f;
	float fVC = m_fTexHei * .5f;

	int i, j, nVerts = 0;
	static float fCos[_segs], fSin[_segs];
	static bool _calc_angle = false;

	if (!_calc_angle)
	{
		for (i = 0; i < _segs; i++)
		{
			float fAng = (A3D_2PI / _segs) * i;
			fCos[i] = (float)cos(fAng);
			fSin[i] = (float)sin(fAng);
		}

		_calc_angle = true;
	}

	float fTexXStep = m_fTexWid * .5f / m_nCircles;
	float fTexYStep = m_fTexHei * .5f / m_nCircles;

	for (i = 0; i <= m_nCircles; i++)
	{
		float fRadius	= (float)sqrt(m_fCoefficient * i * fGrdSz);
		float fTexX		= fTexXStep * i;
		float fTexY		= fTexYStep * i;
		float fHeight	= fGrdSz * i;
		int nStart		= nVerts;

		for (j = 0; j < _segs; j++)
		{
			A3DLVERTEX& ver = m_pVerts[nVerts];
			ver.x	= fRadius * fCos[j];
			ver.y	= fRadius * fSin[j];
			ver.z	= fHeight;
			ver.tu	= fUC + fTexX * fCos[j];
			ver.tv	= fVC - fTexY * fSin[j];
			nVerts++;
		}

		m_pVerts[nVerts] = m_pVerts[nStart];
		nVerts++;
	}

	m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
		m_nRenderLayer,
		A3DFVF_GFXVERTEX,
		sizeof(A3DGFXVERTEX),
		A3DPT_TRIANGLESTRIP,
		m_strTexture,
		m_Shader,
		m_bTileMode,
		m_bTexNoDownSample,
		m_pGfx->IsZTestEnable() && m_bZEnable,
		HasPixelShader(),
        m_bSoftEdge,
		m_bAbsTexPath);
}

int A3DParaboloid::GetVertsCount()
{
	return (_segs + 1) * 2 * m_nCircles;
}

int A3DParaboloid::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return 0;

	if (m_pGfx->IsUsingOuterColor())
		kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());
	A3DMATRIX4 mat;
	mat.Scale(kp.m_fScale, kp.m_fScale, kp.m_fScale);
	A3DMATRIX4 matWorld = mat * GetTranMatrix(kp) * GetParentTM();

	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(matWorld);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
	{
		matWorld.Transpose();
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
	}
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);

#ifdef GFX_EDITOR
	m_AABB.Clear(); 
#endif
	A3DGFXVERTEX* pVerts = (A3DGFXVERTEX*)pBuffer;
	int nBase = 0;

	for (int nCircle = 0; nCircle < m_nCircles; nCircle++)
	{
		int nIndex1	= (_segs + 1) * nCircle;
		int nIndex2	= (_segs + 1) + nIndex1;
	
		for (int i = 0; i < _segs + 1; i++)
		{
			A3DLVERTEX& v1 = m_pVerts[i+nIndex1];
			A3DLVERTEX& v2 = m_pVerts[i+nIndex2];
			A3DGFXVERTEX& des1 = pVerts[nBase];
			A3DGFXVERTEX& des2 = pVerts[nBase+1];
			nBase += 2;

			des1.x			= v1.x;
			des1.y			= v1.y;
			des1.z			= v1.z;
			des1.dwMatIndices = nMatrixIndex;
			des1.tu			= v1.tu + m_fTexU;
			des1.tv			= v1.tv + m_fTexV;
			des1.diffuse	= diffuse;
			des1.specular	= _specu;

			des2.x			= v2.x;
			des2.y			= v2.y;
			des2.z			= v2.z;
			des2.dwMatIndices = nMatrixIndex;
			des2.tu			= v2.tu + m_fTexU;
			des2.tv			= v2.tv + m_fTexV;
			des2.diffuse	= diffuse;
			des2.specular	= _specu;

#ifdef GFX_EDITOR
			A3DMATRIX4 matWorld = mat * GetTranMatrix(kp) * GetParentTM();
			//			if (m_pGfx->IsCalcingAABB())
			{
				A3DAABB& aabb = m_AABB; //m_pGfx->GetAABBOrgRef();

				A3DVECTOR3 v(des1.x, des1.y, des1.z);
				v = matWorld * v;
				if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
				if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
				if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
				if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
				if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
				if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;

				A3DVECTOR3 v2(des2.x, des2.y, des2.z);
				v = matWorld * v2;
				if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
				if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
				if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
				if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
				if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
				if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;
			}
#endif
		}
	}

	return GetVertsCount();
}
