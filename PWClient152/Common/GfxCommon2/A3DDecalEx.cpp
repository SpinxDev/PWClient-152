#include "StdAfx.h"
#include "A3DDecalEx.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DConfig.h"
#include "A3DGFXExMan.h"

static const char* _format_width	= "Width: %f";
static const char* _format_height	= "Height: %f";
static const char* _format_texrow	= "TexRow: %d";
static const char* _format_texcol	= "TexCol: %d";
static const char* _format_texint	= "TexInterval: %d";
static const char* _format_rotview	= "RotFromView: %d";
static const char* _format_norm_only= "GrndNormOnly: %d";
static const char* _format_no_scale	= "NoScale: %d";
static const char* _format_org_pt	= "OrgPt: %f";
static const char* _format_z_offset	= "ZOffset: %f";
static const char* _match_surface	= "MatchSurface: %d";
static const char* _surface_use_parent_dir = "SurfaceUseParentDir: %d";
static const char* _max_extent		= "MaxExtent: %f";
static const char* _format_yaweffect = "YawEffect: %d";
static const char* _format_sc_space	 = "ScreenSpace: %d";

static const float _grnd_grid = 2.0f;
static const int _max_row_or_col = 4;

A3DDecalEx::A3DDecalEx(A3DGFXEx* pGfx, int nType) : A3DGFXElement(pGfx)
{
	m_nEleType		= nType;
	m_fWidth		= 0.5f;
	m_fHeight		= 0.5f;
	m_bRotFromView	= false;
	m_bGrndNormOnly	= false;
	m_bNoWidthScale	= false;
	m_bNoHeightScale= false;
	m_fOrgPtWidth	= 0.5f;
	m_fOrgPtHeight	= 0.5f;
	m_fZOffset		= 0;
	m_bMatchSurface	= false;
	m_bSurfaceUseParentDir = false;
	m_fMaxExtent	= 0.0f;
	m_pVerts		= 0;
	m_pIndices		= 0;
	m_nVertCount	= 0;
	m_nIndexCount	= 0;
	m_bCenterUpdate	= false;
	m_bYawEffectOnGrndNorm = false;
	m_bMatchSurfaceUVEffects = true;
	m_b2DScreenDimension = false;
	SetSize(m_fWidth, m_fHeight);
	m_vOldCenter.Clear();
	m_fOldRadius = 0.0f;

	if (m_nEleType != ID_ELE_TYPE_DECAL_2D)
		m_bTLVert = false;
}

bool A3DDecalEx::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_fWidth, sizeof(m_fWidth), &dwRead);
		pFile->Read(&m_fHeight, sizeof(m_fHeight), &dwRead);
		pFile->Read(&m_bRotFromView, sizeof(m_bRotFromView), &dwRead);
		pFile->Read(&m_bGrndNormOnly, sizeof(m_bGrndNormOnly), &dwRead);
		pFile->Read(&m_bNoWidthScale, sizeof(m_bNoWidthScale), &dwRead);
		pFile->Read(&m_bNoHeightScale, sizeof(m_bNoHeightScale), &dwRead);
		pFile->Read(&m_fOrgPtWidth, sizeof(m_fOrgPtWidth), &dwRead);
		pFile->Read(&m_fOrgPtHeight, sizeof(m_fOrgPtHeight), &dwRead);
		pFile->Read(&m_fZOffset, sizeof(m_fZOffset), &dwRead);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		int nRead;
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_width, &m_fWidth);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_height, &m_fHeight);

		if (dwVersion < 9)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_texrow, &m_nTexRow);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_texcol, &m_nTexCol);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_texint, &m_dwTexInterval);
			m_dwTexInterval *= 50;

			CalcTexAnimation();
		}

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_rotview, &nRead);
		m_bRotFromView = (nRead != 0);

		if (dwVersion >= 26)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_norm_only, &nRead);
			m_bGrndNormOnly = (nRead != 0);
		}

		if (dwVersion >= 36)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_no_scale, &nRead);
			m_bNoWidthScale = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_no_scale, &nRead);
			m_bNoHeightScale = (nRead != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_org_pt, &m_fOrgPtWidth);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_org_pt, &m_fOrgPtHeight);
		}

		if (dwVersion >= 42)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_z_offset, &m_fZOffset);
		}

		if (dwVersion >= 54)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _match_surface, &nRead);
			m_bMatchSurface = (nRead != 0);
		}

		if (dwVersion >= 86)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _surface_use_parent_dir, &nRead);
			m_bSurfaceUseParentDir = (nRead != 0);
		}

		if (dwVersion >= 55)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _max_extent, &m_fMaxExtent);
		}

		if (dwVersion >= 61)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_yaweffect, &nRead);
			m_bYawEffectOnGrndNorm = (nRead != 0);
		}

		if (dwVersion >= 115)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_sc_space, &nRead);
			m_b2DScreenDimension = (nRead != 0);
		}
	}

	SetSize(m_fWidth, m_fHeight);

	if (m_fMaxExtent == 0)
		m_fMaxExtent = max(m_fWidth, m_fHeight) * 1.5f;

	if (dwVersion <= 89)
	{
		// 从89的下一个版本开始起作用
		m_bMatchSurfaceUVEffects = false;
	}

	return true;
}

