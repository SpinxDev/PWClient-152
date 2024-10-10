/********************************************************************
	created:	2006/12/18
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#include <A3DGDI.h>
#include <A3DMacros.h>
#include <AString.h>
#include <AFile.h>


#include "Island.h"
#include "Graph.h"
#include "MoveAgent.h"
#include "PfConstant.h"
#include "bitimage.h"

namespace AutoMove
{

#define  LOCAL_MAGNET_RANGE  10.0f
#define  LOCAL_INVALID_NUM       -9999

#define	   ISL_FILE_MAGIC      (DWORD)(('i'<<24)| ('s'<<16)|('l'<<8)|('f'))
#define    ISL_FILE_VERSION     1

/**
 * \brief tesf if segment A is intersected with segment B
 * \param[in]
 * \param[out]
 * \return
 * \note :  
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 20/12/2006
 * \see 
 */
static bool Seg2IntersectSeg2(const A3DPOINT2& ptA1, const A3DPOINT2& ptA2, 
							  const A3DPOINT2& ptB1, const A3DPOINT2& ptB2)
{
	APointF     DA, DB;
	float len, lenA, lenB;
	DA.x = (float)(ptA2.x - ptA1.x);
	DA.y = (float)(ptA2.y - ptA1.y);
	//normalize
	
	len = sqrtf(DA.x*DA.x + DA.y*DA.y);
	if (len < ZERO_TOLERANCE)
	{
		assert(0);
		return true;
	}
	
	DA.x /= len;
	DA.y /= len;
	lenA = len;

	DB.x = (float)(ptB2.x - ptB1.x);
	DB.y = (float)(ptB2.y - ptB1.y);
	
	len = sqrtf(DB.x*DB.x + DB.y*DB.y);
	if (len < ZERO_TOLERANCE)
	{
		assert(0);
		return true;
	}
	
	DB.x /= len;
	DB.y /= len;
	lenB = len;
	
	APointF  Diff;
	Diff.x = ptB1.x - (float)ptA1.x;
	Diff.y = ptB1.y - (float)ptA1.y;


	float DADotPerpDB = DA.x*DB.y - DA.y*DB.x;
	if (fabs(DADotPerpDB) > ZERO_TOLERANCE)
	{
		//lines intersect at single point
		//test the intersection
		float DiffDotPerpDA= Diff.x * DA.y - Diff.y * DA.x;
		float DiffDotPerpDB= Diff.x * DB.y - Diff.y * DB.x;
		float SA = DiffDotPerpDB / DADotPerpDB;
		float SB = DiffDotPerpDA / DADotPerpDB;

		if (SA >= 0.0f && SA <= lenA 
			&& SB >= 0.0f && SB <= lenB )
		{
			return true;
		}
	}

	//lines are parallel
	len = sqrtf(Diff.x * Diff.x + Diff.y * Diff.y);
	if (len < ZERO_TOLERANCE)
	{
		//assert(0);
		return true;
	}
	APointF  DiffN(Diff);
	DiffN.x /= len;
	DiffN.y /= len;

	float DiffNDotPerpDB = DiffN.x * DB.y - DiffN.y * DB.x;
	if (fabs(DiffNDotPerpDB) <= ZERO_TOLERANCE)
	{
		// lines are colinear
		return true;
	}

	// lines are parallel, but distinct 
	return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIsland::CIsland(int id)
{
	m_Border = new CGGraph;
	m_Num = id;
	m_Parent = -1;  
}

CIsland::~CIsland()
{
	Release();
}

void	CIsland::Release()
{
	if (m_Border)
	{
		delete m_Border;
		m_Border = NULL;
	}

	m_Children.clear();
	m_Passes.clear();
}

void CIsland::AddBorderLine(const A3DPOINT2& pt1, const A3DPOINT2& pt2)
{
	CGNode * n1 = NULL;
	CGNode * n2 = NULL;

	vector<CGNode *>  * pAllNodes = m_Border->GetAllNodes();
	vector<CGNode *>::iterator it;
	for (it = pAllNodes->begin(); it != pAllNodes->end(); ++it)
	{
		A3DPOINT2 pt = (*it)->GetMapCoord();
		if (GetManhDist(pt.x, pt.y, pt1.x, pt1.y) < LOCAL_MAGNET_RANGE)
		{
			n1 = *it;
		}
		if (GetManhDist(pt.x, pt.y, pt2.x, pt2.y) < LOCAL_MAGNET_RANGE)
		{
			n2 = *it;
		}
	}
	
	if (!n1)
	{
		n1 = new CGNode;
		m_Border->AddNode(n1);
		n1->SetMapCoord(pt1);
	}

	if (!n2)
	{
		n2 = new CGNode;
		m_Border->AddNode(n2);
		n2->SetMapCoord(pt2);
	}

	//not check for dup edge, put it off when add island
	m_Border->AddEdge(new  CGEdge(n1->GetNum(), n2->GetNum(), 0.0));
	
}

void CIsland::DelBorderPoint(const A3DPOINT2& pt)
{
	vector<CGNode *>  * pAllNodes = m_Border->GetAllNodes();
	vector<CGNode *>::iterator it;
	for (it = pAllNodes->begin(); it != pAllNodes->end(); ++it)
	{
		A3DPOINT2 pt1 = (*it)->GetMapCoord();
		if (GetManhDist(pt.x, pt.y, pt1.x, pt1.y) < LOCAL_MAGNET_RANGE)
		{
			int dummy;
			m_Border->RemoveNode(*it, dummy);
			return ;
		}
	}
	
}

/**
 * \brief  test if the point is in the polygon
 * \param[in]
 * \param[out]
 * \return
 * \note : counts the number of times a ray starting 
		   from a point P crosses a polygon boundary 
		   edge separating it's inside and outside.
 * \warning
 * \todo   winding number maybe better? seems no difference to our case.
 * \author kuiwu 
 * \date 18/12/2006
 * \see 
 */
bool CIsland::IsPtIn(const A3DPOINT2& pt)
{
	int    cn = 0;    // the crossing number counter
	vector<CGEdge *> * pAllEdges = m_Border->GetAllEdges();
	vector<CGEdge *>::iterator it;
	for (it = pAllEdges->begin(); it != pAllEdges->end(); ++it)
	{
		A3DPOINT2 pt1 = m_Border->GetNode((*it)->GetFrom())->GetMapCoord();
		A3DPOINT2 pt2 = m_Border->GetNode((*it)->GetTo())->GetMapCoord();
		if ((pt1.y <= pt.y && pt2.y > pt.y )  //upward crossing
			|| (pt1.y > pt.y && pt2.y <= pt.y))  //downward crossing
		{
			// compute the actual edge-ray intersect x-coordinate
			float t =  (pt.y - pt1.y) /(float)(pt2.y - pt1.y);
			if (pt.x < pt1.x + t*(pt2.x - pt1.x))
			{
				++cn;   // a valid crossing of pt.y right of pt.x
			}
		}
	}

	 // 0 if even (out), and 1 if odd (in)
	return ((cn&1)== 1);

}

#ifdef _AUTOMOVE_EDITOR
void CIsland::Render(A3DCOLOR color)
{
	A3DPOINT2 ptScr1, ptScr2;

	vector<CGNode*> * pAllNodes = m_Border->GetAllNodes();
	vector<CGNode*>::iterator itn;
	for (itn = pAllNodes->begin(); itn != pAllNodes->end(); ++itn)
	{
		A3DPOINT2 pt = (*itn)->GetMapCoord();
		TransMap2Scr(pt, ptScr1);
		g_pA3DGDI->Draw2DPoint(ptScr1, color, 3.0f);
	}

	vector<CGEdge *> * pAllEdges = m_Border->GetAllEdges();
	vector<CGEdge *>::iterator ite;
	
	for (ite = pAllEdges->begin(); ite != pAllEdges->end(); ++ite)
	{
		A3DPOINT2 pt1 = m_Border->GetNode((*ite)->GetFrom())->GetMapCoord();
		A3DPOINT2 pt2 = m_Border->GetNode((*ite)->GetTo())->GetMapCoord();
		TransMap2Scr(pt1, ptScr1);
		TransMap2Scr(pt2, ptScr2);
		g_pA3DGDI->Draw2DLine(ptScr1, ptScr2, color);
	}
	
}
#endif

//do a simple test
bool CIsland::IsValid(AString& msg)
{
	vector<CGNode *> * pAllNodes = m_Border->GetAllNodes();
	if (pAllNodes->empty())
	{
		msg.Format("the island is empty");
		return false;
	}
	vector<CGNode*>::iterator it;
	for (it = pAllNodes->begin(); it != pAllNodes->end(); ++it)
	{
		if ((*it)->GetAllEdges()->size() != 2)
		{
			msg.Format("(%d,%d) has %d edges", (*it)->GetMapCoord().x, (*it)->GetMapCoord().y, (*it)->GetAllEdges()->size());
			return false;
		}
		 
	}
	return true;
}

void CIsland::RemoveChild(int id)
{
	vector<int>::iterator it;
	for (it = m_Children.begin(); it != m_Children.end(); ++it)
	{
		if (*it == id)
		{
			break;
		}
	}
	
	assert(it != m_Children.end());
	m_Children.erase(it);
}

//return the relationship that the island B is relative to island A
int CIsland::Relation(CIsland * pIslA, CIsland * pIslB)
{

	CGGraph * pBorderA = pIslA->GetBorder();
	vector<CGNode *>* pNodesA = pBorderA->GetAllNodes();
	vector<CGEdge *>* pEdgesA = pBorderA->GetAllEdges();

	CGGraph * pBorderB = pIslB->GetBorder();
	vector<CGNode *>* pNodesB = pBorderB->GetAllNodes();
	vector<CGEdge *>* pEdgesB = pBorderB->GetAllEdges();


	if (pNodesB->empty() || pNodesA->empty())
	{
		return ISLAND_RELATION_ISOLATE;
	}

	//test intersect
	vector<CGEdge*>::iterator iteA, iteB;
	for (iteA = pEdgesA->begin(); iteA != pEdgesA->end(); ++iteA)
	{
		A3DPOINT2 ptA1, ptA2;
		ptA1 = pBorderA->GetNode((*iteA)->GetFrom())->GetMapCoord();
		ptA2 = pBorderA->GetNode((*iteA)->GetTo())->GetMapCoord();
		for (iteB = pEdgesB->begin(); iteB != pEdgesB->end(); ++iteB)
		{
			A3DPOINT2 ptB1, ptB2;
			ptB1 = pBorderB->GetNode((*iteB)->GetFrom())->GetMapCoord();
			ptB2 = pBorderB->GetNode((*iteB)->GetTo())->GetMapCoord();

			if (Seg2IntersectSeg2(ptA1, ptA2, ptB1, ptB2))
			{
				return ISLAND_RELATION_INTERSECT;
			}
		}
	}

	//isolate or contain
	vector<CGNode*>::iterator itn;

	// test  B in A
	unsigned	int nBInA = 0;
	for (itn = pNodesB->begin(); itn != pNodesB->end(); ++itn)
	{
			A3DPOINT2 pt = (*itn)->GetMapCoord();
			if (pIslA->IsPtIn(pt))
			{
				nBInA++;
			}
	}

	if (nBInA == pNodesB->size())
	{
		//all in,  B is contained by A(child)
		return ISLAND_RELATION_CHILD;		
	}

	//test A in B
	unsigned int nAInB = 0;
	for (itn = pNodesA->begin(); itn != pNodesA->end(); ++itn)
	{
			A3DPOINT2 pt = (*itn)->GetMapCoord();
			if (pIslB->IsPtIn(pt))
			{
				nAInB++;
			}
	}

	if (nAInB == pNodesA->size())
	{
		//all in, B contain A
		return ISLAND_RELATION_CONTAIN;
	}

	if (nAInB == 0 && nBInA == 0)
	{
		return ISLAND_RELATION_ISOLATE;
	}
	else
	{
		assert(0);
	}
	

	return 0;
}

void CIsland::Save(AFile& fileimg)
{

	unsigned long  writelen;

	
	fileimg.Write(&m_Num, sizeof(int), &writelen);  //id
	fileimg.Write(&m_Parent, sizeof(int), &writelen); //parent
	unsigned int count;
	unsigned int i;
	//children
	count = m_Children.size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; ++i)
	{
		fileimg.Write(&m_Children[i], sizeof(int), &writelen);
	}

	//border
	vector<CGNode *> * nodes = m_Border->GetAllNodes();
	count =nodes->size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; ++i)
	{
		CGNode * n = nodes->at(i);
		A3DPOINT2 pt(n->GetMapCoord());
		fileimg.Write(&pt, sizeof(A3DPOINT2), &writelen);
	}
	vector<CGEdge*> * edges = m_Border->GetAllEdges();
	count = edges->size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; ++i)
	{
		CGEdge * e = edges->at(i);
		unsigned int which;
		which = e->GetFrom();
		fileimg.Write(&which, sizeof(unsigned int), &writelen);
		which = e->GetTo();
		fileimg.Write(&which, sizeof(unsigned int), &writelen);
	}

	//passes
	count = m_Passes.size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; ++i)
	{
		CIsland::Pass * p = &m_Passes[i];
		fileimg.Write(p, sizeof(CIsland::Pass), &writelen);
	}
	
}



