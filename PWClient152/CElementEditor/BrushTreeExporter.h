/********************************************************************
	created:	2008/04/17
	author:		kuiwu
	
	purpose:	convex hull tree
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#include <A3DGeometry.h>
#include <vector.h>
#include <ARect.h>
#include "aabbcd.h"


using namespace CHBasedCD;

class AFile;
class AString;

class CBrushTreeExporter
{
public:
	struct  BrushNode
	{
		A3DAABB    aabb;
		BrushNode *   children[4];
		abase::vector<CCDBrush*>   brushes;
		bool					  RoI;

		BrushNode()
		{
			children[0]  = NULL;
			brushes.clear();
			RoI = true;
		}
		~BrushNode()
		{
			if (children[0])
			{
				for (int i = 0; i < 4; ++i)
				{
					delete children[i];
					children[i] = NULL;
				}
			}
			brushes.clear();
		}
		bool IsLeaf() const
		{
			return (children[0] == NULL);
		}
		bool IsRoI() const
		{
			return RoI;
		}
		bool IsIn(float x, float z)
		{
			A3DVECTOR3  pos(x, aabb.Center.y, z);
			return aabb.IsPointIn(pos);
		}

	};
public:
	CBrushTreeExporter();
	~CBrushTreeExporter();
	void Build(const A3DVECTOR3& ext);
	bool AddBrush(CCDBrush * brush);
	void Release();

	BrushNode * Pickup(const A3DVECTOR3& pos);
	void SetMinNodeSize(float minNodeSize);

	void RetrieveLeaves(abase::vector<BrushNode*>& leaves);

	BrushNode * Check(AString& failReason);
	BrushNode * GetRootNode()
	{
		return m_pRootNode;
	}
	void Export(const char * szName);
private:
	void _Split(BrushNode* node,  bool recursive);
	bool _TestOverlap(BrushNode * node, CCDBrush * brush);
	BrushNode * _Pickup(BrushNode * node, const A3DVECTOR3& pos);
	BrushNode * _GetParent(BrushNode * curNode, BrushNode * node);
	void _RetrieveLeaves(BrushNode * node, abase::vector<BrushNode*>& leaves);
	void _RetrieveNodes(BrushNode * curNode, int curLevel, int level, abase::vector<BrushNode*>& nodes);
	void _ExportNode(AFile& aFile, BrushNode * node);
	
	bool _AddBrush(BrushNode * node, CCDBrush * brush);
	BrushNode * _Check(BrushNode * curNode, AString& failReason);
private:
	BrushNode				*  m_pRootNode;

	float					   m_fMinNodeSize;
};