bool A3DDecalEx::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_fWidth, sizeof(m_fWidth), &dwWrite);
		pFile->Write(&m_fHeight, sizeof(m_fHeight), &dwWrite);
		pFile->Write(&m_bRotFromView, sizeof(m_bRotFromView), &dwWrite);
		pFile->Write(&m_bGrndNormOnly, sizeof(m_bGrndNormOnly), &dwWrite);
		pFile->Write(&m_bNoWidthScale, sizeof(m_bNoWidthScale), &dwWrite);
		pFile->Write(&m_bNoHeightScale, sizeof(m_bNoHeightScale), &dwWrite);
		pFile->Write(&m_fOrgPtWidth, sizeof(m_fOrgPtWidth), &dwWrite);
		pFile->Write(&m_fOrgPtHeight, sizeof(m_fOrgPtHeight), &dwWrite);
		pFile->Write(&m_fZOffset, sizeof(m_fZOffset), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		sprintf(szLine, _format_width, m_fWidth);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_height, m_fHeight);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_rotview, m_bRotFromView);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_norm_only, m_bGrndNormOnly);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_no_scale, m_bNoWidthScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_no_scale, m_bNoHeightScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_org_pt, m_fOrgPtWidth);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_org_pt, m_fOrgPtHeight);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_z_offset, m_fZOffset);
		pFile->WriteLine(szLine);

		sprintf(szLine, _match_surface, m_bMatchSurface);
		pFile->WriteLine(szLine);

		sprintf(szLine, _surface_use_parent_dir, m_bSurfaceUseParentDir);
		pFile->WriteLine(szLine);

		sprintf(szLine, _max_extent, m_fMaxExtent);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_yaweffect, m_bYawEffectOnGrndNorm);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_sc_space, m_b2DScreenDimension);
		pFile->WriteLine(szLine);
	}

	return true;
}

A3DGFXElement* A3DDecalEx::Clone(A3DGFXEx* pGfx) const
{
	A3DDecalEx* p = new A3DDecalEx(pGfx, m_nEleType);
	return &(*p = *this);
}

A3DDecalEx& A3DDecalEx::operator = (const A3DDecalEx& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);
	m_fWidth		= src.m_fWidth;
	m_fHeight		= src.m_fHeight;
	m_bRotFromView	= src.m_bRotFromView;
	m_bGrndNormOnly	= src.m_bGrndNormOnly;
	m_bNoWidthScale	= src.m_bNoWidthScale;
	m_bNoHeightScale= src.m_bNoHeightScale;
	m_fOrgPtWidth	= src.m_fOrgPtWidth;
	m_fOrgPtHeight	= src.m_fOrgPtHeight;
	m_fWidth1		= src.m_fWidth1;
	m_fWidth2		= src.m_fWidth2;
	m_fHeight1		= src.m_fHeight1;
	m_fHeight2		= src.m_fHeight2;
	m_fZOffset		= src.m_fZOffset;
	m_bMatchSurface	= src.m_bMatchSurface;
	m_bSurfaceUseParentDir = src.m_bSurfaceUseParentDir;
	m_fMaxExtent	= src.m_fMaxExtent;
	m_bYawEffectOnGrndNorm = src.m_bYawEffectOnGrndNorm;
	m_bMatchSurfaceUVEffects = src.m_bMatchSurfaceUVEffects;
	m_b2DScreenDimension = src.m_b2DScreenDimension;
	return *this;
}

bool A3DDecalEx::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	Update(pView);
	return true;
}

bool A3DDecalEx::Init(A3DDevice* pDevice)
{
	A3DGFXElement::Init(pDevice);
	m_nRectNum = (m_nEleType == ID_ELE_TYPE_DECAL_BILLBOARD ?  2 : 1);
	return true;
}

int A3DDecalEx::GetVertsCount()
{
	return _max_row_or_col * _max_row_or_col * 4;
}

int A3DDecalEx::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return 0;

	if (m_pGfx->IsUsingOuterColor())
		kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

	bool b = false;

	switch (m_nEleType)
	{
	case ID_ELE_TYPE_DECAL_3D:
		b = Update_3D(kp, pView, (A3DGFXVERTEX*)pBuffer, nMatrixIndex);
		break;
	case ID_ELE_TYPE_DECAL_2D:
		b = Update_2D(kp, pView, (A3DTLVERTEX*)pBuffer);
		break;
	case ID_ELE_TYPE_DECAL_BILLBOARD:
		b = Update_Billboard(kp, pView, (A3DGFXVERTEX*)pBuffer, nMatrixIndex);
		break;
	}

	return b ? m_nRectNum * 4 : 0;
}

