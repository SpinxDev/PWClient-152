// Filename	: EC_DomainBase.cpp
// Creator	: Han Guanghui
// Date		: 2012/7/18
#include "EC_DomainBase.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EL_Precinct.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DDevice.h"
#include "A3DGdi.h"
#include "Home\\ClosedArea.h"
#include "A3DFlatCollector.h"

void CECDomain::SetDomainInfo(PAUIIMAGEPICTURE pImgDomain, CECDomainInfo* pDomainInfo)
{
	ASSERT(pImgDomain && pDomainInfo);
	m_pImgDomain = pImgDomain;
	A2DSprite *pA2DSprite = m_pImgDomain->GetImage();
	pA2DSprite->SetLinearFilter(true);
	m_vecAreaRect.clear();
	size_t size = pDomainInfo->GetSize();
	for(size_t i = 0; i < size; i++ )
	{
		A3DRECT rect;
		rect.SetRect(9999, 9999, 0, 0);

		const CECDomainInfo::DomainVerts& verts = pDomainInfo->GetVerts(i);
		for(size_t j = 0; j < verts.size(); j++)
		{
			if( verts[j].x < rect.left )
				rect.left = verts[j].x;
			if( verts[j].x > rect.right )
				rect.right = verts[j].x;
			if( verts[j].y < rect.top )
				rect.top = verts[j].y;
			if( verts[j].y > rect.bottom )
				rect.bottom = verts[j].y;
		}
		m_vecAreaRect.push_back(rect);
	}
	m_pDomainInfo = pDomainInfo;
}
CECDomain::~CECDomain()
{
}

//	计算当前鼠标位置 ptMouse 对应图像空间位置与给定图像空间中位置 ptPixel 的距离，用图像控件当前缩放比例中的偏移表示，
//	即若图像控件移动 ptOffset 位置，则 ptMouse 下对应位置即 ptPixel
//	计算方法：根据目标位置 ptPixel 和 函数 CECDomain::ScreenToPixel，
//	推算 CECDomain::ScreenToPixel 中 ptCenter，再和当前值求差即可
A3DPOINT2 CECDomain::CalcImgOffset(const A3DPOINT2 &ptPixel, const A3DPOINT2 &ptMouse)const
{
	//	ptPixel.x = size.cx/2 + (ptMouse.x - pViewportParam->X - ptCenter.x) * size.cx / objRect.Width()
	//	ptCenter.x = ptMouse.x - pViewportParam->x - (ptPixel.x - size.cx/2) * objRect.Width() / size.cx

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	A3DRECT objRect = m_pImgDomain->GetRect();
	A3DPOINT2 ptCenter = objRect.CenterPoint();
	SIZE size;
	size.cx = m_pImgDomain->GetImage()->GetWidth();
	size.cy = m_pImgDomain->GetImage()->GetHeight();

	A3DPOINT2 ptNewCenter;
	ptNewCenter.x = (int)(ptMouse.x -p->X -(ptPixel.x-size.cx/2.0f)*objRect.Width()/size.cx);
	ptNewCenter.y = (int)(ptMouse.y -p->Y -(ptPixel.y-size.cy/2.0f)*objRect.Height()/size.cy);

	return ptNewCenter - ptCenter;
}

// 屏幕坐标转化为图像坐标
A3DPOINT2 CECDomain::ScreenToPixel(A3DPOINT2 pt) const
{
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	pt.x -= p->X;
	pt.y -= p->Y;

	A3DRECT objRect = m_pImgDomain->GetRect();
	A3DPOINT2 ptCenter = objRect.CenterPoint();
	SIZE size;
	size.cx = m_pImgDomain->GetImage()->GetWidth();
	size.cy = m_pImgDomain->GetImage()->GetHeight();
	pt.x = size.cx / 2 + (pt.x - ptCenter.x) * size.cx / objRect.Width();
	pt.y = size.cy / 2 + (pt.y - ptCenter.y) * size.cy / objRect.Height();
	return pt;
}

// 图像坐标转化为PAUIIMAGEPICTURE所在对话框的局部坐标
A3DPOINT2 CECDomain::PixelToScreen(A3DPOINT2 pt) const
{
	A3DRECT objRect = m_pImgDomain->GetRect();
	A3DPOINT2 ptCenter = A3DPOINT2(m_pImgDomain->GetPos(true).x + objRect.Width() / 2, 
		m_pImgDomain->GetPos(true).y + objRect.Height() / 2);
	SIZE size;
	size.cx = m_pImgDomain->GetImage()->GetWidth();
	size.cy = m_pImgDomain->GetImage()->GetHeight();
	pt.x = (pt.x - size.cx / 2) * objRect.Width() / size.cx + ptCenter.x;
	pt.y = (pt.y - size.cy / 2) * objRect.Height() / size.cy + ptCenter.y;

	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	pt.x += p->X;
	pt.y += p->Y;
	return pt;
}

