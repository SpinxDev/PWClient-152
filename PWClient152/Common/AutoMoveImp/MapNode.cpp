/********************************************************************
	created:	2006/09/22
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#include "MapNode.h"
#include "PfConstant.h"

namespace AutoMove
{
//////////////////////////////////////////////////////////////////////////
//define
//low =x, high = z
#define MAKE_KEY2D(x, z)    ((unsigned int)(((unsigned short)((x) & 0xffff)) | ((unsigned int)((unsigned short)((z) & 0xffff))) << 16))
#define GET_KEY2D_X(key)		((unsigned short)((unsigned int)(key) & 0xffff))	
#define GET_KEY2D_Z(key)		((unsigned short)((unsigned int)(key) >> 16))


//////////////////////////////////////////////////////////////////////////
//class MOpenArray
MOpenArray::MOpenArray()
{
}

MOpenArray::~MOpenArray()
{
	Clear();
}

void MOpenArray::Clear()
{
	m_List.clear();
}

void MOpenArray::Init()
{
	m_List.reserve(30);
}



void MOpenArray::Push(const MapNode& node)
{
	m_List.push_back(node);
}


int  MOpenArray::Find(short x, short z)
{
	size_t i = 0;
	while (i < m_List.size())
	{
		if (m_List[i].x == x && m_List[i].z == z)
		{
			break;
		}
		++i;
	}

	return i;
}

void MOpenArray::PopMinCost(MapNode& node)
{
	int iMin = 0;
	for (size_t i = 1; i < m_List.size(); ++i)
	{
		//eliminate branches
		iMin = (m_List[i].g + m_List[i].h < m_List[iMin].g + m_List[iMin].h)? (i):(iMin);
	}

	node = m_List[iMin];
	m_List[iMin] = m_List[m_List.size()-1];
	m_List.pop_back();
}

//////////////////////////////////////////////////////////////////////////
//class MOpenHeap
MOpenHeap::MOpenHeap()
{

}

MOpenHeap::~MOpenHeap()
{
	Clear();
}

void MOpenHeap::_HeapifyUp(int index)
{
	if (index == 0)
	{
		return;
	}

	int parent = (index -1)/2;
	
	//if (fgreater(m_List[parent].GetCost(), m_List[index].GetCost()))
	if (m_List[parent].GetCost() > m_List[index].GetCost())
	{
		MapNode tmp = m_List[parent];
		m_List[parent] = m_List[index];
		m_List[index]  = tmp;
		
		unsigned int key;
		key = MAKE_KEY2D(m_List[parent].x, m_List[parent].z);
		m_Pos2Index[key] = parent;
		key = MAKE_KEY2D(m_List[index].x, m_List[index].z);
		m_Pos2Index[key] = index;
		_HeapifyUp(parent);
	}
	
}

void MOpenHeap::Push(const MapNode& node)
{
	m_List.push_back(node);
	unsigned int key = MAKE_KEY2D(node.x, node.z);
	m_Pos2Index[key] = m_List.size() -1;
	_HeapifyUp(m_List.size()-1);
}

int MOpenHeap::Find(short x, short z)
{
	unsigned int key = MAKE_KEY2D(x, z);
	Pos2Index::iterator it = m_Pos2Index.find(key);
	if (it != m_Pos2Index.end())
	{
		return it->second;
	}
	
	return m_List.size();
}

void MOpenHeap::_HeapifyDown(int index)
{
	int child1 = index*2+1;
	int child2 = index*2+2;
	int which;
	// find smallest child
	if (child1 >= (int)m_List.size())
	{
		return;
	}
	else if (child2 >= (int)m_List.size())
	{
		which = child1;
	}
	//else if (fless(m_List[child1].GetCost(), m_List[child2].GetCost()))
	else if (m_List[child1].GetCost() < m_List[child2].GetCost())
	{
		 which = child1;
	}
	else
	{
		which = child2;
	}

	//if (fless(m_List[which].GetCost(), m_List[index].GetCost()))
	if (m_List[which].GetCost() < m_List[index].GetCost())
	{
		 MapNode tmp = m_List[which];
		 m_List[which] = m_List[index];
		 m_List[index] = tmp;
		 unsigned int key;
		 key = MAKE_KEY2D(m_List[which].x, m_List[which].z);
		 m_Pos2Index[key] = which;
		 key = MAKE_KEY2D(m_List[index].x, m_List[index].z);
		 m_Pos2Index[key] = index;
 	     _HeapifyDown(which);
	}
}


void MOpenHeap::PopMinCost(MapNode& node)
{
	if (Empty())
	{
		assert(0);
		return;
	}
	node = m_List[0];
	m_List[0] = m_List.back();
	unsigned int key = MAKE_KEY2D(m_List[0].x, m_List[0].z);
	m_Pos2Index[key] = 0;
	m_List.pop_back();
	key = MAKE_KEY2D(node.x, node.z);
	m_Pos2Index.erase(key);
	_HeapifyDown(0);
	

	
}

void MOpenHeap::Clear()
{
	m_List.clear();
	m_Pos2Index.clear();
}

void MOpenHeap::Init()
{
	m_List.clear();
	m_Pos2Index.clear();
	m_List.reserve(30);
}

void MOpenHeap::DecreaseKey(int index)
{
	_HeapifyUp(index);
}



//////////////////////////////////////////////////////////////////////////
//class MClose

// bool MClose::Find(short x, short z)
// {
// 	unsigned int key = MAKE_KEY2D(x, z);
// 	return (m_List.find(key) != m_List.end());
// }
MapNode * MClose::Find(short x, short z)
{
	unsigned int key = MAKE_KEY2D(x, z);
	CloseList::pair_type pair;
	pair = m_List.get(key);
	return (pair.second)? (pair.first) : (NULL);
}
void MClose::Remove(short x, short z)
{
	unsigned int key = MAKE_KEY2D(x, z);
	m_List.erase(key);
}
void MClose::Push(const MapNode& node)
{
	unsigned int key = MAKE_KEY2D(node.x, node.z);
	assert(!Find(node.x, node.z));
	m_List.put(key, node);
}

void MClose::GetPrv(short x, short z, short& prv_x, short& prv_z)
{
	const	MapNode * pNode = Find(x, z);
	assert(pNode);
	prv_x = pNode->prv_x;
	prv_z = pNode->prv_z;
}

}