// ret value means in local or world space
inline bool A3DDecalEx::Fill_Verts_3D(
	const KEY_POINT& kp,
	A3DViewport* pView,
	A3DLVERTEX* pVerts)
{
	float fWidth1, fWidth2, fHeight1, fHeight2;

	if (m_bNoWidthScale)
	{
		fWidth1 = m_fWidth1;
		fWidth2 = m_fWidth2;
	}
	else
	{
		fWidth1 = m_fWidth1 * kp.m_fScale;
		fWidth2 = m_fWidth2 * kp.m_fScale;
	}

	if (m_bNoHeightScale)
	{
		fHeight1 = m_fHeight1;
		fHeight2 = m_fHeight2;
	}
	else
	{
		fHeight1 = m_fHeight1 * kp.m_fScale;
		fHeight2 = m_fHeight2 * kp.m_fScale;
	}

	float fParentScale = m_pGfx->GetActualScale();

	if (m_bRotFromView)
	{
		fWidth1 *= fParentScale;
		fWidth2 *= fParentScale;
		fHeight1 *= fParentScale;
		fHeight2 *= fParentScale;

		A3DVECTOR3 vPos = GetParentTM() * kp.m_vPos;
		A3DVECTOR3 vDir = RotateVec(GetParentDir() * kp.m_vDir, _unit_z);
		A3DVECTOR3 vView = vPos - pView->GetCamera()->GetPos();
		A3DVECTOR3 vUp = CrossProduct(vView, vDir);
		vUp.Normalize();

		A3DVECTOR3 verts[4];

		float fU[] = {
			m_fTexU,
			m_fTexU,
			m_fTexU + m_fTexWid,
			m_fTexU + m_fTexWid
		};

		float fV[] = {
			m_fTexV + m_fTexHei,
			m_fTexV,
			m_fTexV + m_fTexHei,
			m_fTexV
		};

		if (m_bUReverse)
		{
			gfx_swap(fU[0], fU[2]);
			gfx_swap(fU[1], fU[3]);
		}

		if (m_bVReverse)
		{
			gfx_swap(fV[0], fV[1]);
			gfx_swap(fV[2], fV[3]);
		}

		if (m_bUVInterchange)
		{
			gfx_swap(fU[0], fV[0]);
			gfx_swap(fU[1], fV[1]);
			gfx_swap(fU[2], fV[2]);
			gfx_swap(fU[3], fV[3]);
		}

		verts[0]	= vPos - (vDir * fHeight1) + (vUp * fWidth2);
		verts[1]	= vPos + (vDir * fHeight2) + (vUp * fWidth2);
		verts[2]	= vPos - (vDir * fHeight1) - (vUp * fWidth1);
		verts[3]	= vPos + (vDir * fHeight2) - (vUp * fWidth1);

		pVerts->x	= verts[0].x;
		pVerts->y	= verts[0].y;
		pVerts->z	= verts[0].z;
		pVerts->tu	= fU[0];
		pVerts->tv	= fV[0];
		pVerts++;

		pVerts->x	= verts[1].x;
		pVerts->y	= verts[1].y;
		pVerts->z	= verts[1].z;
		pVerts->tu	= fU[1];
		pVerts->tv	= fV[1];
		pVerts++;

		pVerts->x	= verts[2].x;
		pVerts->y	= verts[2].y;
		pVerts->z	= verts[2].z;
		pVerts->tu	= fU[2];
		pVerts->tv	= fV[2];
		pVerts++;

		pVerts->x	= verts[3].x;
		pVerts->y	= verts[3].y;
		pVerts->z	= verts[3].z;
		pVerts->tu	= fU[3];
		pVerts->tv	= fV[3];

		m_nRectNum = 1;
		return false;
	}
	else
	{
		if (m_bGroundNormal && !m_bGrndNormOnly && m_pGfx->IsCloseToGrnd() && m_pGfx->MatchGrnd())
		{
			fWidth1 *= fParentScale;
			fWidth2 *= fParentScale;
			fHeight1 *= fParentScale;
			fHeight2 *= fParentScale;
			float fWidth = fWidth1 + fWidth2;
			float fHeight = fHeight1 + fHeight2;
			A3DVECTOR3 vCenter = m_pGfx->GetParentTM() * kp.m_vPos;

			int nRow = int(fHeight / _grnd_grid);
			if (nRow == 0) nRow++;
			if (nRow > _max_row_or_col) nRow = _max_row_or_col;
			int nCol = int(fWidth / _grnd_grid);
			if (nCol == 0) nCol++;
			if (nCol > _max_row_or_col) nCol = _max_row_or_col;

			A3DMATRIX4 matTran(A3DMATRIX4::IDENTITY);
			// Gfx Direction and keypoint's direction will make effect on the decal
			if (m_bYawEffectOnGrndNorm)
			{
				A3DQUATERNION quad = GetParentDir() * kp.m_vDir;
				A3DVECTOR3 vNewNorm = quad * _unit_z;

				A3DQUATERNION quad_mix;

				vNewNorm.Normalize();
				float fAngle = DotProduct(vNewNorm, _unit_y);

				if (0)//(fabs(fAngle) <= _grnd_delta)
				{
					A3DQUATERNION quad2;
 					AxisAngleToQuad(CrossProduct(vNewNorm, _unit_y), acosf(fAngle), quad2);
					quad_mix = quad2 * quad;
				}
				else
					quad_mix = quad;

				quad_mix.Normalize();
 				quad_mix.ConvertToMatrix(matTran);
				matTran.SetRow(3, vCenter);
			}

			float fGrdSzH = fHeight / nRow;
			float fGrdSzW = fWidth / nCol;

			A3DLVERTEX p[(_max_row_or_col + 1) * (_max_row_or_col + 1)];
			float fRow, fCol;
			int nBase, i, j;

			for (i = 0; i <= nRow; i++)
			{
				fRow = fHeight2 - i * fGrdSzH;
				int nBaseRow = i * (nCol + 1);
				float fV = m_fTexV + m_fTexHei * (m_bVReverse ? nRow - i : i) / nRow;

				for (j = 0; j <= nCol; j++)
				{
					nBase = nBaseRow + j;
					A3DLVERTEX& vert = p[nBase];
					float fU = m_fTexU + m_fTexWid * (m_bUReverse ? nCol - j : j) / nCol;

					fCol = -fWidth1 + j * fGrdSzW;
					
					// matTran only use in this case
					if (m_bYawEffectOnGrndNorm)
					{
						A3DVECTOR3 vPos = matTran * A3DVECTOR3(fCol, fRow, 0);
						vert.x = vPos.x;
						vert.z = vPos.z;
					}
					else
					{
						vert.x = vCenter.x + fCol;
						vert.z = vCenter.z + fRow;
					}
					vert.y = AfxGetGrndNorm(*(A3DVECTOR3*)&vert, NULL) + .2f;
					
					if (m_bUVInterchange)
					{
						vert.tu = fV;
						vert.tv = fU;
					}
					else
					{
						vert.tu = fU;
						vert.tv = fV;
					}
				}
			}

			for (i = 0; i < nRow; i++)
			{
				int r1 = i * (nCol + 1);
				int r2 = r1 + nCol + 1;
				int nBaseRow = i * nCol;

				for (j = 0; j < nCol; j++)
				{
					int nBaseCol = nBaseRow + j;
					nBaseCol *= 4;

					A3DLVERTEX& vert1 = p[r1 + j];
					A3DLVERTEX& vert2 = p[r1 + j + 1];
					A3DLVERTEX& vert3 = p[r2 + j];
					A3DLVERTEX& vert4 = p[r2 + j + 1];

					pVerts[nBaseCol  ] = vert1;
					pVerts[nBaseCol+1] = vert2;
					pVerts[nBaseCol+2] = vert3;
					pVerts[nBaseCol+3] = vert4;
				}
			}

			m_nRectNum = nRow * nCol;
			return false;
		}
		else
		{
			float fU[] = {
				m_fTexU,
				m_fTexU + m_fTexWid,
				m_fTexU,
				m_fTexU + m_fTexWid
			};

			float fV[] = {
				m_fTexV,
				m_fTexV,
				m_fTexV + m_fTexHei,
				m_fTexV + m_fTexHei
			};

			if (m_bUReverse)
			{
				gfx_swap(fU[0], fU[1]);
				gfx_swap(fU[2], fU[3]);
			}

			if (m_bVReverse)
			{
				gfx_swap(fV[0], fV[2]);
				gfx_swap(fV[1], fV[3]);
			}

			if (m_bUVInterchange)
			{
				gfx_swap(fU[0], fV[0]);
				gfx_swap(fU[1], fV[1]);
				gfx_swap(fU[2], fV[2]);
				gfx_swap(fU[3], fV[3]);
			}
			
			pVerts[0].x			= -fWidth1;
			pVerts[0].y			= fHeight2;
			pVerts[0].z			= 0.0f;
			pVerts[0].tu		= fU[0];
			pVerts[0].tv		= fV[0];

			pVerts[1].x			= fWidth2;
			pVerts[1].y			= fHeight2;
			pVerts[1].z			= 0.0f;
			pVerts[1].tu		= fU[1];
			pVerts[1].tv		= fV[1];

			pVerts[2].x			= -fWidth1;
			pVerts[2].y			= -fHeight1;
			pVerts[2].z			= 0.0f;
			pVerts[2].tu		= fU[2];
			pVerts[2].tv		= fV[2];

			pVerts[3].x			= fWidth2;
			pVerts[3].y			= -fHeight1;
			pVerts[3].z			= 0.0f;
			pVerts[3].tu		= fU[3];
			pVerts[3].tv		= fV[3];

			m_nRectNum = 1;
			return true;
		}
	}
}

