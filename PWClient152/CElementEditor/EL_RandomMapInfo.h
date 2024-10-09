#ifndef _EL_RANDOMMAPINFO_
#define _EL_RANDOMMAPINFO_

#if defined _ELEMENT_EDITOR_ || defined _ELEMENTCLIENT
#include <AAssist.h>
#endif

#include <vector.h>


// 随机地图的信息
class CRandMapProp
{
public:
	struct FILEHEADER2 
	{
		int iTileCount;				// 块个数
		int iTileSize;				// 每块大小(米)
		int mainLineMaxLen;			// 主线最大长度
		int mainLineMinLen;			// 主线最小长度
		int branchLineNumMax;		// 支线数最大值
		int branchLineNumMin;		// 支线数最小值
		int branchLineMaxLen;		// 支线长度最大值
		int branchLineMinLen;		// 支线长度最小值
	};

	struct FILEHEADER : public FILEHEADER2 // version 3
	{
		float fPosX;
		float fPosY;
		float fPosZ;
	};
	
	enum
	{
		GRID_TYPE_NORMAL = 0,
		GRID_TYPE_START,		// 起点
		GRID_TYPE_END,			// 终点
		GRID_TYPE_NOUSE,		// 无效
		GRID_TYPE_HIDE_COTTAGE, // 隐藏茅屋
		GRID_TYPE_HIDE_ROOM,	// 隐藏房间
		GRID_TYPE_HIDE_TREE,	// 隐藏圣树
		GRID_TYPE_NUM,
	};
	enum
	{
		GRID_CONNECT_NONE	= 0,
		GRID_CONNECT_LEFT   = 0x0001,
		GRID_CONNECT_TOP    = 0x0002,
		GRID_CONNECT_RIGHT  = 0x0004,
		GRID_CONNECT_BOTTOM = 0x0008,
			
		GRID_CON_NUM = 4,
	};
		
#ifdef _ELEMENT_EDITOR_				// for editor
	struct MAP_INFO
	{
		MAP_INFO(){index = 0,type = 0,connection=0;}
		
		int index; // 地图序号
			
		int type; //  0 普通 1 起点 2 终点 3 无用 4 隐藏茅屋 5 隐藏房间 6 隐藏圣树
		int connection; // mask 1 左 2 上 4 右 8 下
			
		AString name;
			
		bool operator==(const MAP_INFO& info)
		{
			return type == info.type && connection == info.connection && name == info.name;
		}
	};

	bool InitGird(int c);
	bool Save();
	bool SaveSev(const char* szPath);
	bool SaveClt(const char* szPath);
	static bool CreateRandomMapFile(const char* szDst);
	static bool OnCreateOneGrid(const char* szProjName);

#elif _ELEMENTCLIENT				// for client
	struct MAP_INFO
	{
		MAP_INFO(){index = 0;}
		
		int index; // 地图序号		
		ACString name;
	};

	bool LoadClt(const char* szPath);

#else								// for server
	struct MAP_INFO
	{
		MAP_INFO(){index = 0,type = 0,connection=0;}
		
		int index; // 地图序号		
		int type; //  0 普通 1 起点 2 终点 3 无用 4 隐藏茅屋 5 隐藏房间 6 隐藏圣树
		int connection; // mask 1 左 2 上 4 右 8 下
	};

	bool LoadSev(const char* szPath);
#endif
		
public:
		
	bool GetGirdProp(int iGrid,MAP_INFO& info);
	void SetGridProp(int iGrid, MAP_INFO& info);
	int GetGridCount() const { return m_GridProp.size();}
	void SetHeader(const FILEHEADER& h) { m_header = h;};
	const FILEHEADER& GetHeader() { return m_header;}
			
protected:
	abase::vector<MAP_INFO> m_GridProp;
	FILEHEADER m_header;
};
 
#endif
 