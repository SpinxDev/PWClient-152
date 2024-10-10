#include "StdAfx.h"
#include "A3DGFXRing.h"
#include "A3DConfig.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DGFXExMan.h"

static const int _default_sects = 36;

static const char _format_radius[]	= "Radius: %f";
static const char _format_height[]	= "Height: %f";
static const char _format_pitch[]	= "Pitch: %f";
static const char _format_sects[]	= "Sects: %d";
static const char _format_noradsca[]= "NoRadScale: %d";
static const char _format_noheisca[]= "NoHeiScale: %d";
static const char _format_orgcent[]	= "OrgAtCenter: %d";

A3DGFXElement* A3DGFXRing::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXRing* p = new A3DGFXRing(pGfx);
	return &(*p = *this);
}

A3DGFXRing& A3DGFXRing::operator = (const A3DGFXRing& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);
	m_fRadius		= src.m_fRadius;
	m_fHeight		= src.m_fHeight;
	m_fPitch		= src.m_fPitch;
	m_nSects		= src.m_nSects;
	m_bNoRadScale	= src.m_bNoRadScale;
	m_bNoHeiScale	= src.m_bNoHeiScale;
	m_bOrgAtCent	= src.m_bOrgAtCent;

	return *this;
}

bool A3DGFXRing::InitStreamBuffer()
{
	if (m_pVerts)
	{
		delete[] m_pVerts;
		m_pVerts = NULL;
		m_nVerts = 0;
	}

	int nSect;

	if (m_nSects) nSect = m_nSects;
	else
		nSect = _default_sects;

	float fBaseAngle = A3D_2PI / nSect;
	float fHalfHeight = .5f * m_fHeight;
	float fLimit = m_fRadius / fHalfHeight;
	float fTan = (float)tan(m_fPitch);
	if (fTan > fLimit) fTan = fLimit;
	else if (fTan < -fLimit) fTan = -fLimit;
	float fDeltaRadius = (float)(fHalfHeight * fTan);
	float fUppderRadius = m_fRadius - fDeltaRadius;
	float fLowerRadius = m_fRadius + fDeltaRadius;
	m_nVerts = (nSect + 1) * 2;

	m_pVerts = new A3DVECTOR3[m_nVerts];
	float fAngle = 0;
	float fTotal = (float)(nSect + 1);

	for (int i = 0; i < nSect; i++)
	{
		int nBase = 2 * i;

		float fSin = (float)sin(fAngle);
		float fCos = (float)cos(fAngle);
		m_pVerts[nBase].x = fLowerRadius  * fCos;
		m_pVerts[nBase].y = m_bOrgAtCent ? -fHalfHeight : 0;
		m_pVerts[nBase].z = fLowerRadius  * fSin;
		m_pVerts[nBase+1].x = fUppderRadius * fCos;
		m_pVerts[nBase+1].y = m_bOrgAtCent ? fHalfHeight : m_fHeight;
		m_pVerts[nBase+1].z = fUppderRadius * fSin;
		fAngle += fBaseAngle;
	}

	m_pVerts[nSect*2  ] = m_pVerts[0];
	m_pVerts[nSect*2+1] = m_pVerts[1];
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

	return true;
}

bool A3DGFXRing::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_fRadius, sizeof(m_fRadius), &dwRead);
		pFile->Read(&m_fHeight, sizeof(m_fHeight), &dwRead);
		pFile->Read(&m_fPitch, sizeof(m_fPitch), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		int nBool;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_radius, &m_fRadius);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_height, &m_fHeight);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_pitch, &m_fPitch);

		if (dwVersion >= 14)
		{
			nBool = 0;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sects, &m_nSects);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_noradsca, &nBool);
			m_bNoRadScale = (nBool != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_noheisca, &nBool);
			m_bNoHeiScale = (nBool != 0);
		}
	
		if (dwVersion >= 15)
		{
			nBool = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_orgcent, &nBool);
			m_bOrgAtCent = (nBool != 0);
		}
	}

	return true;
}