void CIsland::Load(AFile& fileimg)
{
	Release();

	unsigned long readlen;
	
	fileimg.Read(&m_Num, sizeof(int), &readlen);  //id
	fileimg.Read(&m_Parent, sizeof(int), &readlen); //parent

	unsigned int count, i;
	//children
	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	for (i = 0; i < count; ++i)
	{
		int child;
		fileimg.Read(&child, sizeof(int), &readlen);
		m_Children.push_back(child);
	}
	
	//border
	m_Border = new CGGraph;
	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	for (i = 0; i < count; ++i)
	{
		A3DPOINT2 pt;
		fileimg.Read(&pt, sizeof(A3DPOINT2), &readlen);
		CGNode * n  = new CGNode;
		n->SetMapCoord(pt);
		m_Border->AddNode(n);
	}
	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	for (i = 0; i <count; ++i)
	{
		int to, from;
		fileimg.Read(&from, sizeof(int), &readlen);
		fileimg.Read(&to, sizeof(int), &readlen);
		CGEdge * e = new CGEdge(from, to, 1.0);
		m_Border->AddEdge(e);
	}

	//passes
	fileimg.Read(&count, sizeof(unsigned int), &readlen );
	for (i = 0; i < count; ++i)
	{
		m_Passes.push_back(CIsland::Pass());
		fileimg.Read(&m_Passes.back(), sizeof(CIsland::Pass), &readlen);
	}


}


