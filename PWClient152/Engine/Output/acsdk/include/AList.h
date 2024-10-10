/*
 * FILE: AList.h
 *
 * DESCRIPTION: A class which manage one dual-link list;
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ALIST_H_
#define _ALIST_H_

#include "ABaseDef.h"

struct ALISTELEMENT
{
	void*			pData;
	ALISTELEMENT*	pNext;
	ALISTELEMENT*	pLast;

};

class AList
{
	DISABLE_COPY_AND_ASSIGNMENT(AList);
private:

	ALISTELEMENT*	m_pHead;
	ALISTELEMENT*	m_pTail;
	int				m_nSize;

public:
	AList();
	~AList();

	bool Init();
	bool Release();
	bool Reset();
	
	bool Append(void* pDataToAppend, ALISTELEMENT ** ppElement = NULL);
	bool Insert(void* pDataToInsert, ALISTELEMENT * pElement, ALISTELEMENT ** ppElement);
	bool Insert(void* pDataToInsert, void* pDataInsertBefore, ALISTELEMENT ** ppElement);
	bool Delete(ALISTELEMENT * pElement);
	bool Delete(void* pData);

	bool IsValid(ALISTELEMENT * pElement);
	ALISTELEMENT * FindElementByData(void* pData);
	ALISTELEMENT * GetElementByOrder(int nOrder);

	inline int GetSize() { return m_nSize; }
	inline ALISTELEMENT * GetHead() { return m_pHead; }
	inline ALISTELEMENT * GetTail() { return m_pTail; }
	inline ALISTELEMENT * GetFirst() { return m_pHead->pNext; }
};

typedef AList* PAList;

#endif