#include "Global.h"
#include "brush.h"
#include "Render.h"
#include "MainFrm.h"
#include "A3DMacros.h"
#include "a3d.h"

#define MAX_NUM_SECT 36

const float pi_2 = A3D_2PI;
const float pi_delta = pi_2/MAX_NUM_SECT;
const float offset_y = 0.1f;

void CElementBrush::Render(A3DViewport* pA3DViewport)
{
	if(g_Configs.bCircleBrush)
		DrawCircle();
	else DrawRect();
}

void CElementBrush::Tick(DWORD dwTimeDelta)
{
}

void CElementBrush::SetCenter(float x, float z)
{ 
	m_fCenterX = x; m_fCenterY = z; 
};

void CElementBrush::DrawRect()
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	float fRadius = g_Configs.iBrushRadius;

	A3DVECTOR3 vertices[MAX_NUM_SECT];
	A3DVECTOR3 vLeftBottom = A3DVECTOR3(m_fCenterX - fRadius ,0, m_fCenterY - fRadius);
	A3DVECTOR3 vRightBottom = A3DVECTOR3(m_fCenterX -fRadius ,0, m_fCenterY +  fRadius);
	A3DVECTOR3 vRightUp = A3DVECTOR3(m_fCenterX + fRadius ,0,m_fCenterY + fRadius);
	A3DVECTOR3 vLeftUp = A3DVECTOR3(m_fCenterX + fRadius,0,m_fCenterY - fRadius);
	int n = 0;
	int Num = MAX_NUM_SECT/4;
	
	
	for(;n<Num;n++)
	{
		vertices[n] = vLeftBottom + (vRightBottom - vLeftBottom)*0.111111f*n;
		vertices[n].y = pMap->GetTerrain()->GetPosHeight(vertices[n]) + offset_y;//0.1f
	}

	for(;n<Num*2;n++)
	{
		vertices[n] = vRightBottom + (vRightUp - vRightBottom)*0.111111f*(n-Num);
		vertices[n].y = pMap->GetTerrain()->GetPosHeight(vertices[n]) + offset_y;//0.1f
	}

	for(;n<Num*3;n++)
	{
		vertices[n] = vRightUp + (vLeftUp - vRightUp)*0.111111f*(n-Num*2);
		vertices[n].y = pMap->GetTerrain()->GetPosHeight(vertices[n]) + offset_y;//0.1f
	}

	for(;n<Num*4;n++)
	{
		vertices[n] = vLeftUp + (vLeftBottom - vLeftUp)*0.111111f*(n-Num*3);
		vertices[n].y = pMap->GetTerrain()->GetPosHeight(vertices[n]) + offset_y;//0.1f
	}

	DrawLine(vertices,MAX_NUM_SECT);
}

void CElementBrush::DrawCircle()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	float fRadius = g_Configs.iBrushRadius;
	A3DVECTOR3 vertices[MAX_NUM_SECT];
	
	//Calculate vertices array
	if(pMap && pMap->GetTerrain()->GetRender())
	{
		
		float angle = 0;
		//0 - pi2
		for(int i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].x = m_fCenterX + fRadius*cos(angle);
			vertices[i].z = m_fCenterY + fRadius*sin(angle);
			vertices[i].y = pMap->GetTerrain()->GetPosHeight(vertices[i]) + offset_y;
			
			angle += pi_delta;
		}
	}

	//Render wire
	DrawLine(vertices,MAX_NUM_SECT);
}

void CElementBrush::DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum)
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	
	WORD pIndices[MAX_NUM_SECT*2];
	int n = 0;
	for(int i=0; i<MAX_NUM_SECT; i++)
	{
		pIndices[n] = i;
		pIndices[n+1] = (i+1)%(MAX_NUM_SECT);
		n += 2;
	}
	
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,dwNum,pIndices,MAX_NUM_SECT*2,m_dwColor);
	}
	
	pWireCollector->Flush();
}

