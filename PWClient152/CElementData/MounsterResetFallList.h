#ifndef _MOUNSTER_RESET_FALL_LIST_H
#define _MOUNSTER_RESET_FALL_LIST_H

#define _MOUNSTER_FALL_ITEM_VERSION 1

#include "vector.h"
/*
	添加导出功能，导出时归一
	添加选择NPC时可以选择多个
	只读时不能访问数据的标志
*/

struct ITEM_ELEMENT 
{
	unsigned int id;			//物品ID
	float        fProbability;  //掉落概率
};


class CMounsterFallItem
{

public:
	//取得掉落元素的列表，大小是固定的256个元素
	ITEM_ELEMENT*		GetElementList(){ return listElement; }
	float*              GetProbabilityList(){ return listProbability; }
	
	
	int					GetMounsterNum(){ return listMounsterId.size(); }
	unsigned int		GetMounsterID( int idx ){ return listMounsterId[idx]; };
	void                DeleteMounster( int idx){ listMounsterId.erase(&listMounsterId[idx]);}
	void				AddMounster( unsigned int id){ listMounsterId.push_back(id); }
	char*				GetItemName(){ return (char*)szName; }
	void				SetItemName( const char* name)
						{ 
							memset(szName,0,128);
							strcpy(szName,name); 
						}
	int                 GetItemType(){ return nType; }
	void                SetItemType( const int type ){ nType = type; }
	CMounsterFallItem*	Copy()
						{
							CMounsterFallItem* pNew = new CMounsterFallItem;
							if(pNew==0) return 0;
							for( size_t i = 0; i < listMounsterId.size(); ++i)
							pNew->AddMounster(listMounsterId[i]);
							ITEM_ELEMENT *pEL = pNew->GetElementList();
							memcpy(pEL,listElement,sizeof(ITEM_ELEMENT)*256);
							float *pPro = pNew->GetProbabilityList();
							memcpy(pPro,listProbability,sizeof(float)*8);
							pNew->SetItemName(szName);
							pNew->SetItemType(nType);
							return pNew;
						}
	bool        Save(FILE *pFile);
	bool        Load(FILE *pFile, unsigned int dwVersion);

private:
	abase::vector<unsigned int> listMounsterId; 
	float listProbability[8];
	ITEM_ELEMENT listElement[256];
	char szName[128];
	int  nType;//1,表示完全替代;2,同时生效
};

class CMounsterResetFallListData
{

public:
	void				AddItem(CMounsterFallItem *pNewItem){ listItem.push_back(pNewItem); }
	int					GetItemNum(){ return listItem.size(); }
	CMounsterFallItem * GetItem( int idx){ return listItem[idx]; }
	void                SetItem( int idx, CMounsterFallItem *pItem){ listItem[idx] = pItem; }
	void				DeleteItem(int idx)
						{
							delete listItem[idx];
							listItem.erase(&listItem[idx]);
						}
	bool                Load(const char *szPathName);
	bool                Save(const char *szPathName);


	void                Release()
	{
		int n = listItem.size();
		for( int i = 0; i < n; ++i)
			delete listItem[i];
		listItem.clear();
	}

private:
	abase::vector<CMounsterFallItem*> listItem;
};

bool inline CMounsterResetFallListData::Load(const char *szPathName)
{
	FILE *pFile = fopen(szPathName,"rb");
	if(pFile==0) return false;
	unsigned int nVersion;
	fread(&nVersion,sizeof(unsigned int),1,pFile);
	if(nVersion > _MOUNSTER_FALL_ITEM_VERSION) return false;
	unsigned int num = 0;
	fread(&num,sizeof(unsigned int),1,pFile);
	for( unsigned int i = 0; i < num; ++i)
	{
		CMounsterFallItem *pNewItem = new CMounsterFallItem;
		if(!pNewItem->Load(pFile,nVersion)) 
		{
			fclose(pFile);
			delete pNewItem;
			return false;
		}
		listItem.push_back(pNewItem);
	}
	fclose(pFile);
	return true;
}

bool inline CMounsterResetFallListData::Save(const char *szPathName)
{
	FILE *pFile = fopen(szPathName,"wb");
	if(pFile==0) return false;
	unsigned int nVersion = _MOUNSTER_FALL_ITEM_VERSION;
	fwrite(&nVersion,sizeof(unsigned int),1,pFile);
	unsigned int num = listItem.size();
	fwrite(&num,sizeof(unsigned int),1,pFile);
	for( unsigned int i = 0; i < num; ++i)
	{
		if(!listItem[i]->Save(pFile))
		{
			fclose(pFile);
			return false;
		}
	}
	fclose(pFile);
	return true;
}

bool inline CMounsterFallItem::Save(FILE *pFile)
{
	unsigned int num = listMounsterId.size();
	fwrite(&num,sizeof(unsigned int),1,pFile);	
	unsigned int i(0);
	for( i = 0; i < num; i++)
		fwrite(&listMounsterId[i],sizeof(unsigned int),1,pFile);
	for( i = 0; i < 256; i++)
		fwrite(&listElement[i],sizeof(ITEM_ELEMENT),1,pFile);
	fwrite(szName,sizeof(char)*128,1,pFile);
	fwrite(&nType,sizeof(int),1,pFile);
	
	for( i = 0; i < 8; i++)
		fwrite(&listProbability[i],sizeof(float),1,pFile);
	return true;
}

bool inline CMounsterFallItem::Load(FILE *pFile , unsigned int dwVersion)
{
	unsigned int num = 0;
	unsigned int id;
	if(dwVersion == 0)
	{
		fread(&num,sizeof(unsigned int),1,pFile);	
		unsigned int i(0);
		for( i = 0; i < num; i++)
		{
			fread(&id,sizeof(unsigned int),1,pFile);
			listMounsterId.push_back(id);
		}
		for( i = 0; i < 256; i++)
			fread(&listElement[i],sizeof(ITEM_ELEMENT),1,pFile);
		fread(szName,sizeof(char)*128,1,pFile);
		fread(&nType,sizeof(int),1,pFile);
		for( i = 0; i < 8; i++) listProbability[i] = 0.0f;
	}else if(dwVersion > 0)
	{
		fread(&num,sizeof(unsigned int),1,pFile);	
		unsigned int i(0);
		for( i = 0; i < num; i++)
		{
			fread(&id,sizeof(unsigned int),1,pFile);
			listMounsterId.push_back(id);
		}
		for( i = 0; i < 256; i++)
			fread(&listElement[i],sizeof(ITEM_ELEMENT),1,pFile);
		fread(szName,sizeof(char)*128,1,pFile);
		fread(&nType,sizeof(int),1,pFile);
		char szTemp[128];
		sprintf(szTemp,"\n nType = %d",nType);
		TRACE(szTemp);
		for( i = 0; i < 8; i++)
			fread(&listProbability[i],sizeof(float),1,pFile);
	}
	return true;
}

#endif