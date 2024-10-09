/********************************************************************
	created:	2008/04/17
	author:		kuiwu
	
	purpose:	convex hull tree
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#include "Global.h"
#include "BrushTreeExporter.h"
#include <AAssist.h>
#include <A3DCollision.h>
#include <AFile.h>

#include <algorithm>


const float MIN_NODE_SIZE = 16.0f;
const float EXTENDS_Y = 5000.0f;

const DWORD BRUSH_TREE_FILE_MAGIC = (DWORD)(('b'<<24)| ('t'<<16)|('r'<<8)|('f'));
const DWORD BRUSH_TREE_FILE_VERSION = 1;




CBrushTreeExporter::CBrushTreeExporter()
{
	m_pRootNode = NULL;
	m_fMinNodeSize = MIN_NODE_SIZE;
}

CBrushTreeExporter::~CBrushTreeExporter()
{
	Release();
}

void CBrushTreeExporter::Build(const A3DVECTOR3& ext)
{
	m_pRootNode = new BrushNode;
	A3DVECTOR3 vExtents(ext);
	vExtents.y =  EXTENDS_Y;
	m_pRootNode->aabb.Center = A3DVECTOR3(0.0f);
	m_pRootNode->aabb.Extents = vExtents;
	m_pRootNode->aabb.CompleteMinsMaxs();

	_Split(m_pRootNode, true);
}

void CBrushTreeExporter::Release()
{
	if (m_pRootNode)
	{
		int i;
		for (i = 0; i < (int)m_pRootNode->brushes.size(); ++i)
		{
			delete m_pRootNode->brushes[i];
		}

		delete m_pRootNode;
		m_pRootNode = NULL;
	}
}

bool CBrushTreeExporter::AddBrush(CCDBrush * brush)
{
	if (!m_pRootNode || !_AddBrush(m_pRootNode, brush))
	{
		return false;
	}
	return true;
}

bool CBrushTreeExporter::_AddBrush(BrushNode * node, CCDBrush * brush)
{
	if (!_TestOverlap(node, brush))
	{
		return false;
	}
	//abase::vector<CCDBrush*>::iterator it = vec_find(node->brushes, brush);
	abase::vector<CCDBrush*>::iterator it = std::find(node->brushes.begin(), node->brushes.end(), brush);
	if (it != node->brushes.end())
	{
		return true;
	}

	node->brushes.push_back(brush);
	if (node->IsLeaf())
	{
		return true;
	}
	int j;
	for (j = 0; j < 4; ++j)
	{
		_AddBrush(node->children[j], brush);
	}
	return true;
}


void CBrushTreeExporter::SetMinNodeSize(float minNodeSize)
{
	m_fMinNodeSize = a_Max(minNodeSize, MIN_NODE_SIZE);
}




//   ----------------------
//	 |          |         |
//   |     1    |     2   |
//   |          |         |
//   ----------------------
//	 |          |         |
//   |     0    |     3   |
//   |          |         |
//   ----------------------
void CBrushTreeExporter::_Split(BrushNode* node, bool recursive)
{
	assert(node->IsLeaf());
	if (!node->IsRoI())
	{
		return;
	}

	if (node->aabb.Extents.x * 2 < m_fMinNodeSize + 0.1f)
	{
		return;
	}

	
	assert(node->children[0] == NULL);

	A3DVECTOR3 child_ext = node->aabb.Extents * 0.5f;
	child_ext.y = EXTENDS_Y;
	
	node->children[0] = new BrushNode;
	node->children[0]->aabb.Center.x   = node->aabb.Center.x - child_ext.x;
	node->children[0]->aabb.Center.y   = 0.0f;
	node->children[0]->aabb.Center.z   = node->aabb.Center.z - child_ext.z;
	node->children[0]->aabb.Extents = child_ext;
	node->children[0]->aabb.CompleteMinsMaxs();
	node->children[0]->RoI = node->RoI;

	node->children[1] = new BrushNode;
	node->children[1]->aabb.Center.x   = node->aabb.Center.x - child_ext.x;
	node->children[1]->aabb.Center.y   = 0.0f;
	node->children[1]->aabb.Center.z   = node->aabb.Center.z + child_ext.z;
	node->children[1]->aabb.Extents = child_ext;
	node->children[1]->aabb.CompleteMinsMaxs();
	node->children[1]->RoI = node->RoI;

	node->children[2] = new BrushNode;
	node->children[2]->aabb.Center.x   = node->aabb.Center.x + child_ext.x;
	node->children[2]->aabb.Center.y   = 0.0f;
	node->children[2]->aabb.Center.z   = node->aabb.Center.z + child_ext.z;
	node->children[2]->aabb.Extents = child_ext;
	node->children[2]->aabb.CompleteMinsMaxs();
	node->children[2]->RoI = node->RoI;

	node->children[3] = new BrushNode;
	node->children[3]->aabb.Center.x   = node->aabb.Center.x + child_ext.x;
	node->children[3]->aabb.Center.y   = 0.0f;
	node->children[3]->aabb.Center.z   = node->aabb.Center.z - child_ext.z;
	node->children[3]->aabb.Extents = child_ext;
	node->children[3]->aabb.CompleteMinsMaxs();
	node->children[3]->RoI = node->RoI;

	if (recursive)
	{
		int j;
		for (j = 0; j < 4; ++j)
		{
			BrushNode * child = node->children[j];
			_Split(child, recursive);
		}
	}
}

bool CBrushTreeExporter::_TestOverlap(BrushNode * node, CCDBrush * brush)
{
	if (!CLS_AABBAABBOverlap(node->aabb.Center, node->aabb.Extents, brush->GetAABB().Center, brush->GetAABB().Extents))
	{
		return false;
	}

	BrushTraceInfo trc_info;
	trc_info.Init(node->aabb.Center, A3DVECTOR3(0.0f), node->aabb.Extents);
	return brush->Trace(&trc_info);
}



CBrushTreeExporter::BrushNode * CBrushTreeExporter::_GetParent(BrushNode * curNode, BrushNode * node)
{
	if (node == curNode || curNode->IsLeaf())
	{
		return NULL;
	}
	
	int j;
	for (j = 0; j < 4; ++j)
	{
		BrushNode * child = curNode->children[j];
		if (child == node)
		{
			return curNode;
		}
	}
	for (j = 0; j < 4; ++j)
	{
		BrushNode * parent = _GetParent(curNode->children[j], node);
		if (parent)
		{
			return parent;
		}
	}

	return NULL;
}

CBrushTreeExporter::BrushNode * CBrushTreeExporter::Pickup(const A3DVECTOR3& pos)
{
	if (!m_pRootNode)
	{
		return NULL;
	}
	return _Pickup(m_pRootNode, pos);	
}

CBrushTreeExporter::BrushNode * CBrushTreeExporter::_Pickup(BrushNode * node, const A3DVECTOR3& pos)
{
	if (node->IsLeaf() )
	{
		if (node->IsIn(pos.x, pos.z))
		{
			return node;
		}
		else
		{
			return NULL;
		}
	}

	int j;
	for (j = 0; j < 4; ++j)
	{
		BrushNode * child = node->children[j];

		BrushNode * pick = _Pickup(child, pos);
		if (pick != NULL)
		{
			return pick;
		}
	}

	return NULL;
}


void CBrushTreeExporter::RetrieveLeaves(abase::vector<BrushNode*>& leaves)
{

	_RetrieveLeaves(m_pRootNode, leaves);
}

void CBrushTreeExporter::_RetrieveLeaves(BrushNode * node,  abase::vector<BrushNode*>& leaves)
{
	if (node->IsLeaf())
	{
		leaves.push_back(node);
		return;
	}

	int j;
	for (j = 0; j < 4; ++j)
	{
		BrushNode * child = node->children[j];
		_RetrieveLeaves(child, leaves);		
	}

}


void CBrushTreeExporter::Export(const char * szName)
{
	if (!m_pRootNode)
	{
		return;
	}

	AFile aFile;
	if (!aFile.Open(szName, AFILE_CREATENEW | AFILE_BINARY | AFILE_NOHEAD))
	{
		return;
	}

	DWORD write_len;
	
	DWORD flag = BRUSH_TREE_FILE_MAGIC;
	aFile.Write(&flag, sizeof(DWORD), &write_len);
	flag = BRUSH_TREE_FILE_VERSION;
	aFile.Write(&flag, sizeof(DWORD), &write_len);


	//write brush
	int count = (int) m_pRootNode->brushes.size();
	aFile.Write(&count, sizeof(int), &write_len);
	int i;
	for (i = 0; i < count; ++i)
	{
		CCDBrush * cdBrush = m_pRootNode->brushes[i];
		cdBrush->Save(&aFile);
	}

	//write treenode
	abase::vector<BrushNode *>  nodes;
	nodes.push_back(m_pRootNode);
	while (!nodes.empty())
	{
		abase::vector<BrushNode *> children;

		for (i =0; i < (int)nodes.size(); ++i)
		{
			BrushNode * cur_node = nodes[i];
			_ExportNode(aFile, cur_node);
			
			if (!cur_node->IsLeaf())
			{
				int j;
				for (j = 0; j < 4; ++j)
				{
					BrushNode * child = cur_node->children[j];
					children.push_back(child);	
				}
			}
		}
		nodes = children;
	}

}



void CBrushTreeExporter::_ExportNode(AFile& aFile, BrushNode * node)
{
	DWORD write_len;
	aFile.Write(&node->aabb.Center.x, sizeof(float), &write_len);
	aFile.Write(&node->aabb.Center.z, sizeof(float), &write_len);
	aFile.Write(&node->aabb.Extents.x, sizeof(float), &write_len);
	aFile.Write(&node->aabb.Extents.z, sizeof(float), &write_len);
	bool b;
	b = node->IsRoI();
	aFile.Write(&b, sizeof(bool), &write_len);
	b = node->IsLeaf();
	aFile.Write(&b, sizeof(bool), &write_len);
}


void CBrushTreeExporter::_RetrieveNodes(BrushNode * curNode, int curLevel, int level, abase::vector<BrushNode*>& nodes)
{
	if (curLevel == level)
	{
		nodes.push_back(curNode);
		return;
	}
	else if (curLevel > level)
	{
		return;
	}

	if (curNode->IsLeaf())
	{
		return;
	}

	int j;
	for (j = 0; j < 4; ++j)
	{
		BrushNode * child = curNode->children[j];
		_RetrieveNodes(child, curLevel+1, level, nodes);
	}
}



CBrushTreeExporter::BrushNode * CBrushTreeExporter::Check(AString& failReason)
{
	if (!m_pRootNode)
	{
		return NULL;
	}
	return _Check(m_pRootNode, failReason);
}

CBrushTreeExporter::BrushNode * CBrushTreeExporter::_Check(BrushNode * curNode, AString& failReason)
{
/*
	if (!curNode->IsRoI() && !curNode->IsLeaf())
	{
		failReason.Format("node center %f %f %f size %f non-RoI but not leaf!\r\nPlease set RoI or merge.", 
				curNode->aabb.Center.x, curNode->aabb.Center.y, curNode->aabb.Center.z, 
				curNode->aabb.Extents.x);
		return curNode;
	}

	if (curNode->IsLeaf())
	{
		if (curNode->IsRoI() && (int)curNode->brushes.size() > m_nMinBrushCount &&  curNode->aabb.Extents.x * 2 > m_fMinNodeSize + 0.1f)
		{
			failReason.Format("leaf node center %f %f %f size %f convexhull %d, expected(size < %f or hull <%d)!\r\nPlease set non-RoI or split.", 
				curNode->aabb.Center.x, curNode->aabb.Center.y, curNode->aabb.Center.z, 
				curNode->aabb.Extents.x, curNode->brushes.size(), m_fMinNodeSize, m_nMinBrushCount);
			return curNode;
		}
	}
	else
	{
		int j;
		for (j =0; j < 4; ++j)
		{
			BrushNode * fail_node  = _Check(curNode->children[j], failReason);
			if (fail_node)
			{
				return fail_node;
			}
		}
	}
*/
	return NULL;
}