//////////////////////////////////////////////////////////////////////////
//CIslandList
//////////////////////////////////////////////////////////////////////////
CIslandList::CIslandList(CBitImage * pRMap)
{
	m_CurIsland = NULL;
	m_CurPass = -1;
	m_pRMap = pRMap;
}

CIslandList::~CIslandList()
{
	Release();
}

void CIslandList::Release()
{
	
	vector<CIsland *>::iterator it;
	for (it = m_Islands.begin(); it != m_Islands.end(); ++it)
	{
		delete (*it);
	}
	m_Islands.clear();

	m_MainlandPasses.clear();
	m_CurIsland  = NULL;
	m_CurPass = -1;
}

CIsland * CIslandList::NewIsland()
{
	CIsland * pIsl = new CIsland(m_Islands.size());
	m_Islands.push_back(pIsl);
	return pIsl;
}

CIsland * CIslandList::SelectIsland(const A3DPOINT2& pt)
{

	vector<CIsland *>::iterator it;
	for (it = m_Islands.begin(); it != m_Islands.end(); ++it)
	{
		if (!(*it)->IsPtIn(pt))
		{
			continue;
		}
		
		unsigned int i;
		for (i = 0; i < (*it)->m_Children.size(); ++i )
		{
			CIsland * pChild = m_Islands[(*it)->m_Children[i]];
			if (pChild->IsPtIn(pt))
			{
				break;
			}
		}

		if (i == (*it)->m_Children.size())
		{
			return *it;
		}
	}

	return NULL;

}

