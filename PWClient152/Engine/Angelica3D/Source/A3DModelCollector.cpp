#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "A3DModel.h"
#include "A3DModelCollector.h"

inline static int mstrncmp(const char *src, const char *dest,size_t s)
{
	while(*src && *dest && s)
	{
		int tmp = *src - *dest;
		if(tmp ) return tmp;
		src ++;
		dest ++;
		s--;
	}
	return *src - *dest;
}

A3DModelCollector::A3DModelCollector(int capability)
{
	m_capability = capability;
	m_elecount = 0;
	m_eletab = (A3DModelCollectorElement *)malloc(sizeof(A3DModelCollectorElement) * capability);
	assert(m_eletab);
}

A3DModelCollector::~A3DModelCollector()
{
	assert(m_elecount == 0);
	if(m_eletab) free(m_eletab);
}

int A3DModelCollector::Release()
{
	int i;
	for(i = 0; i < m_elecount;i++)
	{
		m_eletab[i].Release();
		m_eletab[i].~A3DModelCollectorElement();
	}
	m_elecount = 0;
	return 0;
}

int A3DModelCollector::PushBack(const char * filename , A3DModel * pModel)
{
	//线性 search......
	int i;
	for(i = 0; i < m_elecount;i++)
	{
		if(mstrncmp(m_eletab[i].m_keyfile,filename,_MAX_PATH) == 0)
		{
			//match
			return m_eletab[i].AddModelObject(pModel);
		}
	}

	//加入到tab中
	if(m_elecount >= m_capability)
	{
		A3DModelCollectorElement *p;
		int capability;
		if(m_capability < 100)
			capability = m_capability * 2; 
		else 
			capability = m_capability * 3/2;
		p = (A3DModelCollectorElement *)realloc(m_eletab,capability * sizeof(A3DModelCollectorElement));
		assert(p);
		if(!p) return -1;
		m_capability = capability;
		m_eletab = p;
	}
	m_eletab[m_elecount].Init(filename);
	m_eletab[m_elecount].AddModelObject(pModel);
	m_elecount ++;
	return 0;
}

A3DModel * A3DModelCollector::PopUp(const char * filename)
{	
	int i;
	for(i = 0; i < m_elecount;i++)
	{
		if(mstrncmp(m_eletab[i].m_keyfile,filename,_MAX_PATH) == 0)
		{
			//match
			return m_eletab[i].GetModelObject();
		}
	}
	return NULL;
}

bool A3DModelCollectorElement::Init(const char *filename)
{
	strncpy(m_keyfile,filename,_MAX_PATH);
	m_modcount	= 0;
	m_capability = DEFAULT_MODEL_NUMBER_IN_CACHE;
	m_modtab	= (A3DModel**)malloc(sizeof(A3DModel *)*DEFAULT_MODEL_NUMBER_IN_CACHE);
	assert(m_modtab);
	return true;
}

A3DModelCollectorElement::~A3DModelCollectorElement()
{
	assert(m_modcount ==0);
	if(m_modtab) free(m_modtab);
}

int A3DModelCollectorElement::Release()
{
	int i;
	for(i = 0; i < m_modcount;i++)
	{
		m_modtab[i]->Release();
		delete m_modtab[i];
		m_modtab[i] = NULL;
	}
	m_modcount = 0;
	return -1;
}


