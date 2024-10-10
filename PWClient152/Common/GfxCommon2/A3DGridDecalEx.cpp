#include "StdAfx.h"
#include "A3DGridDecalEx.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "A3DGFXGridAnimation.h"
#include <A3D.h>

static const char * _format_w_num		= "wNumber: %d";
static const char * _format_h_num		= "hNumber: %d";
static const char * _format_vertex		= "%f %f %f";
static const char * _format_color		= "dwColor: %x";
static const char * _format_gridsize	= "fGridSize: %f";
static const char * _format_zoffset		= "fZOffset: %f";
static const char * _format_affectbyscale = "AffByScl: %d";
static const char * _format_rotfromview = "RotFromView: %d";
static const char * _format_offsetheight = "fOffsetHeight: %f";
static const char * _format_alwaysonground = "bAlwaysOnGround: %d";

static const int _default_vert_num = 4;

const float A3DGridDecalEx::s_fGridSize = .5f;

void A3DGridDecalEx::_setUVCoord(float *_pBufU, float *_pBufV)
{
	float fU0 = m_fTexU;
	float fU1 = m_fTexU + m_fTexWid;

	float fV0 = m_fTexV;
	float fV1 = m_fTexV + m_fTexHei;

	if (m_bUReverse)
		gfx_swap(fU0, fU1);

	if (m_bVReverse)
		gfx_swap(fV0, fV1);

	const float fUStep = (fU1 - fU0) / (m_nWidthVertNum - 1);
	const float fVStep = (fV1 - fV0) / (m_nHeightVertNum - 1);

	float *_pCurV = _pBufV;
	float *_pCurU = _pBufU;

	float fVCur = fV0;
	for (int iRow = 0 ; iRow < m_nHeightVertNum ; ++iRow)
	{
		float fUCur = fU0;
		for (int iCol = 0 ; iCol < m_nWidthVertNum ; ++iCol) {
			_pCurV[iCol] = fVCur;
			_pCurU[iCol] = fUCur;

			fUCur += fUStep;
		}

		fVCur += fVStep;
		_pCurU += m_nWidthVertNum;
		_pCurV += m_nWidthVertNum;
	}
}

//////////////////////////////////////////////////////////////////////////
// A3DGridDecalEx Class


A3DGridDecalEx::A3DGridDecalEx(A3DGFXEx* pGfx, int nType)
: A3DGFXElement(pGfx)
{
	m_nEleType		= nType;

	m_nWidthVertNum	= 0;
	m_nHeightVertNum= 0;

	m_fGridSize		= s_fGridSize;
	m_fZOffset		= 0;
	
	m_pVerts		= 0;
	m_pUVBuf		= 0;
	m_p2DVertBuf	= 0;
	m_pColorBuf		= 0;
	m_pWarp2DBuf	= 0;

	m_bAffectedByScale = false;
	m_bRotFromView = false;
	m_fOffsetHeight = 0.1f;

	m_bAlwaysOnGround = false;

	_setSize(_default_vert_num, _default_vert_num);

	m_nRectNum		= _calcCurRectNum();

	if (m_nEleType != ID_ELE_TYPE_GRID_DECAL_2D)
		m_bTLVert = false;

	m_pGridAnimation = new A3DGfxGridAnimation;
}

A3DGridDecalEx::~A3DGridDecalEx()
{	
	delete m_pGridAnimation;

	// Data for serialize
	delete[] m_pVerts;

	// Buffer for runtime
	delete[] m_pUVBuf;
	delete[] m_p2DVertBuf;
	delete[] m_pColorBuf;
	delete[] m_pWarp2DBuf;
}

bool A3DGridDecalEx::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	DWORD	dwReadLen;
	char	szLine[AFILE_LINEMAXLEN];

	if (pFile->IsBinary())
	{
		pFile->Read(&m_nWidthVertNum, sizeof(m_nWidthVertNum), &dwReadLen);
		pFile->Read(&m_nHeightVertNum, sizeof(m_nHeightVertNum), &dwReadLen);
		
		m_nRectNum = _calcCurRectNum();
		int nVertNum = m_nHeightVertNum * m_nWidthVertNum;
		_resetVertBuffer(nVertNum);
		
		pFile->Read(m_pVerts, sizeof(GridData) * nVertNum, &dwReadLen);
		int i;
		for (i = 0 ; i < nVertNum ; ++i) {
			A3DVECTOR3& tVertex = m_pVerts[i].m_vPos;
			pFile->Read(&tVertex, sizeof(A3DVECTOR3), &dwReadLen);
			
			// Added After 8.27
			A3DCOLOR& dwColor		= m_pVerts[i].m_dwColor;
			pFile->Read(&dwColor, sizeof(A3DCOLOR), &dwReadLen);
		}

		// 网格大小
		pFile->Read(&m_fGridSize, sizeof(m_fGridSize), &dwReadLen);

		if (dwVersion >= 60)
			pFile->Read(&m_fZOffset, sizeof(m_fZOffset), &dwReadLen);

	}
	else
	{
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_w_num, &m_nWidthVertNum);
		
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_h_num, &m_nHeightVertNum);

		m_nRectNum = _calcCurRectNum();
		int nVertNum = m_nHeightVertNum * m_nWidthVertNum;
		_resetVertBuffer(nVertNum);

		int i;
		for (i = 0 ; i < nVertNum ; ++i) {
			A3DVECTOR3& tVertex = m_pVerts[i].m_vPos;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_vertex, &tVertex.x, &tVertex.y, &tVertex.z);

			//	Added After 8.27
			A3DCOLOR& dwColor		= m_pVerts[i].m_dwColor;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_color, &dwColor);
		}

		//	网格大小
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_gridsize, &m_fGridSize);
	
		//	Added After 9.11
		if (dwVersion >= 60)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_zoffset, &m_fZOffset);
		}

		//	Added 2012/1/5
		if (dwVersion >= 99)
		{
			m_pGridAnimation->Load(pFile, dwVersion, nVertNum);
		}

		//	Added 2012/2/24
		if (dwVersion >= 100)
		{
			int iData = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_affectbyscale, &iData);
			m_bAffectedByScale = (iData != 0);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_rotfromview, &iData);
			m_bRotFromView = (iData != 0);
		}
		else
		{
			m_bAffectedByScale = false;
			m_bRotFromView = false;
		}

		//	Added 2012/3/8
		if (dwVersion >= 101)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_offsetheight, &m_fOffsetHeight);
		}
		
		if (dwVersion >= 114)
		{
			int iData = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_alwaysonground, &iData);
			m_bAlwaysOnGround = (iData != 0);
		}
	}
	return true;
}

