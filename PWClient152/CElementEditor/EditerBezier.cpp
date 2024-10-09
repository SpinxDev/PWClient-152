// EditerBezier.cpp: implementation of the CEditerBezier class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "EC_Bezier.h"
#include "EditerBezier.h"
#include "EditerBezierPoint.h"
#include "EditerBezierSegment.h"
#include "Box3D.h"
#include "Render.h"
#include "SceneObjectManager.h"
#include "MainFrm.h"
#include "TerrainObstruct.h"
#include "A3D.h"

//#define new A_DEBUG_NEW
#define     FLT_MAX         3.402823466e+38F        /* max value */
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Name: GEO_2DIntersectSS()
// Desc: 计算2D空间两条线段的交点(-1:两线段平行或重合,0:无效交点,1:有效交点)
//-----------------------------------------------------------------------------
int GEO_2DIntersectSS(float &Px,float &Py,
			float P1x,float P1y,float P2x,float P2y,
			float P3x,float P3y,float P4x,float P4y)
{
	float a,b,c,d,e;			
	float eps=0.01f;		        //计算精度
	a=P2x-P1x;						
	b=P4x-P3x;						
	c=P2y-P1y;					
	d=P4y-P3y;					
	e=a*d-b*c;
	if(fabs(e)<=eps)return -1;	//两线段平行或重合
	Py=(c*d*(P3x-P1x)+a*d*P1y-b*c*P3y)/e;
	if(fabs(c)<=eps) Px=P3x+b*(Py-P3y)/d;
	else Px=P1x+a*(Py-P1y)/c;
	
	
	bool b1 = FALSE; bool b2 = FALSE;
	//判断交点是否在p1p2上
	if(fabs(Px-P1x)<eps) a = 0;
	else a = Px-P1x;

	if(fabs(Px-P2x)<eps) b = 0;
	else b = Px-P2x;

	if(fabs(Py-P1y)<eps) c = 0;
	else c = Py-P1y;
	
	if(fabs(Py-P2y)<eps) d = 0;
	else d = Py-P2y;
	
	if(a*b<0 && c*d< 0)
		b1 = TRUE;	
	//判断交点是否在p3p4上
	if(fabs(Px-P3x)<eps) a = 0;
	else a = Px-P3x;

	if(fabs(Px-P4x)<eps) b = 0;
	else b = Px-P4x;

	if(fabs(Py-P3y)<eps) c = 0;
	else c = Py-P3y;
	
	if(fabs(Py-P4y)<eps) d = 0;
	else d = Py-P4y;
	
	if(a*b<0 && c*d< 0)
		b2 = TRUE;		
		
	if(b1&&b2) return 1;	//有效交点
	else return 0;			//无效交点
	
}

CEditerBezier::CEditerBezier()
{
	m_nObjectType		= CSceneObject::SO_TYPE_BEZIER;
	m_int_map			= INVALIDOBJECTID;
	m_ptr_a3ddevice		= NULL;
	m_float_pointradius = 0.5f;
	m_float_segradius	= 0.5f;
	m_float_anchorsize	= 0.5f;
	m_nCurSegment		= INVALIDOBJECTID;
	m_nCurPoint			= INVALIDOBJECTID;
	m_int_link			= 0;
	m_vPos				= A3DVECTOR3(0.0f);
	m_bTestStart        = false;
	m_bIsBezier         = true;
	bezier_type.AddElement("折线", 0);
	bezier_type.AddElement("曲线", 1);
	BuildBezierProperty();
	m_nNextBezierID     = -1;
	m_nGlobalID         = -1;
    m_fLenght           = 0.0f;
}

CEditerBezier::CEditerBezier(int map, A3DDevice * pA3DDevice, float ptradius, float segradius, float anchorradius)
{
	m_nObjectType		= CSceneObject::SO_TYPE_BEZIER;
	m_int_map			= map;
	m_ptr_a3ddevice		= pA3DDevice;
	m_float_pointradius	= ptradius;
	m_float_segradius	= segradius;
	m_float_anchorsize	= anchorradius;
	m_int_link			= 0;
	m_vPos				= A3DVECTOR3(0.0f);
	m_bTestStart        = false;
	m_bIsBezier         = true;
	m_nNextBezierID     = -1; 
	m_nGlobalID         = -1;
	m_fLenght           = 0.0f;
	BuildBezierProperty();
}

CEditerBezier::~CEditerBezier()
{
	ReleaseA3DObjects();
}

int CEditerBezier::GetMap()
{
	return m_int_map;
}

void CEditerBezier::SetMap(int map)
{
	m_int_map = map;
}

void CEditerBezier::Release()
{
	ReleaseA3DObjects();
	CSceneObject::Release();
}

void CEditerBezier::ReleaseA3DObjects()
{
	int n = m_list_points.GetCount();
	// 清除所有点 
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		A3DBezierPoint * pPt = (A3DBezierPoint *)m_list_points.GetNext(pos);
		pPt->Release();
		delete pPt;
	}
	
	m_list_points.RemoveAll();
	
	// 清除所有连接
	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		A3DBezierSegment * pSeg = (A3DBezierSegment *)m_list_segments.GetNext(pos);
		pSeg->Release();
		delete pSeg;
	}
	
	m_list_segments.RemoveAll();
	
}

CEditerBezierPoint* CEditerBezier::CreatePoint(const A3DVECTOR3 &pos, int map, int point)
{
	// 创建点，并根据点的数目创建连接
	ASSERT(m_ptr_a3ddevice);
	CEditerBezierPoint * pPt = new CEditerBezierPoint();
	ASSERT(pPt);
	pPt->Init(m_ptr_a3ddevice, A3DVECTOR3(pos.x, pos.y, pos.z), A3DCOLORRGBA(255,0,0,255), m_float_pointradius);
	pPt->SetID(point);

	if(map != INVALIDOBJECTID && m_int_map != INVALIDOBJECTID && m_list_points.GetCount() == 0)
	{
		m_int_map = map;
	}

	// 如果已经存在其它的点，则先添加一个连接
	if(!m_list_points.IsEmpty())
	{
		CEditerBezierPoint * pPrevPoint = (CEditerBezierPoint *)m_list_points.GetTail();
		ASSERT(pPrevPoint);
		CEditerBezierSegment * pSeg = new CEditerBezierSegment();
		ASSERT(pSeg);

		A3DVECTOR3 head = pPrevPoint->GetPos(), 
				   tail = pPt->GetPos(),
				   anchor1 = pPrevPoint->GetPos(),
				   anchor2 = pPt->GetPos();

		pSeg->Init(m_ptr_a3ddevice, head, tail, anchor1, anchor2, m_float_segradius, m_float_anchorsize);
		pSeg->SetTailID(pPt->GetID());
		pSeg->SetHeadID(pPrevPoint->GetID());
		pSeg->SetLink(GeneratePointID());//设置段的ID
		pSeg->SetSegmentColor(m_dwColor);
		m_list_segments.AddTail(pSeg);
	}

	// 将点添加到链表
	m_list_points.AddTail(pPt);
	return pPt;
}

