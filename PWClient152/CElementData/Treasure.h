// Treasure.h: interface for the CTreasure class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __TREASURE_H__
#define __TREASURE_H__

#ifdef _WINDOWS
typedef wchar_t wide_char;
#else
typedef unsigned short wide_char;
#endif

#include "vector.h"


extern const char *szMainType[];

#define BIT_PRESENT			(0x1 << 0) //新品
#define BIT_RECOMMEND		(0x1 << 1) //推荐品
#define BIT_SALES_PROMOTION (0x1 << 2) //促销品

struct  TREASURE_ITEM_VERSION4
{
	unsigned int id;	//对应物品ID
	char         szName[128]; //商品名字
	unsigned int count; //物品个数
	
	struct  
	{
		unsigned int price; //价格
		unsigned int until_time;//时间点（秒,如果这儿是0，下面的时间有效） 0,表示无限制
		unsigned int time; //租借时间（秒，0表示无期）
		
	}buy_fashion[4];//购买方式
	
	DWORD        others;//标志
	wide_char         desc[512];//文字描述原长64字，2006.8.7 改为512字

	//2006.8.7新加,版本4
	unsigned int tid; //商品id, localize 用.
	unsigned int sid; //分类id
	char		 file_icon[128];//图标文件路径名
};


struct  TREASURE_ITEM
{
	unsigned int id;	//对应物品ID
	char         szName[128]; //商品名字
	unsigned int count; //物品个数
	
	struct  
	{
		unsigned int price; //价格
		unsigned int until_time;//时间点（秒,如果这儿是0，下面的时间有效） 0,表示无限制
		unsigned int time; //租借时间（秒，0表示无期）
		
	}buy_fashion[4];//购买方式
	
	DWORD        others;//标志
	wide_char         desc[512];//文字描述原长64字，2006.8.7 改为512字

	unsigned int tid; //商品id, localize 用.
	unsigned int main_type;//0 - 7
	unsigned int sub_type;// 0 - 8
	char		 file_icon[128];//图标文件路径名
};



struct SUB_TYPE
{
	unsigned int id;
	char szName[64];
};


struct MAIN_TYPE
{
	char szName[64];
	abase::vector<SUB_TYPE> listSubType;
};

void Init_Treasure_item(TREASURE_ITEM * item);



class CTreasureMan
{
	enum
	{
		TREASURE_VERSION = 5,
	};
	
public:

	CTreasureMan()
	{
		m_nSubCurID = 0;
		m_nCurTID = 0;
		m_bChanged = false;
		for( int i = 0; i < 9; ++i)
		{
			sprintf(listMainType[i].szName,szMainType[i]);
		}
	}
	~CTreasureMan(){}

	bool Load(const char * szName);
	bool Save(const char * szName);
	
	bool Export(const char* szPath);
	
	//为了本地化工具准备的接口,不过没用了。
	bool LoadL(const char* szPath);
	bool ExportL(const char *szPath);

	void Delete( int idx );
	void DeleteByID( int id)
	{
		for( size_t i = 0; i < listTreasure.size(); ++i )
		{
			if(listTreasure[i]->tid == id) 
				Delete(i);				
		}
	}

	void DeleteSubType( int id);

	void Add(TREASURE_ITEM* pNewItem);
	void Release();
	int  GetTreasureCount(){ return listTreasure.size(); }
	TREASURE_ITEM * GetTreasure( int idx){ return listTreasure[idx]; }
	TREASURE_ITEM * GetTreasureByID( int id)
	{
		for( size_t i = 0; i < listTreasure.size(); ++i )
			if(listTreasure[i]->tid == id) return listTreasure[i];
		return NULL;
	}
	MAIN_TYPE * GetMainTypeList(){ return listMainType; }
	abase::vector<TREASURE_ITEM*>* GetTreasureList(){ return &listTreasure; }

	int GenSubID(){ return ++m_nSubCurID; }
	int GenTID(){ return ++m_nCurTID; }

	bool IsChanged(){ return m_bChanged; }
	
private:
	int GetSubIndex( int idx_main, int id_sub );
	
	abase::vector<TREASURE_ITEM*> listTreasure;
	MAIN_TYPE listMainType[9];
	int   m_nSubCurID;
	int   m_nCurTID;
	bool  m_bChanged;
};

#endif
