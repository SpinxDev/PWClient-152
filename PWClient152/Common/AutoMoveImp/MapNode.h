/********************************************************************
	created:	2006/09/22
	author:		kuiwu
	
	purpose:	
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#pragma  once

#include <vector.h>
#include <hashtab.h>
#include <hashmap.h>
using namespace abase;


namespace AutoMove
{

class MapNode
{
public:
   short x, z;   //pos
   short prv_x, prv_z;   //prv node
   float h;   
   float g;

   float GetCost()
   {
	   return g+h;
   }
};

class MOpen
{
public:
	MOpen(){};
	virtual ~MOpen(){};
	virtual void Init() = 0;
	virtual void Clear() = 0;
	virtual bool Empty() const = 0;
	virtual void Push(const MapNode& node) = 0;
	virtual void PopMinCost(MapNode& node) = 0;
	virtual int  Find(short x, short z) = 0;
	virtual int  GetSize() = 0;
	virtual	MapNode *   At(int index) = 0;
	virtual void  DecreaseKey(int index) = 0;
protected:
private:
};


/*
 * open list, currently use array
 * @desc :  
 * @todo:   
 * @author: kuiwu [20/12/2005]
 * @ref:
 */
class MOpenArray : public MOpen
{
public:
	MOpenArray();
	virtual ~MOpenArray();
	virtual void Init();
	virtual void Clear();
	
	virtual bool Empty() const 
	{
		return (m_List.empty());
	}
	virtual	void Push(const MapNode& node);
	virtual void PopMinCost(MapNode& node);
	virtual void DecreaseKey(int index)
	{
		//do nothing
	}
	/*
	 *
	 * @desc :
	 * @param :     
	 * @return :  size of the list if not found
	 * @note:
	 * @todo:   
	 * @author: kuiwu [21/12/2005]
	 * @ref:
	 */
	virtual	int Find(short x, short z);
	virtual int GetSize()
	{
		return m_List.size();
	}
	virtual MapNode * At(int index)
	{
		assert(index >= 0 && index < m_List.size());
		return &m_List[index];
	}
private:
	vector<MapNode> m_List;
};

class MOpenHeap : public MOpen
{
public:
	typedef 	hash_map<unsigned int, int>  Pos2Index;

public:
	MOpenHeap();
	virtual ~MOpenHeap();
	virtual void Init();
	virtual void Clear();
	
	virtual bool Empty() const 
	{
		return (m_List.empty());
	}
	virtual	void Push(const MapNode& node);
	virtual void PopMinCost(MapNode& node);
	virtual	int Find(short x, short z);
	virtual int GetSize()
	{
		return m_List.size();
	}
	virtual MapNode * At(int index)
	{
		assert(index >= 0 && index < m_List.size());
		return &m_List[index];
	}

	virtual void   DecreaseKey(int index);
private:
	void  _HeapifyUp(int index);
	void  _HeapifyDown(int index);
private:
	vector<MapNode> m_List;
	Pos2Index  m_Pos2Index;
};




/*
 * close list, currently use hash table
 * @desc :
 * @todo:   
 * @author: kuiwu [21/12/2005]
 * @ref:
 */
class MClose
{
	typedef hashtab<MapNode, unsigned int, abase::_hash_function>  CloseList;
public:
	MClose()
		:m_List(256)
	{
	}
	~MClose()
	{
		Clear();
	}
	//inline bool Find(short x, short z);
	//bool Find(short x, short z);	
	//inline void Push(const MapNode& node);
	void Push(const MapNode& node);
	//inline void GetPrv(short x, short z, short& prv_x, short& prv_z);
	void GetPrv(short x, short z, short& prv_x, short& prv_z);
	void        Clear()
	{
		m_List.clear();
	}
	MapNode * Find(short x, short z);
	void Remove(short x, short z);
private:
	CloseList    m_List;	
};


}