void CEditerBezier::InsertPoint(const A3DVECTOR3 & pos,CEditerBezierSegment *pseg)
{
	ASSERT(pseg);
	A3DVECTOR3 vHead = pseg->GetHead();
	A3DVECTOR3 vTail = pseg->GetTail();
	int nHeadId = pseg->GetHeadID();
	int nTailId = pseg->GetTailID();
	int nNewPointId = GeneratePointID();
	
	//寻找点表的插入位置
	POSITION dotinsert;
	POSITION _ptpos = m_list_points.GetHeadPosition();
	while(_ptpos)
	{
		dotinsert = _ptpos;
		CEditerBezierPoint* pt = (CEditerBezierPoint*)m_list_points.GetNext(_ptpos);
		if(pt->GetID() == pseg->GetHeadID())
			break;
	}
	//产生一个新点
	CEditerBezierPoint * pPt = new CEditerBezierPoint();
	ASSERT(pPt);
	pPt->Init(m_ptr_a3ddevice, pos, A3DCOLORRGB(255,0,0), m_float_pointradius);
	pPt->SetID(nNewPointId);
	//产生两个新段
	CEditerBezierSegment *pSeg1 = new CEditerBezierSegment();
	CEditerBezierSegment *pSeg2 = new CEditerBezierSegment();
	pSeg1->Init(m_ptr_a3ddevice,vHead,A3DVECTOR3(pos.x,pos.y,pos.z),vHead,A3DVECTOR3(pos.x,pos.y,pos.z), m_float_segradius, m_float_anchorsize);
	pSeg2->Init(m_ptr_a3ddevice,A3DVECTOR3(pos.x,pos.y,pos.z),vTail,A3DVECTOR3(pos.x,pos.y,pos.z),vTail, m_float_segradius, m_float_anchorsize);

	pSeg1->SetSegmentColor(m_dwColor);
	pSeg2->SetSegmentColor(m_dwColor);
	
	int segid = GenerateSegmentID();
	pSeg1->SetLink(segid);
	pSeg2->SetLink(segid+1);

	pSeg1->SetHeadID(nHeadId);
	pSeg1->SetTailID(nNewPointId);

	pSeg2->SetHeadID(nNewPointId);
	pSeg2->SetTailID(nTailId);

	POSITION listpos = m_list_segments.GetHeadPosition();
	POSITION deletepos;
	bool suc = false;
	while(listpos)
	{
		deletepos = listpos;
		CEditerBezierSegment* ptemp = (CEditerBezierSegment*)m_list_segments.GetNext(listpos);
		if(ptemp == pseg)
		{
			if(deletepos==NULL) 
			{
				m_list_segments.AddHead(pSeg2); 
				m_list_segments.AddHead(pSeg1);
			}else 
			{
				m_list_segments.InsertAfter(deletepos,pSeg2);
				m_list_segments.InsertAfter(deletepos,pSeg1);
			}
			if(dotinsert==NULL) m_list_points.AddHead(pPt);
			else m_list_points.InsertAfter(dotinsert,pPt);
			suc = true;
			break;
		}
	}

	if(suc)
	{
		pseg->Release();
		delete pseg;
		m_list_segments.RemoveAt(deletepos);
	}else
	{
		pPt->Release();
		delete pPt;
		pSeg1->Release();
		pSeg2->Release();
		delete pSeg1;
		delete pSeg2;
	}
	
}

CEditerBezierPoint* CEditerBezier::FindPoint(int ptid)
{
	POSITION listpos = m_list_points.GetTailPosition();
	while(listpos)
	{
		CEditerBezierPoint *pt = (CEditerBezierPoint*)m_list_points.GetPrev(listpos);
		if(pt->GetID() == ptid)
		{
			return pt;
		}
	}
	return NULL;
}

void CEditerBezier::DeletePoint(int ptid)
{
	POSITION listOld;
	POSITION listpos = m_list_points.GetTailPosition();
	while(listpos)
	{
		listOld = listpos;
		CEditerBezierPoint *pt = (CEditerBezierPoint*)m_list_points.GetPrev(listpos);
		if(pt->GetID() == ptid)
		{
			m_list_points.RemoveAt(listOld);
			break;
		}
		
	}

	POSITION listseg = m_list_segments.GetTailPosition();
	POSITION insertPos;
	int link[2] = { -1,-1 };
	A3DVECTOR3 dot[2];
	while(listseg)
	{
		listOld = listseg;
		CEditerBezierSegment*ps = (CEditerBezierSegment*)m_list_segments.GetPrev(listseg);
		if(ps->GetTailID() == ptid)
		{
			dot[0] = ps->GetHead();
			link[0] = ps->GetHeadID();
			m_list_segments.RemoveAt(listOld);
			insertPos = listseg;
			listseg = m_list_segments.GetTailPosition();
		}else
		if(ps->GetHeadID() == ptid)
		{
			dot[1] = ps->GetTail();
			link[1] = ps->GetTailID();
			m_list_segments.RemoveAt(listOld);
			listseg = m_list_segments.GetTailPosition();
		}
	}

	if(link[0]!=-1 && link[1]!=-1)
	{
		CEditerBezierSegment*pNewSeg = new CEditerBezierSegment();
		pNewSeg->Init(g_Render.GetA3DDevice(),dot[0],dot[1],dot[0],dot[1],m_float_segradius, m_float_anchorsize);
		pNewSeg->SetHeadID(link[0]);
		pNewSeg->SetTailID(link[1]);
		pNewSeg->SetLink(GenerateSegmentID());//设置段的ID
		pNewSeg->SetSegmentColor(m_dwColor);
		if(insertPos == NULL) m_list_segments.AddHead(pNewSeg);
		else m_list_segments.InsertAfter(insertPos,pNewSeg);
	}
}


void CEditerBezier::OptimizeSmooth()
{
	// 优化最后三个点和两个连接使之圆滑
	// 正常情况下要求有三个点和两个连接
	if(m_list_points.GetCount() < 2 || m_list_segments.GetCount() < 1) return;
	if(m_list_segments.GetCount() == 1)
	{
		// 仅有一段曲线，那么将曲线拉成直线
		CEditerBezierPoint * pt1 = NULL,
						   * pt2 = NULL;

		POSITION listpos = m_list_points.GetTailPosition();
		pt2 = (CEditerBezierPoint *)m_list_points.GetPrev(listpos);
		pt1 = (CEditerBezierPoint *)m_list_points.GetPrev(listpos);

		CEditerBezierSegment * pSeg1 = NULL;
		POSITION segpos = m_list_segments.GetTailPosition();
		pSeg1 = (CEditerBezierSegment *)m_list_segments.GetPrev(segpos);

		pSeg1->SetHead(pt1->GetPos());
		pSeg1->SetTail(pt2->GetPos());
		pSeg1->SetAnchor1(pt1->GetPos());
		pSeg1->SetAnchor2(pt2->GetPos());
		return;
	}

	CEditerBezierPoint	* pt1 = NULL,
						* pt2 = NULL,
						* pt3 = NULL;

	POSITION listpos = m_list_points.GetTailPosition();
	pt3 = (CEditerBezierPoint *)m_list_points.GetPrev(listpos);
	pt2 = (CEditerBezierPoint *)m_list_points.GetPrev(listpos);
	pt1 = (CEditerBezierPoint *)m_list_points.GetPrev(listpos);

	CEditerBezierSegment * pSeg1 = NULL,
						 * pSeg2 = NULL;

	POSITION segpos = m_list_segments.GetTailPosition();
	pSeg2 = (CEditerBezierSegment *)m_list_segments.GetPrev(segpos);
	pSeg1 = (CEditerBezierSegment *)m_list_segments.GetPrev(segpos);

	// 取三个关键点位置，如下
	/* pt1----------------pt2-----------------pt3 */
	A3DVECTOR3	pos1 = pt1->GetPos(),
				pos2 = pt2->GetPos(),
				pos3 = pt3->GetPos();

	//计算这三个点构成的三角形的三边长及周长
	double long12 = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y) + (pos2.z - pos1.z) * (pos2.z - pos1.z));
	double long23 = sqrt((pos3.x - pos2.x) * (pos3.x - pos2.x) + (pos3.y - pos2.y) * (pos3.y - pos2.y) + (pos3.z - pos2.z) * (pos3.z - pos2.z));
	double long31 = sqrt((pos3.x - pos1.x) * (pos3.x - pos1.x) + (pos3.y - pos1.y) * (pos3.y - pos1.y) + (pos3.z - pos1.z) * (pos3.z - pos1.z));
	double grith = long12 + long23 + long31;

	//取得一个pos2顶点对边长度与周长之比，和两个邻边分别与两个邻边和之比
	double vecscale31 = (long31 / grith),
		   vecscale12 = (long12) / (long12 + long23),
		   vecscale23 = (long23) / (long12 + long23);

	//计算两个曲线的一阶导数范围，分别为:
	//与对边平行的方向上，两个邻边的比例乘以对边与周长之比，为该点进、出方向的导数
	A3DVECTOR3 vec12, vec23;
	vec12.x = (float)((pos3.x - pos1.x) * vecscale31 * vecscale12);
	vec12.y = (float)((pos3.y - pos1.y) * vecscale31 * vecscale12);
	vec12.z = (float)((pos3.z - pos1.z) * vecscale31 * vecscale12);
	vec23.x = (float)((pos3.x - pos1.x) * vecscale31 * vecscale23);
	vec23.y = (float)((pos3.y - pos1.y) * vecscale31 * vecscale23);
	vec23.z = (float)((pos3.z - pos1.z) * vecscale31 * vecscale23);

	A3DVECTOR3 c1leave, c2enter;
	c1leave.x = pos2.x - vec12.x;
	c1leave.y = pos2.y - vec12.y;
	c1leave.z = pos2.z - vec12.z;
	pSeg1->SetAnchor2(c1leave);
	pSeg1->SetTail(pos2);
//	pt2->SetInAnchor(c1leave);

	c2enter.x = pos2.x + vec23.x;
	c2enter.y = pos2.y + vec23.y;
	c2enter.z = pos2.z + vec23.z;
	pSeg2->SetAnchor1(c2enter);
	pSeg2->SetAnchor2(pos3);
	pSeg2->SetHead(pos2);
	pSeg2->SetTail(pos3);
}

