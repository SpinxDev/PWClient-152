// CloudSprite.cpp: implementation of the CCloudSprite class.
//
//////////////////////////////////////////////////////////////////////

#include "EL_CloudSprite.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
A3DCOLORVALUE CELCloudSprite::m_clrSun = A3DCOLORVALUE(1.0f,1.0f,1.0f,1.0f);
A3DCOLORVALUE CELCloudSprite::m_clrAmb = A3DCOLORVALUE(1.0f,1.0f,1.0f,1.0f);
A3DTLVERTEX* CELCloudSprite::m_pVertexBuffer = NULL;

CELCloudSprite::CELCloudSprite()
{
	m_vGroupToSun.Set(0.0f,0.0f,0.0f);
	m_fAlpha = 1.0f;

	m_pA3DDevice = NULL;

	//for sprite motion
	m_fMoveDist = 0.0f;
	m_bFarAway = (rand() % 2) > 0 ? true:false;
	m_dwLastTime = 0;
	m_dwStepTime = 40 + rand() % 50; 
}

CELCloudSprite::~CELCloudSprite()
{
}

// idxTexure: from 1 to 16
void CELCloudSprite::SetTexture(int idxTexture)
{
	//calculate texture coords, base on 1024*1024 texture
	int x = (idxTexture-1) % 4;
	int y = (idxTexture-1) / 4;

	m_fTextureU[0] = x*0.25f;
	m_fTextureV[0] = y*0.25f;
	m_fTextureU[1] = (x+1)*0.25f;
	m_fTextureV[1] = m_fTextureV[0];
	m_fTextureU[2] = m_fTextureU[0];
	m_fTextureV[2] = (y+1)*0.25f;
	m_fTextureU[3] = m_fTextureU[1];
	m_fTextureV[3] = m_fTextureV[2];
}

A3DVECTOR3 CELCloudSprite::GetCenterPos()
{
	return m_vMovedPos;
}


void CELCloudSprite::SetGroup(UINT nGroup)
{
	m_nGroup = nGroup;
}

UINT CELCloudSprite::GetGroup()
{
	return m_nGroup;
}

void CELCloudSprite::Init(A3DDevice* pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;
}

void CELCloudSprite::SetGroupToSunVector(const A3DVECTOR3 &vGroupToSun)
{
	m_vGroupToSun = vGroupToSun;
}

void CELCloudSprite::ReShading(A3DViewport *pViewport,float fSpriteSize)
{
	A3DVECTOR3 vDirUp = pViewport->GetCamera()->GetUp();
	A3DVECTOR3 vDirRight = pViewport->GetCamera()->GetRight();
	A3DVECTOR3 v[4];
	float fHalfSize = fSpriteSize * 0.5f;
	v[0] = m_vCenterPos + ( vDirUp - vDirRight ) * fHalfSize;
	v[1] = m_vCenterPos + ( vDirUp + vDirRight ) * fHalfSize;
	v[2] = m_vCenterPos + (-vDirUp - vDirRight ) * fHalfSize;
	v[3] = m_vCenterPos + (-vDirUp + vDirRight ) * fHalfSize;

	//sunlight
	float f=1;
	A3DCOLORVALUE clrSun,clrAmb;

	A3DVECTOR3 vVertexToGroup;
	for(int i=0;i<4;i++)
	{
		//sun . vertex
		vVertexToGroup = v[i] - m_vGroupCenterPos;
		vVertexToGroup.Normalize();
		f = DotProduct(m_vGroupToSun,vVertexToGroup);
		// map f from [-1,1] to [0,1]
		clrSun = 0.5f * (f+1) * m_clrSun;
		clrAmb = m_vAmbColor[i];
		clrAmb *= m_clrAmb;
			
		m_vResultColor[i] = clrAmb + clrSun;
		
		float max = max(max(m_vResultColor[i].r,m_vResultColor[i].g),m_vResultColor[i].b);
		if( max > 1.0 )
		{
			max = 1/max;
			m_vResultColor[i] *= max;
		}		
	}
}

void CELCloudSprite::SetGroupCenterPos(const A3DVECTOR3 &vGroupCenter)
{
	m_vGroupCenterPos = vGroupCenter;
}

void CELCloudSprite::SetAlphaFromGroup(float fAlpha)
{
	m_fAlpha = fAlpha;
}

void CELCloudSprite::SetSpriteInfo(const A3DVECTOR3 &vCenterPos, int idxTexture, int nLevel,DWORD dwColor[])
{
	m_vCenterPos = vCenterPos;
	m_vMovedPos = vCenterPos;
	m_nCloudLevel = nLevel;
	SetTexture(idxTexture);
	for(int i=0;i<4;i++)
		m_vAmbColor[i] = dwColor[i];
}

int CELCloudSprite::GetCloudLevel()
{
	return m_nCloudLevel;
}

