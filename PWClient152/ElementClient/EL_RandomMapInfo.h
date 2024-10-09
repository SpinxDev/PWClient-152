 #ifndef _EL_RANDOMMAPINFO_
#define _EL_RANDOMMAPINFO_

#if defined _ELEMENT_EDITOR_ || defined _ELEMENTCLIENT
#include <AAssist.h>
#endif

#include <vector.h>


// �����ͼ����Ϣ
class CRandMapProp
{
public:
	struct FILEHEADER2 
	{
		int iTileCount;				// �����
		int iTileSize;				// ÿ���С(��)
		int mainLineMaxLen;			// ������󳤶�
		int mainLineMinLen;			// ������С����
		int branchLineNumMax;		// ֧�������ֵ
		int branchLineNumMin;		// ֧������Сֵ
		int branchLineMaxLen;		// ֧�߳������ֵ
		int branchLineMinLen;		// ֧�߳�����Сֵ
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
		GRID_TYPE_START,		// ���
		GRID_TYPE_END,			// �յ�
		GRID_TYPE_NOUSE,		// ��Ч
		GRID_TYPE_HIDE_COTTAGE, // ����é��
		GRID_TYPE_HIDE_ROOM,	// ���ط���
		GRID_TYPE_HIDE_TREE,	// ����ʥ��
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
		
		int index; // ��ͼ���
			
		int type; //  0 ��ͨ 1 ��� 2 �յ� 3 ���� 4 ����é�� 5 ���ط��� 6 ����ʥ��
		int connection; // mask 1 �� 2 �� 4 �� 8 ��
			
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
		
		int index; // ��ͼ���		
		ACString name;
	};

	bool LoadClt(const char* szPath);

#else								// for server
	struct MAP_INFO
	{
		MAP_INFO(){index = 0,type = 0,connection=0;}
		
		int index; // ��ͼ���		
		int type; //  0 ��ͨ 1 ��� 2 �յ� 3 ���� 4 ����é�� 5 ���ط��� 6 ����ʥ��
		int connection; // mask 1 �� 2 �� 4 �� 8 ��
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
 