#ifdef _AUTOMOVE_EDITOR
void CIslandList::Render()
{
	
	int mapW, mapL;
	m_pRMap->GetImageSize(mapW, mapL);

	CIsland::Pass * pPass = GetCurPass();
	CIsland * pDest = NULL;
	if (pPass && pPass->dest >= 0)
	{
		pDest = m_Islands[pPass->dest];
	}

	A3DCOLOR  cNormal = A3DCOLORRGB(255,255, 255);
	A3DCOLOR  cSel    = A3DCOLORRGB(255, 128, 0);
	A3DCOLOR  cDest   = A3DCOLORRGB(0,0,128);
	vector<CIsland *>::iterator it;
	for (it = m_Islands.begin(); it != m_Islands.end(); ++it)
	{
		if (*it == m_CurIsland)
		{
			(*it)->Render(cSel);
		}
		else if (*it == pDest)
		{
			(*it)->Render(cDest);
		}
		else
		{
			(*it)->Render(cNormal);
			
		}
	}

	if (pPass && m_pRMap->GetPixel(pPass->through.x, pPass->through.y))
	{
		A3DPOINT2 ptScr;
		TransMap2Scr(pPass->through, ptScr);
		g_pA3DGDI->Draw2DPoint(ptScr, cSel, 3.0f);
	}

	A3DPOINT2 ptMap0, ptMap1, ptMap2, ptMap3;
	ptMap0.x = 0;
	ptMap0.y = 0;
	ptMap1.x = 0;
	ptMap1.y = mapL;
	ptMap2.x = mapW; 
	ptMap2.y = mapL;
	ptMap3.x = mapW;
	ptMap3.y =  0;
	A3DPOINT2 ptScr0, ptScr1, ptScr2, ptScr3;
	TransMap2Scr(ptMap1, ptScr1);
	TransMap2Scr(ptMap2, ptScr2);
	TransMap2Scr(ptMap0, ptScr0);
	TransMap2Scr(ptMap3, ptScr3);

	if (m_CurIsland)
	{
		g_pA3DGDI->Draw2DLine(ptScr0, ptScr1,  cNormal);
		g_pA3DGDI->Draw2DLine(ptScr1, ptScr2,  cNormal);
		g_pA3DGDI->Draw2DLine(ptScr2, ptScr3,  cNormal);
		g_pA3DGDI->Draw2DLine(ptScr3, ptScr0,  cNormal);
	}
	else
	{
		g_pA3DGDI->Draw2DLine(ptScr0, ptScr1,  cSel);
		g_pA3DGDI->Draw2DLine(ptScr1, ptScr2,  cSel);
		g_pA3DGDI->Draw2DLine(ptScr2, ptScr3,  cSel);
		g_pA3DGDI->Draw2DLine(ptScr3, ptScr0,  cSel);
	}

	
	if (pPass && !pDest)
	{
		g_pA3DGDI->Draw2DLine(ptScr0, ptScr1,  cDest);
		g_pA3DGDI->Draw2DLine(ptScr1, ptScr2,  cDest);
		g_pA3DGDI->Draw2DLine(ptScr2, ptScr3,  cDest);
		g_pA3DGDI->Draw2DLine(ptScr3, ptScr0,  cDest);
	}

}
#endif

