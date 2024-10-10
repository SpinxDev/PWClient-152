#include "StdAfx.h"
#include "A3DTrailEx.h"
#include "A3DConfig.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DGFXMan.h"
#include "A3DCDS.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DGFXExMan.h"
#include "TrailList.h"

static const char _format_shrink[]	= "Shrink: %d";

//////////////////////////////////////////////////////////////////////////
//
//	Implement A3DTrailEx
//
//////////////////////////////////////////////////////////////////////////

A3DTrailEx::A3DTrailEx(A3DGFXEx* pGfx) : A3DTrail(pGfx)
{
    m_nEleType			= ID_ELE_TYPE_TRAIL_EX;
    m_iSplineMode		= SPLINE_MODE;
    m_iSampleFreq		= 3;
    m_nShrinkTime       = -1;

}

A3DTrailEx::~A3DTrailEx()
{

}

A3DGFXElement* A3DTrailEx::Clone(A3DGFXEx* pGfx) const
{
    A3DTrailEx* p = new A3DTrailEx(pGfx);
    return &(*p = *this);
}

A3DTrailEx& A3DTrailEx::operator=(const A3DTrailEx& src)
{
    A3DTrail::operator =(src);
    this->m_nShrinkTime = src.m_nShrinkTime;
    return *this;
}

float A3DTrailEx::GetPortionFactor(int iRenderIdx)
{
    assert(m_pTrailImpl);
    int nMaxRenderCount = m_pTrailImpl->GetRenderCount();

    if (m_dwLife < 0xFFFFFFFF && m_nShrinkTime > 0)
    {
        float fAlpha = 1.0f;
        float fMaxLife = (float)(m_nShrinkTime);
        float fLife = (float)(m_dwLife);
        fAlpha = 1.0f - (fLife - 25.0f) / fMaxLife;
        a_Clamp(fAlpha, 0.0f, 1.0f);
 
        int nReducedCount = (int)(fAlpha * nMaxRenderCount);
        iRenderIdx = a_Max(0, iRenderIdx - nReducedCount);
        nMaxRenderCount -= nReducedCount;
        if (nMaxRenderCount == 0)
            return 0;
    }
    float fPortion = iRenderIdx / (float)nMaxRenderCount;
    return fPortion;
}

float A3DTrailEx::GetPortionAlpha(float fPortion)
{
    //if (m_dwLife == 0xFFFFFFFF)
        return 1.0f;

    //float fMaxLife = (float)(0.5f * m_nSegLife);
    //float fLife = (float)(m_dwLife);
    //float fAlpha = (fLife - 0.025f) / fMaxLife;
    //a_Clamp(fAlpha, 0.0f, 1.0f);
    //return fAlpha;
}

bool A3DTrailEx::Init(A3DDevice* pDevice)
{
    return A3DTrail::Init(pDevice);
}

bool A3DTrailEx::Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion)
{
    if (!A3DTrail::Load(pDevice, pFileToLoad, dwVersion))
        return false;

    char	szLine[AFILE_LINEMAXLEN];
    DWORD	dwReadLen;

    if (pFileToLoad->IsBinary())
    {
    }
    else
    {
        pFileToLoad->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
        sscanf(szLine, _format_shrink, &m_nShrinkTime);
    }
    return true;
}

bool A3DTrailEx::Save(AFile* pFile)
{
    if (!A3DTrail::Save(pFile))
        return false;

    if (pFile->IsBinary())
    {
    }
    else
    {
        char	szLine[AFILE_LINEMAXLEN];
        sprintf(szLine, _format_shrink, m_nShrinkTime);
        pFile->WriteLine(szLine);
    }
    return true;
}

bool A3DTrailEx::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
    switch(nOp)
    {
    case ID_GFXOP_TRAIL_SHRINK:
        m_nShrinkTime = prop;
        break;
    default:
        return A3DTrail::SetProperty(nOp, prop);
    }
    return true;
}

GFX_PROPERTY A3DTrailEx::GetProperty(int nOp) const
{
    switch(nOp)
    {
    case ID_GFXOP_TRAIL_SHRINK:
        return GFX_PROPERTY(m_nShrinkTime);
        break;
   }
    return A3DTrail::GetProperty(nOp);
}

A3DTrail::TRAILMODE A3DTrailEx::GetDefaultMode() const
{
    return SPLINE_MODE;
}