void CEditerBezier::OptimizeSmooth(POSITION posPoint,POSITION posSeg)
{
	if(!posPoint || !posSeg) return;
	
	CEditerBezierPoint	* pt1 = NULL,
						* pt2 = NULL,
						* pt3 = NULL;

	POSITION posPt = posPoint;
	pt3 = (CEditerBezierPoint *)m_list_points.GetPrev(posPt);
	if(!posPt) return;
	pt2 = (CEditerBezierPoint *)m_list_points.GetPrev(posPt);
	if(!posPt) return;
	pt1 = (CEditerBezierPoint *)m_list_points.GetPrev(posPt);

	CEditerBezierSegment * pSeg1 = NULL,
						 * pSeg2 = NULL;

	POSITION segpos = posSeg;
	pSeg2 = (CEditerBezierSegment *)m_list_segments.GetPrev(segpos);
	if(!segpos) return;
	pSeg1 = (CEditerBezierSegment *)m_list_segments.GetPrev(segpos);

	// 取三个关键点位置，如下
	/* pt1----------------pt2-----------------pt3 */
	A3DVECTOR3	pos1 = pt1->GetPos(),
				pos2 = pt2->GetPos(),
				pos3 = pt3->GetPos();

	//计算这三个点构成的三角形的三边长及周长
	double long12 = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y) + (pos2.z - pos1.z) * (pos2.z - pos1.z));
	double long23 = sqrt((pos3.x - pos2.x) * (pos3.x - pos2.x) + (pos3.y - pos2.y) * (pos3.y - pos2.y) + (pos3.z - pos2.z) * (pos3.z - pos2.z));
	double long31 = sqrt((pos3.x - pos1.x) * (pos3.x - pos1.x) + (pos3.y - pos1.y) * (pos3.y - pos1.y) + (pos3.z - pos1.z) * (pos3.z - pos1.z));
	double grith = long12 + long23 + long31;

	//取得一个pos2顶点对边长度与周长之比，和两个邻边分别与两个邻边和之比
	double vecscale31 = (long31 / grith),
		   vecscale12 = (long12) / (long12 + long23),
		   vecscale23 = (long23) / (long12 + long23);

	//计算两个曲线的一阶导数范围，分别为:
	//与对边平行的方向上，两个邻边的比例乘以对边与周长之比，为该点进、出方向的导数
	A3DVECTOR3 vec12, vec23;
	vec12.x = (float)((pos3.x - pos1.x) * vecscale31 * vecscale12);
	vec12.y = (float)((pos3.y - pos1.y) * vecscale31 * vecscale12);
	vec12.z = (float)((pos3.z - pos1.z) * vecscale31 * vecscale12);
	vec23.x = (float)((pos3.x - pos1.x) * vecscale31 * vecscale23);
	vec23.y = (float)((pos3.y - pos1.y) * vecscale31 * vecscale23);
	vec23.z = (float)((pos3.z - pos1.z) * vecscale31 * vecscale23);

	A3DVECTOR3 c1leave, c2enter;
	c1leave.x = pos2.x - vec12.x;
	c1leave.y = pos2.y - vec12.y;
	c1leave.z = pos2.z - vec12.z;
	pSeg1->SetAnchor2(c1leave);
	pSeg1->SetTail(pos2);
//	pt2->SetInAnchor(c1leave);

	c2enter.x = pos2.x + vec23.x;
	c2enter.y = pos2.y + vec23.y;
	c2enter.z = pos2.z + vec23.z;
	pSeg2->SetAnchor1(c2enter);
	pSeg2->SetAnchor2(pos3);
	pSeg2->SetHead(pos2);
	pSeg2->SetTail(pos3);
}

void CEditerBezier::SmoothBezier()
{
	if(m_list_points.GetCount()>=3)
	{
		POSITION listpos = m_list_points.GetHeadPosition();
		m_list_points.GetNext(listpos);
		POSITION listseg = m_list_segments.GetHeadPosition();
		while(listseg && listpos)
		{
			CEditerBezierPoint *pPt = (CEditerBezierPoint *)m_list_points.GetNext(listpos);
			CEditerBezierSegment *pSeg = (CEditerBezierSegment*)m_list_segments.GetNext(listseg);
			OptimizeSmooth(listpos,listseg);
		}
	}
}

int CEditerBezier::GetPointCount()
{
	return m_list_points.GetCount();
}

CEditerBezierPoint * CEditerBezier::GetTailPoint()
{
	if(m_list_points.IsEmpty()) return NULL;
	return (CEditerBezierPoint *)m_list_points.GetTail();
}

A3DVECTOR3 CEditerBezier::GetHeadPos()
{
	CEditerBezierPoint * phead = (CEditerBezierPoint *)m_list_points.GetHead();
	ASSERT(phead);
	return phead->GetPos();
}

A3DVECTOR3 CEditerBezier::GetTailPos()
{
	CEditerBezierPoint * ptail = GetTailPoint();	
	ASSERT(ptail);
	return ptail->GetPos();
}

void CEditerBezier::Tick(DWORD timeDelta)
{
}

void CEditerBezier::Render(A3DViewport *pViewport)
{
	ASSERT(pViewport);
	if(!m_bTestStart)
	{
		POSITION pos = m_list_points.GetHeadPosition();
		while(pos)
		{
			CEditerBezierPoint * pPt = (CEditerBezierPoint *)m_list_points.GetNext(pos);
			pPt->Render(pViewport);
		}
		
		pos = m_list_segments.GetHeadPosition();
		while(pos)
		{
			CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
			if(m_bIsBezier) pSeg->Render(pViewport, m_bSelected);
			else pSeg->Render(pViewport, false);
		}
		
		CSceneObject::Render(pViewport);
	}
}

void CEditerBezier::DeleteAllPoint()
{
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint * pPt = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		pPt->Release();
		delete pPt;
	}
	m_list_points.RemoveAll();
}

void CEditerBezier::DeleteAllSegment()
{
	POSITION pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		pSeg->Release();
		delete pSeg;
	}
	m_list_segments.RemoveAll();
}