bool A3DDecalEx::Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex)
{
	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());

	if (A3DCOLOR_GETALPHA(diffuse) < 5)
		return false;

	A3DLVERTEX VertBuf[_max_row_or_col * _max_row_or_col * 4];
	bool bLocal = Fill_Verts_3D(kp, pView, VertBuf);
	int nVertCount = 4 * m_nRectNum;
	A3DMATRIX4 matWorld = bLocal ? GetTranMatrix(kp) * GetParentTM() : _identity_mat4;

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

	for (int i = 0; i < nVertCount; i++)
	{
		A3DGFXVERTEX* pVert = &pVerts[i];
		A3DLVERTEX& v = VertBuf[i];

		pVert->x			= v.x;
		pVert->y			= v.y;
		pVert->z			= v.z;
		pVert->dwMatIndices	= nMatrixIndex; 
		pVert->tu			= v.tu;
		pVert->tv			= v.tv;
		pVert->diffuse		= diffuse;
		pVert->specular		= A3DCOLORRGBA(0, 0, 0, 255);

#ifdef GFX_EDITOR

//		if (m_pGfx->IsCalcingAABB())
		{
			A3DVECTOR3 v(pVert->x, pVert->y, pVert->z);

			if (bLocal)
			{
				A3DMATRIX4 matTran = GetTranMatrix(kp) * GetParentTM();
				v = matTran * v;
			}
			
//			A3DAABB& aabb = m_pGfx->GetAABBOrgRef();
			A3DAABB& aabb = m_AABB;

			if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
			if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
			if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
			if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
			if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
			if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;
		}
#endif

	}

	return true;
}

// 如果是mirror的camera,则把v交换
inline void _map_id(int index[4], bool bMirror)
{
	if (bMirror)
	{
		index[0] = 2;
		index[1] = 3;
		index[2] = 0;
		index[3] = 1;
	}
	else
	{
		index[0] = 0;
		index[1] = 1;
		index[2] = 2;
		index[3] = 3;
	}
}

