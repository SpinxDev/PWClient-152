/**
* 该文件实现了记录世界对象中可到达的位置，供玩家移动
* 收到OBJECT_MOVE, OBJECT_STOP_MOVE的时候，记录所在point的坐标
* 玩家移动时，根据这些坐标，发送下一步位置的坐标到服务器
* 将大世界的所有可到达点存完需要 8*11*1024*1024*sizeof(A3DVECTOR3) = 264MB
*/

#ifndef __MOVEAGENT_H__
#define __MOVEAGENT_H__

#include "Common.h"
#include <vector>

#define MOVEAGENT_ROW		4000
#define MOVEAGENT_COL		4000
#define MOVEAGENT_STEP 		1.f
#define MOVEAGENT_X_START	-2000.f
#define MOVEAGENT_Z_START	-2000.f
#define MOVEAGENT_X_END		(MOVEAGENT_X_START + MOVEAGENT_ROW)
#define MOVEAGENT_Z_END		(MOVEAGENT_Z_START + MOVEAGENT_COL)
#define MOVEAGENT_SEARCH_RANGE 5

unsigned char DirConvert(const A3DVECTOR3 & dir);	//返回值0-255

class MoveAgent
{
public:
	void Init(const char* filename);
	void Quit();
	void Learn(const A3DVECTOR3 & rpos);
	bool GetMovePos(const A3DVECTOR3 & cur_pos, const A3DVECTOR3 & dest_pos, float range, A3DVECTOR3 & next_pos);
	bool GetMovePos(const A3DVECTOR3 & cur_pos, unsigned char dir, float range, A3DVECTOR3 & next_pos);

	void GetRegion(A3DVECTOR3& vMin, A3DVECTOR3& vMax)
	{
		vMin = m_vMinRegion;
		vMax = m_vMaxRegion;
	}

	void SetRegion(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax)
	{
		m_vMinRegion = vMin;
		m_vMaxRegion = vMax;
	}

	MoveAgent()
	{
		int i, j;
		reachable_table = new A3DVECTOR3 **[MOVEAGENT_ROW];
		for(i=0; i<MOVEAGENT_ROW; i++)
			reachable_table[i] = new A3DVECTOR3 *[MOVEAGENT_COL];

		for(i=0; i<MOVEAGENT_ROW; i++)
			for(j=0; j<MOVEAGENT_COL; j++)
				reachable_table[i][j] = NULL;

		for(i=1; i<=MOVEAGENT_SEARCH_RANGE; i++)
			for(j=0; j<=i; j++)
			{
				off_node_list[0].push_back(off_node_t(i,j));
				off_node_list[7].push_back(off_node_t(i,-j));
				off_node_list[3].push_back(off_node_t(-i,j));
				off_node_list[4].push_back(off_node_t(-i,-j));
			}
		for(j=1; j<=MOVEAGENT_SEARCH_RANGE; j++)
			for(i=0; i<=j; i++)
			{
				off_node_list[1].push_back(off_node_t(i,j));
				off_node_list[2].push_back(off_node_t(-i,j));
				off_node_list[5].push_back(off_node_t(-i,-j));
				off_node_list[6].push_back(off_node_t(i,-j));
			}
	}
	~MoveAgent()
	{
		for(int i=0; i<MOVEAGENT_ROW; i++)
			for(int j=0; j<MOVEAGENT_COL; j++)
				if(reachable_table[i][j]) delete reachable_table[i][j];
				
				for(int k=0; k<MOVEAGENT_ROW; k++)
					delete[] reachable_table[k];
				delete[] reachable_table;
	}

	static MoveAgent& GetSingleton();
	
private:
	bool Locate(const A3DVECTOR3 & pos, int & row, int & col);
	bool InRegion(const A3DVECTOR3& pos);

private:
	AString m_sFilename;
	A3DVECTOR3*** reachable_table;

	// 玩家可自由活动的区域
	A3DVECTOR3 m_vMinRegion;
	A3DVECTOR3 m_vMaxRegion;

	struct off_node_t
	{
		int x_off;
		int z_off;
		off_node_t(int x,int z):x_off(x),z_off(z){}
	};
	/*
	  \  2 | 1  /
		\  |  /
	 3    \|/    0
	---------------
	 4    /|\    7
		/  |  \
	  /	 5 |  6 \
	*/
	std::vector<off_node_t> off_node_list[8];
};

#endif