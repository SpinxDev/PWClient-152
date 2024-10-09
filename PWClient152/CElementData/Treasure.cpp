#include "stdafx.h"
#include <time.h>
#include "Treasure.h"


void Init_Treasure_item(TREASURE_ITEM * item)
{
	item->count = 1;
	memset(item->desc,0,sizeof(item->desc));
	memset(item->file_icon,0,128);
	item->id = 0;
	item->others = BIT_PRESENT;
	item->main_type = 0;
	item->sub_type = 0;
	strcpy(item->szName,"宝物1");
	for( int i = 0; i < 4; ++i)
	{
		item->buy_fashion[i].price = 0;
		item->buy_fashion[i].until_time = 0;
		item->buy_fashion[i].time  = 0;
	}
}

void CTreasureMan::Add(TREASURE_ITEM* pNewItem)
{
	if(pNewItem) listTreasure.push_back(pNewItem);
}

void CTreasureMan::Delete( int idx)
{
	TREASURE_ITEM *pObj = listTreasure[idx];
	listTreasure.erase( &listTreasure[idx] );
	delete pObj;
}

bool CTreasureMan::Load(const char * szName)
{
	char szPath[MAX_PATH] = {0};
	sprintf(szPath,"TreasureData\\%s",szName);
	
	FILE *pFile = fopen(szPath,"rb");
	if(pFile==0) return false;
	DWORD dwVersion;
	fread(&dwVersion,sizeof(DWORD),1,pFile);
	if(dwVersion == 1)
	{
		int size = 0;
		fread(&size,sizeof(int),1,pFile);
		for( int i = 0; i < size; ++i)
		{
			TREASURE_ITEM *pNew = new TREASURE_ITEM;
			if(pNew==0) return false;
			Init_Treasure_item(pNew);
			
			fread(&pNew->id,sizeof(unsigned int),1,pFile);
			fread(&pNew->szName,128,1,pFile);
			fread(&pNew->count,sizeof(unsigned int),1,pFile);
			fread(&pNew->buy_fashion[0].price,sizeof(unsigned int),1,pFile);
			fread(&pNew->others,sizeof(DWORD),1,pFile);
			memset(pNew->desc,0,sizeof(pNew->desc));
			pNew->tid = ++m_nCurTID;
			listTreasure.push_back(pNew);
		}
	}else if(dwVersion == 2)
	{
		int size = 0;
		fread(&size,sizeof(int),1,pFile);
		for( int i = 0; i < size; ++i)
		{
			TREASURE_ITEM *pNew = new TREASURE_ITEM;
			if(pNew==0) return false;
			Init_Treasure_item(pNew);
			
			fread(&pNew->id,sizeof(unsigned int),1,pFile);
			fread(&pNew->szName,128,1,pFile);
			fread(&pNew->count,sizeof(unsigned int),1,pFile);
			for( int j = 0; j < 4; j++)
			{
				fread(&pNew->buy_fashion[j].price,sizeof(unsigned int),1,pFile);
				fread(&pNew->buy_fashion[j].time,sizeof(unsigned int),1,pFile);
				pNew->buy_fashion[j].until_time = 0;
			}
			fread(&pNew->others,sizeof(DWORD),1,pFile);
			memset(pNew->desc,0,sizeof(pNew->desc));
			pNew->tid = ++m_nCurTID;
			listTreasure.push_back(pNew);
		}
	}else if( dwVersion == 3)
	{
		int size = 0;
		fread(&size,sizeof(int),1,pFile);
		for( int i = 0; i < size; ++i)
		{
			TREASURE_ITEM *pNew = new TREASURE_ITEM;
			if(pNew==0) return false;
			Init_Treasure_item(pNew);

			fread(&pNew->id,sizeof(unsigned int),1,pFile);
			fread(&pNew->szName,128,1,pFile);
			fread(&pNew->count,sizeof(unsigned int),1,pFile);
			for( int j = 0; j < 4; j++)
			{
				fread(&pNew->buy_fashion[j].price,sizeof(unsigned int),1,pFile);
				fread(&pNew->buy_fashion[j].until_time,sizeof(unsigned int),1,pFile);
				fread(&pNew->buy_fashion[j].time,sizeof(unsigned int),1,pFile);
			}
			fread(&pNew->others,sizeof(DWORD),1,pFile);
			fread(pNew->desc,64*sizeof(WORD),1,pFile);
			pNew->tid = ++m_nCurTID;
			listTreasure.push_back(pNew);
		}
		
	}else if( dwVersion == 4)
	{
		int size = 0;
		fread(&size,sizeof(int),1,pFile);
		int i(0);
		for( i = 0; i < size; ++i)
		{
			TREASURE_ITEM *pNew = new TREASURE_ITEM;
			if(pNew==0) return false;
			Init_Treasure_item(pNew);
			TREASURE_ITEM_VERSION4 temp;
			fread(&temp,sizeof(TREASURE_ITEM_VERSION4),1,pFile);
			
			memcpy(&pNew->buy_fashion[0],&temp.buy_fashion[0],sizeof(unsigned int)*3);
			memcpy(&pNew->buy_fashion[1],&temp.buy_fashion[1],sizeof(unsigned int)*3);
			memcpy(&pNew->buy_fashion[2],&temp.buy_fashion[3],sizeof(unsigned int)*3);
			memcpy(&pNew->buy_fashion[3],&temp.buy_fashion[3],sizeof(unsigned int)*3);
			pNew->count = temp.count;
			pNew->id = temp.id;
			pNew->others = temp.others;
			strcpy(pNew->szName,temp.szName);
			strcpy(pNew->file_icon,temp.file_icon);
			wcscpy(pNew->desc,temp.desc);
			pNew->tid = temp.tid;
			if(pNew->tid > m_nCurTID) m_nCurTID = pNew->tid;
			pNew->main_type = temp.sid;
			pNew->sub_type = temp.sid + 1;
			listTreasure.push_back(pNew);
		}
		//制造一个临时的子类型表
		for( i = 0; i < 9; ++i)
		{
			SUB_TYPE temp;
			temp.id = GenSubID();
			strcpy(temp.szName,"临时子类型");
			listMainType[i].listSubType.push_back(temp);
		}
		m_bChanged = true;
	}else if( dwVersion == TREASURE_VERSION)
	{
		int size = 0;
		fread(&size,sizeof(int),1,pFile);
		int i(0);
		for( i = 0; i < size; ++i)
		{
			TREASURE_ITEM *pNew = new TREASURE_ITEM;
			if(pNew==0) return false;
			fread(pNew,sizeof(TREASURE_ITEM),1,pFile);
			if(pNew->tid > m_nCurTID) m_nCurTID = pNew->tid;
			listTreasure.push_back(pNew);
		}

		//读类型表
		for( i = 0; i < 9; ++i)
		{
			fread(&size,sizeof(int),1,pFile);
			for( int j = 0; j < size; ++j)
			{
				SUB_TYPE temp;
				fread(&temp.id,sizeof(int),1,pFile);
				if(temp.id > m_nSubCurID) m_nSubCurID = temp.id;
				fread(&temp.szName,64,1,pFile);
				listMainType[i].listSubType.push_back(temp);
			}
		}
	}
	fclose(pFile);
	return true;
}

