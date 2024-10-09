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

#define BIT_PRESENT			(0x1 << 0) //��Ʒ
#define BIT_RECOMMEND		(0x1 << 1) //�Ƽ�Ʒ
#define BIT_SALES_PROMOTION (0x1 << 2) //����Ʒ

struct  TREASURE_ITEM_VERSION4
{
	unsigned int id;	//��Ӧ��ƷID
	char         szName[128]; //��Ʒ����
	unsigned int count; //��Ʒ����
	
	struct  
	{
		unsigned int price; //�۸�
		unsigned int until_time;//ʱ��㣨��,��������0�������ʱ����Ч�� 0,��ʾ������
		unsigned int time; //���ʱ�䣨�룬0��ʾ���ڣ�
		
	}buy_fashion[4];//����ʽ
	
	DWORD        others;//��־
	wide_char         desc[512];//��������ԭ��64�֣�2006.8.7 ��Ϊ512��

	//2006.8.7�¼�,�汾4
	unsigned int tid; //��Ʒid, localize ��.
	unsigned int sid; //����id
	char		 file_icon[128];//ͼ���ļ�·����
};


struct  TREASURE_ITEM
{
	unsigned int id;	//��Ӧ��ƷID
	char         szName[128]; //��Ʒ����
	unsigned int count; //��Ʒ����
	
	struct  
	{
		unsigned int price; //�۸�
		unsigned int until_time;//ʱ��㣨��,��������0�������ʱ����Ч�� 0,��ʾ������
		unsigned int time; //���ʱ�䣨�룬0��ʾ���ڣ�
		
	}buy_fashion[4];//����ʽ
	
	DWORD        others;//��־
	wide_char         desc[512];//��������ԭ��64�֣�2006.8.7 ��Ϊ512��

	unsigned int tid; //��Ʒid, localize ��.
	unsigned int main_type;//0 - 7
	unsigned int sub_type;// 0 - 8
	char		 file_icon[128];//ͼ���ļ�·����
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
	
	//Ϊ�˱��ػ�����׼���Ľӿ�,����û���ˡ�
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