int CECDomain::GetDomainIndexByCoordinate(A3DPOINT2 pt) const
{
	pt = ScreenToPixel(pt);
	DWORD i, j;
	POINT_FLOAT pf;
	pf.x = (float)pt.x;
	pf.y = (float)pt.y;
	int ret = OUT_OF_DOMAIN;
	CClosedArea cArea;
	for(i = 0; i < m_vecAreaRect.size(); i++)
	{
		const CECDomainInfo::DomainVerts& verts = m_pDomainInfo->GetVerts(i);	
		if( m_vecAreaRect[i].PtInRect(pt.x, pt.y) )
		{
			cArea.Clear();
			for(j = 0; j < verts.size(); j++)
			{
				POINT_FLOAT temp;
				temp.x = (float)verts[j].x;
				temp.y = (float)verts[j].y;
				cArea.AddPoint(temp);
			}
			cArea.AreaFinish();
			if( cArea.IsInArea(pf) )
			{
				ret = i;
				break;
			}
		}
	}
	return ret;
}

void CECDomain::RenderArrow(int nIndex1, int nIndex2, A2DSprite *pArrow)
{
	if (NULL == pArrow) return;
	A3DPOINT2 pt1 = PixelToScreen(m_pDomainInfo->GetCenter(nIndex1));
	if( nIndex2 == -1 )
	{
		pArrow->DrawToBack(pt1.x, pt1.y);
		return;
	}

	A3DPOINT2 pt2 = PixelToScreen(m_pDomainInfo->GetCenter(nIndex2));
	int nDegree = 90 + (int)RAD2DEG(atan2((float)(pt2.y - pt1.y), (float)(pt2.x - pt1.x)));
	pArrow->SetDegree(nDegree);
	int dx = pt2.x - pt1.x;
	int dy = pt2.y - pt1.y;
	float fScale = (float)(sqrt((float)(dy * dy + dx * dx)) - 20) / pArrow->GetHeight();
	if( fScale < 0.6f )
		fScale = 0.6f;
	pArrow->SetScaleX(fScale);
	pArrow->SetScaleY(fScale);
	pArrow->DrawToBack((pt2.x + pt1.x) / 2, (pt2.y + pt1.y) / 2);
}

void CECDomain::RenderArea(int nIndex, A3DCOLOR cl)
{
	A3DFlatCollector *pFC = g_pGame->GetA3DEngine()->GetA3DFlatCollector();
	DWORD i;
	const CECDomainInfo::DomainVerts& verts = m_pDomainInfo->GetVerts(nIndex);
	const CECDomainInfo::DomainFaces& faces = m_pDomainInfo->GetFaces(nIndex);
	A3DVECTOR3 *verts3d = (A3DVECTOR3*)a_malloctemp(sizeof(A3DVECTOR3)*verts.size());
	WORD *indices = (WORD*)a_malloctemp(sizeof(WORD)*faces.size());
	for(i = 0; i < verts.size(); i++ )
	{
		A3DPOINT2 pt = PixelToScreen(verts[i]);
		verts3d[i].x = (float)pt.x;
		verts3d[i].y = (float)pt.y;
		verts3d[i].z = 0.0f;
	}
	for(i = 0; i < faces.size(); i++ )
		indices[i] = faces[i];

	A3DCULLTYPE oldtype = g_pGame->GetA3DEngine()->GetA3DDevice()->GetFaceCull();
	g_pGame->GetA3DEngine()->GetA3DDevice()->SetFaceCull(A3DCULL_NONE);
	pFC->AddRenderData_2D(verts3d, verts.size(), indices, faces.size(), cl);
	pFC->Flush_2D();
	g_pGame->GetA3DEngine()->GetA3DDevice()->SetFaceCull(oldtype);
	a_freetemp(indices);
	a_freetemp(verts3d);
}

void CECDomain::RenderImage(int nIndex, A2DSprite *pImg)
{
	RenderArrow(nIndex, -1, pImg);
}

void CECDomain::RenderImageOffsetCenter(int nIndex, const A3DPOINT2 & offset, A2DSprite* pImg, bool bRePosition)
{
	if (NULL == pImg) return;
	A3DPOINT2 pt = PixelToScreen(m_pDomainInfo->GetCenter(nIndex) + offset);
	if (bRePosition)
	{
		A3DRECT rcSize = pImg->GetItem(0)->GetRect();
		pImg->DrawToBack(pt.x - rcSize.Width() / 2, pt.y - rcSize.Height() / 2);
	}
	else pImg->DrawToBack(pt.x, pt.y);
}