bool CTreasureMan::Save(const char * szName)
{
	char szPath[MAX_PATH] = {0};
	sprintf(szPath,"TreasureData\\%s",szName);
	FILE *pFile = fopen(szPath,"wb");
	if(pFile==0) return false;
	DWORD dwVersion = TREASURE_VERSION;
	fwrite(&dwVersion,sizeof(DWORD),1,pFile);
	int size = listTreasure.size();
	fwrite(&size,sizeof(int),1,pFile);
	int i(0);
	for( i = 0; i < size; ++i)
		fwrite(listTreasure[i],sizeof(TREASURE_ITEM),1,pFile);
	
	//写类型表
	for( i = 0; i < 9; ++i)
	{
		size = listMainType[i].listSubType.size();
		fwrite(&size,sizeof(int),1,pFile);
		for( int j = 0; j < size ; ++j)
		{
			fwrite(&listMainType[i].listSubType[j].id,sizeof(int),1,pFile);
			fwrite(&listMainType[i].listSubType[j].szName,64,1,pFile);
		}
	}
	
	fclose(pFile);
	return true;
}


bool CTreasureMan::LoadL(const char* szPath)
{
	/*
	FILE *pFile = fopen(szPath,"rb");
	if(pFile==0) return false;
	fread(&m_dwTimeStamp,sizeof(DWORD),1,pFile);
	int size;
	fread(&size,sizeof(int),1,pFile);
	for( int i = 0; i < size; ++i)
	{
		TREASURE_ITEM *pNew = new TREASURE_ITEM;
		
		fread(&pNew->id,sizeof(unsigned int),1,pFile);
		fread(&pNew->count,sizeof(unsigned int),1,pFile);
		
		for( int j = 0; j < 4; j++)
		{
			fread(&pNew->buy_fashion[j].price, sizeof(unsigned int),1,pFile);
			fread(&pNew->buy_fashion[j].until_time, sizeof(unsigned int),1,pFile);
			fread(&pNew->buy_fashion[j].time, sizeof(unsigned int),1,pFile);
		}
		fread(&pNew->others,sizeof(DWORD),1,pFile);
		fread(&pNew->desc,sizeof(WORD)*64,1,pFile);
		listTreasure.push_back(pNew);
	}
	fclose(pFile);*/
	return true;	
}