void CEditerBezier::CalculateDirection()
{
	// 添加所有的连接到地图中
	POSITION segpos = m_list_segments.GetHeadPosition();
	int seg = 0;
	while(segpos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(segpos);
		CEditerBezierPoint *pPt1 = FindPoint(pSeg->GetHeadID());
		CEditerBezierPoint *pPt2 = FindPoint(pSeg->GetTailID());
		float d = 300.0f;
		
		// 首点
		A3DVECTOR3 spot1 = pSeg->GetAnchor1() - pSeg->GetHead();
		spot1.Normalize();
		if((spot1.x + spot1.y + spot1.z) == 0.0f)
		{
			// 计算新切线方向
			A3DVECTOR3 temp = pSeg->Bezier(0.01f);
			spot1 =temp - pSeg->GetHead();
			spot1.Normalize();
		}

		if((spot1.x + spot1.y + spot1.z) == 0.0f) spot1.z = 1.0f;
		pSeg->SetHeadSpot(A3DVECTOR3(spot1.x * d, spot1.y * d, spot1.z * d));
		pPt1->SetDirection(pSeg->GetHeadSpot());
		// 尾点
		A3DVECTOR3 spot2 = pSeg->GetTail() - pSeg->GetAnchor2();
		spot2.Normalize();
		if((spot2.x + spot2.y + spot2.z) == 0.0f)
		{
			// 计算新切线方向
			A3DVECTOR3 temp = pSeg->Bezier(0.99f);
			spot2 = pSeg->GetTail() - temp;
			spot2.Normalize();
		}

		if((spot2.x + spot2.y + spot2.z) == 0.0f) spot2.z = 1.0f;
		pSeg->SetTailSpot(A3DVECTOR3(spot2.x * d, spot2.y * d, spot2.z * d));
		pPt2->SetDirection(pSeg->GetTailSpot());
		
		seg ++;
	}

}
/*
// 根据当前曲线生成map对象，并舍弃最后一点
APathMap * CEditerBezier::GeneralMap()
{
	APathMap * pMap = new APathMap();
	
	// 添加所有的点和连接到地图中
	POSITION ptpos = m_list_points.GetHeadPosition();
	while(ptpos && ptpos != m_list_points.GetTailPosition())
	{
		CEditerBezierPoint * pBezierPt = (CEditerBezierPoint *)m_list_points.GetNext(ptpos);
		APathPoint * pPt = new APathPoint();
		pPt->SetPos(pBezierPt->GetPos());
		pMap->AttachPoint(pPt);
	}

	// 添加所有的连接到地图中
	POSITION segpos = m_list_segments.GetHeadPosition();
	int seg = 0;
	while(segpos && segpos != m_list_segments.GetTailPosition())
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(segpos);
		APathPoint * pPt1 = pMap->GetPoint(seg),
				   * pPt2 = pMap->GetPoint(seg + 1);

		if(pPt1 && pPt2)
		{
			pMap->CreateLink(
				pPt1->GetId(), 
				pPt2->GetId(), 
				pSeg->GetAnchor1() - pSeg->GetHead(),
				pSeg->GetAnchor2() - pSeg->GetTail(),
				false);
		}

		float d = 300.0f;
		// 首点
		A3DVECTOR3 spot1 = Normalize(pSeg->GetAnchor1() - pSeg->GetHead());
		if((spot1.x + spot1.y + spot1.z) == 0.0f)
		{
			// 计算新切线方向
			A3DVECTOR3 temp = pSeg->Bezier(0.1f);
			spot1 = Normalize(temp - pSeg->GetHead());
		}

		if((spot1.x + spot1.y + spot1.z) == 0.0f) spot1.z = 1.0f;
		pPt1->SetInSpot(A3DVECTOR3(spot1.x * d, spot1.y * d, spot1.z * d));

		// 尾点
		A3DVECTOR3 spot2 = Normalize(pSeg->GetTail() - pSeg->GetAnchor2());
		if((spot2.x + spot2.y + spot2.z) == 0.0f)
		{
			// 计算新切线方向
			A3DVECTOR3 temp = pSeg->Bezier(0.9f);
			spot2 = Normalize(pSeg->GetTail() - temp);
		}

		if((spot2.x + spot2.y + spot2.z) == 0.0f) spot2.z = 1.0f;
		pPt2->SetInSpot(A3DVECTOR3(spot2.x * d, spot2.y * d, spot2.z * d));
		seg ++;
	}

	// 生成一种随机颜色
	pMap->SetStartPoint(0);
	return pMap;
}

// 将当前曲线合并到map对象中，并舍弃最后一个点
BOOL CEditerBezier::UniteMap(APathMap *pMap)
{
	ASSERT(pMap);
	POSITION ptpos = m_list_points.GetHeadPosition();
	POSITION segpos = m_list_segments.GetHeadPosition();
	int prevptid = INVALIDOBJECTID;
	while(ptpos && ptpos != m_list_points.GetTailPosition())
	{
		CEditerBezierPoint * pPt = (CEditerBezierPoint *)m_list_points.GetNext(ptpos);
		int ptid = INVALIDOBJECTID;
		APathPoint * pPoint = NULL;
		if(!ISINVALIDID(pPt->GetID()))
		{
			ptid = pPt->GetID();
			pPoint = pMap->GetPoint(ptid);
		}
		else
		{
			pPoint = new APathPoint();
			pPoint->SetPos(pPt->GetPos());
			ptid = pMap->AttachPoint(pPoint);
		}

		if(!ISINVALIDID(ptid) && !ISINVALIDID(prevptid) && segpos != m_list_segments.GetTailPosition())
		{
			APathPoint * pPrevPoint = pMap->GetPoint(prevptid);
			CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(segpos);
			int segid = pMap->CreateLink(
				prevptid, 
				ptid, 
				pSeg->GetAnchor1(), 
				pSeg->GetAnchor2(),
				false);
		}

		prevptid = ptid;
	}
	
	return TRUE;
}
*/

A3DVECTOR3 CEditerBezier::GetMin()
{
	return A3DVECTOR3(0.0f,0.0f,0.0f);
}

A3DVECTOR3 CEditerBezier::GetMax()
{
	return A3DVECTOR3(0.0f,0.0f,0.0f);
}

void CEditerBezier::UpdateProperty(bool bGet)
{
	if(bGet)
	{
		AString temp = AString(m_pProperty->GetPropVal(0));
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			CSceneObject*pObj = pSManager->FindSceneObject(temp);
			if(pObj != NULL && pObj != this)
			{
				MessageBox(NULL,"该对象的名字已经存在，编辑器将恢复原对象名字！",NULL,MB_OK);
				m_pProperty->SetPropVal(0,m_strName);
				m_pProperty->SetPropVal(1,m_vPos);
				m_pProperty->SetPropVal(2,m_dwColor);
				m_pProperty->SetPropVal(3,m_bIsBezier);
				m_pProperty->SetPropVal(4,m_nNextBezierID);
				m_pProperty->SetPropVal(5,m_nGlobalID);
				m_pProperty->SetPropVal(6,m_fLenght);
				m_pProperty->SetPropVal(7,m_strActionName);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName = temp;
		m_vPos = m_pProperty->GetPropVal(1);
		m_dwColor = m_pProperty->GetPropVal(2);
		bool bnew = m_pProperty->GetPropVal(3);
		m_nNextBezierID = m_pProperty->GetPropVal(4);
		m_nGlobalID = m_pProperty->GetPropVal(5);
		m_strActionName = AString(m_pProperty->GetPropVal(7));
		if(m_list_points.GetCount()>0)
		{
			A3DVECTOR3 vOldPos;
			CEditerBezierPoint* pObj = (CEditerBezierPoint*)m_list_points.GetHead();
			if(pObj) vOldPos = pObj->GetPos();
			vOldPos = m_vPos - vOldPos;
			Translate(vOldPos);
			m_matAbs = m_matScale * m_matRotate * m_matTrans;
		}
		UpdateSegmentProperty();
		BuildBoundingBox();
		Convert();
		
		if(bnew != m_bIsBezier && !bnew)
		{
			if(IntersectWithSelf()) AfxMessageBox("不能转变成折线区域，在X,Z平面内有相交线段!");
			else m_bIsBezier = bnew;
		}else m_bIsBezier = bnew;
		
	}else
	{
		if(m_list_points.GetCount()>0)
		{
			CEditerBezierPoint* pObj = (CEditerBezierPoint*)m_list_points.GetHead();
			if(pObj) m_vPos = pObj->GetPos();
		}
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_dwColor);
		m_pProperty->SetPropVal(3,m_bIsBezier);
		m_pProperty->SetPropVal(4,m_nNextBezierID);
		m_pProperty->SetPropVal(5,m_nGlobalID);
		m_pProperty->SetPropVal(6,m_fLenght);
		m_pProperty->SetPropVal(7,m_strActionName);
	}	
}

void CEditerBezier::UpdateSegmentProperty()
{
	//Segment
	POSITION pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		pSeg->SetControlColor(A3DCOLORRGB(0,255,0));
		pSeg->SetSegmentRadius(m_float_segradius);
		pSeg->SetSegmentColor(m_dwColor);
		pSeg->SetControlRadius(m_float_anchorsize);
	}
}

void CEditerBezier::BuildBezierProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置");
		m_pProperty->DynAddProperty(AVariant(m_dwColor), "曲线颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant((int)m_bIsBezier), "是曲线", &bezier_type);
		m_pProperty->DynAddProperty(AVariant(m_nNextBezierID), "连接的下一条曲线");
		m_pProperty->DynAddProperty(AVariant(m_nGlobalID), "曲线ID");
		m_pProperty->DynAddProperty(AVariant(m_fLenght), "长度(m)",NULL,NULL,WAY_READONLY);
		m_pProperty->DynAddProperty(AVariant(m_strActionName), "动作名");
		
	}	
}

bool CEditerBezier::RayTrace(A3DVECTOR3& vStart,A3DVECTOR3& vEnd,bool bAdd)
{
	float pd = FLT_MAX;
	float sd = FLT_MAX;
	A3DVECTOR3 trace_point_pt;
	A3DVECTOR3 trace_seg_pt;
	A3DVECTOR3 trace_pt;
	CEditerBezierSegment* m_pCurSegment = NULL;
	CEditerBezierPoint*   m_pCurPoint = NULL;
	// Point
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint * pPoint = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		A3DVECTOR3 trace_pt;
		float tpd = 0.0f;
		if(pPoint->Pick(vStart, vEnd - vStart, &trace_pt, &pd))
		{
			if(tpd < pd)
			{
				pd = tpd;
				m_pCurPoint = pPoint;
				trace_point_pt = trace_pt;
			}
		}
	}
	if(m_pCurPoint!=NULL) return true;

	
	
	//Segment
	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		float tssd = FLT_MAX,tscd = FLT_MAX;
		if(pSeg->PickSegment(vStart, vEnd - vStart, &trace_pt, &tssd))
		{
			if(tssd < sd)
			{
				m_pCurSegment = pSeg;
				sd = tssd;
				trace_seg_pt = trace_pt;
			}
		}
	}
	if(m_pCurSegment!=NULL) return true;
	return false;
}