void CELCloudSprite::ReFillStream(A3DViewport* pViewport,int idxStreamPos,float fSpriteSize)
{
	float fAlphaFactor = m_fAlpha;
	
	A3DVECTOR3 cameraPos = pViewport->GetCamera()->GetPos();
	A3DVECTOR3 cameraDir = pViewport->GetCamera()->GetDir();
	A3DVECTOR3 dir = m_vMovedPos - cameraPos;
	dir.Normalize();	
	
	//face to camera?
	float fTheta = DotProduct(dir,cameraDir);
	if( fTheta < 0 )
	{
		FillStream(pViewport, idxStreamPos,0.0f,0.0f,0.0f);
		return;
	}

	//be in lock distance?
	float fDist = (m_vMovedPos.x -cameraPos.x) / dir.x;		
	if( fDist < fSpriteSize )
	{
		float fValve = fSpriteSize*0.23f;
		if( fDist < fValve)
		{
			FillStream(pViewport, idxStreamPos,0.0f,0.0f,0.0f);
			return;
		}
		
		//fTheta = 1 - acos(fTheta)*2/3.1415926;
		float fAlpha = (fDist - fValve) / (fSpriteSize - fValve);
		fAlphaFactor *= fTheta*fAlpha;

		if( fTheta < 0 || fAlpha < 0 )
			int x = 0;
	}
	 
	//find v0 pos after projection
	A3DVECTOR3 vDirUp = pViewport->GetCamera()->GetUp();
	A3DVECTOR3 vDirRight = pViewport->GetCamera()->GetRight();

	float fHalfSize = fSpriteSize * 0.5f;
	A3DVECTOR3 v0 = m_vMovedPos + ( vDirUp - vDirRight ) * fHalfSize;
	A3DVECTOR3 v3 = m_vMovedPos - ( vDirUp - vDirRight ) * fHalfSize;
	pViewport->Transform(v0,v0);
	pViewport->Transform(v3,v3);
	
	A3DVIEWPORTPARAM* pParam = pViewport->GetParam();
	if( (v0.x < 0 && v3.x < 0) || ( v0.x >= pParam->Width && v3.x >= pParam->Width )
		|| (v0.y < 0 && v3.y < 0) || ( v0.y >= pParam->Height && v3.y >= pParam->Height )
		|| v0.z < 0 || v0.z > 1.0f )
	{
		FillStream(pViewport, idxStreamPos,0.0f,0.0f,0.0f);
		return;
	}

	FillStream(pViewport, idxStreamPos,fAlphaFactor,v0,v3);
	return;
}