inline bool A3DDecalEx::Fill_Verts_2D(
	const KEY_POINT& kp,
	A3DViewport* pView,
	A3DTLVERTEX* pVerts,
	float& fWidth,
	float& fHeight)
{
	A3DVECTOR3 vCenterWorld = GetParentTM() * kp.m_vPos;
	A3DMATRIX4 matVPS = pView->GetCamera()->GetVPTM() * pView->GetViewScale();
	A3DVECTOR4 vCenter = TransformToScreen(vCenterWorld, matVPS);

	if (vCenter.z <= 0 || vCenter.z >= 1.0f)
		return false;

	if (m_fZOffset != 0.0f)
	{
		A3DVECTOR3 vZCenter = vCenterWorld - pView->GetCamera()->GetDir() * (m_fZOffset * m_pGfx->GetActualScale());
		vZCenter = matVPS * vZCenter;
		vCenter.z = vZCenter.z;
		a_Clamp(vCenter.z, 0.0f, 1.0f);
	}

	A3DCameraBase* pCamera = pView->GetCamera();
	float fScale;

	if (m_b2DScreenDimension)
		fScale = m_pGfx->GetActualScale();
	else
	{
		A3DVECTOR3 vExts = vCenterWorld + pCamera->GetRight();
		vExts = matVPS * vExts;
		fScale = (vExts.x - vCenter.x) * m_pGfx->GetActualScale();
	}

	float fWidth1, fWidth2, fHeight1, fHeight2;
	if (m_bNoWidthScale)
	{
		fWidth1 = m_fWidth1 * fScale;
		fWidth2 = m_fWidth2 * fScale;
	}
	else
	{
		fWidth1 = m_fWidth1 * kp.m_fScale * fScale;
		fWidth2 = m_fWidth2 * kp.m_fScale * fScale;
	}

	if (m_bNoHeightScale)
	{
		fHeight1 = m_fHeight1 * fScale;
		fHeight2 = m_fHeight2 * fScale;
	}
	else
	{
		fHeight1 = m_fHeight1 * kp.m_fScale * fScale;
		fHeight2 = m_fHeight2 * kp.m_fScale * fScale;
	}

	fWidth = 0.5f * (fWidth1 + fWidth2);
	fHeight = 0.5f * (fHeight1 + fHeight2);

	A3DVECTOR3 vert[] = {
		A3DVECTOR3(-fWidth1,  fHeight2, vCenter.z),
		A3DVECTOR3( fWidth2,  fHeight2, vCenter.z),
		A3DVECTOR3(-fWidth1, -fHeight1, vCenter.z),
		A3DVECTOR3( fWidth2, -fHeight1, vCenter.z)
	};

	float fU[] = {
		m_fTexU,
		m_fTexU + m_fTexWid,
		m_fTexU,
		m_fTexU + m_fTexWid
	};

	float fV[] = {
		m_fTexV + m_fTexHei,
		m_fTexV + m_fTexHei,
		m_fTexV,
		m_fTexV
	};

	if (m_bUReverse)
	{
		gfx_swap(fU[0], fU[1]);
		gfx_swap(fU[2], fU[3]);
	}

	if (m_bVReverse)
	{
		gfx_swap(fV[0], fV[2]);
		gfx_swap(fV[1], fV[3]);
	}

	if (m_bUVInterchange)
	{
		gfx_swap(fU[0], fV[0]);
		gfx_swap(fU[1], fV[1]);
		gfx_swap(fU[2], fV[2]);
		gfx_swap(fU[3], fV[3]);
	}

	float fSin = 0;
	float fCos = 1.0f;

	if (kp.m_fRad2D)
	{
		float fAngle = pView->GetCamera()->IsMirrored() ? -kp.m_fRad2D : kp.m_fRad2D;
		fSin = (float)sin(fAngle);
		fCos = (float)cos(fAngle);
	}

	pVerts->x	= vCenter.x + vert[0].x * fCos + vert[0].y * fSin;
	pVerts->y	= vCenter.y + vert[0].y * fCos - vert[0].x * fSin;
	pVerts->z	= vert[0].z;
	pVerts->rhw = vCenter.w;
	pVerts->tu	= fU[0];
	pVerts->tv	= fV[0];
	pVerts++;

	pVerts->x	= vCenter.x + vert[1].x * fCos + vert[1].y * fSin;
	pVerts->y	= vCenter.y + vert[1].y * fCos - vert[1].x * fSin;
	pVerts->z	= vert[1].z;
	pVerts->rhw = vCenter.w;
	pVerts->tu	= fU[1];
	pVerts->tv	= fV[1];
	pVerts++;

	pVerts->x	= vCenter.x + vert[2].x * fCos + vert[2].y * fSin;
	pVerts->y	= vCenter.y + vert[2].y * fCos - vert[2].x * fSin;
	pVerts->z	= vert[2].z;
	pVerts->rhw = vCenter.w;
	pVerts->tu	= fU[2];
	pVerts->tv	= fV[2];
	pVerts++;

	pVerts->x	= vCenter.x + vert[3].x * fCos + vert[3].y * fSin;
	pVerts->y	= vCenter.y + vert[3].y * fCos - vert[3].x * fSin;
	pVerts->z	= vert[3].z;
	pVerts->rhw = vCenter.w;
	pVerts->tu	= fU[3];
	pVerts->tv	= fV[3];

	return true;
}

#ifdef GFX_EDITOR
	#define CALC_AABB_2D \
		{ \
			A3DVECTOR3 v(pVerts->x, pVerts->y, pVerts->z); \
			pView->InvTransform(v, v); \
		\
			A3DAABB& aabb = m_AABB; m_pGfx->GetAABBOrgRef(); \
			if (v.x < aabb.Mins.x) aabb.Mins.x = v.x; \
			if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x; \
			if (v.y < aabb.Mins.y) aabb.Mins.y = v.y; \
			if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y; \
			if (v.z < aabb.Mins.z) aabb.Mins.z = v.z; \
			if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z; \
		}
#else
	#define CALC_AABB_2D
#endif

bool A3DDecalEx::Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts)
{
	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());

	if (A3DCOLOR_GETALPHA(diffuse) < 5)
		return false;

	A3DTLVERTEX vert[4];
	float fWidth, fHeight;

	if (!Fill_Verts_2D(kp, pView, vert, fWidth, fHeight))
		return false;

#ifdef GFX_EDITOR
	m_AABB.Clear(); 
#endif

	int id[4];
	_map_id(id, pView->GetCamera()->IsMirrored());
	
	pVerts->x		= vert[0].x;
	pVerts->y		= vert[0].y;
	pVerts->z		= vert[0].z;
	pVerts->rhw		= vert[0].rhw;
	pVerts->tu		= vert[id[0]].tu;
	pVerts->tv		= vert[id[0]].tv;
	pVerts->diffuse	= diffuse;
	pVerts->specular= A3DCOLORRGBA(0, 0, 0, 255);
	CALC_AABB_2D
	pVerts++;

	pVerts->x		= vert[1].x;
	pVerts->y		= vert[1].y;
	pVerts->z		= vert[1].z;
	pVerts->rhw		= vert[1].rhw;
	pVerts->tu		= vert[id[1]].tu;
	pVerts->tv		= vert[id[1]].tv;
	pVerts->diffuse	= diffuse;
	pVerts->specular= A3DCOLORRGBA(0, 0, 0, 255);
	CALC_AABB_2D
	pVerts++;

	pVerts->x		= vert[2].x;
	pVerts->y		= vert[2].y;
	pVerts->z		= vert[2].z;
	pVerts->rhw		= vert[2].rhw;
	pVerts->tu		= vert[id[2]].tu;
	pVerts->tv		= vert[id[2]].tv;
	pVerts->diffuse	= diffuse;
	pVerts->specular= A3DCOLORRGBA(0, 0, 0, 255);
	CALC_AABB_2D
	pVerts++;

	pVerts->x		= vert[3].x;
	pVerts->y		= vert[3].y;
	pVerts->z		= vert[3].z;
	pVerts->rhw		= vert[3].rhw;
	pVerts->tu		= vert[id[3]].tu;
	pVerts->tv		= vert[id[3]].tv;
	pVerts->diffuse	= diffuse;
	pVerts->specular= A3DCOLORRGBA(0, 0, 0, 255);
	CALC_AABB_2D

	return true;
}