bool CEditerBezier::PickControl(A3DVECTOR3& vStart,A3DVECTOR3& vEnd, int &nFlag)
{
	CEditerBezierSegment* m_pCurSegment = NULL;
	CEditerBezierPoint*   m_pCurPoint = NULL;
	float pd = FLT_MAX;
	float sd = FLT_MAX;
	m_bFirstCtrl = false;
	bool bStartCtrl = false;
	m_nFlag = 0;
	A3DVECTOR3 trace_point_pt;
	A3DVECTOR3 trace_seg_pt;
	A3DVECTOR3 trace_ctrl_pt;

	bool bPickedPt = false;
	bool bPickedSeg = false;
	bool bPickedCtrl = false;
	
	if(m_bSelected)
	{
		
		// Point
		POSITION pos = m_list_points.GetHeadPosition();
		while(pos)
		{
			CEditerBezierPoint * pPoint = (CEditerBezierPoint *)m_list_points.GetNext(pos);
			A3DVECTOR3 trace_pt;
			float tpd = 0.0f;
			if(pPoint->Pick(vStart, vEnd - vStart, &trace_pt, &pd))
			{
				if(tpd < pd)
				{
					pd = tpd;
					m_pCurPoint = pPoint;
					m_nCurPoint = m_pCurPoint->GetID();
					trace_point_pt = trace_pt;
					bPickedPt = true;
				}
			}
		}
		
		//Segment
		pos = m_list_segments.GetHeadPosition();
		while(pos)
		{
			CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
			A3DVECTOR3 trace_pt;
			float tssd = FLT_MAX;
			sd = FLT_MAX;
			if(pSeg->PickSegment(vStart, vEnd - vStart, &trace_pt, &tssd))
			{
				if(tssd < sd)
				{
					m_pCurSegment = pSeg;
					m_nCurSegment = m_pCurSegment->GetLink();
					sd = tssd;
					trace_seg_pt = trace_pt;
					m_vSegHitPos = trace_seg_pt;
					bPickedSeg  = true;
				}
			}
			bool bStartControl = false;
			// 在显示控制点的情况下，trace控制点
			sd = FLT_MAX;
			if(pSeg->PickControl(vStart, vEnd - vStart, &bStartControl, &trace_pt))
			{
				float cd = (trace_pt - vStart).Magnitude();
				if(cd < sd)
				{
					m_pCurSegment = pSeg;
					m_nCurCtrlSeg = m_pCurSegment->GetLink();
					sd = cd;
					m_bFirstCtrl = bStartControl;
					bPickedCtrl = true;
					trace_ctrl_pt = trace_pt;
				}
			}
		}
		if(!bPickedPt && !bPickedSeg && !bPickedCtrl)
			return false;
		
		float dis1 = FLT_MAX,dis2 = FLT_MAX, dis3 = FLT_MAX;
		if(bPickedPt) dis1 =  (vStart - trace_point_pt).Magnitude();
		if(bPickedSeg) dis2 =  (vStart - trace_seg_pt).Magnitude();
		if(bPickedCtrl) dis3 =  (vStart - trace_ctrl_pt).Magnitude();
		
		if(dis1>dis2)
		{
			if(dis2>dis3)
			{//返回控制点
				nFlag = 3;
			}else
			{//返回段
				nFlag = 2;
			}
		}else
		{
			if(dis1>dis3)
			{//返回控制点
				nFlag = 3;
			}else
			{//返回点
				nFlag = 1;
			}
		}
		m_nFlag = nFlag;
		return true;
	}
	return false;
}

CEditerBezierSegment* CEditerBezier::GetCurSegment(A3DVECTOR3 &vTarget)
{ 
	POSITION pos = m_list_segments.GetHeadPosition();
	vTarget = m_vSegHitPos;
	while(pos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		if(pSeg->GetLink() == m_nCurSegment) return pSeg;
	}
	
	return NULL; 
};


CEditerBezierPoint* CEditerBezier::GetCurPoint(CEditerBezierSegment**segPre,CEditerBezierSegment**segNex)
{ 
	CEditerBezierPoint * pPt = NULL;
	*segPre = NULL; *segNex = NULL;
	
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		pPt = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		if(pPt->GetID() == m_nCurPoint) break;
	}

	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		if(pSeg->GetTailID() == m_nCurPoint) *segPre = pSeg;
		if(pSeg->GetHeadID() == m_nCurPoint) *segNex = pSeg;
	}
	
	return pPt;
};

CEditerBezierSegment* CEditerBezier::GetCurCtrl(bool &bStart)
{
	POSITION pos = m_list_segments.GetHeadPosition();
	bStart = m_bFirstCtrl;
	while(pos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		if(pSeg->GetLink() == m_nCurCtrlSeg) return pSeg;
	}
	return NULL;
}

void CEditerBezier::AddPointTail(CEditerBezierPoint *pt)
{
	ASSERT(pt);
	m_list_points.AddTail(pt);
}

void CEditerBezier::AddSegmentTail(CEditerBezierSegment *seg)
{
	ASSERT(seg);
	m_list_segments.AddTail(seg);
}

int CEditerBezier::GeneratePointID()
{
	int newId = 0;
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		if(ptemp->GetID()>newId) newId = ptemp->GetID();
	}
	return newId + 1;
}

int CEditerBezier::GenerateSegmentID()
{
	int newId = 0;
	POSITION pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		if(ptemp->GetLink()>newId) newId = ptemp->GetLink();
	}
	return newId + 1;
}

void CEditerBezier::ReCalculateLength()
{
	m_fLenght = 0.0f;
	POSITION pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		ptemp->ReCalculateLength();
		m_fLenght += ptemp->GetLenght();
	}
}

A3DVECTOR3  CEditerBezier::GetPos()
{
	CEditerBezierPoint *pt = (CEditerBezierPoint*)m_list_points.GetHead();
	if(pt) return pt->GetPos();
	else return A3DVECTOR3(0.0f);
}

void CEditerBezier::SetPos(const A3DVECTOR3& vPos)
{
	A3DVECTOR3 vOldPos = GetPos();
	A3DVECTOR3 vDelta = vPos - vOldPos;
	Translate(vDelta);
}

void  CEditerBezier::Translate(const A3DVECTOR3& vDelta)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	//point list
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		A3DVECTOR3 vPos = ptemp->GetPos();
		vPos = vPos + vDelta;
		if(!m_bIsBezier) vPos.y = pMap->GetTerrain()->GetPosHeight(vPos) + 0.1f;
		ptemp->SetPos(vPos);
	}
	
	if(m_list_points.GetCount()>0)
	{
		CEditerBezierPoint* pObj = (CEditerBezierPoint*)m_list_points.GetHead();
		if(pObj) m_vPos = pObj->GetPos();
		if(!m_bIsBezier) m_vPos.y = pMap->GetTerrain()->GetPosHeight(m_vPos) + 0.1f;
	}

	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp1 = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		A3DVECTOR3 v = ptemp1->GetAnchor1();
		v = v + vDelta;
		if(!m_bIsBezier) v.y = pMap->GetTerrain()->GetPosHeight(v) + 0.1f;
		ptemp1->SetAnchor1(v);

		v = ptemp1->GetAnchor2();
		v = v+ vDelta;
		if(!m_bIsBezier) v.y = pMap->GetTerrain()->GetPosHeight(v) + 0.1f;
		ptemp1->SetAnchor2(v);

		v = ptemp1->GetHead();
		v = v+ vDelta;
		if(!m_bIsBezier) v.y = pMap->GetTerrain()->GetPosHeight(v) + 0.1f;
		ptemp1->SetHead(v);

		v = ptemp1->GetTail();
		v = v + vDelta;
		if(!m_bIsBezier) v.y = pMap->GetTerrain()->GetPosHeight(v) + 0.1f;
		ptemp1->SetTail(v);
	}
}

CSceneObject* CEditerBezier::CopyObject()
{
	if(!m_bIsBezier) return NULL;
	CEditerBezier *pNewObject = new CEditerBezier(0,g_Render.GetA3DDevice(),m_float_pointradius,m_float_segradius,m_float_anchorsize);
	ASSERT(pNewObject);
	pNewObject->BuildBezierProperty();
	pNewObject->SetProperty(m_dwColor);
	//point list
	int PointNum = 0;
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		A3DVECTOR3 vPos = ptemp->GetPos();
		pNewObject->CreatePoint(vPos,m_int_map,PointNum++);
		pNewObject->OptimizeSmooth();
	}
	
	pNewObject->UpdateSegmentProperty();
	int BezierNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Bezier_%d",BezierNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				BezierNum++;
			}else 
			{
				BezierNum++;
				break;
			}
		}
		pNewObject->m_nObjectID = pSManager->GenerateObjectID();
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->SetPos(m_vPos);
		
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	return pNewObject;
}

