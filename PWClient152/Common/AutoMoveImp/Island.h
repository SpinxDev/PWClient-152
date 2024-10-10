/********************************************************************
	created:	2006/12/18
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#pragma once

#include <vector.h>
using namespace abase;
#include <A3DTypes.h>



class AString;
class AFile;

namespace AutoMove
{

class CGGraph;
class CIsland;
class CIslandList;
class CBitImage;

class CIsland  
{
	friend class CIslandList;
public:
	enum
	{
		ISLAND_RELATION_ISOLATE,  
		ISLAND_RELATION_INTERSECT,
		ISLAND_RELATION_CONTAIN,
		ISLAND_RELATION_CHILD,
	};

	struct Pass 
	{
		int dest;
		A3DPOINT2 through;
	};

	CIsland(int id);
	virtual ~CIsland();
	void Release();

	void AddBorderLine(const  A3DPOINT2& pt1, const A3DPOINT2& pt2);
	void DelBorderPoint(const A3DPOINT2& pt);
	
	bool IsPtIn(const A3DPOINT2& pt);

#ifdef _AUTOMOVE_EDITOR
	void Render(A3DCOLOR  color);
#endif
	
	bool IsValid(AString& msg);
	void RemoveChild(int id);
	CGGraph * GetBorder()
	{
		return m_Border;
	}
	vector<Pass> * GetAllPasses()
	{
		return &m_Passes;
	}
	int GetId()
	{
		return m_Num;
	}
	static	int  Relation(CIsland * pIslA, CIsland * pIslB);

	void   Save(AFile& fileimg);
	void   Load(AFile& fileimg);

private:
	int    m_Num;   //the id, -1 indicates mainland
	int    m_Parent;// parent id,  default mainland
	vector<int> m_Children;  // children id

	CGGraph *   m_Border;    //the polygon border,  maybe not convex
	
	vector<Pass>  m_Passes;  // the pass to others 

};


class CIslandList
{
public:
	CIslandList(CBitImage * pRMap);
	virtual ~CIslandList();
	
#ifdef _AUTOMOVE_EDITOR
	void	Render();
#endif	
	CIsland * NewIsland();
	CIsland * SelectIsland(const A3DPOINT2& pt);
	void      Release();

	CIsland * GetCurIsland()
	{
		return m_CurIsland;
	}
	void SetCurIsland(CIsland * pIsland)
	{
		m_CurIsland = pIsland;
		m_CurPass = -1;
	}
	void SetCurIsland(int index)
	{
		if (index >= 0)
		{
			m_CurIsland = m_Islands[index];
		}
		else
		{
			m_CurIsland = NULL;
		}
		m_CurPass = -1;
	}
	void SetCurPass(int index)
	{
		m_CurPass = index;
	}
	
	CIsland::Pass * GetCurPass()
	{
		CIsland::Pass * pPass = NULL;
		if (m_CurPass >= 0)
		{
			if (m_CurIsland)
			{
				pPass = &(m_CurIsland->m_Passes[m_CurPass]);
			}
			else
			{
				pPass = &(m_MainlandPasses[m_CurPass]);
			}
		}
		return pPass;
	}

	bool Update(CIsland * pIsl, AString& msg);
	
	void DelIsland(CIsland * pIsl);
	
	void CreateAllPasses();
	vector<CIsland::Pass> * GetMainlandPasses()
	{
		return &m_MainlandPasses;
	}
	vector<CIsland*> * GetAllIslands()
	{
		return &m_Islands;
	}
	bool CheckAllPasses(CIsland*&err_isl, int& err_pass,  AString& msg);
	
	CIsland::Pass * FindPass(CIsland * pStartIsl, CIsland * pDestIsl);

	bool      Save(const char * szName);
	bool      Load(const char * szName);


private:
	vector<CIsland *> m_Islands;
	CIsland        *  m_CurIsland;
	int               m_CurPass;
	vector<CIsland::Pass> m_MainlandPasses;
	CBitImage      *  m_pRMap;
private:
	void _RemoveChild(CIsland * pParent, CIsland * pChild);
	void _ChangeParent(CIsland * pChild, int newParent);
	void _CutOff(CIsland * pIsl);
	void _DelPass(CIsland * pDest);
	void _DelPass(vector<CIsland::Pass>* pPasses, CIsland * pDest);
	
	void _CreatePass(CIsland * pIsl);

	bool _CheckPass(CIsland * pIsl,	int& err_pass,	AString& msg);
	//bool _CheckPass(CIsland::Pass * pPass, AString& msg);

	CIsland::Pass * _FindPass(vector<CIsland::Pass>* passes, int dest);
};


}