void CIslandList::_RemoveChild(CIsland * pParent, CIsland * pChild)
{
	pParent->RemoveChild(pChild->m_Num);
}

void CIslandList::_ChangeParent(CIsland * pChild, int newParent)
{
	if (newParent >= 0)
	{
		CIsland * pNewParent= m_Islands[newParent];
		pNewParent->m_Children.push_back(pChild->m_Num);
	}
	pChild->m_Parent = newParent;
	
}

void CIslandList::_CutOff(CIsland * pIsl)
{
	if (pIsl->m_Parent >= 0)
	{
		CIsland * pParent = m_Islands[pIsl->m_Parent];
		_RemoveChild(pParent, pIsl);
	}
	unsigned int i;
	for (i = 0; i < pIsl->m_Children.size(); ++i)
	{
		CIsland * pChild = m_Islands[pIsl->m_Children[i]];
		_ChangeParent(pChild, pIsl->m_Parent);
	}
	pIsl->m_Parent = -1;
	pIsl->m_Children.clear();
}


bool CIslandList::Update(CIsland * pIsl, AString& msg)
{
	assert(pIsl);
	if (!pIsl->IsValid(msg))
	{
		return false;
	}
	vector<CIsland*>::iterator it;
	
	//first check intersect
	for (it = m_Islands.begin(); it != m_Islands.end(); ++it)
	{
		if (*it == pIsl)
		{
			continue;
		}
		
		int relation = CIsland::Relation(*it, pIsl);
		if (relation == CIsland::ISLAND_RELATION_INTERSECT)
		{
			msg.Format("the island is intersected with others");
			return false;
		}
	}

	//now, only contain or isolate relationship

	unsigned int i;
	//cut off the old
	_CutOff(pIsl);

	//rebuild the relationship
	for (it = m_Islands.begin(); it != m_Islands.end(); ++it)
	{
		if (*it == pIsl)
		{
			continue;
		}
		
		int relation = CIsland::Relation(*it, pIsl);

		if (relation == CIsland::ISLAND_RELATION_ISOLATE)
		{
			continue;
		}
		
		if (relation == CIsland::ISLAND_RELATION_CHILD)
		{
			//check the children
			for (i = 0; i < (*it)->m_Children.size(); ++i)
			{
				CIsland * pChild = m_Islands[(*it)->m_Children[i]];
				int child_relation = CIsland::Relation(pChild, pIsl);
				if (child_relation != CIsland::ISLAND_RELATION_ISOLATE)
				{
					break;
				}
			}
			if (i == (*it)->m_Children.size())
			{
				//ok, we find it
				_ChangeParent(pIsl, (*it)->m_Num);
			}
		}
		else if (relation == CIsland::ISLAND_RELATION_CONTAIN)
		{
			//check the old parent
			bool bChild = true;
			if ((*it)->m_Parent >= 0)
			{
				CIsland * pParent = m_Islands[(*it)->m_Parent];
				int parent_relation = CIsland::Relation(pParent, pIsl);
				if (parent_relation == CIsland::ISLAND_RELATION_CONTAIN)
				{
					bChild = false;
				}
			}

			if (bChild)
			{
				_ChangeParent(*it, pIsl->m_Num);
			}
		}
		else
		{
			assert(0);
			msg.Format("oops, unknown bug");
			return false;
		}

	}
	

	return true;
}