bool A3DGridDecalEx::Save(AFile* pFile)
{
	if (pFile->IsBinary())
	{
		DWORD	dwWriteLen;
		pFile->Write(&m_nWidthVertNum, sizeof(m_nWidthVertNum), &dwWriteLen);
		pFile->Write(&m_nHeightVertNum, sizeof(m_nHeightVertNum), &dwWriteLen);

		int nVertNum = m_nHeightVertNum * m_nWidthVertNum;
		if (m_pVerts)
			pFile->Write(m_pVerts, sizeof(GridData) * nVertNum, &dwWriteLen);

		pFile->Write(&m_fGridSize, sizeof(m_fGridSize), &dwWriteLen);

		pFile->Write(&m_fZOffset, sizeof(m_fZOffset), & dwWriteLen);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		sprintf(szLine, _format_w_num, m_nWidthVertNum);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_h_num, m_nHeightVertNum);
		pFile->WriteLine(szLine);

		int nVertNum = m_nHeightVertNum * m_nWidthVertNum;
		int i;
		for (i = 0 ; i < nVertNum ; ++i) {
			A3DVECTOR3& tVertex = m_pVerts[i].m_vPos;
			sprintf(szLine, _format_vertex, tVertex.x, tVertex.y, tVertex.z);
			pFile->WriteLine(szLine);

			//	Added After 8.27
			A3DCOLOR& dwColor = m_pVerts[i].m_dwColor;
			sprintf(szLine, _format_color, dwColor);
			pFile->WriteLine(szLine);
		}
		
		sprintf(szLine, _format_gridsize, m_fGridSize);
		pFile->WriteLine(szLine);
		
		//	Added After 9.11
		sprintf(szLine, _format_zoffset, m_fZOffset);
		pFile->WriteLine(szLine);

		m_pGridAnimation->Save(pFile, nVertNum);

		//  Added After 2012.2.24
		sprintf(szLine, _format_affectbyscale, m_bAffectedByScale);
		pFile->WriteLine(szLine);

		sprintf(szLine, _format_rotfromview, m_bRotFromView);
		pFile->WriteLine(szLine);

		//	Added After 2012.3.8
		sprintf(szLine, _format_offsetheight, m_fOffsetHeight);
		pFile->WriteLine(szLine);

		//	Added After 2012.8.30
		sprintf(szLine, _format_alwaysonground, m_bAlwaysOnGround);
		pFile->WriteLine(szLine);

	}
	return true;
}

A3DGFXElement* A3DGridDecalEx::Clone(A3DGFXEx* pGfx) const
{
	A3DGridDecalEx* p = new A3DGridDecalEx(pGfx, m_nEleType);
	return &(*p = *this);
}

bool A3DGridDecalEx::Init(A3DDevice* pDevice)
{
	A3DGFXElement::Init(pDevice);
	
	return true;
}

A3DGridDecalEx& A3DGridDecalEx::operator = (const A3DGridDecalEx& src)
{
	if (&src == this)
		return *this;

	_CloneBase(&src);
	Init(src.m_pDevice);
	m_nWidthVertNum		=	src.m_nWidthVertNum;
	m_nHeightVertNum	=	src.m_nHeightVertNum;
	int nVertNum		=	m_nHeightVertNum * m_nWidthVertNum;
	_resetVertBuffer(nVertNum);
	memcpy(m_pVerts, src.m_pVerts, sizeof(GridData) * nVertNum);
	m_fGridSize			=	src.m_fGridSize;
	m_fZOffset			=	src.m_fZOffset;
	
	m_nRectNum			=	src.m_nRectNum;
	m_bAffectedByScale	=	src.m_bAffectedByScale;
	m_bRotFromView		=	src.m_bRotFromView;
	m_fOffsetHeight		=   src.m_fOffsetHeight;
	m_bAlwaysOnGround	=	src.m_bAlwaysOnGround;

	(*m_pGridAnimation) = (*src.m_pGridAnimation);

	return *this;
}