//Load data
bool CEditerBezier::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags) 
{
	if(dwVersion>1 && dwVersion<=0xd)
	{//2.0版本开放
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_float_pointradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_anchorsize,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_segradius,sizeof(float)))
			return false;
		
		//读点的数据
		int Num;
		int ID;
		A3DVECTOR3 temp;
		
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(int k=0; k<Num; k++)
		{
			if(!ar.Read(&ID,sizeof(int)))
				return false;
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
				return false;
			
			CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
			pNewPt->Init(m_ptr_a3ddevice, A3DVECTOR3(temp.x, temp.y, temp.z), A3DCOLORRGBA(255,0,0,255), m_float_pointradius);
			pNewPt->SetID(ID);
			
			m_list_points.AddTail(pNewPt);
		}
		
		//读取段的数据
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(k = 0; k<Num; k++)
		{
			CEditerBezierSegment *pNewSeg = new CEditerBezierSegment();
			pNewSeg->Init(m_ptr_a3ddevice, temp, temp, temp, temp, m_float_segradius, m_float_anchorsize);
			pNewSeg->SetSegmentColor(m_dwColor);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor1(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor2(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHead(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTail(temp);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHeadID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTailID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetLink(ID);
			
			m_list_segments.AddTail(pNewSeg);
		}
	}else if(dwVersion>0xd && dwVersion<=0xf)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_float_pointradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_anchorsize,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_segradius,sizeof(float)))
			return false;
		
		//读点的数据
		int Num;
		int ID;
		A3DVECTOR3 temp;
		
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(int k=0; k<Num; k++)
		{
			if(!ar.Read(&ID,sizeof(int)))
				return false;
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
				return false;
			
			CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
			pNewPt->Init(m_ptr_a3ddevice, A3DVECTOR3(temp.x, temp.y, temp.z), A3DCOLORRGBA(255,0,0,255), m_float_pointradius);
			pNewPt->SetID(ID);
			
			m_list_points.AddTail(pNewPt);
		}
		
		//读取段的数据
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(k = 0; k<Num; k++)
		{
			CEditerBezierSegment *pNewSeg = new CEditerBezierSegment();
			pNewSeg->Init(m_ptr_a3ddevice, temp, temp, temp, temp, m_float_segradius, m_float_anchorsize);
			pNewSeg->SetSegmentColor(m_dwColor);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor1(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor2(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHead(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTail(temp);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHeadID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTailID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetLink(ID);
			
			m_list_segments.AddTail(pNewSeg);
		}	
	}else if(dwVersion > 0x22 && dwVersion < 0x24)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_nGlobalID,sizeof(int)))
			return false;
		if(!ar.Read(&m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_float_pointradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_anchorsize,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_segradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsBezier,sizeof(bool)))
			return false;
		//读点的数据
		int Num;
		int ID;
		A3DVECTOR3 temp;
		
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(int k=0; k<Num; k++)
		{
			if(!ar.Read(&ID,sizeof(int)))
				return false;
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
				return false;
			
			CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
			pNewPt->Init(m_ptr_a3ddevice, A3DVECTOR3(temp.x, temp.y, temp.z), A3DCOLORRGBA(255,0,0,255), m_float_pointradius);
			pNewPt->SetID(ID);
			
			m_list_points.AddTail(pNewPt);
		}
		
		//读取段的数据
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(k = 0; k<Num; k++)
		{
			CEditerBezierSegment *pNewSeg = new CEditerBezierSegment();
			pNewSeg->Init(m_ptr_a3ddevice, temp, temp, temp, temp, m_float_segradius, m_float_anchorsize);
			pNewSeg->SetSegmentColor(m_dwColor);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor1(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor2(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHead(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTail(temp);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHeadID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTailID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetLink(ID);
			
			m_list_segments.AddTail(pNewSeg);
		}		
	
	}if(dwVersion>0xf && dwVersion <= 0x22)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_float_pointradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_anchorsize,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_segradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsBezier,sizeof(bool)))
			return false;
		//读点的数据
		int Num;
		int ID;
		A3DVECTOR3 temp;
		
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(int k=0; k<Num; k++)
		{
			if(!ar.Read(&ID,sizeof(int)))
				return false;
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
				return false;
			
			CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
			pNewPt->Init(m_ptr_a3ddevice, A3DVECTOR3(temp.x, temp.y, temp.z), A3DCOLORRGBA(255,0,0,255), m_float_pointradius);
			pNewPt->SetID(ID);
			
			m_list_points.AddTail(pNewPt);
		}
		
		//读取段的数据
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(k = 0; k<Num; k++)
		{
			CEditerBezierSegment *pNewSeg = new CEditerBezierSegment();
			pNewSeg->Init(m_ptr_a3ddevice, temp, temp, temp, temp, m_float_segradius, m_float_anchorsize);
			pNewSeg->SetSegmentColor(m_dwColor);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor1(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor2(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHead(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTail(temp);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHeadID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTailID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetLink(ID);
			
			m_list_segments.AddTail(pNewSeg);
		}		
	
	}if(dwVersion>0x23 && dwVersion <= 0x39)
	{
		m_strName = AString(ar.ReadString());
		m_strActionName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_nGlobalID,sizeof(int)))
			return false;
		if(!ar.Read(&m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_float_pointradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_anchorsize,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_segradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsBezier,sizeof(bool)))
			return false;
		//读点的数据
		int Num;
		int ID;
		A3DVECTOR3 temp;
		
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(int k=0; k<Num; k++)
		{
			if(!ar.Read(&ID,sizeof(int)))
				return false;
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
				return false;
			
			CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
			pNewPt->Init(m_ptr_a3ddevice, A3DVECTOR3(temp.x, temp.y, temp.z), A3DCOLORRGBA(255,0,0,255), m_float_pointradius);
			pNewPt->SetID(ID);
			
			m_list_points.AddTail(pNewPt);
		}
		
		//读取段的数据
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(k = 0; k<Num; k++)
		{
			CEditerBezierSegment *pNewSeg = new CEditerBezierSegment();
			pNewSeg->Init(m_ptr_a3ddevice, temp, temp, temp, temp, m_float_segradius, m_float_anchorsize);
			pNewSeg->SetSegmentColor(m_dwColor);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor1(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor2(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHead(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTail(temp);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHeadID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTailID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetLink(ID);
			
			m_list_segments.AddTail(pNewSeg);
		}		
	}if(dwVersion>0x39)
	{
		m_strName = AString(ar.ReadString());
		m_strActionName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_nGlobalID,sizeof(int)))
			return false;
		if(!ar.Read(&m_nNextBezierID,sizeof(int)))
			return false;
		if(!ar.Read(&m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_float_pointradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_anchorsize,sizeof(float)))
			return false;
		if(!ar.Read(&m_float_segradius,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsBezier,sizeof(bool)))
			return false;
		//读点的数据
		int Num;
		int ID;
		A3DVECTOR3 temp;
		
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(int k=0; k<Num; k++)
		{
			if(!ar.Read(&ID,sizeof(int)))
				return false;
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
				return false;
			
			CEditerBezierPoint *pNewPt = new CEditerBezierPoint();
			pNewPt->Init(m_ptr_a3ddevice, A3DVECTOR3(temp.x, temp.y, temp.z), A3DCOLORRGBA(255,0,0,255), m_float_pointradius);
			pNewPt->SetID(ID);
			
			m_list_points.AddTail(pNewPt);
		}
		
		//读取段的数据
		if(!ar.Read(&Num,sizeof(int)))
			return false;
		for(k = 0; k<Num; k++)
		{
			CEditerBezierSegment *pNewSeg = new CEditerBezierSegment();
			pNewSeg->Init(m_ptr_a3ddevice, temp, temp, temp, temp, m_float_segradius, m_float_anchorsize);
			pNewSeg->SetSegmentColor(m_dwColor);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor1(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetAnchor2(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHead(temp);
			
			if(!ar.Read(&temp,sizeof(A3DVECTOR3)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTail(temp);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetHeadID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetTailID(ID);
			
			if(!ar.Read(&ID,sizeof(int)))
			{
				delete pNewSeg;
				return false;
			}
			pNewSeg->SetLink(ID);
			
			m_list_segments.AddTail(pNewSeg);
		}		
	}
	ReCalculateLength();
	BuildBoundingBox();
	return true; 
}

void CEditerBezier::ConvertToObstruct()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
	
	
	
	POSITION pos = m_list_points.GetHeadPosition();
	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		pSeg->SetStartCtrl(pSeg->GetHead());
		pSeg->SetAnchor1(pSeg->GetHead());
		pSeg->SetEndCtrl(pSeg->GetTail());
		pSeg->SetAnchor2(pSeg->GetTail());
	}

	AUX_GetMainFrame()->GetToolTabWnd()->UpdateSceneObjectList();
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllSceneObjectButton();
	AUX_GetMainFrame()->GetToolTabWnd()->RiseUpAllTerrainOperationButton();
	AUX_GetMainFrame()->SetOtherOperation(false);
}

void CEditerBezier::Convert()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint * pPt = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		if(m_bIsBezier)
		{
			m_float_pointradius = 0.3f;
			pPt->SetRadius(0.3f);
		}else
		{
			pPt->SetRadius(1.0f);
			m_float_pointradius = 1.0f;
		}
	}
	
	if(!m_bIsBezier)
	{
		pos = m_list_segments.GetHeadPosition();
		while(pos)
		{
			CEditerBezierSegment * pSeg = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
			pSeg->SetStartCtrl(pSeg->GetHead());
			pSeg->SetAnchor1(pSeg->GetHead());
			pSeg->SetEndCtrl(pSeg->GetTail());
			pSeg->SetAnchor2(pSeg->GetTail());
		}
	}
}

