/********************************************************************
	created:	2006/11/16
	author:		kuiwu
	
	purpose:	simple heap class
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/


#pragma once

#include <vector.h>
using namespace abase;

namespace AutoMove
{



class CGNode;

class CGHeap  
{
public:
	CGHeap(int defaultsize = 20);
	~CGHeap();
	void Add(CGNode *val);
    void DecreaseKey(CGNode *val);
    bool IsIn(CGNode *val);
    CGNode *Remove();
    bool Empty();
	int GetCount()
	{
		return m_Count;
	}
private:
	void _HeapifyUp(int index);
	void _HeapifyDown(int index);


	vector<CGNode*>   m_Elements;
	int               m_Count;
	
	
};


}