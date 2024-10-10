#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "A3DGraphicsFX.h"
#include "A3DGFXCollector.h"

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

A3DGFXCollector::A3DGFXCollector(int capability)
{
	m_capability = capability;
	m_elecount = 0;
	m_eletab = (A3DGFXCollectorElement *)malloc(sizeof(A3DGFXCollectorElement) * capability);
	assert(m_eletab);
}

A3DGFXCollector::~A3DGFXCollector()
{
	assert(m_elecount == 0);
	if(m_eletab) free(m_eletab);
}

int A3DGFXCollector::Release()
{
	int i;
	for(i = 0; i < m_elecount;i++)
	{
		m_eletab[i].Release();
		m_eletab[i].~A3DGFXCollectorElement();
	}
	m_elecount = 0;
	return 0;
}

int A3DGFXCollector::PushBack(const char * filename , A3DGraphicsFX * gfx)
{
	//线性 search......
	int i;
	for(i = 0; i < m_elecount;i++)
	{
		if(mstrncmp(m_eletab[i].m_keyfile,filename,_MAX_PATH) == 0)
		{
			//match
			return m_eletab[i].AddGFXObject(gfx);
		}
	}

	//加入到tab中
	if(m_elecount >= m_capability)
	{
		A3DGFXCollectorElement *p;
		int capability;
		if(m_capability < 100)
			capability = m_capability * 2; 
		else 
			capability = m_capability * 3/2;
		p = (A3DGFXCollectorElement *)realloc(m_eletab,capability * sizeof(A3DGFXCollectorElement));
		assert(p);
		if(!p) return -1;
		m_capability = capability;
		m_eletab = p;
	}
	m_eletab[m_elecount].Init(filename);
	m_eletab[m_elecount].AddGFXObject(gfx);
	m_elecount ++;
	return 0;
}

A3DGraphicsFX * A3DGFXCollector::PopUp(const char * filename)
{	
	int i;
	for(i = 0; i < m_elecount;i++)
	{
		if(mstrncmp(m_eletab[i].m_keyfile,filename,_MAX_PATH) == 0)
		{
			//match
			return m_eletab[i].GetGFXObject();
		}
	}
	return NULL;
}

void A3DGFXCollectorElement::Init(const char *filename)
{
	strncpy(m_keyfile,filename,_MAX_PATH);
	m_gfxcount	= 0;
	m_capability = DEFAULT_GFX_NUMBER_IN_CACHE;
	m_gfxtab	= (A3DGraphicsFX**)malloc(sizeof(A3DGraphicsFX *)*DEFAULT_GFX_NUMBER_IN_CACHE);
	assert(m_gfxtab);
}

A3DGFXCollectorElement::~A3DGFXCollectorElement()
{
	assert(m_gfxcount ==0);
	if(m_gfxtab) free(m_gfxtab);
}

int A3DGFXCollectorElement::Release()
{
	int i;
	for(i = 0; i < m_gfxcount;i++)
	{
		m_gfxtab[i]->Release();
		delete m_gfxtab[i];
		m_gfxtab[i] = NULL;
	}
	m_gfxcount = 0;
	return -1;
}