//Save data
bool CEditerBezier::Save(CELArchive& ar,DWORD dwVersion) 
{
	if(dwVersion>1)
	{//2.0版本开放
		ar.WriteString(m_strName);
		ar.WriteString(m_strActionName);
		if(!ar.Write(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Write(&m_nGlobalID,sizeof(int)))
			return false;
		if(!ar.Write(&m_nNextBezierID,sizeof(int)))
			return false;
		if(!ar.Write(&m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_float_pointradius,sizeof(float)))
			return false;
		if(!ar.Write(&m_float_anchorsize,sizeof(float)))
			return false;
		if(!ar.Write(&m_float_segradius,sizeof(float)))
			return false;
		if(!ar.Write(&m_bIsBezier,sizeof(bool)))
			return false;
		
		//写点的数据
		int Num;
		int ID;
		A3DVECTOR3 temp;
		Num = m_list_points.GetCount();
		if(!ar.Write(&Num,sizeof(int)))
			return false;
		POSITION pos = m_list_points.GetHeadPosition();
		while(pos)
		{
			CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
			ID = ptemp->GetID();
			if(!ar.Write(&ID,sizeof(int)))
				return false;
			temp = ptemp->GetPos();
			if(!ar.Write(&temp,sizeof(A3DVECTOR3)))
				return false;
		}
		//写段的数据
		Num = m_list_segments.GetCount();
		if(!ar.Write(&Num,sizeof(int)))
			return false;
		pos = m_list_segments.GetHeadPosition();
		while(pos)
		{
			CEditerBezierSegment *ptemp1 = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
			temp = ptemp1->GetAnchor1();
			if(!ar.Write(&temp,sizeof(A3DVECTOR3)))
				return false;
			temp = ptemp1->GetAnchor2();
			if(!ar.Write(&temp,sizeof(A3DVECTOR3)))
				return false;
			temp = ptemp1->GetHead();
			if(!ar.Write(&temp,sizeof(A3DVECTOR3)))
				return false;
			temp = ptemp1->GetTail();
			if(!ar.Write(&temp,sizeof(A3DVECTOR3)))
				return false;
			ID = ptemp1->GetHeadID();
			if(!ar.Write(&ID,sizeof(int)))
				return false;
			ID = ptemp1->GetTailID();
			if(!ar.Write(&ID,sizeof(int)))
				return false;
			ID = ptemp1->GetLink();
			if(!ar.Write(&ID,sizeof(int)))
				return false;
		}
	}
	return true; 
}

int CEditerBezier::GetPointIdx(DWORD id)
{
	int i = 0;
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		if(ptemp->GetID()==id) return i;
		i++;
	}
	ASSERT(false);
	return 0;
}

void CEditerBezier::BuildBoundingBox()
{
	A3DVECTOR3 vMin(99999.0f,99999.0f,99999.0f),vMax(-99999.0f,-99999.0f,-99999.0f),vPos;
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		vPos = ptemp->GetPos();
		
		if(vMin.x >= vPos.x) vMin.x = vPos.x;
		if(vMin.y >= vPos.y) vMin.y = vPos.y;
		if(vMin.z >= vPos.z) vMin.z = vPos.z;

		if(vMax.x <= vPos.x) vMax.x = vPos.x;
		if(vMax.y <= vPos.y) vMax.y = vPos.y;
		if(vMax.z <= vPos.z) vMax.z = vPos.z;
	}
	m_vMins = vMin;
	m_vMaxs = vMax;
}

//判断区域是否跟该区域相交
bool CEditerBezier::IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax)
{

	if(m_vMins.x>vMax.x) return false;
	if(m_vMins.z>vMax.z) return false;
	if(m_vMaxs.x<vMin.x) return false;
	if(m_vMaxs.z<vMin.z) return false;
	return true;
}

/* Bezier export data struct
 * DWORD      verion	
 * int		  object	id
 * ------------------------
 * int		  point		num
 * A3DVECTOR3 point		pos
 * A3DVECTOR3 point		dir
 * -------------------------
 * int        segment	num
 * A3DVECTOR3 segment	AnchorHead
 * A3DVECTOR3 segment   AnchorTail
 * int        segment   HeadPointIndex
 * int        segment   TailPointIndex
 * float      segment   Length
 */

bool CEditerBezier::ExportBezier(AFile*pFile ,const A3DVECTOR3& vOffset)
{
	CalculateDirection();
	DWORD version = BEZIER_FILE_VERSION;
	DWORD dwRr;
	if(!pFile->Write(&version,sizeof(DWORD),&dwRr)) return false;
	if(!pFile->Write(&m_nObjectID,sizeof(int),&dwRr)) return false;
	if(!pFile->Write(&m_nGlobalID,sizeof(int),&dwRr)) return false;
	if(!pFile->Write(&m_nNextBezierID,sizeof(int),&dwRr)) return false;
	pFile->WriteString(m_strActionName);//new 动作名字，为空的话就是默认的动作

	//写点的数据
	int Num;
	int ID;
	A3DVECTOR3 temp;
	Num = m_list_points.GetCount();
	if(!pFile->Write(&Num,sizeof(int),&dwRr)) return false;
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		
		temp = ptemp->GetPos() + vOffset;
		if(!pFile->Write(&temp,sizeof(A3DVECTOR3),&dwRr))
			return false;
		temp = ptemp->GetDirection();
		if(!pFile->Write(&temp,sizeof(A3DVECTOR3),&dwRr))
			return false;
	}
	//写段的数据
	Num = m_list_segments.GetCount();
	if(!pFile->Write(&Num,sizeof(int),&dwRr))
		return false;
	if(Num < 1) 
	{
		CString msg;
		msg.Format("Bezier segment is 0, cannot export bezier %s!",m_strName);
		AfxMessageBox(msg);
		return false;
	}
	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp1 = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		ptemp1->ReCalculateLength();
		temp = ptemp1->GetAnchor1() + vOffset;
		if(!pFile->Write(&temp,sizeof(A3DVECTOR3),&dwRr))
			return false;
		temp = ptemp1->GetAnchor2() + vOffset;
		if(!pFile->Write(&temp,sizeof(A3DVECTOR3),&dwRr))
			return false;
		
		// Tanslate id to index
		ID = ptemp1->GetHeadID();
		int index = GetPointIdx(ID);
		if(!pFile->Write(&index,sizeof(int),&dwRr))
			return false;
		ID = ptemp1->GetTailID();
		index = GetPointIdx(ID);
		if(!pFile->Write(&index,sizeof(int),&dwRr))
			return false;

		float length = ptemp1->GetLenght();
		if(!pFile->Write(&length,sizeof(float),&dwRr))
			return false;
	}
	return true;
}