bool A3DGridDecalEx::Play()
{
	if (!IsInit())
	{
		InitBaseData();

		if (m_nEleType == ID_ELE_TYPE_GRID_DECAL_2D)
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

bool A3DGridDecalEx::Render(A3DViewport* pView)
{
	if (!CanRender())
		return true;

	Update(pView);
	return true;
}

bool A3DGridDecalEx::TickAnimation( DWORD dwTickTime )
{
	bool ret =  A3DGFXElement::TickAnimation(dwTickTime);
	m_pGridAnimation->GetValue(m_KeyPointSet.GetTimeElapsed(), m_pVerts, m_nHeightVertNum * m_nWidthVertNum);
	return ret;
}

bool A3DGridDecalEx::Update(A3DViewport* pView)
{
	if (m_bWarp)
	{
		AfxGetGFXExMan()->AddWarpEle(this);
		return false;
	}

	if (m_dwRenderSlot)
		AfxGetGFXExMan()->GetRenderMan().RegisterEleForRender(m_dwRenderSlot, this);

	return true;
}

int A3DGridDecalEx::GetVertsCount()
{
	// 这里返回的是render所需的count
	return m_nRectNum * 4;
}

int A3DGridDecalEx::FillVertexBuffer(void* pBuffer, int nMatrixIndex, A3DViewport* pView)
{
	KEY_POINT kp;

	if (!GetCurKeyPoint(&kp))
		return 0;

	if (m_pGfx->IsUsingOuterColor())
		kp.m_color = (A3DCOLORVALUE(kp.m_color) * m_pGfx->GetOuterColor()).ToRGBAColor();

	bool bRet = false;
	switch(m_nEleType) {
	case ID_ELE_TYPE_GRID_DECAL_3D:
		bRet = Update_3D(kp, pView, (A3DGFXVERTEX*)pBuffer, nMatrixIndex);
		break;
	case ID_ELE_TYPE_GRID_DECAL_2D:
		bRet = Update_2D(kp, pView, (A3DTLVERTEX*)pBuffer);
		break;
	default:
		break;
	}

	return bRet ? m_nRectNum * 4 : 0;
}

bool A3DGridDecalEx::Update_3D(const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex)
{
	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());

	if (A3DCOLOR_GETALPHA(diffuse) < 5)
		return false;
	
	// bLocal Means 3D Position in local space / world space
	bool bLocal = Fill_Verts_3D(kp, pView, pVerts, nMatrixIndex, diffuse, A3DCOLORRGBA(0, 0, 0, 255));
	int nVertCount = m_nRectNum * 4;
	A3DMATRIX4 matWorld;
	if (bLocal)
	{
		if (m_bAffectedByScale)
			matWorld = GetTranMatrix(kp) * a3d_Scaling( kp.m_fScale, kp.m_fScale, kp.m_fScale ) * GetParentTM();
		else
			matWorld = GetTranMatrix(kp) * GetParentTM();
	}
	else
	{
		matWorld = _identity_mat4;
	}
	
	// Common Code : Mean what ?
	if (A3DGFXRenderSlotMan::g_RenderMode == GRMSoftware)
		m_pDevice->SetWorldMatrix(matWorld);
	else if (A3DGFXRenderSlotMan::g_RenderMode == GRMVertexShader)
	{
		matWorld.Transpose();
		m_pDevice->SetVertexShaderConstants(nMatrixIndex * 3 + GFX_VS_CONST_BASE, &matWorld, 3);
	}
	else
		m_pDevice->SetIndexedVertexBlendMatrix(nMatrixIndex, matWorld);

	// only editor need this part
	// use pre-compile to reduce calculation pay-load
#ifdef GFX_EDITOR
	m_AABB.Clear();
	for (int i = 0; i < nVertCount; i++)
	{
		A3DGFXVERTEX* pVert = &pVerts[i];

		A3DVECTOR3 v(pVert->x, pVert->y, pVert->z);

		if (bLocal)
		{
			A3DMATRIX4 matWorld = GetTranMatrix(kp) * GetParentTM();
			v = matWorld * v;
		}

		A3DAABB& aabb = m_AABB; // m_pGfx->GetAABBOrgRef();
		if (v.x < aabb.Mins.x) aabb.Mins.x = v.x;
		if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x;
		if (v.y < aabb.Mins.y) aabb.Mins.y = v.y;
		if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y;
		if (v.z < aabb.Mins.z) aabb.Mins.z = v.z;
		if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z;
	}
#endif

	return true;
}

// For FillVertexBuffer 3D
inline bool A3DGridDecalEx::Fill_Verts_3D(
	const KEY_POINT& kp, A3DViewport* pView, A3DGFXVERTEX* pVerts, int nMatrixIndex, DWORD diffuse, DWORD specular)
{
	bool bMatchGround = m_bAlwaysOnGround || (m_bGroundNormal && m_pGfx->IsCloseToGrnd() && m_pGfx->MatchGrnd());
	bool bLocal = !m_bRotFromView && !bMatchGround;

	// diffuse is KeyPoint Diffuse and Gfx Alpha Composed color
	// add self point color by multiply
	int nVertsNum = GetVertRealNumber();		// local vertice number
	float *_localbuf = m_pUVBuf;
	float *_pBufU = _localbuf;
	float *_pBufV = _localbuf + nVertsNum;

	_setUVCoord(_pBufU, _pBufV);

	if (m_bUVInterchange)
	{
		gfx_swap(_pBufU, _pBufV);
	}

	_setColorComposed(diffuse, nVertsNum);
	A3DCOLOR * _pColorBuf = m_pColorBuf;
	
	A3DMATRIX4 matWorld, matScale;

	if (bMatchGround)
	{
		if (m_bAffectedByScale)
			matWorld = GetTranMatrix(kp) * a3d_Scaling( kp.m_fScale, kp.m_fScale, kp.m_fScale ) * GetParentTM();
		else
			matWorld = GetTranMatrix(kp) * GetParentTM();
	}
	else 
	if (m_bRotFromView)
	{
		float parentScale = m_pGfx->GetActualScale();
		matScale = a3d_Scaling(parentScale, parentScale, parentScale);

		m_vPos = GetParentTM() * kp.m_vPos;
		m_vDir = RotateVec(GetParentDir() * kp.m_vDir, _unit_z);
		m_vView = m_vPos - pView->GetCamera()->GetPos();
		m_vUp = CrossProduct(m_vView, m_vDir);
		m_vView = CrossProduct(m_vDir, m_vUp);
		m_vDir.Normalize();
		m_vView.Normalize();
		m_vUp.Normalize();

		if (m_bAffectedByScale)
		{
			matScale *= a3d_Scaling(kp.m_fScale, kp.m_fScale, kp.m_fScale);
		}
	}

	int nCol = m_nWidthVertNum - 1;
	int nRow = m_nHeightVertNum - 1;
	int r1 = 0;
	int r2 = 0;
	int nBaseRow = 0;
	for (int i = 0; i < nRow; i++)
	{
		r2 = r1 + m_nWidthVertNum;

		for (int j = 0; j < nCol; j++)
		{
			int nBaseCol = nBaseRow + j;
			nBaseCol <<= 2;
			
			int iIndex1 = r1 + j; int iIndex2 = r1 + j + 1; int iIndex3 = r2 + j; int iIndex4 = r2 + j + 1;
			
			if (bMatchGround)
			{
				_fillGfxVertexMatchGround(pVerts[nBaseCol    ], m_pVerts[iIndex1].m_vPos, nMatrixIndex, _pColorBuf[iIndex1], specular, _pBufU[iIndex1], _pBufV[iIndex1], matWorld); 
				_fillGfxVertexMatchGround(pVerts[nBaseCol + 1], m_pVerts[iIndex2].m_vPos, nMatrixIndex, _pColorBuf[iIndex2], specular, _pBufU[iIndex2], _pBufV[iIndex2], matWorld);
				_fillGfxVertexMatchGround(pVerts[nBaseCol + 2], m_pVerts[iIndex3].m_vPos, nMatrixIndex, _pColorBuf[iIndex3], specular, _pBufU[iIndex3], _pBufV[iIndex3], matWorld);
				_fillGfxVertexMatchGround(pVerts[nBaseCol + 3], m_pVerts[iIndex4].m_vPos, nMatrixIndex, _pColorBuf[iIndex4], specular, _pBufU[iIndex4], _pBufV[iIndex4], matWorld);		
			}
			else
			if (m_bRotFromView)
			{
				_fillGfxVertex(pVerts[nBaseCol    ], getVertRotFromView(m_pVerts[iIndex1].m_vPos, matScale), nMatrixIndex, _pColorBuf[iIndex1], specular, _pBufU[iIndex1], _pBufV[iIndex1]); 
				_fillGfxVertex(pVerts[nBaseCol + 1], getVertRotFromView(m_pVerts[iIndex2].m_vPos, matScale), nMatrixIndex, _pColorBuf[iIndex2], specular, _pBufU[iIndex2], _pBufV[iIndex2]);
				_fillGfxVertex(pVerts[nBaseCol + 2], getVertRotFromView(m_pVerts[iIndex3].m_vPos, matScale), nMatrixIndex, _pColorBuf[iIndex3], specular, _pBufU[iIndex3], _pBufV[iIndex3]);
				_fillGfxVertex(pVerts[nBaseCol + 3], getVertRotFromView(m_pVerts[iIndex4].m_vPos, matScale), nMatrixIndex, _pColorBuf[iIndex4], specular, _pBufU[iIndex4], _pBufV[iIndex4]);
			}
			else
			{
				_fillGfxVertex(pVerts[nBaseCol    ], m_pVerts[iIndex1].m_vPos, nMatrixIndex, _pColorBuf[iIndex1], specular, _pBufU[iIndex1], _pBufV[iIndex1]); 
				_fillGfxVertex(pVerts[nBaseCol + 1], m_pVerts[iIndex2].m_vPos, nMatrixIndex, _pColorBuf[iIndex2], specular, _pBufU[iIndex2], _pBufV[iIndex2]);
				_fillGfxVertex(pVerts[nBaseCol + 2], m_pVerts[iIndex3].m_vPos, nMatrixIndex, _pColorBuf[iIndex3], specular, _pBufU[iIndex3], _pBufV[iIndex3]);
				_fillGfxVertex(pVerts[nBaseCol + 3], m_pVerts[iIndex4].m_vPos, nMatrixIndex, _pColorBuf[iIndex4], specular, _pBufU[iIndex4], _pBufV[iIndex4]);

			}
		}

		r1 += m_nWidthVertNum;
		nBaseRow += nCol;
	}

	return bLocal;
}

// For Draw To Buffer 3D
inline bool A3DGridDecalEx::Fill_Verts_3D(const KEY_POINT& kp, A3DViewport* pView, A3DWARPVERTEX* pVerts, float fAlpha)
{
	int nVertsNum = GetVertRealNumber();		// local vertice number
	float *_localbuf = m_pUVBuf;
	float *_pBufU = _localbuf;
	float *_pBufV = _localbuf + nVertsNum;

	_setUVCoord(_pBufU, _pBufV);


	if (m_bUVInterchange)
	{
		gfx_swap(_pBufU, _pBufV);
	}
	
	int nCol = m_nWidthVertNum - 1;
	int nRow = m_nHeightVertNum - 1;
	int r1 = 0;
	int r2 = 0;
	int nBaseRow = 0;
	for (int i = 0; i < nRow; i++)
	{
		r2 = r1 + m_nWidthVertNum;

		for (int j = 0; j < nCol; j++)
		{
			int nBaseCol = nBaseRow + j;
			nBaseCol <<= 2;
			
			int iIndex1 = r1 + j; int iIndex2 = r1 + j + 1; int iIndex3 = r2 + j; int iIndex4 = r2 + j + 1;

			float fAlpha1 = A3DCOLOR_GETALPHA(m_pVerts[iIndex1].m_dwColor) / 255.0f * fAlpha;
			float fAlpha2 = A3DCOLOR_GETALPHA(m_pVerts[iIndex2].m_dwColor) / 255.0f * fAlpha;
			float fAlpha3 = A3DCOLOR_GETALPHA(m_pVerts[iIndex3].m_dwColor) / 255.0f * fAlpha;
			float fAlpha4 = A3DCOLOR_GETALPHA(m_pVerts[iIndex4].m_dwColor) / 255.0f * fAlpha;

			pVerts[nBaseCol    ] = A3DWARPVERTEX(m_pVerts[iIndex1].m_vPos, _pBufU[iIndex1], _pBufV[iIndex1], 0, 0, fAlpha1, fAlpha1);
			pVerts[nBaseCol + 1] = A3DWARPVERTEX(m_pVerts[iIndex2].m_vPos, _pBufU[iIndex2], _pBufV[iIndex2], 0, 0, fAlpha2, fAlpha2);
			pVerts[nBaseCol + 2] = A3DWARPVERTEX(m_pVerts[iIndex3].m_vPos, _pBufU[iIndex3], _pBufV[iIndex3], 0, 0, fAlpha3, fAlpha3);
			pVerts[nBaseCol + 3] = A3DWARPVERTEX(m_pVerts[iIndex4].m_vPos, _pBufU[iIndex4], _pBufV[iIndex4], 0, 0, fAlpha4, fAlpha4);
		}

		r1 += m_nWidthVertNum;
		nBaseRow += nCol;
	}

	m_nRectNum = nCol * nRow;
	return true;
}

bool A3DGridDecalEx::Update_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts)
{
	A3DCOLOR diffuse = kp.MultiplyAlpha(m_pGfx->GetActualAlpha());

	if (A3DCOLOR_GETALPHA(diffuse) < 5)
		return false;

	float fHeight, fWidth;
	// Fill 2D will fill the buffer with vertice in the screen space
	if (!Fill_Verts_2D(kp, pView, pVerts, diffuse, A3DCOLORRGBA(0, 0, 0, 255), fWidth, fHeight))
		return false;

	m_nRectNum = _calcCurRectNum();
	int nVertsNum = m_nRectNum * 4;

#ifdef GFX_EDITOR
	m_AABB.Clear();
	{
		for (int i = 0 ; i < nVertsNum ; ++i) {
			A3DVECTOR3 v(pVerts->x, pVerts->y, pVerts->z); 
			pView->InvTransform(v, v); 
			A3DAABB& aabb = m_AABB;

			if (v.x < aabb.Mins.x) aabb.Mins.x = v.x; 
			if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x; 
			if (v.y < aabb.Mins.y) aabb.Mins.y = v.y; 
			if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y; 
			if (v.z < aabb.Mins.z) aabb.Mins.z = v.z; 
			if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z; 

			++pVerts;
		}
	}
#endif

	return true;
}