void CELCloudSprite::FillStream(A3DViewport* pViewport, int idxStreamPos, float fAlpha,A3DVECTOR3 v0, A3DVECTOR3 v3)
{
	ARect<float> rect1(pViewport->GetParam()->X - 0.5f, pViewport->GetParam()->Y - 0.5f, 
		pViewport->GetParam()->X + pViewport->GetParam()->Width + 0.5f, 
		pViewport->GetParam()->Y + pViewport->GetParam()->Height + 0.5f);
	ARect<float> rect2(v0.x, v0.y, v3.x, v3.y);
	ARect<float> inter = rect1 & rect2;

	for(int i=0;i<4;i++)
		m_vResultColor[i].a = fAlpha;

	float fx0, fx3, fy0, fy3;
	if( inter.IsEmpty() )
	{
		// invisible
		v0.x = v0.y = v3.x = v3.y = 0.0f;
		fx0 = fx3 = fy0 = fy3 = 0.0f;
	}
	else
	{
		// clipped
		fx0 = (inter.left - v0.x) / (v3.x - v0.x);
		fx3 = (inter.right - v0.x) / (v3.x - v0.x);
		fy0 = (inter.top - v0.y) / (v3.y - v0.y);
		fy3 = (inter.bottom - v0.y) / (v3.y - v0.y);

		v0.x = inter.left;
		v0.y = inter.top;
		v3.x = inter.right;
		v3.y = inter.bottom;
	}

#define LERP_COLOR(x, y) \
	(((x) + (y)) < 1.0f ? \
	m_vResultColor[0] + (m_vResultColor[1] - m_vResultColor[0]) * (x) + (m_vResultColor[2] - m_vResultColor[0]) * y: \
	m_vResultColor[3] + (m_vResultColor[2] - m_vResultColor[3]) * (1.0f - x) + (m_vResultColor[1] - m_vResultColor[3]) * (1.0f - y))

	
	m_pVertexBuffer[idxStreamPos].x = v0.x;
	m_pVertexBuffer[idxStreamPos].y = v0.y; 
	m_pVertexBuffer[idxStreamPos].z = v0.z;
	m_pVertexBuffer[idxStreamPos].rhw = 1.0f;
	m_pVertexBuffer[idxStreamPos].tu = m_fTextureU[0] + (m_fTextureU[1] - m_fTextureU[0]) * fx0;
	m_pVertexBuffer[idxStreamPos].tv = m_fTextureV[0] + (m_fTextureV[2] - m_fTextureV[0]) * fy0;
	m_pVertexBuffer[idxStreamPos].diffuse = LERP_COLOR(fx0, fy0).ToRGBAColor();
	m_pVertexBuffer[idxStreamPos].specular = 0xff000000;

	m_pVertexBuffer[idxStreamPos+1].x = v3.x;
	m_pVertexBuffer[idxStreamPos+1].y = v0.y;
	m_pVertexBuffer[idxStreamPos+1].z = v0.z;
	m_pVertexBuffer[idxStreamPos+1].rhw = 1.0f;
	m_pVertexBuffer[idxStreamPos+1].tu = m_fTextureU[0] + (m_fTextureU[1] - m_fTextureU[0]) * fx3;
	m_pVertexBuffer[idxStreamPos+1].tv = m_pVertexBuffer[idxStreamPos].tv;
	m_pVertexBuffer[idxStreamPos+1].diffuse = LERP_COLOR(fx3, fy0).ToRGBAColor();
	m_pVertexBuffer[idxStreamPos+1].specular = 0xff000000;
	
	m_pVertexBuffer[idxStreamPos+2].x = v0.x;
	m_pVertexBuffer[idxStreamPos+2].y = v3.y; 
	m_pVertexBuffer[idxStreamPos+2].z = v0.z;
	m_pVertexBuffer[idxStreamPos+2].rhw = 1.0f;
	m_pVertexBuffer[idxStreamPos+2].tu = m_pVertexBuffer[idxStreamPos].tu;
	m_pVertexBuffer[idxStreamPos+2].tv = m_fTextureV[0] + (m_fTextureV[2] - m_fTextureV[0]) * fy3;
	m_pVertexBuffer[idxStreamPos+2].diffuse = LERP_COLOR(fx0, fy3).ToRGBAColor();
	m_pVertexBuffer[idxStreamPos+2].specular = 0xff000000;

	m_pVertexBuffer[idxStreamPos+3].x = v3.x;
	m_pVertexBuffer[idxStreamPos+3].y = v3.y; 
	m_pVertexBuffer[idxStreamPos+3].z = v0.z;
	m_pVertexBuffer[idxStreamPos+3].rhw = 1.0f;
	m_pVertexBuffer[idxStreamPos+3].tu = m_pVertexBuffer[idxStreamPos+1].tu;
	m_pVertexBuffer[idxStreamPos+3].tv = m_pVertexBuffer[idxStreamPos+2].tv;
	m_pVertexBuffer[idxStreamPos+3].diffuse = LERP_COLOR(fx3, fy3).ToRGBAColor();
	m_pVertexBuffer[idxStreamPos+3].specular = 0xff000000;
}

A3DVECTOR3 CELCloudSprite::CalculateSpritePos(float &fAlpha)
{
	A3DVECTOR3 vMoveDir = m_vGroupCenterPos - m_vCenterPos;
	float fDist = vMoveDir.Magnitude();
	if( fDist < 10.0f )
		return m_vMovedPos;
	vMoveDir.Normalize();

	float fStepDist = 0.15f,fValve = 0.5f * fDist;

	DWORD timeNow = GetTickCount();
	DWORD dwDeltaTime = timeNow - m_dwLastTime;
	if( dwDeltaTime > m_dwStepTime)
	{
		//------------------move-------------------
		if( timeNow - m_dwHoldTime > 2000)
			fStepDist = 0.15f;
		else
			fStepDist = 0.0f;

		float value = m_bFarAway?fStepDist:-fStepDist;
		//speed
		if( m_fMoveDist >= 0 )
			m_fMoveDist += (1 - m_fMoveDist / (1.2f*fValve))*value;
		else
			m_fMoveDist += (1 + m_fMoveDist / (1.2f*fValve))*value;

		if ( m_fMoveDist > fValve )
		{
			m_fMoveDist = fValve;
			m_bFarAway = false;

			m_dwHoldTime = timeNow;

		}
		else if ( m_fMoveDist < -0.5f*fValve )
		{
			m_fMoveDist = -0.5f*fValve;
			m_bFarAway = true;

			m_dwHoldTime = timeNow;
		}
	
		m_dwLastTime = timeNow;
	}
		
	//alpha
	if( m_fMoveDist > 0.5f*fValve )
	{
		fAlpha *= max(1 - ( m_fMoveDist - 0.5f*fValve ) / (0.4f*fValve),0.0f);
	}
	else if ( m_fMoveDist < -0.1f*fValve)
	{
		fAlpha *= max(1 + ( m_fMoveDist + 0.1f*fValve) / (0.4f*fValve),0.0f);
	}

	return m_vCenterPos + m_fMoveDist * vMoveDir;
}

bool CELCloudSprite::Update(int nDeltaTime)
{
	m_vMovedPos = CalculateSpritePos(m_fAlpha);
	return true;
}