int A3DDecalEx::DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts)
{
	if (GetVertsCount() > nMaxVerts)
		return 0;

	KEY_POINT kp;
	if (!GetCurKeyPoint(&kp)) return 0;

	float fAlpha = A3DCOLOR_GETALPHA(kp.m_color) * m_pGfx->GetActualAlpha() / 255.f * .5f;

	A3DLVERTEX VertBuf[_max_row_or_col * _max_row_or_col * 4];
	bool bLocal = Fill_Verts_3D(kp, pView, VertBuf);

	int nVertCount = 4 * m_nRectNum;

	if (bLocal)
	{
		A3DMATRIX4 matWorld = GetTranMatrix(kp) * GetParentTM();

		for (int i = 0; i < nVertCount; i++)
		{
			A3DWARPVERTEX* pVert = &pVerts[i];
			A3DLVERTEX& v = VertBuf[i];

			pVert->pos.Set(v.x, v.y, v.z);
			pVert->pos	= matWorld * pVert->pos;
			pVert->u1	= v.tu;
			pVert->v1	= v.tv;
			pVert->u3	= fAlpha;
			pVert->v3	= fAlpha;
		}
	}
	else
	{
		for (int i = 0; i < nVertCount; i++)
		{
			A3DWARPVERTEX* pVert = &pVerts[i];
			A3DLVERTEX& v = VertBuf[i];

			pVert->pos.Set(v.x, v.y, v.z);
			pVert->u1	= v.tu;
			pVert->v1	= v.tv;
			pVert->u3	= fAlpha;
			pVert->v3	= fAlpha;
		}
	}

	return nVertCount;
}

int A3DDecalEx::DrawToBuffer(
	A3DViewport* pView,
	A3DTLWARPVERTEX* pVerts,
	int nMaxVerts,
	float rw,
	float rh)
{
	if (GetVertsCount() > nMaxVerts)
		return 0;

	KEY_POINT kp;
	if (!GetCurKeyPoint(&kp)) return 0;

	float fAlpha = A3DCOLOR_GETALPHA(kp.m_color) * m_pGfx->GetActualAlpha() / 255.f;

	A3DTLVERTEX vert[4];
	float fWidth, fHeight;

	if (!Fill_Verts_2D(kp, pView, vert, fWidth, fHeight))
		return 0;

	float fInvAlpha = min(1.0f - fWidth * fWidth * rw * rw * 4.0f, 1.0f - fHeight * fHeight * rh * rh * 4.0f);
	a_Clamp(fInvAlpha, 0.0f, 1.0f);

	if (fInvAlpha < 0.1f)
		return 0;

	pVerts->pos.Set(vert[0].x, vert[0].y, vert[0].z, 1.0f);
	pVerts->u1	= vert[0].tu;
	pVerts->v1	= vert[0].tv;
	pVerts->u2	= pVerts->pos.x * rw;
	pVerts->v2	= pVerts->pos.y * rh;
	pVerts->u3	= fWidth * rw * fAlpha * fInvAlpha;
	a_Clamp(pVerts->u3, 0.0f, 0.5f);
	pVerts->v3	= fHeight * rh * fAlpha * fInvAlpha;
	a_Clamp(pVerts->v3, 0.0f, 0.5f);
	pVerts++;

	pVerts->pos.Set(vert[1].x, vert[1].y, vert[1].z, 1.0f);
	pVerts->u1	= vert[1].tu;
	pVerts->v1	= vert[1].tv;
	pVerts->u2	= pVerts->pos.x * rw;
	pVerts->v2	= pVerts->pos.y * rh;
	pVerts->u3	= fWidth * rw * fAlpha * fInvAlpha;
	a_Clamp(pVerts->u3, 0.0f, 0.5f);
	pVerts->v3	= fHeight * rh * fAlpha * fInvAlpha;
	a_Clamp(pVerts->v3, 0.0f, 0.5f);
	pVerts++;

	pVerts->pos.Set(vert[2].x, vert[2].y, vert[2].z, 1.0f);
	pVerts->u1	= vert[2].tu;
	pVerts->v1	= vert[2].tv;
	pVerts->u2	= pVerts->pos.x * rw;
	pVerts->v2	= pVerts->pos.y * rh;
	pVerts->u3	= fWidth * rw * fAlpha * fInvAlpha;
	a_Clamp(pVerts->u3, 0.0f, 0.5f);
	pVerts->v3	= fHeight * rh * fAlpha * fInvAlpha;
	a_Clamp(pVerts->v3, 0.0f, 0.5f);
	pVerts++;

	pVerts->pos.Set(vert[3].x, vert[3].y, vert[3].z, 1.0f);
	pVerts->u1	= vert[3].tu;
	pVerts->v1	= vert[3].tv;
	pVerts->u2	= pVerts->pos.x * rw;
	pVerts->v2	= pVerts->pos.y * rh;
	pVerts->u3	= fWidth * rw * fAlpha * fInvAlpha;
	a_Clamp(pVerts->u3, 0.0f, 0.5f);
	pVerts->v3	= fHeight * rh * fAlpha * fInvAlpha;
	a_Clamp(pVerts->v3, 0.0f, 0.5f);

	return 4;
}