void CIslandList::DelIsland(CIsland * pIsl)
{
	assert(pIsl);
	if (pIsl == GetCurIsland()){
		SetCurIsland((CIsland *)NULL);
	}
	//cut off
	_CutOff(pIsl);
	_DelPass(pIsl);
	if (m_Islands.back() == pIsl)
	{
		m_Islands.pop_back();
		delete pIsl;
		return;
	}

	CIsland * tmp = m_Islands.back();
	int old_id = tmp->m_Num;
	int new_id = pIsl->m_Num;
	m_Islands[new_id] = tmp;
	tmp->m_Num = new_id;
	m_Islands.pop_back();
	delete pIsl;

	vector<CIsland *>::iterator it;
	
	unsigned int i = 0;
	for (it = m_Islands.begin(); it != m_Islands.end(); ++it)
	{

		if ((*it)->m_Parent == old_id )
		{
			(*it)->m_Parent = new_id;
		}
		for (i = 0; i < (*it)->m_Children.size(); ++i)
		{
			if ((*it)->m_Children[i] == old_id)
			{
				(*it)->m_Children[i] = new_id;
			}
		}
		for (i = 0; i < (*it)->m_Passes.size(); ++i)
		{
			if ((*it)->m_Passes[i].dest == old_id)
			{
				(*it)->m_Passes[i].dest = new_id;
			}
		}
	}
	for (i = 0; i < m_MainlandPasses.size(); ++i)
	{
		if (m_MainlandPasses[i].dest == old_id)
		{
			m_MainlandPasses[i].dest = new_id;
		}
	}

}