// 将点在Local空间中的位置，加上Scale以及整体在空间中的偏移，填充到顶点缓冲中去
inline bool A3DGridDecalEx::Fill_Verts_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLVERTEX* pVerts, A3DCOLOR diffuse, A3DCOLOR specular, float& fWidth, float& fHeight)
{
	A3DVECTOR3 vCenterWorld = GetParentTM() * kp.m_vPos;
	A3DMATRIX4 matVPS = pView->GetCamera()->GetVPTM() * pView->GetViewScale();
	A3DVECTOR4 vCenter = TransformToScreen(vCenterWorld, matVPS);

	if (vCenter.z <= 0 || vCenter.z >= 1.0f)
		return false;
	
	//	ZOffset for 距视点偏移
	if (m_fZOffset != 0.0f)
	{
		A3DVECTOR3 vZCenter = vCenterWorld - pView->GetCamera()->GetDir() * (m_fZOffset * m_pGfx->GetActualScale());
		pView->Transform(vZCenter, vZCenter);
		vCenter.z = vZCenter.z;
		a_Clamp(vCenter.z, 0.0f, 1.0f);
	}

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVECTOR3 vExts = vCenterWorld + pCamera->GetRight();
	vExts = matVPS * vExts;
	float fScale = (vExts.x - vCenter.x) * m_pGfx->GetActualScale();

	const int nVertsNum = GetVertRealNumber();		// local vertice number
	float *_localUVbuf = m_pUVBuf;
	float *_pBufU = _localUVbuf;
	float *_pBufV = _localUVbuf + nVertsNum;

	_setUVCoord(_pBufU, _pBufV);

	if (m_bUVInterchange)
	{
		gfx_swap(_pBufU, _pBufV);
	}

	_setColorComposed(diffuse, nVertsNum);
	A3DCOLOR * _pColorBuf = m_pColorBuf;

	float fSin = 0;
	float fCos = 1.0f;

	if (kp.m_fRad2D)
	{
		float fAngle = pView->GetCamera()->IsMirrored() ? -kp.m_fRad2D : kp.m_fRad2D;
		fSin = (float)sin(fAngle);
		fCos = (float)cos(fAngle);
	}

	A3DVECTOR3 *_localVertBuf = m_p2DVertBuf;
	for (int _i = 0 ; _i < nVertsNum ; _i++) {
		_localVertBuf[_i].x = (m_pVerts[_i].m_vPos.x * fCos + m_pVerts[_i].m_vPos.y * fSin) * fScale + vCenter.x;
		_localVertBuf[_i].y = (-m_pVerts[_i].m_vPos.y * fCos + m_pVerts[_i].m_vPos.x * fSin) * fScale + vCenter.y;
		_localVertBuf[_i].z = vCenter.z;
	}

	fWidth = (m_nWidthVertNum - 1) * m_fGridSize * fScale;
	fHeight = (m_nHeightVertNum - 1) * m_fGridSize * fScale;
	
	int nCol = m_nWidthVertNum - 1;
	int nRow = m_nHeightVertNum - 1;
	int r1 = 0;
	int r2 = 0;
	int nBaseRow = 0;
	for (int i = 0 ; i < nRow ; ++i) {
		r2 = r1 + m_nWidthVertNum;

		for (int j = 0 ; j < nCol ; ++j) {
			int nBaseCol = nBaseRow + j;
			nBaseCol <<= 2;
			
			int iIndex1 = r1 + j; int iIndex2 = r1 + j + 1; int iIndex3 = r2 + j; int iIndex4 = r2 + j + 1;
			A3DTLVERTEX& v0 = pVerts[nBaseCol    ];
			A3DTLVERTEX& v1 = pVerts[nBaseCol + 1];
			A3DTLVERTEX& v2 = pVerts[nBaseCol + 2];
			A3DTLVERTEX& v3 = pVerts[nBaseCol + 3];

			A3DVECTOR3& pos0 = _localVertBuf[iIndex1];
			A3DVECTOR3& pos1 = _localVertBuf[iIndex2];
			A3DVECTOR3& pos2 = _localVertBuf[iIndex3];
			A3DVECTOR3& pos3 = _localVertBuf[iIndex4];

			v0.x = pos0.x;
			v0.y = pos0.y;
			v0.z = pos0.z;
			v0.rhw = vCenter.w;
			v0.diffuse = _pColorBuf[iIndex1];
			v0.specular = specular;
			v0.tu = _pBufU[iIndex1];
			v0.tv = _pBufV[iIndex1];

			v1.x = pos1.x;
			v1.y = pos1.y;
			v1.z = pos1.z;
			v1.rhw = vCenter.w;
			v1.diffuse = _pColorBuf[iIndex2];
			v1.specular = specular;
			v1.tu = _pBufU[iIndex2];
			v1.tv = _pBufV[iIndex2];

			v2.x = pos2.x;
			v2.y = pos2.y;
			v2.z = pos2.z;
			v2.rhw = vCenter.w;
			v2.diffuse = _pColorBuf[iIndex3];
			v2.specular = specular;
			v2.tu = _pBufU[iIndex3];
			v2.tv = _pBufV[iIndex3];

			v3.x = pos3.x;
			v3.y = pos3.y;
			v3.z = pos3.z;
			v3.rhw = vCenter.w;
			v3.diffuse = _pColorBuf[iIndex4];
			v3.specular = specular;
			v3.tu = _pBufU[iIndex4];
			v3.tv = _pBufV[iIndex4];
		}

		r1 += m_nWidthVertNum;
		nBaseRow += nCol;
	}

	return true;
}