bool CTreasureMan::ExportL(const char *szPath)
{
	/*
	FILE *pFile = fopen(szPath,"wb");
	if(pFile==0) return false;
	
	fwrite(&m_dwTimeStamp,sizeof(DWORD),1,pFile);
	int size = listTreasure.size();
	fwrite(&size,sizeof(int),1,pFile);
	for( int i = 0; i < size; ++i)
	{
		fwrite(&listTreasure[i]->id,sizeof(unsigned int),1,pFile);
		fwrite(&listTreasure[i]->count,sizeof(unsigned int),1,pFile);

		for( int j = 0; j < 4; j++)
		{
			fwrite(&listTreasure[i]->buy_fashion[j].price, sizeof(unsigned int),1,pFile);
			fwrite(&listTreasure[i]->buy_fashion[j].until_time, sizeof(unsigned int),1,pFile);
			fwrite(&listTreasure[i]->buy_fashion[j].time, sizeof(unsigned int),1,pFile);
		}
		fwrite(&listTreasure[i]->others,sizeof(DWORD),1,pFile);
		fwrite(&listTreasure[i]->desc,sizeof(WORD)*64,1,pFile);
	}
	fclose(pFile);
	*/
	return true;
}

bool CTreasureMan::Export(const char* szPath)
{
	abase::vector<TREASURE_ITEM*> listTemp;
	char szInfo[256];
	size_t i(0);
	for( i = 0; i < listTreasure.size(); ++i)
	{
		if(listTreasure[i]->id == 0)
		{
			sprintf(szInfo, "在名为 \"%s\" 的商品中，没有指定关联物品!", listTreasure[i]->szName);
#ifndef UNICODE
			AfxMessageBox(szInfo, MB_ICONSTOP);
#endif
			return false;
		}
		if(listTreasure[i]->main_type <= 7)
			listTemp.push_back(listTreasure[i]);
	}
	
	FILE *pFile = fopen(szPath,"wb");
	if(pFile==0) return false;
	DWORD dwTimeStamp = (DWORD)time(NULL);
	fwrite(&dwTimeStamp,sizeof(DWORD),1,pFile);
	int size = listTemp.size();
	fwrite(&size,sizeof(int),1,pFile);
	for( i = 0; i < size; ++i)
	{
		fwrite(&listTemp[i]->tid,sizeof(unsigned int),1,pFile);
		fwrite(&listTemp[i]->main_type,sizeof(unsigned int),1,pFile);
		unsigned int subidx = GetSubIndex( listTemp[i]->main_type, listTemp[i]->sub_type);
		fwrite(&subidx,sizeof(unsigned int),1,pFile);
		memset(szInfo,0,256);
		strcpy(szInfo,listTemp[i]->file_icon);
		fwrite(szInfo,128,1,pFile);
		fwrite(&listTemp[i]->id,sizeof(unsigned int),1,pFile);
		fwrite(&listTemp[i]->count,sizeof(unsigned int),1,pFile);

		bool bNoPrice = true;
		bool bEndPrice = false;
		bool bForeverPrice = false;
		bool bSkip = false;
		for( int j = 0; j < 4; j++)
		{
			if( listTemp[i]->buy_fashion[j].price > 0 )
				bNoPrice = false;
			if( listTemp[i]->buy_fashion[j].price == 0 )
				bEndPrice = true;
			if( bForeverPrice && listTemp[i]->buy_fashion[j].price > 0 && listTemp[i]->buy_fashion[j].time == 0 )
			{
				char szInfo[256];
				sprintf(szInfo, "在名为 %s 的商品中，永久买断的物品价格有两个!", listTemp[i]->szName);
#ifndef UNICODE
				AfxMessageBox(szInfo, MB_ICONSTOP);
#endif
				return false;
			}
			if( listTemp[i]->buy_fashion[j].price > 0 && listTemp[i]->buy_fashion[j].time == 0 )
				bForeverPrice = true;

			if( bEndPrice == true && listTemp[i]->buy_fashion[j].price > 0 )
			{
				char szInfo[256];
				sprintf(szInfo, "在名为 %s 的商品中，价格为0的条目后还有价格非0的东西!", listTemp[i]->szName);
#ifndef UNICODE
				AfxMessageBox(szInfo, MB_ICONSTOP);
#endif
				return false;
			}

			if(listTemp[i]->buy_fashion[j].until_time!=0 && listTemp[i]->buy_fashion[j].time!=0)
				bSkip = true;
			
			
			fwrite(&listTemp[i]->buy_fashion[j].price, sizeof(unsigned int),1,pFile);
			fwrite(&listTemp[i]->buy_fashion[j].until_time, sizeof(unsigned int),1,pFile);
			fwrite(&listTemp[i]->buy_fashion[j].time, sizeof(unsigned int),1,pFile);
		}

		if(bSkip)
		{
			char szInfo[256];
			sprintf(szInfo, "在名为 %s 的商品的购买方式中,租借时间必须有一个为0!", listTemp[i]->szName);
#ifndef UNICODE
			AfxMessageBox(szInfo, MB_ICONSTOP);
#endif
			return false;
		}
	

		if( bNoPrice )
		{
			char szInfo[256];
			sprintf(szInfo, "在名为 %s 的商品中，没有标明合法的价格!", listTemp[i]->szName);
#ifndef UNICODE
			AfxMessageBox(szInfo, MB_ICONSTOP);
#endif
			return false;
		}
		fwrite(&listTemp[i]->others,sizeof(DWORD),1,pFile);
		wchar_t swTemp[512];
		memset(swTemp,0,sizeof(swTemp));
		wcscpy(swTemp,listTemp[i]->desc);
		fwrite(swTemp,sizeof(swTemp),1,pFile);//这儿改64到512

		//导出商品名字*
		wchar_t szBuffer[32];
		memset(szBuffer,0,sizeof(szBuffer));
		memset(szInfo,0,256);
		strcpy(szInfo,listTemp[i]->szName);
		MultiByteToWideChar(CP_ACP,0,szInfo,32,szBuffer,32);
		fwrite(&szBuffer,sizeof(szBuffer),1,pFile);
	}
	
	//写类型表*
	for( i = 0; i < 8; ++i)
	{
		wchar_t szBuffer[64];
		memset(szBuffer,0,sizeof(szBuffer));
		memset(szInfo,0,256);
		strcpy(szInfo,listMainType[i].szName);
		MultiByteToWideChar(CP_ACP,0,szInfo,64,szBuffer,64);
		fwrite(&szBuffer,sizeof(szBuffer),1,pFile);

		//子类型
		int size = listMainType[i].listSubType.size();
		fwrite(&size,sizeof(int),1,pFile);
		for( int j = 0; j < size; ++j)
		{
			memset(szBuffer,0,sizeof(szBuffer));
			memset(szInfo,0,256);
			strcpy(szInfo,listMainType[i].listSubType[j].szName);
			MultiByteToWideChar(CP_ACP,0,szInfo,64,szBuffer,64);
			fwrite(&szBuffer,sizeof(szBuffer),1,pFile);
		}
	}
	
	fclose(pFile);
	return true;	
}

