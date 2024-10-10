// GHeap.cpp: implementation of the CGHeap class.
//
//////////////////////////////////////////////////////////////////////

#include "GHeap.h"
#include "Graph.h"
#include "PfConstant.h"


namespace AutoMove
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGHeap::CGHeap(int defaultsize)
{
	m_Count = 0;
	m_Elements.reserve(defaultsize);
}

CGHeap::~CGHeap()
{

}


void CGHeap::_HeapifyUp(int index)
{
  if (index == 0) return;
  int parent = (index-1)/2;

  if (fgreater(m_Elements[parent]->GetKey(), m_Elements[index]->GetKey()))
  {
	  CGNode * tmp = m_Elements[parent];
	  m_Elements[parent] = m_Elements[index];
	  m_Elements[index] = tmp;
	  m_Elements[parent]->key = parent;
	  m_Elements[index]->key  = index;
	  _HeapifyUp(parent);
  }
}

void CGHeap::_HeapifyDown(int index)
{
  int child1 = index*2+1;
  int child2 = index*2+2;
  int which;
	
  // find smallest child
  if (child1 >= m_Count)
    return;
  else if (child2 >= m_Count)
    which = child1;
  else if (fless(m_Elements[child1]->GetKey(), m_Elements[child2]->GetKey()))	
      which = child1;
  else
    which = child2;

  if (fless(m_Elements[which]->GetKey(), m_Elements[index]->GetKey()))
  {
	  CGNode * tmp = m_Elements[which];
	  m_Elements[which] = m_Elements[index];
	  m_Elements[index] = tmp;
	  m_Elements[which]->key = which;
	  m_Elements[index]->key = index;
	  _HeapifyDown(which);
  }
}


void CGHeap::Add(CGNode *val)
{

  val->key = m_Count ;
  m_Elements.push_back(val);
  m_Count++;
  _HeapifyUp(val->key);
}

/**
 * Indicate that the key for a particular object has decreased.
 */
void CGHeap::DecreaseKey(CGNode *val)
{
	_HeapifyUp(val->key);	
}


/**
 * Returns true if the object is in the heap.
 */
bool CGHeap::IsIn(CGNode *val)
{
  if (val->key < m_Elements.size() && (m_Elements[val->key] == val))
  {
	  return true;
  }

  return false;
}

/**
 * Remove the item with the lowest key from the heap & re-heapify.
 */
CGNode * CGHeap::Remove()
{
  if (Empty())
	 return NULL;
  m_Count--;
  CGNode * ans = m_Elements[0];
  m_Elements[0] = m_Elements[m_Count];
  m_Elements[0]->key = 0;
  m_Elements.pop_back();
  _HeapifyDown(0);
  return ans;	
}

/**
 * Returns true if no items are in the heap.
 */
bool CGHeap::Empty()
{
  return (m_Count == 0);
}

}