// Fill 2D Verts Buffer for Warp
inline bool A3DGridDecalEx::Fill_Verts_2D(const KEY_POINT& kp, A3DViewport* pView, A3DTLWARPVERTEX* pVerts, float& fWidth, float& fHeight)
{
	A3DVECTOR3 vCenterWorld = GetParentTM() * kp.m_vPos;
	A3DMATRIX4 matVPS = pView->GetCamera()->GetVPTM() * pView->GetViewScale();
	A3DVECTOR4 vCenter = TransformToScreen(vCenterWorld, matVPS);

	if (vCenter.z <= 0 || vCenter.z >= 1.0f)
		return false;

	if (m_fZOffset != 0.0f)
	{
		A3DVECTOR3 vZCenter = vCenterWorld - pView->GetCamera()->GetDir() * (m_fZOffset * m_pGfx->GetActualScale());
		pView->Transform(vZCenter, vZCenter);
		vCenter.z = vZCenter.z;
		a_Clamp(vCenter.z, 0.0f, 1.0f);
	}

	A3DCameraBase* pCamera = pView->GetCamera();
	A3DVECTOR3 vExts = vCenterWorld + pCamera->GetRight();
	vExts = matVPS * vExts;
	float fScale = (vExts.x - vCenter.x) * m_pGfx->GetActualScale();

	const int nVertsNum = GetVertRealNumber();		// local vertice number
	float *_localUVbuf = m_pUVBuf;
	float *_pBufU = _localUVbuf;
	float *_pBufV = _localUVbuf + nVertsNum;

	_setUVCoord(_pBufU, _pBufV);

	if (m_bUVInterchange)
	{
		gfx_swap(_pBufU, _pBufV);
	}

	float fSin = 0;
	float fCos = 1.0f;

	if (kp.m_fRad2D)
	{
		float fAngle = pView->GetCamera()->IsMirrored() ? -kp.m_fRad2D : kp.m_fRad2D;
		fSin = (float)sin(fAngle);
		fCos = (float)cos(fAngle);
	}

	A3DVECTOR3 *_localVertBuf = m_p2DVertBuf;
	for (int _i = 0 ; _i < nVertsNum ; _i++) {
		_localVertBuf[_i].x = (m_pVerts[_i].m_vPos.x * fCos + m_pVerts[_i].m_vPos.y * fSin) * fScale + vCenter.x;
		_localVertBuf[_i].y = (-m_pVerts[_i].m_vPos.y * fCos + m_pVerts[_i].m_vPos.x * fSin) * fScale + vCenter.y;
		_localVertBuf[_i].z = vCenter.z;
	}

	fWidth = (m_nWidthVertNum - 1) * m_fGridSize * fScale;
	fHeight = (m_nHeightVertNum - 1) * m_fGridSize * fScale;
	
	int nCol = m_nWidthVertNum - 1;
	int nRow = m_nHeightVertNum - 1;
	int r1 = 0;
	int r2 = 0;
	int nBaseRow = 0;
	for (int i = 0 ; i < nRow ; ++i) {
		r2 = r1 + m_nWidthVertNum;

		for (int j = 0 ; j < nCol ; ++j) {
			int nBaseCol = nBaseRow + j;
			nBaseCol <<= 2;
			
			int iIndex1 = r1 + j; int iIndex2 = r1 + j + 1; int iIndex3 = r2 + j; int iIndex4 = r2 + j + 1;
			A3DTLWARPVERTEX& v0 = pVerts[nBaseCol    ];
			A3DTLWARPVERTEX& v1 = pVerts[nBaseCol + 1];
			A3DTLWARPVERTEX& v2 = pVerts[nBaseCol + 2];
			A3DTLWARPVERTEX& v3 = pVerts[nBaseCol + 3];

			A3DVECTOR3& pos0 = _localVertBuf[iIndex1];
			A3DVECTOR3& pos1 = _localVertBuf[iIndex2];
			A3DVECTOR3& pos2 = _localVertBuf[iIndex3];
			A3DVECTOR3& pos3 = _localVertBuf[iIndex4];

			float fAlpha1 = A3DCOLOR_GETALPHA(m_pVerts[iIndex1].m_dwColor) / 255.5f;
			float fAlpha2 = A3DCOLOR_GETALPHA(m_pVerts[iIndex2].m_dwColor) / 255.5f;
			float fAlpha3 = A3DCOLOR_GETALPHA(m_pVerts[iIndex3].m_dwColor) / 255.5f;
			float fAlpha4 = A3DCOLOR_GETALPHA(m_pVerts[iIndex4].m_dwColor) / 255.5f;

			v0.pos.Set(pos0.x, pos0.y, pos0.z, vCenter.w);
			v0.u1 = _pBufU[iIndex1];
			v0.v1 = _pBufV[iIndex1];
			v0.u3 = fAlpha1;
			v0.v3 = fAlpha1;

			v1.pos.Set(pos1.x, pos1.y, pos1.z, vCenter.w);
			v1.u1 = _pBufU[iIndex2];
			v1.v1 = _pBufV[iIndex2];
			v1.u3 = fAlpha2;
			v1.v3 = fAlpha2;

			v2.pos.Set(pos2.x, pos2.y, pos2.z, vCenter.w);
			v2.u1 = _pBufU[iIndex3];
			v2.v1 = _pBufV[iIndex3];
			v2.u3 = fAlpha3;
			v2.v3 = fAlpha3;
			
			v3.pos.Set(pos3.x, pos3.y, pos3.z, vCenter.w);
			v3.u1 = _pBufU[iIndex4];
			v3.v1 = _pBufV[iIndex4];
			v3.u3 = fAlpha4;
			v3.v3 = fAlpha4;
		}
		r1 += m_nWidthVertNum;
		nBaseRow += nCol;
	}

	return true;
}