void CIslandList::_DelPass(CIsland * pDest)
{
	//mainland
	_DelPass(&m_MainlandPasses, pDest);

	vector<CIsland *>::iterator it;
	for (it = m_Islands.begin(); it != m_Islands.end(); ++it)
	{
		if ((*it) != pDest)
		{
			_DelPass((*it)->GetAllPasses(), pDest);
		}
	}
}


void CIslandList::_DelPass(vector<CIsland::Pass>* pPasses, CIsland * pDest)
{
	vector<CIsland::Pass>::iterator it;
	for (it = pPasses->begin(); it != pPasses->end(); ++it)
	{
		if (it->dest == pDest->m_Num)
		{
			break;
		}
	}

	if (it != pPasses->end())
	{
		pPasses->erase(it);
	}

}

void CIslandList::CreateAllPasses()
{
	//mainland passes
	vector<CIsland::Pass>   tmp;
	CIsland::Pass * pPass;
	unsigned int i;
	for (i = 0; i < m_Islands.size(); ++i)
	{
		CIsland * pDest = m_Islands[i];
		assert(pDest->m_Num == (int)i);
		pPass = _FindPass(&m_MainlandPasses, pDest->m_Num);
		if (pPass)
		{
			tmp.push_back(*pPass);
		}
		else
		{
			CIsland::Pass tmp_pass;
			tmp_pass.dest = pDest->m_Num;
			tmp_pass.through = A3DPOINT2(LOCAL_INVALID_NUM, LOCAL_INVALID_NUM);
			tmp.push_back(tmp_pass);
		}
	}	
	m_MainlandPasses.swap(tmp);

	for (i = 0; i < m_Islands.size(); ++i)
	{
		CIsland * pIsl = m_Islands[i];
		_CreatePass(pIsl);
	}	

	
}

void CIslandList::_CreatePass(CIsland * pIsl)
{
	vector<CIsland::Pass>   tmp;
	CIsland::Pass * pPass;
	//mainland
	pPass = _FindPass(pIsl->GetAllPasses(), -1);
	if (pPass)
	{
		tmp.push_back(*pPass);
	}
	else
	{
		CIsland::Pass tmp_pass;
		tmp_pass.dest = -1;
		tmp_pass.through = A3DPOINT2(LOCAL_INVALID_NUM, LOCAL_INVALID_NUM);
		tmp.push_back(tmp_pass);
	}
	unsigned int i;
	for (i = 0; i < m_Islands.size(); ++i)
	{
		CIsland * pDest = m_Islands[i];
		if (pDest == pIsl)
		{
			continue;
		}
		assert(pDest->m_Num == i);
		pPass = _FindPass(pIsl->GetAllPasses(), pDest->m_Num);
		if (pPass)
		{
			tmp.push_back(*pPass);
		}
		else
		{
			CIsland::Pass tmp_pass;
			tmp_pass.dest = pDest->m_Num;
			tmp_pass.through = A3DPOINT2(LOCAL_INVALID_NUM, LOCAL_INVALID_NUM);
			tmp.push_back(tmp_pass);
		}
	}

	pIsl->m_Passes.swap(tmp);
}


CIsland::Pass * CIslandList::_FindPass(vector<CIsland::Pass>* passes, int dest)
{
	vector<CIsland::Pass>::iterator it;
	for (it = passes->begin(); it != passes->end(); ++it)
	{
		if (it->dest == dest)
		{
			return it;
		}
	}
	
	return NULL;
}

bool CIslandList::CheckAllPasses(CIsland *&err_isl, int& err_pass, AString& msg)
{
	//mainland
	err_isl = NULL;
	if (!_CheckPass(err_isl, err_pass, msg))
	{
		return  false;
	}
	unsigned int i;
	for (i = 0; i < m_Islands.size(); ++i)
	{
		err_isl = m_Islands[i];
		if (!_CheckPass(err_isl, err_pass, msg))
		{
			return false;
		}
	}
	
	return true;
}