bool A3DDecalEx::Update_Billboard(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex)
{
	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());

	if (A3DCOLOR_GETALPHA(diffuse) < 5)
		return false;

	float fWidth	= kp.m_fScale * m_fWidth;
	float fHeight	= kp.m_fScale * m_fHeight;

	A3DMATRIX4 matWorld = GetTranMatrix(kp) * GetParentTM();

	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(matWorld);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
	{
		matWorld.Transpose();
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
	}
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);

	float fU[] = {
		m_fTexU,
		m_fTexU + m_fTexWid,
		m_fTexU,
		m_fTexU + m_fTexWid
	};

	float fV[] = {
		m_fTexV,
		m_fTexV,
		m_fTexV + m_fTexHei,
		m_fTexV + m_fTexHei
	};

	if (m_bUReverse)
	{
		gfx_swap(fU[0], fU[1]);
		gfx_swap(fU[2], fU[3]);
	}

	if (m_bVReverse)
	{
		gfx_swap(fV[0], fV[2]);
		gfx_swap(fV[1], fV[3]);
	}

	if (m_bUVInterchange)
	{
		gfx_swap(fU[0], fV[0]);
		gfx_swap(fU[1], fV[1]);
		gfx_swap(fU[2], fV[2]);
		gfx_swap(fU[3], fV[3]);
	}

	pVerts[0].x			= -fWidth;
	pVerts[0].y			= fHeight;
	pVerts[0].z			= 0.0f;
	pVerts[0].dwMatIndices = nMatrixIndex;
	pVerts[0].tu		= fU[0];
	pVerts[0].tv		= fV[0];
	pVerts[0].diffuse	= diffuse;
	pVerts[0].specular	= A3DCOLORRGBA(0, 0, 0, 255);

	pVerts[1].x			= fWidth;
	pVerts[1].y			= fHeight;
	pVerts[1].z			= 0.0f;
	pVerts[1].dwMatIndices = nMatrixIndex;
	pVerts[1].tu		= fU[1];
	pVerts[1].tv		= fV[1];
	pVerts[1].diffuse	= diffuse;
	pVerts[1].specular	= A3DCOLORRGBA(0, 0, 0, 255);

	pVerts[2].x			= -fWidth;
	pVerts[2].y			= -fHeight;
	pVerts[2].z			= 0.0f;
	pVerts[2].dwMatIndices = nMatrixIndex;
	pVerts[2].tu		= fU[2];
	pVerts[2].tv		= fV[2];
	pVerts[2].diffuse	= diffuse;
	pVerts[2].specular	= A3DCOLORRGBA(0, 0, 0, 255);

	pVerts[3].x			= fWidth;
	pVerts[3].y			= -fHeight;
	pVerts[3].z			= 0.0f;
	pVerts[3].dwMatIndices = nMatrixIndex;
	pVerts[3].tu		= fU[3];
	pVerts[3].tv		= fV[3];
	pVerts[3].diffuse	= diffuse;
	pVerts[3].specular	= A3DCOLORRGBA(0, 0, 0, 255);

	pVerts[4].x			= 0.0f;
	pVerts[4].y			= fHeight;
	pVerts[4].z			= fWidth;
	pVerts[4].dwMatIndices = nMatrixIndex;
	pVerts[4].tu		= fU[0];
	pVerts[4].tv		= fV[0];
	pVerts[4].diffuse	= diffuse;
	pVerts[4].specular	= A3DCOLORRGBA(0, 0, 0, 255);

	pVerts[5].x			= 0.0f;
	pVerts[5].y			= fHeight;
	pVerts[5].z			= -fWidth;
	pVerts[5].dwMatIndices = nMatrixIndex;
	pVerts[5].tu		= fU[1];
	pVerts[5].tv		= fV[1];
	pVerts[5].diffuse	= diffuse;
	pVerts[5].specular	= A3DCOLORRGBA(0, 0, 0, 255);

	pVerts[6].x			= 0.0f;
	pVerts[6].y			= -fHeight;
	pVerts[6].z			= fWidth;
	pVerts[6].dwMatIndices = nMatrixIndex;
	pVerts[6].tu		= fU[2];
	pVerts[6].tv		= fV[2];
	pVerts[6].diffuse	= diffuse;
	pVerts[6].specular	= A3DCOLORRGBA(0, 0, 0, 255);

	pVerts[7].x			= 0.0f;
	pVerts[7].y			= -fHeight;
	pVerts[7].z			= -fWidth;
	pVerts[7].dwMatIndices = nMatrixIndex;
	pVerts[7].tu		= fU[3];
	pVerts[7].tv		= fV[3];
	pVerts[7].diffuse	= diffuse;
	pVerts[7].specular	= A3DCOLORRGBA(0, 0, 0, 255);