int A3DGridDecalEx::DrawToBuffer(A3DViewport* pView, A3DWARPVERTEX* pVerts, int nMaxVerts)
{
	if (GetVertsCount() > nMaxVerts)
		return 0;

	KEY_POINT kp;
	if (!GetCurKeyPoint(&kp)) return 0;

	float fAlpha = A3DCOLOR_GETALPHA(kp.m_color) * m_pGfx->GetActualAlpha() / 255.f * .5f;

	bool bLocal = Fill_Verts_3D(kp, pView, pVerts, fAlpha);

	int nVertCount = 4 * m_nRectNum;

	if (bLocal)
	{
		A3DMATRIX4 matWorld;
		if (m_bAffectedByScale)
			matWorld = GetTranMatrix(kp) * a3d_Scaling( kp.m_fScale, kp.m_fScale, kp.m_fScale ) * GetParentTM();
		else
			matWorld = GetTranMatrix(kp) * GetParentTM();

		for (int i = 0; i < nVertCount; i++)
		{
			A3DWARPVERTEX* pVert = &pVerts[i];
			pVert->pos	= matWorld * pVert->pos;
		}
	}

#ifdef GFX_EDITOR
	{
		m_AABB.Clear();
		for (int i = 0 ; i < nVertCount ; ++i) {
			A3DVECTOR3 v(pVerts->pos); 

			A3DAABB& aabb = m_AABB;
			if (v.x < aabb.Mins.x) aabb.Mins.x = v.x; 
			if (v.x > aabb.Maxs.x) aabb.Maxs.x = v.x; 
			if (v.y < aabb.Mins.y) aabb.Mins.y = v.y; 
			if (v.y > aabb.Maxs.y) aabb.Maxs.y = v.y; 
			if (v.z < aabb.Mins.z) aabb.Mins.z = v.z; 
			if (v.z > aabb.Maxs.z) aabb.Maxs.z = v.z; 
			
			++pVerts;
		}
	}
#endif

	return nVertCount;
}