bool CEditerBezier::ExportBezier(FILE *pFile, const A3DVECTOR3& vOffset)
{
	CalculateDirection();
	DWORD version = BEZIER_FILE_VERSION;

	if(0==fwrite(&version,sizeof(DWORD),1,pFile)) return false;
	if(0==fwrite(&m_nObjectID,sizeof(int),1,pFile)) return false;
	if(0==fwrite(&m_nGlobalID,sizeof(int),1,pFile)) return false;
	if(0==fwrite(&m_nNextBezierID,sizeof(int),1,pFile)) return false;
	
	//new 动作名字，为空的话就是默认的动作
	int iLen = m_strActionName.GetLength();
	fwrite(&iLen, 1, sizeof (int), pFile);
	if (iLen)
		fwrite((const char*)m_strActionName, 1, iLen, pFile);

	//写点的数据
	int Num;
	int ID;
	A3DVECTOR3 temp;
	Num = m_list_points.GetCount();
	if(0==fwrite(&Num,sizeof(int),1,pFile)) return false;
	//if(!ar.Write(&Num,sizeof(int))) return false;
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		
		temp = ptemp->GetPos() + vOffset;
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		//if(!ar.Write(&temp,sizeof(A3DVECTOR3))) return false;
		temp = ptemp->GetDirection();
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		//if(!ar.Write(&temp,sizeof(A3DVECTOR3))) return false;
	}
	//写段的数据
	Num = m_list_segments.GetCount();
	if(0==fwrite(&Num,sizeof(int),1,pFile)) return false;

	if(Num < 1) 
	{
		CString msg;
		msg.Format("Bezier segment is 0, cannot export bezier %s!",m_strName);
		AfxMessageBox(msg);
		return false;
	}
	//if(!ar.Write(&Num,sizeof(int)))	return false;
	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp1 = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		ptemp1->ReCalculateLength();
		temp = ptemp1->GetAnchor1() + vOffset;
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		//if(!ar.Write(&temp,sizeof(A3DVECTOR3))) return false;
		temp = ptemp1->GetAnchor2() + vOffset;
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		//if(!ar.Write(&temp,sizeof(A3DVECTOR3))) return false;
		
		// Tanslate id to index
		ID = ptemp1->GetHeadID();
		int index = GetPointIdx(ID);
		if(0==fwrite(&index,sizeof(int),1,pFile)) return false;
		//if(!ar.Write(&index,sizeof(int))) return false;
		ID = ptemp1->GetTailID();
		index = GetPointIdx(ID);
		if(0==fwrite(&index,sizeof(int),1,pFile)) return false;
		//if(!ar.Write(&index,sizeof(int))) return false;

		float length = ptemp1->GetLenght();
		if(0==fwrite(&length,sizeof(float),1,pFile)) return false;
		//if(!ar.Write(&length,sizeof(float))) return false;
	}
	return true;	
}

//	Export beizer data (server version)
bool CEditerBezier::ExportSevBezier(FILE *pFile, const A3DVECTOR3& vOffset)
{
	CalculateDirection();
	DWORD version = BEZIER_FILE_VERSION;

	if(0==fwrite(&version,sizeof(DWORD),1,pFile)) return false;
	if(0==fwrite(&m_nObjectID,sizeof(int),1,pFile)) return false;
	if(0==fwrite(&m_nGlobalID,sizeof(int),1,pFile)) return false;
	if(0==fwrite(&m_nNextBezierID,sizeof(int),1,pFile)) return false;
	
	//	写点的数据
	int Num;
	int ID;
	A3DVECTOR3 temp;
	Num = m_list_points.GetCount();
	if(0==fwrite(&Num,sizeof(int),1,pFile))
		return false;

	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		
		temp = ptemp->GetPos() + vOffset;
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		//if(!ar.Write(&temp,sizeof(A3DVECTOR3))) return false;
		temp = ptemp->GetDirection();
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		//if(!ar.Write(&temp,sizeof(A3DVECTOR3))) return false;
	}

	//	写段的数据
	Num = m_list_segments.GetCount();
	if(0==fwrite(&Num,sizeof(int),1,pFile))
		return false;
	if(Num < 1) 
	{
		CString msg;
		msg.Format("Bezier segment is 0, cannot export bezier %s!",m_strName);
		AfxMessageBox(msg);
		return false;
	}
	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp1 = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		ptemp1->ReCalculateLength();
		temp = ptemp1->GetAnchor1() + vOffset;
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		temp = ptemp1->GetAnchor2() + vOffset;
		if(0==fwrite(&temp,sizeof(A3DVECTOR3),1,pFile)) return false;
		
		// Tanslate id to index
		ID = ptemp1->GetHeadID();
		int index = GetPointIdx(ID);
		if(0==fwrite(&index,sizeof(int),1,pFile)) return false;
		ID = ptemp1->GetTailID();
		index = GetPointIdx(ID);
		if(0==fwrite(&index,sizeof(int),1,pFile)) return false;

		float length = ptemp1->GetLenght();
		if(0==fwrite(&length,sizeof(float),1,pFile)) return false;
	}

	return true;	
}

void CEditerBezier::GeneralECBezier(CECBezier *pECBezier)
{
	if(pECBezier== NULL) return; 
	CalculateDirection();
	pECBezier->SetObjectID(m_nObjectID);
	pECBezier->Assign(m_list_points.GetCount(),m_list_segments.GetCount());
	//写点的数据
	int i = 0;
	POSITION pos = m_list_points.GetHeadPosition();
	while(pos)
	{
		CEditerBezierPoint *ptemp = (CEditerBezierPoint *)m_list_points.GetNext(pos);
		
		CECBezierPoint pt;
		pt.SetDir(ptemp->GetDirection());
		pt.SetPos(ptemp->GetPos());
		pECBezier->AddBezierPoint((CECBezierPoint*)&pt,i++);
	}
	i = 0;
	pos = m_list_segments.GetHeadPosition();
	while(pos)
	{
		CEditerBezierSegment *ptemp1 = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		ptemp1->ReCalculateLength();
		CECBezierSeg seg;
		seg.SetAnchorHead( ptemp1->GetAnchor1());
		seg.SetAnchorTail( ptemp1->GetAnchor2());
		seg.SetHeadPoint(GetPointIdx(ptemp1->GetHeadID()));
		seg.SetTailPoint(GetPointIdx(ptemp1->GetTailID()));
		seg.SetSegLenght(ptemp1->GetLenght());
		pECBezier->AddBezierSeg((CECBezierSeg*)&seg,i++);
	}
}

bool CEditerBezier::IntersectWithSelf()
{
	CEditerBezierSegment *pNext = NULL;
	CEditerBezierSegment *pLast = NULL;
	if(m_list_segments.GetCount()==0) return false;
	POSITION pos1 = m_list_segments.GetHeadPosition();
	while(pos1)
	{
		CEditerBezierSegment *ptemp1 = (CEditerBezierSegment *)m_list_segments.GetNext(pos1);
		POSITION pos = pos1;
		if(pos) pNext = (CEditerBezierSegment *)m_list_segments.GetNext(pos);
		
		A3DVECTOR3 head1 = ptemp1->GetHead();
		A3DVECTOR3 tail1 = ptemp1->GetTail();
		POSITION pos2 = m_list_segments.GetHeadPosition();
		while(pos2)
		{
			CEditerBezierSegment *ptemp2 = (CEditerBezierSegment *)m_list_segments.GetNext(pos2);
			if(ptemp2!=ptemp1)
			{
				A3DVECTOR3 head2 = ptemp2->GetHead();
				A3DVECTOR3 tail2 = ptemp2->GetTail();

				float px, pz;
				int br = GEO_2DIntersectSS(px,pz,head1.x,head1.z,tail1.x,tail1.z,head2.x,head2.z,tail2.x,tail2.z);
				if(br == 1)
				{
					//CString strEr;
					//strEr.Format("%4.2f %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f ",head1.x,head1.z,tail1.x,tail1.z,head2.x,head2.z,tail2.x,tail2.z);
					ptemp1->SetSegmentColor(A3DCOLORRGB(0,0,0));
					ptemp2->SetSegmentColor(A3DCOLORRGB(255,255,255));
					//AfxMessageBox(strEr);
					return true;
				}
			}
		}
		pLast = ptemp1;
	}

	CEditerBezierSegment *pHead = (CEditerBezierSegment *)m_list_segments.GetHead();
	CEditerBezierSegment *pTail= (CEditerBezierSegment *)m_list_segments.GetTail();
	if(pHead==NULL || pTail==NULL) return false;
	A3DVECTOR3 head = GetHeadPos();
	A3DVECTOR3 tail = GetTailPos();
	POSITION pos3 = m_list_segments.GetHeadPosition();
	while(pos3)
	{
		CEditerBezierSegment *ptemp3 = (CEditerBezierSegment *)m_list_segments.GetNext(pos3);
		//if(ptemp3!=pHead && ptemp3!=pTail)
		{
			A3DVECTOR3 head3 = ptemp3->GetTail();
			A3DVECTOR3 tail3 = ptemp3->GetHead();
			
			float px, pz;
			int br = GEO_2DIntersectSS(px,pz,head.x,head.z,tail.x,tail.z,head3.x,head3.z,tail3.x,tail3.z);
			if(br == 1)
			{
				ptemp3->SetSegmentColor(A3DCOLORRGB(0,0,0));
				//AfxMessageBox("2");
				return true;
			}
		}
	}
	return false;
}