#ifdef GFX_EDITOR
	A3DMATRIX4 matTran = GetTranMatrix(kp) * GetParentTM();

	m_AABB.Clear();
	for (int i = 0; i < 8; i++)
	{
		A3DGFXVERTEX* pVert = &pVerts[i];
		A3DVECTOR3 v(pVert->x, pVert->y, pVert->z);
		v = matTran * v;

//		if (m_pGfx->IsCalcingAABB())
		{
//			A3DAABB& aabb = m_pGfx->GetAABBOrgRef();
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

	return true;
}

bool A3DDecalEx::Update(A3DViewport* pView)
{
	if (m_bMatchSurface)
	{
		KEY_POINT kp;

		if (!GetCurKeyPoint(&kp))
			return false;

		float fWidth = kp.m_fScale * m_fWidth * m_pGfx->GetScale();
		float fHeight = kp.m_fScale * m_fHeight * m_pGfx->GetScale();
		A3DVECTOR3 vCenter = m_pGfx->GetParentTM() * kp.m_vPos;
		float fRadius;

//#ifdef GFX_EDITOR
		fRadius = max(fWidth, fHeight) * 1.5f;
//#else
//		fRadius = m_fMaxExtent;
//#endif

		a_Clamp(fRadius, 0.1f, 15.0f);
		//	Originally, radius passed to client is limited by the max extent
		//	but the max extent is not affected by scale, which may lead to a unwanted result
		//	remove this limitation might lead to another situation in which artists may set something
		//	that make the radius be a value up to 100 meters or something like that
		//	then we need the client to make some validation on the passed in radius...

		if (SetupSurfaceData(vCenter, fRadius))
		{
			A3DVECTOR3 vDir;
			A3DQUATERNION q;

			if (m_bSurfaceUseParentDir)
			{
				if (kp.m_fRad2D)
					q = GetParentDir() * kp.m_vDir * A3DQUATERNION(_unit_y, kp.m_fRad2D);
				else
					q = GetParentDir() * kp.m_vDir;

				vDir = RotateVec(q, _unit_z);
				vDir.y = 0;

				if (vDir.Normalize() == 0.0f)
					vDir = _unit_z;
			}
			else
			{
				if (kp.m_fRad2D)
					vDir = RotateVec(A3DQUATERNION(_unit_y, kp.m_fRad2D), _unit_z);
				else
					vDir = _unit_z;
			}
			
			if (m_pGfx->IsUsingOuterColor())
				kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

			//	For compatible reason
			bool bUReverse = false;
			bool bVReverse = false;
			bool bUVInterChange = false;
			if (m_bMatchSurfaceUVEffects)
			{
				bUReverse = m_bUReverse;
				bVReverse = m_bVReverse;
				bUVInterChange = m_bUVInterchange;
			}

			bool ret = AfxGetGFXExMan()->GetSurfaceRender().Render(
				pView,
				m_pVerts,
				m_nVertCount,
				m_pIndices,
				m_nIndexCount,
				vCenter,
				fWidth,
				fHeight,
				fRadius,
				m_pTexture,
				m_Shader.SrcBlend,
				m_Shader.DestBlend,
				kp.m_color,
				vDir,
				bUReverse,
				bVReverse,
				bUVInterChange);

			if (ret)
			{
				AfxGetGFXExMan()->AddTotalPrimCount(m_nIndexCount/3);
			}
		}

#ifdef GFX_EDITOR


		float ext = max(fWidth, fHeight) * 1.5f;
		float left = vCenter.x - ext;
		float right = vCenter.x + ext;
		float top = vCenter.z + ext;
		float bottom = vCenter.z - ext;
		m_AABB.Clear();
		A3DAABB& aabb = m_AABB; 
		if (left < aabb.Mins.x) aabb.Mins.x = left;
		if (right > aabb.Maxs.x) aabb.Maxs.x = right;
		if (bottom < aabb.Mins.z) aabb.Mins.z = bottom;
		if (top > aabb.Maxs.z) aabb.Maxs.z = top;
		float fHalfLength = max(top-bottom, right-left)/2;
// 		if (vCenter.y < aabb.Mins.y) aabb.Mins.y = vCenter.y;
// 		if (vCenter.y > aabb.Maxs.y) aabb.Maxs.y = vCenter.y;
		aabb.Mins.y = vCenter.y - fHalfLength;
		aabb.Maxs.y = vCenter.y + fHalfLength;

		if (m_fMaxExtent < ext) m_fMaxExtent = ext;

		int nPrimCount = m_nIndexCount / 3;
		if(nPrimCount > 0)
			AfxGetGFXExMan()->AddElementPrimCount(GetEleTypeId(), nPrimCount);
#endif

		return false;
	}

	if (m_bWarp)
	{
		AfxGetGFXExMan()->AddWarpEle(this);
		return false;
	}

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

bool A3DDecalEx::Play()
{
	if (!IsInit())
	{
		InitBaseData();

		if (m_bMatchSurface)
		{
			m_bCenterUpdate = false;
			ChangeTexture();
		}
		else if (m_nEleType == ID_ELE_TYPE_DECAL_2D)
		{
			m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
				m_nRenderLayer,
				A3DFVF_A3DTLVERTEX,
				sizeof(A3DTLVERTEX),
				A3DPT_TRIANGLELIST,
				m_strTexture,
				m_Shader,
				m_bTileMode,
				m_bTexNoDownSample,
				m_pGfx->IsZTestEnable() && m_bZEnable,
				HasPixelShader(),
                m_bSoftEdge,
				m_bAbsTexPath);
		}
		else
		{
			m_dwRenderSlot = AfxGetGFXExMan()->GetRenderMan().RegisterSlot(
				m_nRenderLayer,
				A3DFVF_GFXVERTEX,
				sizeof(A3DGFXVERTEX),
				A3DPT_TRIANGLELIST,
				m_strTexture,
				m_Shader,
				m_bTileMode,
				m_bTexNoDownSample,
				m_pGfx->IsZTestEnable() && m_bZEnable,
				HasPixelShader(),
                m_bSoftEdge,
				m_bAbsTexPath);
		}

		SetInit(true);
	}

	return true;
}

bool A3DDecalEx::SetupSurfaceData(const A3DVECTOR3& vCenter, float fRadius)
{
#ifdef GFX_EDITOR

	m_bCenterUpdate = false;

#endif

	if (m_bCenterUpdate)
	{
		if (fRadius <= m_fOldRadius && (vCenter - m_vOldCenter).SquaredMagnitude() < 1e-6)
			return true;
	}
	else
	{
		if (!m_pVerts)
			m_pVerts = new A3DVECTOR3[_max_verts];

		if (!m_pIndices)
			m_pIndices = new WORD[_max_indices];
	}

	int nVertCount = _max_verts;
	int nIndexCount = _max_indices;

	if (!gGfxGetSurfaceData(vCenter, fRadius, m_pVerts, nVertCount, m_pIndices, nIndexCount))
		return false;

	if (nVertCount > _max_verts || nIndexCount > _max_indices)
		return false;

	m_vOldCenter = vCenter;
	m_fOldRadius = fRadius;
	m_bCenterUpdate = true;
	m_nVertCount = nVertCount;
	m_nIndexCount = nIndexCount;
	return true;
}