void CTreasureMan::Release()
{
	int size = listTreasure.size();
	int i(0);
	for( i = 0; i < size; ++i)
		delete listTreasure[i];	
	listTreasure.clear();
	m_nCurTID = 0; 

	for(i = 0; i < 9; ++i)
		listMainType[i].listSubType.clear();
}

void CTreasureMan::DeleteSubType( int id)
{
	for(int i = 0; i < 9; ++i)
	{
		for( size_t j = 0; j < listMainType[i].listSubType.size(); ++j)
		{
			if(listMainType[i].listSubType[j].id == id)
			{
				listMainType[i].listSubType.erase(&(listMainType[i].listSubType[j]));
				
				//abase::vector<TREASURE_ITEM**> listTemp;
				for( size_t k = 0; k < listTreasure.size(); ++k)
				{
					if(listTreasure[k]->sub_type == id && listTreasure[k]->main_type == i)
					{
						//CString str;
						//str.Format("\n%s ** main_type = %d; sub_type = %d",listTreasure[k]->szName,listTreasure[k]->main_type,listTreasure[k]->sub_type);
						//TRACE(str);
						
						delete listTreasure[k];
						listTreasure.erase(&listTreasure[k]);
						k = -1;
					}
				}

				
				
				//for( k = 0; k < listTemp.size(); ++k)
				//	listTreasure.erase(listTemp[k]);

				return;
			}
		}
	}
}

int CTreasureMan::GetSubIndex( int idx_main, int id_sub )
{
	int size = listMainType[idx_main].listSubType.size();
	for( int i = 0; i < size; ++i)
	{
		if(listMainType[idx_main].listSubType[i].id==id_sub)
			return i;
	}
	ASSERT(FALSE);
	return 0;
}