bool A3DGFXRing::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_fRadius, sizeof(m_fRadius), &dwWrite);
		pFile->Write(&m_fHeight, sizeof(m_fHeight), &dwWrite);
		pFile->Write(&m_fPitch, sizeof(m_fPitch), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		
		sprintf(szLine, _format_radius, m_fRadius);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_height, m_fHeight);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_pitch, m_fPitch);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sects, m_nSects);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_noradsca, (int)m_bNoRadScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_noheisca, (int)m_bNoHeiScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_orgcent, (int)m_bOrgAtCent);
		pFile->WriteLine(szLine);
	}

	return true;
}

void A3DGFXRing::Release()
{
	A3DGFXElement::Release();
	ReleaseStreamBuffer();
}

bool A3DGFXRing::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

int A3DGFXRing::GetVertsCount()
{
	return m_nVerts;
}

int A3DGFXRing::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	if (m_nVerts == 0)
		return 0;

	A3DCOLOR diffuse;
	KEY_POINT kp;
	A3DMATRIX4 matScale;

	if (m_bDummy)
	{
		assert (m_pDummyMat);
		A3DMATRIX4 matWorld = GetKeyPointCount() > 0 ? GetKeyPointMat(0) * (*m_pDummyMat) : *m_pDummyMat;

		if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
			m_pDevice->SetWorldMatrix(matWorld);
		else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
		{
			matWorld.Transpose();
			m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
		}
		else
			m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);

		diffuse = m_clDummy;
	}
	else
	{
		if (!GetCurKeyPoint(&kp))
			return 0;

		float fRadScale, fHeiScale;

		if (m_bNoRadScale) fRadScale = 1.0f;
		else fRadScale = kp.m_fScale;

		if (m_bNoHeiScale) fHeiScale = 1.0f;
		else fHeiScale = kp.m_fScale;

		diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());

		matScale.Scale(fRadScale, fHeiScale, fRadScale);
		A3DMATRIX4 matWorld = matScale * GetTranMatrix(kp, _unit_y) * GetParentTM();

		if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
			m_pDevice->SetWorldMatrix(matWorld);
		else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
		{
			matWorld.Transpose();
			m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
		}
		else
			m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);
	}

	if (m_pGfx->IsUsingOuterColor())
		diffuse = (A3DCOLORVALUE(diffuse) * m_pGfx->GetOuterColor()).ToRGBAColor();

	A3DGFXVERTEX* pVerts = (A3DGFXVERTEX*)pBuffer;
	int nPairs = m_nVerts >> 1;
	int nSects = nPairs - 1;
	static const A3DCOLOR _specu = A3DCOLORRGBA(0, 0, 0, 255);

#ifdef GFX_EDITOR
	m_AABB.Clear();
#endif

	for (int i = 0; i < nPairs; i++)
	{
		int nBase = i * 2;
		pVerts[nBase].tu = pVerts[nBase+1].tu = i * m_fTexWid / nSects + m_fTexU;
		pVerts[nBase].diffuse = pVerts[nBase+1].diffuse = diffuse;
		pVerts[nBase].specular = pVerts[nBase+1].specular = _specu;

		pVerts[nBase].x		= m_pVerts[nBase].x;
		pVerts[nBase].y		= m_pVerts[nBase].y;
		pVerts[nBase].z		= m_pVerts[nBase].z;
		pVerts[nBase].dwMatIndices = nMatrixIndex;
		pVerts[nBase].tv	= m_fTexV + m_fTexHei;

		pVerts[nBase+1].x	= m_pVerts[nBase+1].x;
		pVerts[nBase+1].y	= m_pVerts[nBase+1].y;
		pVerts[nBase+1].z	= m_pVerts[nBase+1].z;
		pVerts[nBase+1].dwMatIndices = nMatrixIndex;
		pVerts[nBase+1].tv	= m_fTexV;

#ifdef GFX_EDITOR
//		if (m_pGfx->IsCalcingAABB())
		{
			A3DMATRIX4 matTran;

			if (!m_bDummy)
				matTran = matScale * GetTranMatrix(kp, _unit_y) * GetParentTM();
			else
				matTran = *m_pDummyMat;

			for (int i = 0; i < 2; i++)
			{
				A3DGFXVERTEX* pVert = &pVerts[nBase + i];
				A3DVECTOR3 v(pVert->x, pVert->y, pVert->z);
				v = matTran * v;

				A3DAABB& aabb = m_AABB;
				if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
				if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
				if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
				if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
				if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
				if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;
			}
		}
#endif
	}

	return m_nVerts;
}
