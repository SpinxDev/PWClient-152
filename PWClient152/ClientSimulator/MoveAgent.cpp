/**
* 该文件实现了记录世界对象中可到达的位置，供玩家移动
* 收到OBJECT_MOVE, OBJECT_STOP_MOVE的时候，记录所在point的坐标
* 玩家移动时，根据这些坐标，发送下一步位置的坐标到服务器
*/

#include "moveagent.h"

unsigned char DirConvert(const A3DVECTOR3 & dir)
{
	if(dir.x < 1e-6 && dir.x > -1e-6)
	{
		if(dir.z > 0.f) return 64;
		else return 192;
	}

	float rad = atanf(dir.z/dir.x);
	if(dir.x < 0) rad += 3.1416f;
	else if(dir.z < 0) rad += 2*3.1416f;
	return (unsigned char)(rad/(2*3.1416f)*256);
}

MoveAgent& MoveAgent::GetSingleton()
{
	static MoveAgent obj;
	return obj;
}

void MoveAgent::Init(const char* filename)
{
	m_sFilename = filename;
	FILE * file = fopen(filename, "rb");
	if(!file) return;

	int cnt=0;
	while(!feof(file))
	{
		A3DVECTOR3 pos(0.0f,0.0f,0.0f);
		fread(&pos, sizeof(pos), 1, file);
		Learn(pos);
		++cnt;
	}

	fclose(file);

	m_vMinRegion.Set(-2000.0f, 0.0f, -2000.0f);
	m_vMaxRegion.Set( 2000.0f, 0.0f,  2000.0f);
}

void MoveAgent::Quit()
{
	FILE* file = fopen(m_sFilename, "wb");
	if(!file) return;
	
	int cnt=0;
	for(int i=0; i<MOVEAGENT_ROW; i++)
	{
		for(int j=0; j<MOVEAGENT_COL; j++)
		{
			if(reachable_table[i][j] == NULL) continue;
			A3DVECTOR3 & pos = *(reachable_table[i][j]);
			fwrite(&pos, sizeof(pos), 1, file);
			++cnt;
		}
	}

	fclose(file);
}

void MoveAgent::Learn(const A3DVECTOR3 & rpos)
{
	int row, col;
	if(!Locate(rpos, row, col)) return;

	if( reachable_table[row][col] == NULL )
	{
		reachable_table[row][col] = new A3DVECTOR3(rpos.x, rpos.y, rpos.z);
	}
}

bool MoveAgent::GetMovePos(const A3DVECTOR3 & cur_pos, const A3DVECTOR3 & dest_pos, float range, A3DVECTOR3 & next_pos)
{
	if((dest_pos - cur_pos).SquaredMagnitude() <= range*range)
	{
		next_pos = dest_pos;	
		return true;
	}
	A3DVECTOR3 dir = dest_pos;
	dir -= cur_pos;
	return GetMovePos(cur_pos,DirConvert(dir),range,next_pos);
}

bool MoveAgent::GetMovePos(const A3DVECTOR3 & cur_pos, unsigned char dir, float range, A3DVECTOR3 & next_pos)
{
	int row, col;
	if(!Locate(cur_pos, row, col)) return false;
	range *= range;
	
	bool ret = false;
	int tmp_row, tmp_col;
	A3DVECTOR3 tmp_pos;
	std::vector<off_node_t> & off_list = off_node_list[dir/32];
	for(int i=off_list.size()-1; i>=0; i--)
	{
		tmp_row = row + off_list[i].z_off;
		tmp_col = col + off_list[i].x_off;
		if(tmp_row < 0 || tmp_row > MOVEAGENT_ROW-1 || tmp_col < 0 || tmp_col > MOVEAGENT_COL-1)
			continue;
		if(!reachable_table[tmp_row][tmp_col])
			continue;
		tmp_pos = *(reachable_table[tmp_row][tmp_col]);	
		if((tmp_pos - cur_pos).SquaredMagnitude() > range)
			continue;

		if( !InRegion(tmp_pos) )
			continue;
		
		ret = true;
		next_pos = tmp_pos;
		break;
	}
	
	return ret;
}

bool MoveAgent::Locate(const A3DVECTOR3 & pos, int & row, int & col)
{
	row = int((pos.z - MOVEAGENT_Z_START)/MOVEAGENT_STEP);
	col = int((pos.x - MOVEAGENT_X_START)/MOVEAGENT_STEP);
	if(row < 0 || row > MOVEAGENT_ROW-1 || col < 0 || col > MOVEAGENT_COL-1)
	{
		row = col = 0;
		return false;
	}
	return true;
}

// 检测指定的点是否在限定区域内
bool MoveAgent::InRegion(const A3DVECTOR3& pos)
{
	if( pos.x < m_vMinRegion.x || pos.x > m_vMaxRegion.x ||
		pos.z < m_vMinRegion.z || pos.z > m_vMaxRegion.z )
		return false;
	else
		return true;
}