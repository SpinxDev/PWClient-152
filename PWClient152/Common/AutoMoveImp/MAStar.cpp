
#include "MAStar.h"
#include "MoveMap.h"
#include "MoveAgent.h"
#include "PfConstant.h"
#include "bitimage.h"

namespace AutoMove
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMAStar::CMAStar()
:m_MaxExpand(-1)
{
}

CMAStar::~CMAStar()
{
}

float CMAStar::Search(int iPfAlg, CBitImage * pRMap, const A3DPOINT2& ptStart, const A3DPOINT2& ptGoal, vector<A3DPOINT2>& path, IMExpandLimit * pExpandLmt /* = NULL */)
{
	m_State = PF_STATE_SEARCHING;
	m_NodesExpanded = 0;
	m_NodesTouched = 0;
	path.clear();
	double cost = -1.0;

	if (ptGoal == ptStart)
	{
		m_State = PF_STATE_FOUND;
		path.push_back(ptGoal);
		cost = 0.0f;
		return (float)cost;
	}

	MOpen * openlist;
	if (iPfAlg == PF_ALGORITHM_CLU_MA)
	{
		openlist = new MOpenArray; 
	}
	else
	{
		openlist = new MOpenHeap; 
	}
	

	openlist->Init();
	MClose  closelist;

	MapNode curNode;
	curNode.x = ptStart.x;
	curNode.z = ptStart.y;
	curNode.prv_x = PF_MAP_INVALID_NODE;
	curNode.prv_z = PF_MAP_INVALID_NODE;
	curNode.h = GetManhDist(ptStart.x, ptStart.y, ptGoal.x, ptGoal.y);
	curNode.g = 0.0f;

	openlist->Push(curNode);

	MapNode nextNode;
	MapNode * tmp_node_ptr;
	while ((m_State == PF_STATE_SEARCHING) && !openlist->Empty())
	{
		m_NodesExpanded++;
		if (m_MaxExpand > 0 && m_NodesExpanded > m_MaxExpand)
		{
			m_State = PF_STATE_EXCEED;
			break;
		}

		openlist->PopMinCost(curNode);

// 		AString msg;
// 		msg.Format("open select (%d,%d) g %f h %f\n", curNode.x, curNode.z, curNode.g, curNode.h);
// 		OutputDebugStringA(msg);
		
		if (curNode.x == ptGoal.x && curNode.z == ptGoal.y)
		{
			m_State = PF_STATE_FOUND;

// 			msg.Format("add2close (%d,%d)\n", curNode.x, curNode.z);
// 			OutputDebugStringA(msg);

			closelist.Push(curNode);
			break;
		}
		// check the 8-neighbors
		int i;
		for (i = 0; i <PF_NEIGHBOR_COUNT; ++i)
		{
			m_NodesTouched++;
			nextNode.x = curNode.x + PF2D_NeighborD[i*2];
			nextNode.z = curNode.z + PF2D_NeighborD[i*2+1];
			if (!pRMap->GetPixel(nextNode.x, nextNode.z))
			{
				continue;
			}
			//check to expand
			if (pExpandLmt&& !pExpandLmt->Test(&nextNode))
			{
				continue;
			}


			nextNode.prv_x = curNode.x;
			nextNode.prv_z = curNode.z;
			nextNode.h = GetManhDist(nextNode.x, nextNode.z, ptGoal.x, ptGoal.y);
			nextNode.g = (PF2D_NeighborD[i*2] == 0 || PF2D_NeighborD[i*2+1] == 0) 
						? (curNode.g +1.0f) : (curNode.g + PF_ROOT_TWO);
			int open_index;
			if ((open_index = openlist->Find(nextNode.x, nextNode.z)) != openlist->GetSize())
			{
				//in open
				//see if update the node in open list
				tmp_node_ptr = openlist->At(open_index);
				if (tmp_node_ptr->GetCost() > nextNode.GetCost())
				{
					//update
					tmp_node_ptr->prv_x = nextNode.prv_x;
					tmp_node_ptr->prv_z = nextNode.prv_z;
					tmp_node_ptr->g     = nextNode.g;
					tmp_node_ptr->h     = nextNode.h;
					openlist->DecreaseKey(open_index);
					
// 					msg.Format("updataopen (%d,%d)\n", nextNode.x, nextNode.z);
// 					OutputDebugStringA(msg);
				}
			}
			else if (tmp_node_ptr = closelist.Find(nextNode.x, nextNode.z)) 
			{
				//in close
 				if(tmp_node_ptr->g + tmp_node_ptr->h > nextNode.g + nextNode.h + 0.01f)
				{
					//move the node from close to open
					closelist.Remove(nextNode.x, nextNode.z);
					openlist->Push(nextNode);

// 					msg.Format("close2open (%d,%d)\n", nextNode.x, nextNode.z);
// 					OutputDebugStringA(msg);
				}
			}
			else
			{  
				//not in open nor close
				openlist->Push(nextNode);

// 				msg.Format("add2open (%d,%d)\n", nextNode.x, nextNode.z);
// 				OutputDebugStringA(msg);
			}
			
		}
// 		msg.Format("add2close (%d,%d)\n", curNode.x, curNode.z);
// 		OutputDebugStringA(msg);

		//add to close
		closelist.Push(curNode);
	}
	
	if (m_State == PF_STATE_SEARCHING && openlist->Empty())
	{
		m_State = PF_STATE_NOPATH;
	}
	
	delete openlist;

	if (m_State == PF_STATE_FOUND)
	{
		cost = _GeneratePath(closelist, curNode, path);
	}

	return (float)cost;
}




double  CMAStar::_GeneratePath(MClose& closelist, MapNode& dest, vector<A3DPOINT2>& path)
{
	//space-time tradeoff
	//first pass, fill temp path
	vector<A3DPOINT2> tmpPath;
	A3DPOINT2 pos, last;
	double cost = 0.0;
	short x, z;
	x = dest.x;
	z = dest.z;
	last.x = dest.x;
	last.y = dest.z;
	double weight;
	while (x != PF_MAP_INVALID_NODE && z != PF_MAP_INVALID_NODE)
	{
		pos.x = x;
		pos.y = z;
		
		last -= pos;
		if (last.x == 0 && last.y == 0)
		{
			weight = 0.0;
		}
		else if ((last.x == -1 || last.x == 1) && (last.y == -1 || last.y == 1))
		{
			weight = PF_ROOT_TWO;
		}
		else 
		{
			assert(last.x ==0 || last.y == 0);
			weight = 1.0;
		}
		cost += weight;	
		tmpPath.push_back(pos);
		last = pos;		
		closelist.GetPrv((short)pos.x, (short)pos.y, x, z);
	}
	//second pass, reverse the path order
	path.clear();
	int i = tmpPath.size() -1;
	while (i >= 0)
	{
		path.push_back(tmpPath[i]);
		--i;
	}

	return cost;

}


}