int A3DGridDecalEx::DrawToBuffer(A3DViewport* pView, A3DTLWARPVERTEX* pVerts, int nMaxVerts, float rw, float rh)
{
	if (GetVertsCount() > nMaxVerts)
		return 0;

	KEY_POINT kp;
	if (!GetCurKeyPoint(&kp)) return 0;

	float fAlpha = A3DCOLOR_GETALPHA(kp.m_color) * m_pGfx->GetActualAlpha() / 255.f;

	float fWidth, fHeight;

	if (!Fill_Verts_2D(kp, pView, pVerts, fWidth, fHeight))
		return 0;

	int nVertCount = m_nRectNum * 4;

	float fInvAlpha = min(1.0f - fWidth * fWidth * rw * rw * 4.0f, 1.0f - fHeight * fHeight * rh * rh * 4.0f);
	a_Clamp(fInvAlpha, 0.0f, 1.0f);

	if (fInvAlpha < 0.1f)
		return 0;

	float fU3 = fWidth * rw * fAlpha * fInvAlpha;
	float fV3 = fHeight * rh * fAlpha * fInvAlpha;
	a_Clamp(fU3, 0.0f, 0.5f);
	a_Clamp(fV3, 0.0f, 0.5f);

	for (int i = 0 ; i < nVertCount ; ++i) {
		A3DTLWARPVERTEX * _pVert = &pVerts[i];
		_pVert->u2	= _pVert->pos.x * rw;
		_pVert->v2	= _pVert->pos.y * rh;
		_pVert->u3	*= fU3;
		_pVert->v3	*= fV3;
	}
	
#ifdef GFX_EDITOR
	{
		m_AABB.Clear();
		for (int i = 0 ; i < nVertCount ; ++i) {
			A3DVECTOR3 v(pVerts[i].pos.x, pVerts[i].pos.y, pVerts[i].pos.z); 
			pView->InvTransform(v, v); 

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

	return nVertCount;
}


bool A3DGridDecalEx::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	
	switch(nOp) {
	case ID_GFXOP_GRIDDECAL_W_VNUM:
		_setSize((int)prop, m_nHeightVertNum);
		break;
	case ID_GFXOP_GRIDDECAL_H_VNUM:
		_setSize(m_nWidthVertNum, (int)prop);
		break;
	case ID_GFXOP_GRIDDECAL_GRIDSIZE:
		_scaleVertPos((float)prop / m_fGridSize);
		m_fGridSize = (float)prop;
		break;
	case ID_GFXOP_GRIDDECAL_Z_OFFSET:
		m_fZOffset = (float)prop;
		break;
	case ID_GFXOP_GRIDDECAL_AFFBYSCALE:
		m_bAffectedByScale = (bool)prop;
		break;
	case ID_GFXOP_GRIDDECAL_ROTFROMVIEW:
		m_bRotFromView = (bool)prop;
		break;
	case ID_GFXOP_GRIDDECAL_OFFSET_HEIGHT:
		m_fOffsetHeight = (float)prop;
		break;
	case ID_GFXOP_GRIDDECAL_ALWAYS_ONGROUND:
		m_bAlwaysOnGround = (bool)prop;
		break;
	default:
		return A3DGFXElement::SetProperty(nOp, prop);
	}
	return true;
}

GFX_PROPERTY A3DGridDecalEx::GetProperty(int nOp) const
{
	switch(nOp) {
	case ID_GFXOP_GRIDDECAL_W_VNUM:
		return GFX_PROPERTY(m_nWidthVertNum);
	case ID_GFXOP_GRIDDECAL_H_VNUM:
		return GFX_PROPERTY(m_nHeightVertNum);
	case ID_GFXOP_GRIDDECAL_GRIDSIZE:
		return GFX_PROPERTY(m_fGridSize);
	case ID_GFXOP_GRIDDECAL_Z_OFFSET:
		return GFX_PROPERTY(m_fZOffset);
	case ID_GFXOP_GRIDDECAL_AFFBYSCALE:
		return GFX_PROPERTY(m_bAffectedByScale);
	case ID_GFXOP_GRIDDECAL_ROTFROMVIEW:
		return GFX_PROPERTY(m_bRotFromView);
	case ID_GFXOP_GRIDDECAL_OFFSET_HEIGHT:
		return GFX_PROPERTY(m_fOffsetHeight);
	case ID_GFXOP_GRIDDECAL_ALWAYS_ONGROUND:
		return GFX_PROPERTY(m_bAlwaysOnGround);
	}
	return A3DGFXElement::GetProperty(nOp);
}


#ifdef GFX_EDITOR

void _GetNewAddCoordRow(int iRow, int iHeightVertNum, bool bAbove, int& iRow1, int & iRow2, bool& bBetween)
{
	if (bAbove)
	{
		if (iRow == 0)
		{
			iRow1 = iRow + 1;
			iRow2 = iRow;
			bBetween = false;
		}
		else
		{
			iRow1 = iRow;
			iRow2 = iRow - 1;
			bBetween = true;
		}
	}
	else
	{
		if (iRow == iHeightVertNum - 1)
		{
			iRow1 = iRow - 1;
			iRow2 = iRow;
			bBetween = false;
		}
		else
		{
			iRow1 = iRow;
			iRow2 = iRow + 1;
			bBetween = true;
		}
	}
}

void _GetNewAddCoordCol(int iCol, int iWidthVertNum, bool bLeft, int& iCol1, int& iCol2, bool& bBetween)
{
	_GetNewAddCoordRow(iCol, iWidthVertNum, bLeft, iCol1, iCol2, bBetween);
}

inline void _CopyGridData(A3DGridDecalEx::GridData* pDestBuffer, int iDestBufferOffset, const A3DGridDecalEx::GridData* pSrcData, int iWidth)
{
	A3DGridDecalEx::GridData* pDestData = pDestBuffer + iDestBufferOffset;
	memcpy(pDestData, pSrcData, iWidth * sizeof(A3DGridDecalEx::GridData));
}

inline void _InterpGridData(A3DGridDecalEx::GridData* pDestData, const A3DGridDecalEx::GridData& data1, const A3DGridDecalEx::GridData& data2, bool bBetween)
{
	if (bBetween)
	{
		pDestData->m_vPos = (data1.m_vPos + data2.m_vPos) / 2;
		A3DCOLOR rNew = (A3DCOLOR_GETRED(data1.m_dwColor) + A3DCOLOR_GETRED(data2.m_dwColor)) >> 1;
		A3DCOLOR gNew = (A3DCOLOR_GETGREEN(data1.m_dwColor) + A3DCOLOR_GETGREEN(data2.m_dwColor)) >> 1;
		A3DCOLOR bNew = (A3DCOLOR_GETBLUE(data1.m_dwColor) + A3DCOLOR_GETBLUE(data2.m_dwColor)) >> 1;
		A3DCOLOR aNew = (A3DCOLOR_GETALPHA(data1.m_dwColor) + A3DCOLOR_GETALPHA(data2.m_dwColor)) >> 1;
		pDestData->m_dwColor = A3DCOLORRGBA(rNew, gNew, bNew, aNew);
	}
	else
	{
		pDestData->m_vPos = 2 * data2.m_vPos - data1.m_vPos;
		A3DCOLOR rNew = ((A3DCOLOR_GETRED(data2.m_dwColor) << 1) - A3DCOLOR_GETRED(data1.m_dwColor)) & 0xFF;
		A3DCOLOR gNew = ((A3DCOLOR_GETGREEN(data2.m_dwColor) << 1) - A3DCOLOR_GETGREEN(data1.m_dwColor)) & 0xFF;
		A3DCOLOR bNew = ((A3DCOLOR_GETBLUE(data2.m_dwColor) << 1) - A3DCOLOR_GETBLUE(data1.m_dwColor)) & 0xFF;
		A3DCOLOR aNew = ((A3DCOLOR_GETALPHA(data2.m_dwColor) << 1) - A3DCOLOR_GETALPHA(data1.m_dwColor)) & 0xFF;
		pDestData->m_dwColor = A3DCOLORRGBA(rNew, gNew, bNew, aNew);
	}
}

bool A3DGridDecalEx::AddRowAt(int iRow, bool bAbove)
{
	if (iRow < 0 || iRow >= m_nHeightVertNum)
		return false;

	int iRow1, iRow2;
	bool bBetween;
	_GetNewAddCoordRow(iRow, m_nHeightVertNum, bAbove, iRow1, iRow2, bBetween);

	GridData* pNewBuffer = new GridData[(m_nHeightVertNum + 1) * m_nWidthVertNum];
	GridData* pNewTmpBuffer = new GridData[m_nWidthVertNum];
	
	GridData* pRow1Data = GetRow(iRow1);
	GridData* pRow2Data = GetRow(iRow2);
	for (int iNewIdx = 0; iNewIdx < m_nWidthVertNum; ++iNewIdx)
	{
		GridData& data1 = pRow1Data[iNewIdx];
		GridData& data2 = pRow2Data[iNewIdx];
		_InterpGridData(&pNewTmpBuffer[iNewIdx], data1, data2, bBetween);
	}

	bool bCopySeq = (iRow1 - iRow2 < 0);
	int iDestRowIdx = 0;

	for (int iRowIdx = 0; iRowIdx < m_nHeightVertNum; ++iRowIdx, ++iDestRowIdx)
	{
		if (iRowIdx == iRow2 && ((bBetween && bCopySeq) || (!bBetween && !bCopySeq)))
		{
			_CopyGridData(pNewBuffer, iDestRowIdx * m_nWidthVertNum, pNewTmpBuffer, m_nWidthVertNum);
			++iDestRowIdx;
		}
		
		GridData* pSrcData = GetRow(iRowIdx);
		_CopyGridData(pNewBuffer, iDestRowIdx * m_nWidthVertNum, pSrcData, m_nWidthVertNum);
		
		if (iRowIdx == iRow2 && ((!bBetween && bCopySeq) || (bBetween && !bCopySeq)))
		{
			++iDestRowIdx;
			_CopyGridData(pNewBuffer, iDestRowIdx * m_nWidthVertNum, pNewTmpBuffer, m_nWidthVertNum);
		}
	}

	m_nHeightVertNum++;

	m_nRectNum = _calcCurRectNum();
	int nVertNum = m_nHeightVertNum * m_nWidthVertNum;

	delete [] pNewTmpBuffer;
	delete [] m_pVerts;
	m_pVerts = pNewBuffer;

	// 每一次顶点数目变动，也同样需要变动 UV缓存和2D顶点缓存 (这两个缓存仅在局部使用，每次使用时自行赋值)
	_resetUVBuffer(nVertNum);
	_reset2DVertBuffer(nVertNum);
	_resetColorBuffer(nVertNum);
	return true;
}

bool A3DGridDecalEx::AddColAt(int iCol, bool bLeft)
{
	if (iCol < 0 || iCol >= m_nWidthVertNum)
		return false;

	int iCol1, iCol2;
	bool bBetween;
	_GetNewAddCoordCol(iCol, m_nWidthVertNum, bLeft, iCol1, iCol2, bBetween);

	GridData* pNewBuffer = new GridData[m_nHeightVertNum * (m_nWidthVertNum + 1)];
	GridData* pNewTmpBuffer = new GridData[m_nHeightVertNum];
	
	for (int iNewIdx = 0; iNewIdx < m_nHeightVertNum; ++iNewIdx)
	{
		GridData* pCurRow = GetRow(iNewIdx);
		GridData& data1 = pCurRow[iCol1];
		GridData& data2 = pCurRow[iCol2];

		_InterpGridData(&pNewTmpBuffer[iNewIdx], data1, data2, bBetween);
	}

	bool bCopySeq = (iCol1 - iCol2 < 0);
	int iDestRowIdx = 0;

	for (int iRowIdx = 0; iRowIdx < m_nHeightVertNum; ++iRowIdx)
	{
		GridData* pCurRow = GetRow(iRowIdx);
		int iDestColIdx = 0;
		int iBaseOffset = iRowIdx * (m_nWidthVertNum + 1);
		for (int iColIdx = 0; iColIdx < m_nWidthVertNum; ++iColIdx, ++iDestColIdx)
		{
			if (iColIdx == iCol2 && ((bBetween && bCopySeq) || (!bBetween && !bCopySeq)))
			{
				pNewBuffer[iBaseOffset + iDestColIdx] = pNewTmpBuffer[iRowIdx];
				iDestColIdx++;
			}

			pNewBuffer[iBaseOffset + iDestColIdx] = pCurRow[iColIdx];

			if (iColIdx == iCol2 && ((!bBetween && bCopySeq) || (bBetween && !bCopySeq)))
			{
				iDestColIdx++;
				pNewBuffer[iBaseOffset + iDestColIdx] = pNewTmpBuffer[iRowIdx];
			}
		}
	}

	m_nWidthVertNum++;
	m_nRectNum = _calcCurRectNum();
	int nVertNum = m_nHeightVertNum * m_nWidthVertNum;

	delete [] pNewTmpBuffer;
	delete [] m_pVerts;
	m_pVerts = pNewBuffer;

	// 每一次顶点数目变动，也同样需要变动 UV缓存和2D顶点缓存 (这两个缓存仅在局部使用，每次使用时自行赋值)
	_resetUVBuffer(nVertNum);
	_reset2DVertBuffer(nVertNum);
	_resetColorBuffer(nVertNum);
	return true;
}

bool A3DGridDecalEx::RemoveRowAt(int iRow)
{
	if (iRow >= m_nHeightVertNum || iRow < 0)
		return false;

	GridData* pNewBuffer = new GridData[(m_nHeightVertNum - 1) * m_nWidthVertNum];
	GridData* pDestBuffer = pNewBuffer;

	for (int iRowIdx = 0; iRowIdx < m_nHeightVertNum; ++iRowIdx)
	{
		if (iRowIdx == iRow)
		{
			continue;
		}

		GridData* pSrcData = GetRow(iRowIdx);
		_CopyGridData(pDestBuffer, 0, pSrcData, m_nWidthVertNum);
		pDestBuffer += m_nWidthVertNum;
	}


	m_nHeightVertNum--;

	m_nRectNum = _calcCurRectNum();
	int nVertNum = m_nHeightVertNum * m_nWidthVertNum;

	delete [] m_pVerts;
	m_pVerts = pNewBuffer;
	// 每一次顶点数目变动，也同样需要变动 UV缓存和2D顶点缓存 (这两个缓存仅在局部使用，每次使用时自行赋值)
	_resetUVBuffer(nVertNum);
	_reset2DVertBuffer(nVertNum);
	_resetColorBuffer(nVertNum);

	return true;
}

bool A3DGridDecalEx::RemoveColAt(int iCol)
{
	if (iCol >= m_nWidthVertNum || iCol < 0)
		return false;

	GridData* pNewBuffer = new GridData[m_nHeightVertNum * (m_nWidthVertNum - 1)];
	GridData* pDestBuffer = pNewBuffer;

	for (int iRowIdx = 0; iRowIdx < m_nHeightVertNum; ++iRowIdx)
	{
		GridData* pCurRow = GetRow(iRowIdx);
		int iDestIdx = 0;
		for (int iColIdx = 0; iColIdx < m_nWidthVertNum; ++iColIdx)
		{
			if (iColIdx == iCol)
			{
				continue;
			}

			pDestBuffer[iDestIdx] =  pCurRow[iColIdx];
			iDestIdx++;			
		}

		pDestBuffer += m_nWidthVertNum - 1;
	}

	m_nWidthVertNum--;

	delete [] m_pVerts;
	m_pVerts = pNewBuffer;

	m_nRectNum = _calcCurRectNum();
	int nVertNum = m_nHeightVertNum * m_nWidthVertNum;
	_resetUVBuffer(nVertNum);
	_reset2DVertBuffer(nVertNum);
	_resetColorBuffer(nVertNum);

	return true;
}


#endif