bool CIslandList::_CheckPass(CIsland* pIsl, int& err_pass, AString& msg)
{
	vector<CIsland::Pass> * pPasses;
	int cur;
	if (pIsl)
	{
		pPasses = pIsl->GetAllPasses();
		cur = pIsl->GetId();
	}
	else
	{
		pPasses = &m_MainlandPasses;
		cur = -1;
	}

	unsigned int i;
	for (i = 0; i < pPasses->size(); ++i)
	{
		CIsland::Pass * pPass = &(pPasses->at(i));
		if (!m_pRMap->GetPixel(pPass->through.x, pPass->through.y))
		{
			msg.Format("%d->%d through (%d, %d) unreachable",  cur, pPass->dest, pPass->through.x, pPass->through.y);
			err_pass = i;
			return false;
		}

		if (pIsl && !pIsl->IsPtIn(pPass->through))
		{
			msg.Format("%d->%d through (%d, %d) invalid", cur, pPass->dest,  pPass->through.x, pPass->through.y);
			err_pass = i;
			return false;
		}

	}

	return true;
}


bool CIslandList::Save(const char * szName)
{
	CIsland * pIsl;
	int       iPass;
	AString   msg;
	if (!CheckAllPasses(pIsl, iPass, msg))
	{
		return false;
	}

	//if (m_MainlandPasses.empty() || m_Islands.empty())
	if (m_Islands.empty())
	{
		return false;
	}

	AFile   fileimg;
	if (!fileimg.Open(szName, AFILE_BINARY|AFILE_CREATENEW))
	{
		return false;
	}
	unsigned long  writelen;
	
	DWORD  flag =ISL_FILE_MAGIC;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);
	flag = ISL_FILE_VERSION;
	fileimg.Write(&flag, sizeof(DWORD), &writelen);

	unsigned int count, i;
	//islands
	count = m_Islands.size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; ++i)
	{
		CIsland * pIsl = m_Islands[i];
		pIsl->Save(fileimg);
	}
	

	//mainland passes
	count = m_MainlandPasses.size();
	fileimg.Write(&count, sizeof(unsigned int), &writelen);
	for (i = 0; i < count; ++i)
	{
		CIsland::Pass * p = &m_MainlandPasses[i];
		fileimg.Write(p, sizeof(CIsland::Pass), &writelen);
	}

	return true;
}

bool CIslandList::Load(const char * szName)
{
	Release();
	
	AFileImage   fileimg;
	if (!fileimg.Open(szName, AFILE_BINARY|AFILE_OPENEXIST))
	{
		return false;
	}
	unsigned long readlen;
	DWORD flag;
	fileimg.Read(&flag, sizeof(DWORD), &readlen);
	if (flag != ISL_FILE_MAGIC)
	{
		return false;
	}
	fileimg.Read(&flag, sizeof(DWORD), &readlen);  //version
	if (flag != ISL_FILE_VERSION)
	{
		return false;
	}

	unsigned int count, i;
	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	for (i = 0; i < count; ++i)
	{
		CIsland  * pIsl = new CIsland(i);
		pIsl->Load(fileimg);
		m_Islands.push_back(pIsl);
	}

	fileimg.Read(&count, sizeof(unsigned int), &readlen);
	for (i = 0; i < count; ++i)
	{
		m_MainlandPasses.push_back(CIsland::Pass());
		fileimg.Read(&m_MainlandPasses.back(), sizeof(CIsland::Pass), &readlen);
	}

	return  true;
}


CIsland::Pass * CIslandList::FindPass(CIsland * pStartIsl, CIsland * pDestIsl)
{
	vector<CIsland::Pass> * pStartPasses  =(pStartIsl)? (pStartIsl->GetAllPasses()):(&m_MainlandPasses);
	int dest = (pDestIsl)?(pDestIsl->GetId()):(-1);
	
	return _FindPass(pStartPasses, dest);
}

#undef  LOCAL_MAGNET_RANGE
#undef  LOCAL_INVALID_NUM
}